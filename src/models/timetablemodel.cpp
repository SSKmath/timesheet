#include "TimetableModel.h"
#include "roommodel.h"
#include <QVariant>
#include <QDebug>
#include "lesson.h"
#include "lessonmodel.h"
#include "school.h"
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSaveFile>
#include <QStandardPaths>
#include <map>
#include <set>
#include <vector>


TimetableModel::TimetableModel(QObject *parent)
    : QAbstractTableModel(parent),
    m_roomCount(0),
    m_slotCount(0),
    m_roomModel(nullptr),
    m_lessonModel(nullptr),
    m_lessonUsageRevision(0)
{
}

QString TimetableModel::baseStoragePath() const
{
    return QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
}

QString TimetableModel::schoolTimesheetDirPath(const QString &schoolId) const
{
    return QDir(baseStoragePath() + "/timesheet").filePath(schoolId);
}

QString TimetableModel::autosaveFilePath(const QString &schoolId) const
{
    return QDir(schoolTimesheetDirPath(schoolId)).filePath("autosave.json");
}

bool TimetableModel::ensureSchoolTimesheetDir(const QString &schoolId) const
{
    if (schoolId.isEmpty())
        return false;

    QDir baseDir(baseStoragePath());
    if (!baseDir.exists() && !baseDir.mkpath("."))
        return false;

    if (!baseDir.exists("timesheet") && !baseDir.mkdir("timesheet"))
        return false;

    QDir timesheetRoot(baseDir.filePath("timesheet"));
    if (timesheetRoot.exists(schoolId))
        return true;

    return timesheetRoot.mkpath(schoolId);
}

QString TimetableModel::currentSchoolId() const
{
    QObject *owner = nullptr;

    if (m_roomModel)
        owner = m_roomModel->parent();

    if (!owner && m_lessonModel)
        owner = m_lessonModel->parent();

    School *school = qobject_cast<School *>(owner);
    if (!school)
        return QString();

    return school->id();
}

void TimetableModel::saveToStorage() const
{
    if (m_loadingFromStorage || m_suspendAutosave)
        return;

    if (m_roomCount <= 0 || m_slotCount <= 0)
        return;

    if (m_loadedSignature.isEmpty())
        return;

    const QString schoolId = currentSchoolId();
    if (schoolId.isEmpty())
        return;

    if (schoolId != m_loadedSchoolId)
        return;

    if (!ensureSchoolTimesheetDir(schoolId))
    {
        qWarning() << "Не удалось создать папку для расписания школы" << schoolId;
        return;
    }

    QJsonObject root;
    root["schoolId"] = schoolId;
    root["roomCount"] = m_roomCount;
    root["slotCount"] = m_slotCount;
    root["savedAt"] = QDateTime::currentDateTimeUtc().toString(Qt::ISODate);

    QJsonArray cellsArray;
    for (int row = 0; row < m_slotCount; ++row)
    {
        for (int column = 0; column < m_roomCount; ++column)
        {
            const LessonAssignment &cell = m_cells[cellIndex(row, column)];
            if (cell.lessonId.isEmpty() && cell.lessonName.isEmpty())
                continue;

            QJsonObject obj;
            obj["row"] = row;
            obj["column"] = column;
            obj["lessonId"] = cell.lessonId;
            obj["lessonName"] = cell.lessonName;
            cellsArray.append(obj);
        }
    }

    root["cells"] = cellsArray;

    const QByteArray json = QJsonDocument(root).toJson(QJsonDocument::Indented);
    const QString path = autosaveFilePath(schoolId);

    QSaveFile file(path);
    if (!file.open(QIODevice::WriteOnly))
    {
        qWarning() << "Не удалось открыть файл расписания для записи:" << path;
        return;
    }

    if (file.write(json) == -1)
    {
        qWarning() << "Ошибка записи расписания:" << path;
        file.cancelWriting();
        return;
    }

    if (!file.commit())
    {
        qWarning() << "Не удалось зафиксировать файл расписания:" << path;
        return;
    }
}

