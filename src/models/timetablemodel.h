#ifndef TIMETABLEMODEL_H
#define TIMETABLEMODEL_H

#include <QAbstractTableModel>
#include <QList>
#include <QString>
#include <QMetaObject>

struct LessonAssignment {
    QString lessonId;
    QString lessonName;
};

class TimetableModel : public QAbstractTableModel
{
    Q_OBJECT

    // Эта "ревизия" нужна, чтобы QML понял:
    // "список использованных/неиспользованных уроков изменился, пересчитай bindings"
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

    // Главный метод: поставить урок в конкретную ячейку
    Q_INVOKABLE bool placeLesson(int row, int column,
                                 const QString &lessonId,
                                 const QString &lessonName);

    // Очистить ячейку
    Q_INVOKABLE bool clearLesson(int row, int column);

    // Проверить, используется ли урок уже в таблице
    Q_INVOKABLE bool isLessonUsed(const QString &lessonId) const;

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

    int m_roomCount;
    int m_slotCount;
    QList<LessonAssignment> m_cells;
    QObject *m_roomModel;
    QObject *m_lessonModel;

    int m_lessonUsageRevision = 0;

    QString m_loadedSignature;
    bool m_loadingFromStorage = false;
    bool m_suspendAutosave = false;

    QMetaObject::Connection m_roomRowsInsertedConnection;
    QMetaObject::Connection m_roomRowsRemovedConnection;
    QMetaObject::Connection m_roomModelResetConnection;
    QMetaObject::Connection m_roomDataChangedConnection;
};

#endif // TIMETABLEMODEL_H
