#include "roommodel.h"

RoomModel::RoomModel(QObject *parent) : QAbstractListModel(parent) {}

int RoomModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_rooms.count();
}

QVariant RoomModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_rooms.count())
        return QVariant();
    Room *r = m_rooms.at(index.row());

    switch (role) {
    case NameRole: return r->name();
    case SizeRole: return r->size();
    default: return QVariant();
    }
}

QHash<int, QByteArray> RoomModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[NameRole] = "name";
    roles[SizeRole] = "size";
    return roles;
}

void RoomModel::appendRoom(const QString &name, const QString &size)
{
    if (name.isEmpty() || size.isEmpty())
        return;

    const int ind = m_rooms.count();
    beginInsertRows(QModelIndex(), ind, ind);
    Room *r = new Room(name, size, this);
    m_rooms.append(r);
    endInsertRows();
}

void RoomModel::removeAt(int index)
{
    if (index < 0 || index >= m_rooms.count())
        return;
    beginRemoveRows(QModelIndex(), index, index);
    Room *r = m_rooms.takeAt(index);
    endRemoveRows();
    if (r)
        r->deleteLater();
}

int RoomModel::count() const
{
    return m_rooms.count();
}
