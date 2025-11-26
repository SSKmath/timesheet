#include "teacher.h"

Teacher::Teacher(QObject *parent) : QObject(parent) {}

Teacher::Teacher(const QString &surname, const QString &name, const QString &patronymic, const QString &subject, QObject *parent) : QObject(parent), m_surname(surname), m_name(name), m_patronymic(patronymic), m_subject(subject) {}

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


