#include "schoolstorage.h"
#include <QStandardPaths>
#include <QDir>
#include <QSaveFile>
#include <QJsonObject>
#include <QJsonArray>
#include "../models/school.h"

SchoolStorage::SchoolStorage(QObject *parent) : QObject(parent)
{
    QString base = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir(base);
    if (!dir.exists())
        dir.mkpath(".");
    if (!dir.exists("schools"))
        dir.mkdir("schools");
    m_dir = dir.filePath("schools");
    qDebug() << "Школы хранятся в" << m_dir;
}

QString SchoolStorage::storageDirectory() const
{
    return m_dir;
}

bool SchoolStorage::writeJsonToFile(const QString &path, const QByteArray &json)
{
    QSaveFile file(path);
    if (!file.open(QIODevice::WriteOnly))
    {
        qWarning() << "Не удалось открыть для записи" << path;
        return false;
    }
    qint64 written = file.write(json);
    if (written == -1)
    {
        qWarning() << "Ошибка записи в" << path;
        file.cancelWriting();
        return false;
    }
    if (!file.commit())
    {
        qWarning() << "Не удалось зафиксировать файл" << path;
        return false;
    }
    return true;
}

bool SchoolStorage::saveSchool(const QVariantMap &schoolData)
{
    QString id = schoolData.value("id").toString();
    if (id.isEmpty())
    {
        qWarning() << "saveSchool: пустой id";
        return false;
    }
    QJsonObject obj;
    obj["id"] = id;
    obj["name"] = schoolData.value("name").toString();

    QJsonArray roomsArr;
    QVariantList rooms = schoolData.value("rooms").toList();
    for (const QVariant &rv : std::as_const(rooms))
    {
        QVariantMap rmap = rv.toMap();
        QJsonObject ro;
        ro["name"] = rmap.value("name").toString();
        ro["size"] = rmap.value("size").toString();
        roomsArr.append(ro);
    }
    obj["rooms"] = roomsArr;

    QJsonArray teachersArr;
    QVariantList teachers = schoolData.value("teachers").toList();
    for (const QVariant &tv : std::as_const(teachers))
    {
        QVariantMap tmap = tv.toMap();
        QJsonObject to;
        to["id"]         = tmap.value("id").toInt();
        to["surname"]    = tmap.value("surname").toString();
        to["name"]       = tmap.value("name").toString();
        to["patronymic"] = tmap.value("patronymic").toString();
        to["subject"]    = tmap.value("subject").toString();

        QJsonArray daysArr;
        QVariantList daysList = tmap.value("workingDays").toList();
        for (const QVariant &dv : std::as_const(daysList))
            daysArr.append(dv.toBool());

        to["workingDays"] = daysArr;
        teachersArr.append(to);
    }
    obj["teachers"] = teachersArr;

    QJsonDocument doc(obj);
    QString path = QDir(m_dir).filePath(id + ".json");
    return writeJsonToFile(path, doc.toJson(QJsonDocument::Indented));
}

bool SchoolStorage::saveSchool(School *school)
{
    if (!school)
        return false;
    QVariantMap m;
    m["id"] = school->id();
    m["name"] = school->name();

    QVariantList rooms;
    RoomModel *rm = qobject_cast<RoomModel*>(school->roomsModel());
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
    m["rooms"] = rooms;

    QVariantList teachers;
    TeacherModel *tm = qobject_cast<TeacherModel*>(school->teachersModel());
    if(tm)
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
    m["teachers"] = teachers;

    return saveSchool(m);
}

QVariantMap SchoolStorage::loadSchool(const QString &id) const
{
    QVariantMap ans;
    QString path = QDir(m_dir).filePath(id + ".json");
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qWarning() << "Не открывается файл школы" << path;
        return ans;
    }
    QByteArray data = f.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject())
        return ans;
    QJsonObject o = doc.object();
    ans["id"] = o.value("id").toString();
    ans["name"] = o.value("name").toString();

    QVariantList rooms;
    QJsonArray ra = o.value("rooms").toArray();
    for (const QJsonValue &v : std::as_const(ra))
    {
        QJsonObject ro = v.toObject();
        QVariantMap rm;
        rm["name"] = ro.value("name").toString();
        rm["size"] = ro.value("size").toString();
        rooms.append(rm);
    }
    ans["rooms"] = rooms;

    QVariantList teachers;
    QJsonArray ta = o.value("teachers").toArray();
    for (const QJsonValue &v : std::as_const(ta))
    {
        QJsonObject to = v.toObject();
        QVariantMap tm;
        tm["id"]         = to.value("id").toInt();
        tm["surname"]    = to.value("surname").toString();
        tm["name"]       = to.value("name").toString();
        tm["patronymic"] = to.value("patronymic").toString();
        tm["subject"]    = to.value("subject").toString();

        QVariantList days;
        QJsonArray da = to.value("workingDays").toArray();
        for (const QJsonValue &dv : std::as_const(da))
            days.append(dv.toBool());
        tm["workingDays"] = days;
        teachers.append(tm);
    }
    ans["teachers"] = teachers;

    return ans;
}

QList<QVariantMap> SchoolStorage::loadAllSchools() const
{
    QList<QVariantMap> ans;
    QDir dir(m_dir);
    QStringList files = dir.entryList(QStringList() << "*.json", QDir::Files, QDir::Name);
    for (const QString &fname : std::as_const(files))
    {
        QString path = dir.filePath(fname);
        QFile f(path);
        if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
            continue;
        QByteArray data = f.readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        if (!doc.isObject())
            continue;
        QJsonObject o = doc.object();
        QVariantMap m;
        m["id"] = o.value("id").toString();
        m["name"] = o.value("name").toString();

        QVariantList rooms;
        for (const QJsonValue &rv : o.value("rooms").toArray())
        {
            QJsonObject ro = rv.toObject();
            QVariantMap r;
            r["name"] = ro.value("name").toString();
            r["size"] = ro.value("size").toString();
            rooms.append(r);
        }
        m["rooms"] = rooms;

        QVariantList teachers;
        QJsonArray ta = o.value("teachers").toArray();
        for (const QJsonValue &v : std::as_const(ta))
        {
            QJsonObject to = v.toObject();
            QVariantMap tm;
            tm["id"]         = to.value("id").toInt();
            tm["surname"]    = to.value("surname").toString();
            tm["name"]       = to.value("name").toString();
            tm["patronymic"] = to.value("patronymic").toString();
            tm["subject"]    = to.value("subject").toString();

            QVariantList days;
            QJsonArray da = to.value("workingDays").toArray();
            for (const QJsonValue &dv : std::as_const(da))
                days.append(dv.toBool());
            tm["workingDays"] = days;
            teachers.append(tm);
        }
        m["teachers"] = teachers;

        ans.append(m);
    }
    return ans;
}

bool SchoolStorage::removeSchool(const QString &id)
{
    QString path = QDir(m_dir).filePath(id + ".json");
    QFile f(path);
    if (f.exists())
        return f.remove();
    return true;
}
