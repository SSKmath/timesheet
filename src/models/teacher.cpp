#include "teacher.h"

Teacher::Teacher(QObject *parent) : QObject(parent), m_workingDays(6, false) {}

Teacher::Teacher(const QString &surname, const QString &name, const QString &patronymic, const QString &subject, QObject *parent) : QObject(parent), m_surname(surname), m_name(name), m_patronymic(patronymic), m_subject(subject), m_workingDays(6, false) {}

QString Teacher::surname() const
{
    return m_surname;
}

void Teacher::setSurname(const QString &v)
{
    if (m_surname == v)
        return;
    m_surname = v;
    emit surnameChanged();
}

QString Teacher::name() const
{
    return m_name;
}

void Teacher::setName(const QString &v)
{
    if (m_name == v)
        return;
    m_name = v;
    emit nameChanged();
}

QString Teacher::patronymic() const
{
    return m_patronymic;
}

void Teacher::setPatronymic(const QString &v)
{
    if (m_patronymic == v)
        return;
    m_patronymic = v;
    emit patronymicChanged();
}

QString Teacher::subject() const
{
    return m_subject;
}

void Teacher::setSubject(const QString &v)
{
    if (m_subject == v)
        return;
    m_subject = v;
    emit subjectChanged();
}

QList<bool> Teacher::workingDays() const
{
    return m_workingDays;
}

void Teacher::setWorkingDays(const QList<bool> &days)
{
    if (days.size() != 6)
        return;

    if (m_workingDays == days)
        return;

    m_workingDays = days;
    emit workingDaysChanged();
}

bool Teacher::worksOnDay(int index) const
{
    if (index < 0 || index >= m_workingDays.size())
        return false;

    return m_workingDays[index];
}

void Teacher::setWorksOnDay(int index, bool v)
{
    if (index < 0 || index >= m_workingDays.size())
        return;

    if (m_workingDays[index] == v)
        return;

    m_workingDays[index] = v;
    emit workingDaysChanged();
}


