#include "schoolmodel.h"
#include <utility>

SchoolModel::SchoolModel(QObject *parent) : QAbstractListModel(parent)
{
    m_storage = new SchoolStorage(this);
    QList<QVariantMap> all = m_storage->loadAllSchools();
    beginResetModel();
    for (const QVariantMap &m : std::as_const(all))
    {
        School *s = new School(m.value("id").toString(), m.value("name").toString(), this);

        QVariantList rooms = m.value("rooms").toList();
        RoomModel *rm = qobject_cast<RoomModel*>(s->roomsModel());
        for (const QVariant &rv : std::as_const(rooms))
        {
            QVariantMap r = rv.toMap();
            rm->appendRoom(r.value("name").toString(), r.value("size").toString());
        }

        QObject::connect(s, &School::requestSave, this, [this, s]() {
            if (m_storage)
            {
                bool ok = m_storage->saveSchool(s);
                if (!ok)
                    qWarning() << "Не удалось сохранить школу" << s->name();
                else
                    qDebug() << "автосохранение школы" << s->name();
            }
        });

        QVariantList teachers = m.value("teachers").toList();
        TeacherModel *tm = qobject_cast<TeacherModel*>(s->teachersModel());
        for (const QVariant &tv : std::as_const(teachers))
        {
            QVariantMap t = tv.toMap();

            int id = t.value("id").toInt();

            const QVariantList daysVar = t.value("workingDays").toList();
            QList<bool> days;
            for (const QVariant &dv : daysVar)
                days.append(dv.toBool());

            if(id > 0)
            {
            tm->appendTeacherWithId(id,
                              t.value("surname").toString(),
                              t.value("name").toString(),
                              t.value("patronymic").toString(),
                              t.value("subject").toString(),
                              days
                              );
            }
            else
            {
                tm->appendTeacher(t.value("surname").toString(),
                                  t.value("name").toString(),
                                  t.value("patronymic").toString(),
                                  t.value("subject").toString(),
                                  days
                                  );
            }
        }

        QVariantList classes = m.value("classes").toList();
        ClassModel *cm = qobject_cast<ClassModel*>(s->classesModel());
        for (const QVariant &cv : std::as_const(classes))
        {
            QVariantMap c = cv.toMap();
            int id = c.value("id").toInt();
            QString name = c.value("name").toString();

            if (id > 0)
                cm->appendClassWithId(id, name);
            else
                cm->appendClass(name);
        }

        QVariantList lessons = m.value("lessons").toList();
        LessonModel *lm = qobject_cast<LessonModel*>(s->lessonsModel());
        for (const QVariant &lv : std::as_const(lessons))
        {
            QVariantMap l = lv.toMap();

            int lid       = l.value("id").toInt();
            QString name  = l.value("name").toString();
            bool isDouble = l.value("isDouble").toBool();
            int teacherId = l.value("teacherId").toInt();
            int perWeek   = l.value("perWeek", 1).toInt();

            QVariantList classesVar = l.value("classes").toList();
            QList<int> classes;
            for (const QVariant &cv : std::as_const(classesVar))
            {
                if (cv.canConvert<QVariantMap>())
                {
                    QVariantMap cm = cv.toMap();
                    classes.append(cm.value("id").toInt());
                }
                else
                {
                    classes.append(cv.toInt());
                }
            }

            if (lid > 0)
                lm->appendLessonWithId(lid, name, isDouble, teacherId, perWeek, classes);
            else
                lm->appendLesson(name, isDouble, teacherId, perWeek, classes);
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
    case LessonsModelRole:  return QVariant::fromValue(static_cast<QObject*>(s->lessonsModel()));
    case ClassesModelRole:  return QVariant::fromValue(static_cast<QObject*>(s->classesModel()));
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
    role[LessonsModelRole]  = "lessonsModel";
    role[ClassesModelRole]  = "classesModel";
    return role;
}

void SchoolModel::addSchoolFromVariant(const QString &name, const QVariantList &rooms, const QVariantList &teachers)
{
    if (name.trimmed().isEmpty())
        return;

    beginInsertRows(QModelIndex(), m_schools.count(), m_schools.count());

    School *s = new School(QString(), name, this);

    RoomModel *rm = qobject_cast<RoomModel*>(s->roomsModel());
    if (rm)
    {
        for (const QVariant &v : std::as_const(rooms))
        {
            QVariantMap map = v.toMap();
            rm->appendRoom(map.value("name").toString(), map.value("size").toString());
        }
    }

    QObject::connect(s, &School::requestSave, this, [this, s]() {
        if (m_storage)
        {
            bool ok = m_storage->saveSchool(s);
            if (!ok)
                qWarning() << "Не удалось сохранить школу" << s->name();
            else
                qDebug() << "автосохранение школы" << s->name();
        }
    });

    TeacherModel *tm = qobject_cast<TeacherModel*>(s->teachersModel());
    if (tm)
    {
        for (const QVariant &v : std::as_const(teachers))
        {
            QVariantMap t = v.toMap();

            int id = t.value("id").toInt();

            const QVariantList daysVar = t.value("workingDays").toList();
            QList<bool> days;
            for (const QVariant &dv : daysVar)
                days.append(dv.toBool());

            if(id > 0)
            {
                tm->appendTeacherWithId(id,
                                  t.value("surname").toString(),
                                  t.value("name").toString(),
                                  t.value("patronymic").toString(),
                                  t.value("subject").toString(),
                                  days
                                  );
            }
            else
            {
                tm->appendTeacher(t.value("surname").toString(),
                                  t.value("name").toString(),
                                  t.value("patronymic").toString(),
                                  t.value("subject").toString(),
                                  days
                                  );
            }
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
            t["id"]          = tm->data(ind, TeacherModel::IdRole).toInt();
            t["surname"]     = tm->data(ind, TeacherModel::SurnameRole).toString();
            t["name"]        = tm->data(ind, TeacherModel::NameRole).toString();
            t["patronymic"]  = tm->data(ind, TeacherModel::PatronymicRole).toString();
            t["subject"]     = tm->data(ind, TeacherModel::SubjectRole).toString();
            t["workingDays"] = tm->data(ind, TeacherModel::WorkingDaysRole);
            teachers.append(t);
        }
    }
    ans["teachers"] = teachers;

    QVariantList classes;
    ClassModel *cm = qobject_cast<ClassModel*>(s->classesModel());
    if (cm)
    {
        for (int i = 0; i < cm->rowCount(); ++i)
        {
            QModelIndex ind = cm->index(i);
            QVariantMap c;
            c["id"]   = cm->data(ind, ClassModel::IdRole).toInt();
            c["name"] = cm->data(ind, ClassModel::NameRole).toString();
            classes.append(c);
        }
    }
    ans["classes"] = classes;

    QVariantList lessons;
    LessonModel *lm = qobject_cast<LessonModel*>(s->lessonsModel());
    if (lm)
    {
        for (int i = 0; i < lm->rowCount(); ++i)
        {
            QModelIndex ind = lm->index(i);
            QVariantMap l;
            l["id"]        = lm->data(ind, LessonModel::IdRole).toInt();
            l["name"]      = lm->data(ind, LessonModel::NameRole).toString();
            l["isDouble"]  = lm->data(ind, LessonModel::IsDoubleRole).toBool();
            l["teacherId"] = lm->data(ind, LessonModel::TeacherIdRole).toInt();
            l["perWeek"]   = lm->data(ind, LessonModel::PerWeekRole).toInt();

            QVariantList classesVar = lm->data(ind, LessonModel::ClassesRole).toList();
            QVariantList classesJson;
            for (const QVariant &cv : std::as_const(classesVar))
            {
                QVariantMap cm;
                cm["id"] = cv.toInt();
                classesJson.append(cm);
            }
            l["classes"] = classesJson;

            lessons.append(l);
        }
    }

    ans["lessons"] = lessons;
    return ans;
}

int SchoolModel::count() const
{
    return m_schools.count();
}

QObject *SchoolModel::roomModelAt(int index) const
{
    if (index < 0 || index >= m_schools.count())
        return nullptr;
    School *s = m_schools.at(index);
    return s ? static_cast<QObject*>(s->roomsModel()) : nullptr;
}

QObject *SchoolModel::teacherModelAt(int index) const
{
    if (index < 0 || index >= m_schools.count())
        return nullptr;
    School *s = m_schools.at(index);
    return s ? static_cast<QObject*>(s->teachersModel()) : nullptr;
}

QObject *SchoolModel::lessonModelAt(int index) const
{
    if (index < 0 || index >= m_schools.count())
        return nullptr;
    School *s = m_schools.at(index);
    return s ? static_cast<QObject*>(s->lessonsModel()) : nullptr;
}

QObject *SchoolModel::classModelAt(int index) const
{
    if (index < 0 || index >= m_schools.count())
        return nullptr;
    School *s = m_schools.at(index);
    return s ? static_cast<QObject*>(s->classesModel()) : nullptr;
}
