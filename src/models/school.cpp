#include "school.h"

School::School(const QString &id, const QString &name, QObject *parent) : QObject(parent), m_id(id.isEmpty() ? QUuid::createUuid().toString() : id), m_name(name)
{
    m_rooms = new RoomModel(this);
    QObject::connect(m_rooms, &RoomModel::dataModified, this, &School::saveToStorage);

    m_teachers = new TeacherModel(this);
    QObject::connect(m_teachers, &TeacherModel::dataModified, this, &School::saveToStorage);

    m_lessons = new LessonModel(this);
    QObject::connect(m_lessons, &LessonModel::dataModified, this, &School::saveToStorage);

    m_classes = new ClassModel(this);
    QObject::connect(m_classes, &ClassModel::dataModified, this, &School::saveToStorage);
}

QString School::id() const
{
    return m_id;
}

QString School::name() const
{
    return m_name;
}

void School::setName(const QString &n)
{
    if (m_name == n)
        return;
    m_name = n;
    emit nameChanged();
    emit requestSave();
}

QObject* School::roomsModel() const
{
    return m_rooms;
}

QObject* School::teachersModel() const
{
    return m_teachers;
}

QObject* School::lessonsModel() const
{
    return m_lessons;
}

QObject* School::classesModel() const
{
    return m_classes;
}

void School::saveToStorage()
{
    emit requestSave();
    qDebug() << "Школа" << m_name << "запрашивает сохранение";
}
