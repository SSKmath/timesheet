// TimetableModel.cpp
#include "TimetableModel.h"
#include "roommodel.h"  // для доступа к RoomModel::NameRole
#include <QVariant>

TimetableModel::TimetableModel(QObject *parent)
    : QAbstractTableModel(parent),
    m_roomCount(0), m_slotCount(0), m_roomModel(nullptr)
{
    // Изначально модель пуста
}

int TimetableModel::rowCount(const QModelIndex & /*parent*/) const
{
    return m_slotCount;
}

int TimetableModel::columnCount(const QModelIndex & /*parent*/) const
{
    return m_roomCount;
}

QVariant TimetableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() ||
        index.row() < 0 || index.row() >= m_slotCount ||
        index.column() < 0 || index.column() >= m_roomCount)
        return QVariant();

    const LessonAssignment &cell = m_cells[index.row() * m_roomCount + index.column()];
    switch (role) {
    case LessonIdRole:
        // Возвращаем идентификатор урока (например для логики)
        return cell.lessonId;
    case LessonNameRole:
        // Возвращаем название урока для отображения (или пусто)
        return cell.lessonName;
    default:
        return QVariant();
    }
}

bool TimetableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || role != LessonIdRole)
        return false;

    int row = index.row();
    int col = index.column();
    int pos = row * m_roomCount + col;
    if (pos < 0 || pos >= m_cells.size())
        return false;

    QString lessonId = value.toString();
    // Задаем идентификатор урока
    m_cells[pos].lessonId = lessonId;
    // При успешном получении id, заполняем имя урока той же строкой.
    // В реальном приложении здесь можно подгрузить название из модели уроков.
    m_cells[pos].lessonName = lessonId;

    // Сигнализируем, что данные ячейки изменились (для обновления вида)
    emit dataChanged(index, index, {LessonIdRole, LessonNameRole});
    return true;
}

Qt::ItemFlags TimetableModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;
    // Ячейки разрешаем редактировать (нужно для setData) и выделять
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

QHash<int, QByteArray> TimetableModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Qt::DisplayRole] = "display";
    roles[LessonIdRole] = "lessonId";
    roles[LessonNameRole] = "lessonName";
    return roles;
}

QVariant TimetableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return {};

    if (orientation == Qt::Horizontal) {
        if (auto rooms = qobject_cast<RoomModel *>(m_roomModel)) {
            const QString name = rooms->roomNameAt(section);
            qDebug() << section << ' ' << name << '\n';
            if (!name.isEmpty())
                return name;
        }
        return QString("Кабинет %1").arg(section + 1);
    }

    static const QStringList days = {
        "Понедельник", "Вторник", "Среда", "Четверг", "Пятница"
    };

    if (m_slotCount <= 0)
        return QString("Slot %1").arg(section + 1);

    const int slotsPerDay = qMax(1, (m_slotCount + 4) / 5);
    const int dayIndex = section / slotsPerDay;
    const int slotIndex = (section % slotsPerDay) + 1;

    if (dayIndex < days.size()) {
        const int hour = 9 + slotIndex - 1;
        return QString("%1 %2:%3")
            .arg(days[dayIndex])
            .arg(hour, 2, 10, QChar('0'))
            .arg("00");
    }

    return QString("Slot %1").arg(section + 1);
}

void TimetableModel::setRoomCount(int count)
{
    if (count < 0)
        return;
    beginResetModel();
    m_roomCount = count;
    // Пересоздаем массив ячеек
    m_cells.clear();
    m_cells.resize(m_roomCount * m_slotCount);
    endResetModel();
}

void TimetableModel::setSlotCount(int count)
{
    if (count < 0)
        return;
    beginResetModel();
    m_slotCount = count;
    // Пересоздаем массив ячеек
    m_cells.clear();
    m_cells.resize(m_roomCount * m_slotCount);
    endResetModel();
}

void TimetableModel::setRoomModel(QObject *roomModel)
{
    // Принимаем указатель на модель кабинетов (RoomModel*). Можно проверять тип.
    m_roomModel = roomModel;
}
