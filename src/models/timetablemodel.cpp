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
#include <algorithm>
#include <functional>


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

bool TimetableModel::moveLessonToCell(int row, int column,
                                      const QString &lessonId,
                                      const QString &lessonName)
{
    if (!isValidCell(row, column))
        return false;

    const int targetPos = cellIndex(row, column);

    int oldPos = -1;
    for (int i = 0; i < m_cells.size(); ++i)
    {
        if (m_cells[i].lessonId == lessonId)
        {
            oldPos = i;
            break;
        }
    }

    if (oldPos >= 0 && oldPos != targetPos)
    {
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

    if (!m_suspendAutosave)
        saveToStorage();

    return true;
}

bool TimetableModel::placeLesson(int row, int column,
                                 const QString &lessonId,
                                 const QString &lessonName)
{
    return moveLessonToCell(row, column, lessonId, lessonName);
}

bool TimetableModel::setLessonAtCell(int row, int column,
                                     const QString &lessonId,
                                     const QString &lessonName)
{
    if (!isValidCell(row, column))
        return false;

    const int pos = cellIndex(row, column);

    m_cells[pos].lessonId = lessonId;
    m_cells[pos].lessonName = lessonName;

    emit dataChanged(index(row, column), index(row, column),
                     {LessonIdRole, LessonNameRole});

    ++m_lessonUsageRevision;
    emit lessonUsageChanged();

    // Во время генерации лучше не сохранять каждый раз.
    // Тогда generate() может ставить m_suspendAutosave = true.
    if (!m_suspendAutosave)
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





// ============================================================
// Вспомогательные структуры и функции
// ============================================================

struct LessonInfo
{
    Lesson *lesson = nullptr;
    int teacherId = -1;
    QVector<int> classIds;
};

bool isEmptyCell(const LessonAssignment &cell)
{
    return cell.lessonId.isEmpty() && cell.lessonName.isEmpty();
}

QVector<int> extractClassIds(Lesson *lesson)
{
    QVector<int> result;
    if (!lesson)
        return result;

    for (int classId : lesson->classes())
        result.push_back(classId);

    return result;
}

LessonInfo makeLessonInfo(Lesson *lesson)
{
    LessonInfo info;
    info.lesson = lesson;

    if (!lesson)
        return info;

    info.teacherId = lesson->teacherId();
    info.classIds = extractClassIds(lesson);
    return info;
}
bool conflictsWithUsed(const LessonInfo &info, const std::set<int> &usedTeachers, const std::set<int> &usedClasses)
{
    if (usedTeachers.count(info.teacherId) != 0)
        return true;

    for (int classId : info.classIds)
    {
        if (usedClasses.count(classId) != 0)
            return true;
    }

    return false;
}

void addResources(const LessonInfo &info, std::set<int> &usedTeachers, std::set<int> &usedClasses)
{
    usedTeachers.insert(info.teacherId);
    for (int classId : info.classIds)
        usedClasses.insert(classId);
}

void removeResources(const LessonInfo &info, std::set<int> &usedTeachers, std::set<int> &usedClasses)
{
    usedTeachers.erase(info.teacherId);
    for (int classId : info.classIds)
        usedClasses.erase(classId);
}

LessonBuckets splitLessons(const QList<Lesson*> &allLessons, const std::function<bool(Lesson*)> &isUsed)
{
    LessonBuckets buckets;

    for (Lesson *lesson : allLessons)
    {
        if (!lesson)
            continue;

        if (isUsed(lesson))
            continue;

        if (lesson->classes().isEmpty())
            continue;

        const bool isDouble = lesson->isDouble(); // если метод называется иначе — подправь тут
        const bool twoClasses = lesson->classes().size() >= 2;

        if (isDouble)
        {
            if (twoClasses)
                buckets.doubleTwoClass.push_back(lesson);
            else
                buckets.doubleOneClass.push_back(lesson);
        }
        else
        {
            if (twoClasses)
                buckets.singleTwoClass.push_back(lesson);
            else
                buckets.singleOneClass.push_back(lesson);
        }
    }

    return buckets;
}

// ------------------------------------------------------------
// Кун для уроков с одним классом:
// teacher -> class
// ------------------------------------------------------------
bool augment(int v, std::map<int, bool> &used, const std::map<int, std::vector<std::pair<int, int>>> &g,
                    std::map<int, std::pair<int, int>> &match)
{
    if (used[v])
        return false;

    used[v] = true;

    auto it = g.find(v);
    if (it == g.end())
        return false;

    for (const auto &[to, lessonIndex] : it->second)
    {
        auto mt = match.find(to);

        if (mt == match.end() || mt->second.first == -1 ||
            augment(mt->second.first, used, g, match))
        {
            match[to] = {v, lessonIndex};
            return true;
        }
    }

    return false;
}

QList<Lesson*> selectOneClassLessons(const QList<Lesson*> &candidates, int limit)
{
    if (limit <= 0 || candidates.isEmpty())
        return {};

    std::map<int, std::vector<std::pair<int, int>>> g;
    std::map<int, bool> used;

    for (int i = 0; i < candidates.size(); ++i)
    {
        Lesson *lesson = candidates[i];
        if (!lesson)
            continue;

        const auto classes = lesson->classes();
        if (classes.size() != 1)
            continue;

        const int teacherId = lesson->teacherId();
        const int classId = classes[0];

        used[teacherId] = false;
        g[teacherId].push_back({classId, i});
    }

    std::map<int, std::pair<int, int>> match;

    for (const auto &[teacherId, edges] : g)
    {
        Q_UNUSED(edges);

        if (augment(teacherId, used, g, match))
        {
            for (auto &[k, v] : used)
                v = false;
        }
    }

    QList<Lesson*> result;
    for (const auto &[classId, pair] : match)
    {
        Q_UNUSED(classId);

        if (pair.first == -1)
            continue;

        const int lessonIndex = pair.second;
        if (lessonIndex >= 0 && lessonIndex < candidates.size() && candidates[lessonIndex])
            result.push_back(candidates[lessonIndex]);

        if (result.size() >= limit)
            break;
    }

    return result;
}

// ------------------------------------------------------------
// Перебор с отсечениями для уроков с двумя классами
// ------------------------------------------------------------

void searchBestTwoClassSubset(const QVector<LessonInfo> &items, int idx, int limit, std::set<int> &usedTeachers, std::set<int> &usedClasses,
                              QVector<int> &current, QVector<int> &best)
{
    if (current.size() > best.size())
        best = current;

    if (current.size() == limit || idx >= items.size())
        return;

    // Отсечение: даже если взять все оставшиеся элементы,
    // текущий ответ уже нельзя улучшить.
    if (current.size() + (items.size() - idx) <= best.size())
        return;

    // Ветка 1: пропустить текущий элемент
    searchBestTwoClassSubset(items, idx + 1, limit, usedTeachers, usedClasses, current, best);

    // Ветка 2: взять текущий элемент, если нет конфликтов
    const LessonInfo &info = items[idx];
    if (conflictsWithUsed(info, usedTeachers, usedClasses))
        return;

    addResources(info, usedTeachers, usedClasses);
    current.push_back(idx);

    searchBestTwoClassSubset(items, idx + 1, limit, usedTeachers, usedClasses, current, best);

    current.pop_back();
    removeResources(info, usedTeachers, usedClasses);
}

QList<Lesson*> selectTwoClassLessons(const QList<Lesson*> &candidates, int limit)
{
    if (limit <= 0 || candidates.isEmpty())
        return {};

    QVector<LessonInfo> items;
    items.reserve(candidates.size());

    for (Lesson *lesson : candidates)
    {
        if (!lesson)
            continue;

        const auto classes = lesson->classes();
        if (classes.size() < 2)
            continue;

        items.push_back(makeLessonInfo(lesson));
    }

    // Небольшая эвристика: сначала более "тяжёлые" элементы.
    // Это не меняет точность, но обычно помогает отсечениям.
    std::sort(items.begin(), items.end(), [](const LessonInfo &a, const LessonInfo &b) {
        if (a.classIds.size() != b.classIds.size())
            return a.classIds.size() > b.classIds.size();
        return a.teacherId < b.teacherId;
    });

    std::set<int> usedTeachers;
    std::set<int> usedClasses;
    QVector<int> current;
    QVector<int> best;

    searchBestTwoClassSubset(items, 0, limit, usedTeachers, usedClasses, current, best);

    QList<Lesson*> result;
    for (int idx : best)
    {
        if (idx >= 0 && idx < items.size() && items[idx].lesson)
            result.push_back(items[idx].lesson);
    }

    return result;
}

std::set<int> occupiedTeachersInRows(const std::map<int, Lesson*> &lessonById, const QVector<LessonAssignment> &cells,
                                            int roomCount, int row1, int row2)
{
    std::set<int> usedTeachers;

    for (int row : {row1, row2})
    {
        for (int column = 0; column < roomCount; ++column)
        {
            const LessonAssignment &cell = cells[row * roomCount + column];
            if (isEmptyCell(cell))
                continue;

            bool ok = false;
            const int lessonId = cell.lessonId.toInt(&ok);
            if (!ok)
                continue;

            auto it = lessonById.find(lessonId);
            if (it == lessonById.end() || !it->second)
                continue;

            usedTeachers.insert(it->second->teacherId());
        }
    }

    return usedTeachers;
}

std::set<int> occupiedClassesInRows(const std::map<int, Lesson*> &lessonById, const QVector<LessonAssignment> &cells,
                                           int roomCount, int row1, int row2)
{
    std::set<int> usedClasses;

    for (int row : {row1, row2})
    {
        for (int column = 0; column < roomCount; ++column)
        {
            const LessonAssignment &cell = cells[row * roomCount + column];
            if (isEmptyCell(cell))
                continue;

            bool ok = false;
            const int lessonId = cell.lessonId.toInt(&ok);
            if (!ok)
                continue;

            auto it = lessonById.find(lessonId);
            if (it == lessonById.end() || !it->second)
                continue;

            const auto classes = it->second->classes();
            for (int classId : classes)
                usedClasses.insert(classId);
        }
    }

    return usedClasses;
}

std::set<int> occupiedTeachersInRow(const std::map<int, Lesson*> &lessonById, const QVector<LessonAssignment> &cells,
                                           int roomCount, int row)
{
    return occupiedTeachersInRows(lessonById, cells, roomCount, row, row);
}

std::set<int> occupiedClassesInRow(const std::map<int, Lesson*> &lessonById,
                                          const QVector<LessonAssignment> &cells,
                                          int roomCount,
                                          int row)
{
    return occupiedClassesInRows(lessonById, cells, roomCount, row, row);
}

// ============================================================
// Методы TimetableModel
// ============================================================

void TimetableModel::collectOccupiedResourcesForRow(int row,
                                                    const std::map<int, Lesson*> &lessonById,
                                                    std::set<int> &usedTeachers,
                                                    std::set<int> &usedClasses) const
{
    for (int column = 0; column < m_roomCount; ++column)
    {
        const LessonAssignment &cell = m_cells[cellIndex(row, column)];
        if (isEmptyCell(cell))
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

        for (int classId : lesson->classes())
            usedClasses.insert(classId);
    }
}

void TimetableModel::collectOccupiedResourcesForRows(int row1,
                                                     int row2,
                                                     const std::map<int, Lesson*> &lessonById,
                                                     std::set<int> &usedTeachers,
                                                     std::set<int> &usedClasses) const
{
    collectOccupiedResourcesForRow(row1, lessonById, usedTeachers, usedClasses);
    collectOccupiedResourcesForRow(row2, lessonById, usedTeachers, usedClasses);
}

std::vector<int> TimetableModel::freeColumnsForRow(int row) const
{
    std::vector<int> result;
    for (int column = 0; column < m_roomCount; ++column)
    {
        const LessonAssignment &cell = m_cells[cellIndex(row, column)];
        if (isEmptyCell(cell))
            result.push_back(column);
    }
    return result;
}

std::vector<int> TimetableModel::freeColumnsForRows(int row1, int row2) const
{
    std::vector<int> result;
    for (int column = 0; column < m_roomCount; ++column)
    {
        const LessonAssignment &cell1 = m_cells[cellIndex(row1, column)];
        const LessonAssignment &cell2 = m_cells[cellIndex(row2, column)];

        if (isEmptyCell(cell1) && isEmptyCell(cell2))
            result.push_back(column);
    }
    return result;
}

void TimetableModel::placeLessonInTwoRows(int row1, int row2, int column, Lesson *lesson)
{
    if (!lesson)
        return;

    const QString lessonId = QString::number(lesson->id());
    const QString lessonName = lesson->name();

    setLessonAtCell(row1, column, lessonId, lessonName);
    setLessonAtCell(row2, column, lessonId, lessonName);
}

void TimetableModel::placeLessonInRow(int row, int column, Lesson *lesson)
{
    if (!lesson)
        return;

    placeLesson(row, column, QString::number(lesson->id()), lesson->name());
}

void TimetableModel::generateDoubleLessons(LessonBuckets &buckets,
                                           const std::map<int, Lesson*> &lessonById)
{
    for (int row = 0; row + 1 < m_slotCount && (!buckets.doubleOneClass.isEmpty() || !buckets.doubleTwoClass.isEmpty()); row += 2)
    {
        const int nextRow = row + 1;

        std::set<int> usedTeachers;
        std::set<int> usedClasses;
        collectOccupiedResourcesForRows(row, nextRow, lessonById, usedTeachers, usedClasses);

        // Сначала двойные уроки с двумя классами
        {
            const auto freeColumns = freeColumnsForRows(row, nextRow);
            if (!freeColumns.empty())
            {
                QList<Lesson*> available;
                for (Lesson *lesson : buckets.doubleTwoClass)
                {
                    if (!lesson)
                        continue;

                    LessonInfo info = makeLessonInfo(lesson);
                    if (!conflictsWithUsed(info, usedTeachers, usedClasses))
                        available.push_back(lesson);
                }

                QList<Lesson*> selected = selectTwoClassLessons(available, (int)freeColumns.size());

                std::set<int> placedIds;
                int columnIndex = 0;

                for (Lesson *lesson : selected)
                {
                    if (!lesson)
                        continue;

                    while (columnIndex < (int)freeColumns.size())
                    {
                        const int column = freeColumns[columnIndex];
                        const LessonAssignment &cell1 = m_cells[cellIndex(row, column)];
                        const LessonAssignment &cell2 = m_cells[cellIndex(nextRow, column)];

                        if (isEmptyCell(cell1) && isEmptyCell(cell2))
                        {
                            placeLessonInTwoRows(row, nextRow, column, lesson);
                            placedIds.insert(lesson->id());

                            LessonInfo info = makeLessonInfo(lesson);
                            addResources(info, usedTeachers, usedClasses);

                            ++columnIndex;
                            break;
                        }

                        ++columnIndex;
                    }
                }

                QList<Lesson*> next;
                for (Lesson *lesson : buckets.doubleTwoClass)
                {
                    if (!lesson || placedIds.count(lesson->id()) != 0)
                        continue;
                    next.push_back(lesson);
                }
                buckets.doubleTwoClass = next;
            }
        }

        // Затем двойные уроки с одним классом
        {
            const auto freeColumns = freeColumnsForRows(row, nextRow);
            if (!freeColumns.empty())
            {
                QList<Lesson*> available;
                for (Lesson *lesson : buckets.doubleOneClass)
                {
                    if (!lesson)
                        continue;

                    LessonInfo info = makeLessonInfo(lesson);
                    if (!conflictsWithUsed(info, usedTeachers, usedClasses))
                        available.push_back(lesson);
                }

                QList<Lesson*> selected = selectOneClassLessons(available, (int)freeColumns.size());

                std::set<int> placedIds;
                int columnIndex = 0;

                for (Lesson *lesson : selected)
                {
                    if (!lesson)
                        continue;

                    while (columnIndex < (int)freeColumns.size())
                    {
                        const int column = freeColumns[columnIndex];
                        const LessonAssignment &cell1 = m_cells[cellIndex(row, column)];
                        const LessonAssignment &cell2 = m_cells[cellIndex(nextRow, column)];

                        if (isEmptyCell(cell1) && isEmptyCell(cell2))
                        {
                            placeLessonInTwoRows(row, nextRow, column, lesson);
                            placedIds.insert(lesson->id());

                            LessonInfo info = makeLessonInfo(lesson);
                            addResources(info, usedTeachers, usedClasses);

                            ++columnIndex;
                            break;
                        }

                        ++columnIndex;
                    }
                }

                QList<Lesson*> next;
                for (Lesson *lesson : buckets.doubleOneClass)
                {
                    if (!lesson || placedIds.count(lesson->id()) != 0)
                        continue;
                    next.push_back(lesson);
                }
                buckets.doubleOneClass = next;
            }
        }
    }
}

void TimetableModel::generateSingleLessons(LessonBuckets &buckets,
                                           const std::map<int, Lesson*> &lessonById)
{
    for (int row = 0; row < m_slotCount && (!buckets.singleOneClass.isEmpty() || !buckets.singleTwoClass.isEmpty()); ++row)
    {
        std::set<int> usedTeachers;
        std::set<int> usedClasses;
        collectOccupiedResourcesForRow(row, lessonById, usedTeachers, usedClasses);

        // Сначала одинарные уроки с двумя классами
        {
            const auto freeColumns = freeColumnsForRow(row);
            if (!freeColumns.empty())
            {
                QList<Lesson*> available;
                for (Lesson *lesson : buckets.singleTwoClass)
                {
                    if (!lesson)
                        continue;

                    LessonInfo info = makeLessonInfo(lesson);
                    if (!conflictsWithUsed(info, usedTeachers, usedClasses))
                        available.push_back(lesson);
                }

                QList<Lesson*> selected = selectTwoClassLessons(available, (int)freeColumns.size());

                std::set<int> placedIds;
                int columnIndex = 0;

                for (Lesson *lesson : selected)
                {
                    if (!lesson)
                        continue;

                    while (columnIndex < (int)freeColumns.size())
                    {
                        const int column = freeColumns[columnIndex];
                        const LessonAssignment &cell = m_cells[cellIndex(row, column)];

                        if (isEmptyCell(cell))
                        {
                            placeLessonInRow(row, column, lesson);
                            placedIds.insert(lesson->id());

                            LessonInfo info = makeLessonInfo(lesson);
                            addResources(info, usedTeachers, usedClasses);

                            ++columnIndex;
                            break;
                        }

                        ++columnIndex;
                    }
                }

                QList<Lesson*> next;
                for (Lesson *lesson : buckets.singleTwoClass)
                {
                    if (!lesson || placedIds.count(lesson->id()) != 0)
                        continue;
                    next.push_back(lesson);
                }
                buckets.singleTwoClass = next;
            }
        }

        // Затем одинарные уроки с одним классом
        {
            const auto freeColumns = freeColumnsForRow(row);
            if (!freeColumns.empty())
            {
                QList<Lesson*> available;
                for (Lesson *lesson : buckets.singleOneClass)
                {
                    if (!lesson)
                        continue;

                    LessonInfo info = makeLessonInfo(lesson);
                    if (!conflictsWithUsed(info, usedTeachers, usedClasses))
                        available.push_back(lesson);
                }

                QList<Lesson*> selected = selectOneClassLessons(available, (int)freeColumns.size());

                std::set<int> placedIds;
                int columnIndex = 0;

                for (Lesson *lesson : selected)
                {
                    if (!lesson)
                        continue;

                    while (columnIndex < (int)freeColumns.size())
                    {
                        const int column = freeColumns[columnIndex];
                        const LessonAssignment &cell = m_cells[cellIndex(row, column)];

                        if (isEmptyCell(cell))
                        {
                            placeLessonInRow(row, column, lesson);
                            placedIds.insert(lesson->id());

                            LessonInfo info = makeLessonInfo(lesson);
                            addResources(info, usedTeachers, usedClasses);

                            ++columnIndex;
                            break;
                        }

                        ++columnIndex;
                    }
                }

                QList<Lesson*> next;
                for (Lesson *lesson : buckets.singleOneClass)
                {
                    if (!lesson || placedIds.count(lesson->id()) != 0)
                        continue;
                    next.push_back(lesson);
                }
                buckets.singleOneClass = next;
            }
        }
    }
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

    const QList<Lesson*> allLessons = lessonModel->lessons();

    std::map<int, Lesson*> lessonById;
    for (Lesson *lesson : allLessons)
    {
        if (!lesson)
            continue;
        lessonById[lesson->id()] = lesson;
    }

    auto isUsed = [this](Lesson *lesson) -> bool {
        return lesson && isLessonUsed(QString::number(lesson->id()));
    };

    LessonBuckets buckets = splitLessons(allLessons, isUsed);

    // Сначала двойные уроки, потом одинарные
    generateDoubleLessons(buckets, lessonById);
    generateSingleLessons(buckets, lessonById);

    m_suspendAutosave = false;
    saveToStorage();

    qDebug() << "end generating";
}
