#include "lesson.h"

Lesson::Lesson(QObject *parent)
    : QObject(parent),
    m_id(-1),
    m_isDouble(false),
    m_teacherId(-1),
    m_perWeek(1)
{
}

Lesson::Lesson(int id,
               const QString &name,
               bool isDouble,
               int teacherId,
               int perWeek,
               const QList<int> &classes,
               QObject *parent)
    : QObject(parent),
    m_id(id),
    m_name(name),
    m_isDouble(isDouble),
    m_teacherId(teacherId),
    m_perWeek(perWeek),
    m_classes(classes)

{
}

int Lesson::id() const
{
    return m_id;
}

void Lesson::setId(int id)
{
    if (m_id == id)
        return;
    m_id = id;
    emit idChanged();
}

QString Lesson::name() const
{
    return m_name;
}

void Lesson::setName(const QString &name)
{
    if (m_name == name)
        return;
    m_name = name;
    emit nameChanged();
}

bool Lesson::isDouble() const
{
    return m_isDouble;
}

void Lesson::setIsDouble(bool v)
{
    if (m_isDouble == v)
        return;
    m_isDouble = v;
    emit isDoubleChanged();
}

int Lesson::teacherId() const
{
    return m_teacherId;
}

void Lesson::setTeacherId(int id)
{
    if (m_teacherId == id)
        return;
    m_teacherId = id;
    emit teacherIdChanged();
}

QList<int> Lesson::classes() const
{
    return m_classes;
}

void Lesson::setClasses(const QList<int> &c)
{
    if (m_classes == c)
        return;
    m_classes = c;
    emit classesChanged();
}

int Lesson::perWeek() const
{
    return m_perWeek;
}

void Lesson::setPerWeek(int v)
{
    if (m_perWeek == v)
        return;
    m_perWeek = v;
    emit perWeekChanged();
}
