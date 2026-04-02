#include "classmodel.h"

ClassModel::ClassModel(QObject *parent)
    : QAbstractListModel(parent),
    m_nextId(1)
{
}

int ClassModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_classes.count();
}

QVariant ClassModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_classes.count())
        return QVariant();

    SchoolClass *c = m_classes.at(index.row());
    if (!c)
        return QVariant();

    switch (role) {
    case IdRole:   return c->id();
    case NameRole: return c->name();
    default:       return QVariant();
    }
}

QHash<int, QByteArray> ClassModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[IdRole]   = "id";
    roles[NameRole] = "name";
    return roles;
}

void ClassModel::appendClass(const QString &name)
{
    if (name.trimmed().isEmpty())
        return;

    int id = m_nextId++;
    appendClassWithId(id, name);
}

void ClassModel::appendClassWithId(int id, const QString &name)
{
    if (name.trimmed().isEmpty())
        return;

    const int row = m_classes.count();
    beginInsertRows(QModelIndex(), row, row);

    SchoolClass *c = new SchoolClass(id, name, this);
    m_classes.append(c);

    endInsertRows();

    if (id >= m_nextId)
        m_nextId = id + 1;

    emit dataModified();
}

void ClassModel::removeAt(int index)
{
    if (index < 0 || index >= m_classes.count())
        return;

    beginRemoveRows(QModelIndex(), index, index);
    SchoolClass *c = m_classes.takeAt(index);
    endRemoveRows();

    if (c)
        c->deleteLater();

    emit dataModified();
}

int ClassModel::count() const
{
    return m_classes.count();
}

QObject *ClassModel::classAt(int index) const
{
    if (index < 0 || index >= m_classes.count())
        return nullptr;
    return m_classes.at(index);
}

QObject *ClassModel::classById(int id) const
{
    for (SchoolClass *tp : m_classes)
    {
        if (tp->id() == id)
            return tp;
    }
    return nullptr;
}