void TimetableModel::tryLoadFromStorage()
{
    if (m_loadingFromStorage)
        return;

    if (m_roomCount <= 0 || m_slotCount <= 0)
        return;

    const QString schoolId = currentSchoolId();
    if (schoolId.isEmpty())
        return;

    const QString signature = schoolId + "|" +
                              QString::number(m_roomCount) + "|" +
                              QString::number(m_slotCount);

    if (m_loadedSignature == signature)
        return;

    ensureSchoolTimesheetDir(schoolId);

    QList<LessonAssignment> loadedCells;
    loadedCells.resize(m_roomCount * m_slotCount);

    QFile file(autosaveFilePath(schoolId));
    if (file.exists() && file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        const QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        if (doc.isObject())
        {
            const QJsonArray cellsArray = doc.object().value("cells").toArray();
            for (const QJsonValue &value : cellsArray)
            {
                const QJsonObject obj = value.toObject();
                const int row = obj.value("row").toInt(-1);
                const int column = obj.value("column").toInt(-1);

                if (row < 0 || row >= m_slotCount || column < 0 || column >= m_roomCount)
                    continue;

                LessonAssignment assignment;
                assignment.lessonId = obj.value("lessonId").toString();
                assignment.lessonName = obj.value("lessonName").toString();

                loadedCells[row * m_roomCount + column] = assignment;
            }
        }
    }

    m_loadingFromStorage = true;

    beginResetModel();
    m_cells = loadedCells;
    endResetModel();

    m_loadingFromStorage = false;
    m_loadedSignature = signature;
    m_loadedSchoolId = schoolId;

    ++m_lessonUsageRevision;
    emit lessonUsageChanged();
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

    int oldPos = -1;
    for (int i = 0; i < m_cells.size(); ++i) {
        if (m_cells[i].lessonId == lessonId) {
            oldPos = i;
            break;
        }
    }

    if (oldPos >= 0 && oldPos != targetPos) {
        const int oldRow = oldPos / m_roomCount;
        const int oldCol = oldPos % m_roomCount;

        m_cells[oldPos].lessonId.clear();
        m_cells[oldPos].lessonName.clear();

        emit dataChanged(index(oldRow, oldCol), index(oldRow, oldCol),
                         {LessonIdRole, LessonNameRole});
    }

    m_cells[targetPos].lessonId = lessonId;
    m_cells[targetPos].lessonName = lessonName;

    emit dataChanged(index(row, column), index(row, column),
                     {LessonIdRole, LessonNameRole});

    ++m_lessonUsageRevision;
    emit lessonUsageChanged();
    saveToStorage();

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
    saveToStorage();

    return true;
}

void TimetableModel::setRoomCount(int count)
{
    if (count < 0 || count == m_roomCount)
        return;

    QList<LessonAssignment> oldCells = m_cells;
    int oldRoomCount = m_roomCount;
    int oldSlotCount = m_slotCount;

    beginResetModel();

    m_roomCount = count;
    m_cells.clear();
    m_cells.resize(m_roomCount * m_slotCount);

    int copyRows = qMin(oldSlotCount, m_slotCount);
    int copyCols = qMin(oldRoomCount, m_roomCount);

    for (int row = 0; row < copyRows; ++row)
    {
        for (int col = 0; col < copyCols; ++col)
        {
            m_cells[row * m_roomCount + col] =
                oldCells[row * oldRoomCount + col];
        }
    }

    endResetModel();

    ++m_lessonUsageRevision;
    emit lessonUsageChanged();

    if (m_roomCount > 0)
        emit headerDataChanged(Qt::Horizontal, 0, m_roomCount - 1);

    saveToStorage();
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

    m_loadedSignature.clear();

    ++m_lessonUsageRevision;
    emit lessonUsageChanged();

    tryLoadFromStorage();
}

void TimetableModel::setRoomModel(QObject *roomModel)
{
    if (m_roomModel == roomModel)
        return;

    QObject::disconnect(m_roomRowsInsertedConnection);
    QObject::disconnect(m_roomRowsRemovedConnection);
    QObject::disconnect(m_roomModelResetConnection);
    QObject::disconnect(m_roomDataChangedConnection);

    m_roomModel = roomModel;

    QAbstractItemModel *model = qobject_cast<QAbstractItemModel *>(m_roomModel);
    if (!model)
        return;

    setRoomCount(model->rowCount());

    m_roomRowsInsertedConnection = QObject::connect(
        model, &QAbstractItemModel::rowsInserted,
        this,
        [this, model](const QModelIndex &, int, int)
        {
            setRoomCount(model->rowCount());
        }
        );

    m_roomRowsRemovedConnection = QObject::connect(
        model, &QAbstractItemModel::rowsRemoved,
        this,
        [this, model](const QModelIndex &, int, int)
        {
            setRoomCount(model->rowCount());
        }
        );

    m_roomModelResetConnection = QObject::connect(
        model, &QAbstractItemModel::modelReset,
        this,
        [this, model]()
        {
            setRoomCount(model->rowCount());
        }
        );

    m_roomDataChangedConnection = QObject::connect(
        model, &QAbstractItemModel::dataChanged,
        this,
        [this](const QModelIndex &, const QModelIndex &, const QList<int> &)
        {
            if (m_roomCount > 0)
                emit headerDataChanged(Qt::Horizontal, 0, m_roomCount - 1);
        }
        );
}

