#include "school.h"

School::School(const QString &name, QObject *parent) : QObject(parent), m_id(QUuid::createUuid().toString()), m_name(name)
{
    m_rooms = new RoomModel(this);
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
}

QObject* School::roomsModel() const
{
    return m_rooms;
}
