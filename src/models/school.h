#ifndef SCHOOL_H
#define SCHOOL_H

#include <QObject>
#include <QUuid>
#include "roommodel.h"
#include "teachermodel.h"
#include "lessonmodel.h"
#include "classmodel.h"
#include <QString>

class School : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString id READ id CONSTANT)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QObject* roomsModel READ roomsModel CONSTANT)
    Q_PROPERTY(QObject* teachersModel READ teachersModel CONSTANT)
    Q_PROPERTY(QObject* lessonsModel READ lessonsModel CONSTANT)
    Q_PROPERTY(QObject* classesModel READ classesModel CONSTANT)

public:
    explicit School(const QString &id, const QString &name, QObject *parent = nullptr);

    QString id() const;
    QString name() const;
    void setName(const QString &n);

    QObject* roomsModel() const;
    QObject* teachersModel() const;
    QObject* lessonsModel() const;
    QObject* classesModel() const;

signals:
    void nameChanged();
    void requestSave();

private slots:
    void saveToStorage();

private:
    QString m_id;
    QString m_name;
    RoomModel *m_rooms;
    TeacherModel *m_teachers;
    LessonModel *m_lessons;
    ClassModel  *m_classes;
};

#endif // SCHOOL_H
