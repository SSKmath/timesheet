#include "schoolmodel.h"

SchoolModel::SchoolModel(QObject *parent) : QAbstractListModel(parent) {}

int SchoolModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_schools.count();
}

QVariant SchoolModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_schools.count())
        return QVariant();
    School *s = m_schools.at(index.row());
    if (!s)
        return QVariant();

    switch (role) {
    case IdRole: return s->id();
    case NameRole: return s->name();
    case RoomsModelRole: return QVariant::fromValue(static_cast<QObject*>(s->roomsModel()));
    default: return QVariant();
    }
}

QHash<int, QByteArray> SchoolModel::roleNames() const
{
    QHash<int, QByteArray> r;
    r[IdRole] = "id";
    r[NameRole] = "name";
    r[RoomsModelRole] = "roomsModel";
    return r;
}

void SchoolModel::addSchoolFromVariant(const QString &name, const QVariantList &rooms)
{
    if (name.trimmed().isEmpty())
        return;

    beginInsertRows(QModelIndex(), m_schools.count(), m_schools.count());
    School *s = new School(name, this);
    RoomModel *rm = qobject_cast<RoomModel*>(s->roomsModel());
    if (rm)
    {
        for (const QVariant &v : rooms)
        {
            QVariantMap map = v.toMap();
            rm->appendRoom(map.value("name").toString(), map.value("size").toString());
        }
    }
    m_schools.append(s);
    endInsertRows();
    //TODO запись в файл
}

void SchoolModel::removeSchool(int index)
{
    if (index < 0 || index >= m_schools.count())
        return;

    beginRemoveRows(QModelIndex(), index, index);
    School *s = m_schools.takeAt(index);
    endRemoveRows();
    if (s)
        s->deleteLater();
}

QVariantMap SchoolModel::get(int index) const
{
    QVariantMap ans;
    if (index < 0 || index >= m_schools.count())
        return ans;

    School *s = m_schools.at(index);
    if (!s)
        return ans;
    ans["id"] = s->id();
    ans["name"] = s->name();
    QVariantList rooms;
    RoomModel *rm = qobject_cast<RoomModel*>(s->roomsModel());
    if (rm)
    {
        for (int i = 0; i < rm->rowCount(); ++i)
        {
            QModelIndex ind = rm->index(i);
            QVariantMap r;
            r["name"] = rm->data(ind, RoomModel::NameRole).toString();
            r["size"] = rm->data(ind, RoomModel::SizeRole).toString();
            rooms.append(r);
        }
    }
    ans["rooms"] = rooms;
    return ans;
}

int SchoolModel::count() const
{
    return m_schools.count();
}
