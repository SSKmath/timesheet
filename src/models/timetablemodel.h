#ifndef TIMETABLEMODEL_H
#define TIMETABLEMODEL_H

#include <QAbstractTableModel>
#include <QList>
#include <QString>
#include <QMetaObject>
#include "lesson.h"

struct LessonBuckets
{
    QList<Lesson*> doubleOneClass;
    QList<Lesson*> doubleTwoClass;
    QList<Lesson*> singleOneClass;
    QList<Lesson*> singleTwoClass;
};

struct LessonAssignment {
    QString lessonId;
    QString lessonName;
};

static constexpr double kMaxDoubleSharePerPair = 4.0 / 7.0;

class TimetableModel : public QAbstractTableModel
{
    Q_OBJECT
    Q_PROPERTY(int lessonUsageRevision READ lessonUsageRevision NOTIFY lessonUsageChanged)

public:
    explicit TimetableModel(QObject *parent = nullptr);

    enum Roles {
        LessonIdRole = Qt::UserRole + 1,
        LessonNameRole
    };
    Q_ENUM(Roles)

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QHash<int, QByteArray> roleNames() const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    Q_INVOKABLE void setRoomCount(int count);
    Q_INVOKABLE void setSlotCount(int count);
    Q_INVOKABLE void setRoomModel(QObject *roomModel);
    Q_INVOKABLE void setLessonModel(QObject *lessonModel);
    Q_INVOKABLE void setTeacherModel(QObject *teacherModel);

    bool moveLessonToCell(int row, int column, const QString &lessonId, const QString &lessonName);

    Q_INVOKABLE bool placeLesson(int row, int column, const QString &lessonId, const QString &lessonName);

    bool setLessonAtCell(int row, int column, const QString &lessonId, const QString &lessonName);

    Q_INVOKABLE bool clearLesson(int row, int column);
    Q_INVOKABLE bool clearAllLessons();

    Q_INVOKABLE bool isLessonUsed(const QString &lessonId) const;

    void collectOccupiedResourcesForRow(int row, const std::map<int, Lesson*> &lessonById, std::set<int> &usedTeachers, std::set<int> &usedClasses) const;
    void collectOccupiedResourcesForRows(int row1, int row2, const std::map<int, Lesson*> &lessonById, std::set<int> &usedTeachers, std::set<int> &usedClasses) const;
    std::vector<int> freeColumnsForRow(int row) const;
    std::vector<int> freeColumnsForRows(int row1, int row2) const;
    void placeLessonInTwoRows(int row1, int row2, int column, Lesson *lesson);
    void placeLessonInRow(int row, int column, Lesson *lesson);
    void generateDoubleLessons(LessonBuckets &buckets, const std::map<int, Lesson*> &lessonById);
    void generateSingleLessons(LessonBuckets &buckets, const std::map<int, Lesson*> &lessonById);


    Q_INVOKABLE void generate();

    int lessonUsageRevision() const { return m_lessonUsageRevision; }

signals:
    void lessonUsageChanged();

private:
    bool isValidCell(int row, int column) const;
    int cellIndex(int row, int column) const;

    void saveToStorage() const;
    void tryLoadFromStorage();
    QString currentSchoolId() const;
    QString baseStoragePath() const;
    QString schoolTimesheetDirPath(const QString &schoolId) const;
    QString autosaveFilePath(const QString &schoolId) const;
    bool ensureSchoolTimesheetDir(const QString &schoolId) const;

    bool teacherCanWorkOnDay(int teacherId, int dayIndex) const;
    int dayIndexForRow(int row) const;

    int m_roomCount;
    int m_slotCount;
    QList<LessonAssignment> m_cells;
    QObject *m_roomModel;
    QObject *m_lessonModel;
    QObject *m_teacherModel;

    int m_lessonUsageRevision = 0;

    QString m_loadedSignature;
    bool m_loadingFromStorage = false;
    bool m_suspendAutosave = false;
    QString m_loadedSchoolId;

    QMetaObject::Connection m_roomRowsInsertedConnection;
    QMetaObject::Connection m_roomRowsRemovedConnection;
    QMetaObject::Connection m_roomModelResetConnection;
    QMetaObject::Connection m_roomDataChangedConnection;
};

#endif // TIMETABLEMODEL_H
