#include "room.h"

Room::Room(QObject *parent) : QObject(parent) {}

Room::Room(const QString &name, const QString &size, QObject *parent) : QObject(parent), m_name(name), m_size(size) {}

QString Room::name() const
{
    return m_name;
}

void Room::setName(const QString &v)
{
    if (m_name == v)
        return;
    m_name = v;
    emit nameChanged();
}

QString Room::size() const
{
    return m_size;
}

void Room::setSize(const QString &v)
{
    if (m_size == v)
        return;
    m_size = v;
    emit sizeChanged();
}
