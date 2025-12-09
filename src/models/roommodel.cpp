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

bool RoomModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    role += Qt::UserRole; //когда-нибудь: понять, почему qml передаёт считая с 1
    //qDebug() << "RoomsModel::setData row=" << index.row() << "value=" << value << "role=" << role << "SizeRole=" << SizeRole;

    if (!index.isValid() || index.row() < 0 || index.row() >= m_rooms.count())
        return false;
    Room *r = m_rooms.at(index.row());
    if (!r)
        return false;

    switch (role) {
    case NameRole:
        r->setName(value.toString());
        break;
    case SizeRole:
        r->setSize(value.toString());
        break;
    default:
        return false;
    }

    emit dataChanged(index, index, {role});
    emit dataModified();
    return true;
}

Qt::ItemFlags RoomModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;
    return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
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
    emit dataModified();
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
    emit dataModified();
}

int RoomModel::count() const
{
    return m_rooms.count();
}
