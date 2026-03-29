#include "TimetableModel.h"
#include "roommodel.h"
#include <QVariant>
#include <QDebug>
#include "lesson.h"
#include "lessonmodel.h"

TimetableModel::TimetableModel(QObject *parent)
    : QAbstractTableModel(parent),
    m_roomCount(0),
    m_slotCount(0),
    m_roomModel(nullptr),
    m_lessonModel(nullptr),
    m_lessonUsageRevision(0)
{
}

bool TimetableModel::isValidCell(int row, int column) const
{
    return row >= 0 && row < m_slotCount &&
           column >= 0 && column < m_roomCount;
}

int TimetableModel::cellIndex(int row, int column) const
{
    return row * m_roomCount + column;
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
        return cell.lessonId;
    case LessonNameRole:
        return cell.lessonName;
    default:
        return QVariant();
    }
}

bool TimetableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || role != LessonIdRole)
        return false;

    // Для совместимости с обычным редактированием через model.setData()
    // считаем, что lessonName = lessonId.
    return placeLesson(index.row(), index.column(), value.toString(), value.toString());
}

Qt::ItemFlags TimetableModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

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

bool TimetableModel::isLessonUsed(const QString &lessonId) const
{
    if (lessonId.isEmpty())
        return false;

    for (const LessonAssignment &cell : m_cells) {
        if (cell.lessonId == lessonId)
            return true;
    }
    return false;
}

bool TimetableModel::placeLesson(int row, int column, const QString &lessonId, const QString &lessonName)
{
    if (!isValidCell(row, column))
        return false;

    const int targetPos = cellIndex(row, column);

    // Ищем, не стоит ли этот же урок уже в другой ячейке
    int oldPos = -1;
    for (int i = 0; i < m_cells.size(); ++i) {
        if (m_cells[i].lessonId == lessonId) {
            oldPos = i;
            break;
        }
    }

    // Если урок уже был в другой ячейке — очищаем старую ячейку
    if (oldPos >= 0 && oldPos != targetPos) {
        const int oldRow = oldPos / m_roomCount;
        const int oldCol = oldPos % m_roomCount;

        m_cells[oldPos].lessonId.clear();
        m_cells[oldPos].lessonName.clear();

        emit dataChanged(index(oldRow, oldCol), index(oldRow, oldCol),
                         {LessonIdRole, LessonNameRole});
    }

    // Если в целевой ячейке был другой урок — просто перезапишем его
    m_cells[targetPos].lessonId = lessonId;
    m_cells[targetPos].lessonName = lessonName;

    emit dataChanged(index(row, column), index(row, column),
                     {LessonIdRole, LessonNameRole});

    ++m_lessonUsageRevision;
    emit lessonUsageChanged();

    /*for (auto x : m_cells)
        qDebug() << x.lessonId << ' ' << x.lessonName << "  ";
    qDebug() << '\n';*/

    return true;
}

bool TimetableModel::clearLesson(int row, int column)
{
    if (!isValidCell(row, column))
        return false;

    const int pos = cellIndex(row, column);

    if (m_cells[pos].lessonId.isEmpty() && m_cells[pos].lessonName.isEmpty())
        return true;

    m_cells[pos].lessonId.clear();
    m_cells[pos].lessonName.clear();

    emit dataChanged(index(row, column), index(row, column),
                     {LessonIdRole, LessonNameRole});

    ++m_lessonUsageRevision;
    emit lessonUsageChanged();

    return true;
}

void TimetableModel::setRoomCount(int count)
{
    if (count < 0)
        return;

    beginResetModel();
    m_roomCount = count;
    m_cells.clear();
    m_cells.resize(m_roomCount * m_slotCount);
    endResetModel();

    ++m_lessonUsageRevision;
    emit lessonUsageChanged();
}

void TimetableModel::setSlotCount(int count)
{
    if (count < 0)
        return;

    beginResetModel();
    m_slotCount = count;
    m_cells.clear();
    m_cells.resize(m_roomCount * m_slotCount);
    endResetModel();

    ++m_lessonUsageRevision;
    emit lessonUsageChanged();
}

void TimetableModel::setRoomModel(QObject *roomModel)
{
    m_roomModel = roomModel;
}

void TimetableModel::setLessonModel(QObject *lessonModel)
{
    m_lessonModel = lessonModel;
}

using namespace std;

bool augment(int v, map<int, bool> &used, map<int, vector<pair<int, int>>> &g, vector<pair<int, int>> &match)
{
    if (used[v])
        return false;
    used[v] = true;

    for (auto [to, id] : g[v])
    {
        if (match[to].first == -1 || augment(match[to].first, used, g, match))
        {
            match[to] = {v, id};
            return true;
        }
    }
    return false;
}

vector<pair<int, int>> kuhn(QList<Lesson*> &lessons)
{
    map<int, vector<pair<int, int>>> g;
    map<int, bool> used;
    for (Lesson *pl : lessons)
    {
        used[pl->teacherId()] = false;
        g[pl->teacherId()].push_back({pl->classes()[0], pl->id()});
    }

    vector<pair<int, int>> match(100, {-1, -1});

    for (auto [v, classes] : g)
    {
        if (augment(v, used, g, match))
            for (auto & [tmp, b] : used)
                b = false;
    }

    return match;
}

Q_INVOKABLE void TimetableModel::generate()
{
    qDebug() << "generating";

    QList<Lesson*> lessons = qobject_cast<LessonModel *>(m_lessonModel)->lessons();

    int row = 0;
    while (lessons.size() > 0)
    {
        qDebug() << lessons.size();
        vector<pair<int, int>> match = kuhn(lessons);

        set<int> deleted;
        int cnt = 0;
        for (int i = 0; cnt < m_roomCount && i < match.size(); ++i)
        {
            if (match[i].first == -1)
                continue;

            QString name;
            for (Lesson *les : lessons)
                if (les->id() == match[i].second)
                {
                    name = les->name();
                    break;
                }

            placeLesson(row, cnt++, QString::number(match[i].second), name);
            deleted.insert(match[i].second);
        }
        row++;

        QList<Lesson*> newLessons;
        for (int i = 0; i < lessons.size(); ++i)
        {
            if (!deleted.count(lessons[i]->id()))
                newLessons.push_back(lessons[i]);
        }
        lessons = newLessons;
    }

    qDebug() << "end generating";
}
