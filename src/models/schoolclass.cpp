#include "schoolclass.h"

SchoolClass::SchoolClass(int id,
                         const QString &name,
                         QObject *parent)
    : QObject(parent),
    m_id(id),
    m_name(name)
{
}

int SchoolClass::id() const
{
    return m_id;
}

QString SchoolClass::name() const
{
    return m_name;
}

void SchoolClass::setName(const QString &name)
{
    if (m_name == name)
        return;

    m_name = name;
    emit nameChanged();
}