void TimetableModel::setLessonModel(QObject *lessonModel)
{
    m_lessonModel = lessonModel;
    m_loadedSignature.clear();
    tryLoadFromStorage();
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

    vector<pair<int, int>> match(500, {-1, -1});

    for (auto [v, classes] : g)
    {
        if (augment(v, used, g, match))
            for (auto & [tmp, b] : used)
                b = false;
    }

    return match;
}

void TimetableModel::generate()
{
    if (!m_lessonModel || m_roomCount <= 0 || m_slotCount <= 0)
        return;

    qDebug() << "generating";

    m_suspendAutosave = true;

    LessonModel *lessonModel = qobject_cast<LessonModel *>(m_lessonModel);
    if (!lessonModel)
    {
        m_suspendAutosave = false;
        return;
    }

    QList<Lesson*> lessons = lessonModel->lessons();

    map<int, Lesson*> lessonById;
    for (Lesson *lesson : lessons)
    {
        if (!lesson)
            continue;

        lessonById[lesson->id()] = lesson;
    }

    QList<Lesson*> remainingLessons;
    for (Lesson *lesson : lessons)
    {
        if (!lesson)
            continue;

        if (isLessonUsed(QString::number(lesson->id())))
            continue;

        if (lesson->classes().isEmpty())
            continue;

        remainingLessons.push_back(lesson);
    }

    lessons = remainingLessons;

    int row = 0;
    while (row < m_slotCount && lessons.size() > 0)
    {
        set<int> usedTeachers;
        set<int> usedClasses;

        for (int column = 0; column < m_roomCount; ++column)
        {
            const LessonAssignment &cell = m_cells[cellIndex(row, column)];
            if (cell.lessonId.isEmpty() && cell.lessonName.isEmpty())
                continue;

            bool ok = false;
            const int lessonId = cell.lessonId.toInt(&ok);
            if (!ok)
                continue;

            auto it = lessonById.find(lessonId);
            if (it == lessonById.end() || !it->second)
                continue;

            Lesson *lesson = it->second;

            usedTeachers.insert(lesson->teacherId());

            if (!lesson->classes().isEmpty())
                usedClasses.insert(lesson->classes()[0]);
        }

        QList<Lesson*> availableLessons;
        for (Lesson *lesson : lessons)
        {
            if (!lesson)
                continue;

            if (usedTeachers.count(lesson->teacherId()) != 0)
                continue;

            if (lesson->classes().isEmpty())
                continue;

            if (usedClasses.count(lesson->classes()[0]) != 0)
                continue;

            availableLessons.push_back(lesson);
        }

        int freeCells = 0;
        for (int column = 0; column < m_roomCount; ++column)
        {
            const LessonAssignment &cell = m_cells[cellIndex(row, column)];
            if (cell.lessonId.isEmpty() && cell.lessonName.isEmpty())
                ++freeCells;
        }

        if (freeCells <= 0 || availableLessons.isEmpty())
        {
            ++row;
            continue;
        }

        vector<pair<int, int>> match = kuhn(availableLessons);

        set<int> deleted;
        int placed = 0;
        int column = 0;

        for (int i = 0; placed < freeCells && i < (int)match.size(); ++i)
        {
            if (match[i].first == -1)
                continue;

            int lessonId = match[i].second;
            Lesson *lesson = nullptr;

            for (Lesson *les : availableLessons)
            {
                if (les && les->id() == lessonId)
                {
                    lesson = les;
                    break;
                }
            }

            if (!lesson)
                continue;

            while (column < m_roomCount)
            {
                const LessonAssignment &cell = m_cells[cellIndex(row, column)];
                if (cell.lessonId.isEmpty() && cell.lessonName.isEmpty())
                    break;

                ++column;
            }

            if (column >= m_roomCount)
                break;

            placeLesson(row, column, QString::number(lesson->id()), lesson->name());
            deleted.insert(lesson->id());

            ++placed;
            ++column;
        }

        QList<Lesson*> newLessons;
        for (Lesson *lesson : lessons)
        {
            if (!lesson)
                continue;

            if (!deleted.count(lesson->id()))
                newLessons.push_back(lesson);
        }

        lessons = newLessons;
        ++row;
    }

    m_suspendAutosave = false;
    saveToStorage();

    qDebug() << "end generating";
}
