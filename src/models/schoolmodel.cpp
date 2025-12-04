#include "schoolmodel.h"

SchoolModel::SchoolModel(QObject *parent) : QAbstractListModel(parent)
{
    m_storage = new SchoolStorage(this);
    QList<QVariantMap> all = m_storage->loadAllSchools();
    beginResetModel();
    for (const QVariantMap &m : std::as_const(all))
    {
        School *s = new School(m.value("name").toString(), this);

        QVariantList rooms = m.value("rooms").toList();
        RoomModel *rm = qobject_cast<RoomModel*>(s->roomsModel());
        for (const QVariant &rv : std::as_const(rooms))
        {
            QVariantMap r = rv.toMap();
            rm->appendRoom(r.value("name").toString(), r.value("size").toString());
        }

        QVariantList teachers = m.value("teachers").toList();
        TeacherModel *tm = qobject_cast<TeacherModel*>(s->teachersModel());
        for (const QVariant &tv : std::as_const(teachers))
        {
            QVariantMap t = tv.toMap();

            const QVariantList daysVar = t.value("workingDays").toList();
            QList<bool> days;
            for (const QVariant &dv : daysVar)
                days.append(dv.toBool());

            tm->appendTeacher(
                t.value("surname").toString(),
                t.value("name").toString(),
                t.value("patronymic").toString(),
                t.value("subject").toString(),
                days
                );
        }
        m_schools.append(s);
    }
    endResetModel();
}

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
    case IdRole:            return s->id();
    case NameRole:          return s->name();
    case RoomsModelRole:    return QVariant::fromValue(static_cast<QObject*>(s->roomsModel()));
    case TeachersModelRole: return QVariant::fromValue(static_cast<QObject*>(s->teachersModel()));
    default: return QVariant();
    }
}

QHash<int, QByteArray> SchoolModel::roleNames() const
{
    QHash<int, QByteArray> role;
    role[IdRole]            = "id";
    role[NameRole]          = "name";
    role[RoomsModelRole]    = "roomsModel";
    role[TeachersModelRole] = "teachersModel";
    return role;
}

void SchoolModel::addSchoolFromVariant(const QString &name, const QVariantList &rooms, const QVariantList &teachers)
{
    if (name.trimmed().isEmpty())
        return;

    beginInsertRows(QModelIndex(), m_schools.count(), m_schools.count());
    School *s = new School(name, this);

    RoomModel *rm = qobject_cast<RoomModel*>(s->roomsModel());
    if (rm)
    {
        for (const QVariant &v : std::as_const(rooms))
        {
            QVariantMap map = v.toMap();
            rm->appendRoom(map.value("name").toString(), map.value("size").toString());
        }
    }

    TeacherModel *tm = qobject_cast<TeacherModel*>(s->teachersModel());
    if (tm)
    {
        for (const QVariant &v : std::as_const(teachers))
        {
            QVariantMap t = v.toMap();

            const QVariantList daysVar = t.value("workingDays").toList();
            QList<bool> days;
            for (const QVariant &dv : daysVar)
                days.append(dv.toBool());

            tm->appendTeacher(
                t.value("surname").toString(),
                t.value("name").toString(),
                t.value("patronymic").toString(),
                t.value("subject").toString(),
                days
                );
        }
    }

    m_schools.append(s);
    endInsertRows();

    if (m_storage)
    {
        bool ok = m_storage->saveSchool(s);
        if (!ok)
            qWarning() << "Не удалось сохранить школу в файл";
    }
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

    TeacherModel *tm = qobject_cast<TeacherModel*>(s->teachersModel());
    QVariantList teachers;
    if (tm)
    {
        for (int i = 0; i < tm->rowCount(); ++i)
        {
            QModelIndex ind = tm->index(i);
            QVariantMap t;
            t["surname"]     = tm->data(ind, TeacherModel::SurnameRole).toString();
            t["name"]        = tm->data(ind, TeacherModel::NameRole).toString();
            t["patronymic"]  = tm->data(ind, TeacherModel::PatronymicRole).toString();
            t["subject"]     = tm->data(ind, TeacherModel::SubjectRole).toString();
            t["workingDays"] = tm->data(ind, TeacherModel::WorkingDaysRole);
            teachers.append(t);
        }
    }
    ans["teachers"] = teachers;
    return ans;
}

int SchoolModel::count() const
{
    return m_schools.count();
}
