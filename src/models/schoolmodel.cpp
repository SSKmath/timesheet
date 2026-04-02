#include "schoolmodel.h"
#include <utility>
#include <QPdfWriter>
#include <QPainter>
#include <QStandardPaths>
#include <QDateTime>
#include <QDir>
#include <QUrl>
#include <algorithm>
#include <QRegularExpression>

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

QString SchoolModel::generateTimetablePdf(int index)
{
    if (index < 0 || index >= m_schools.count())
        return QString();

    School *s = m_schools.at(index);
    if (!s)
        return QString();

    RoomModel    *rm = qobject_cast<RoomModel*>(s->roomsModel());
    TeacherModel *tm = qobject_cast<TeacherModel*>(s->teachersModel());
    ClassModel   *cm = qobject_cast<ClassModel*>(s->classesModel());
    LessonModel  *lm = qobject_cast<LessonModel*>(s->lessonsModel());

    if (!cm || !lm)
        return QString();

    const int DAYS = 6;
    const int PERIODS = 8;

    QHash<int, int> classIdToIdx;
    QVector<int> classIds;
    QVector<QString> classNames;
    classIds.reserve(cm->rowCount());
    classNames.reserve(cm->rowCount());

    for (int i = 0; i < cm->rowCount(); ++i)
    {
        QModelIndex ind = cm->index(i);
        int cid = cm->data(ind, ClassModel::IdRole).toInt();
        QString cname = cm->data(ind, ClassModel::NameRole).toString();

        if (cid <= 0)
            continue;

        classIdToIdx[cid] = classIds.size();
        classIds.push_back(cid);
        classNames.push_back(cname);
    }

    const int C = classIds.size();
    if (C == 0)
        return QString();

    QVector<QVector<QVector<QString>>> tableText(
        C,
        QVector<QVector<QString>>(DAYS, QVector<QString>(PERIODS))
        );

    QVector<QVector<QVector<int>>> tableLesson(
        C,
        QVector<QVector<int>>(DAYS, QVector<int>(PERIODS, 0))
        );

    QVector<QVector<QHash<int, int>>> lessonCountByDay(
        C,
        QVector<QHash<int, int>>(DAYS)
        );

    QHash<int, QVector<bool>> teacherDays;
    QHash<int, QString> teacherShort;
    QHash<int, QVector<QVector<bool>>> teacherBusy;

    if (tm)
    {
        for (int i = 0; i < tm->rowCount(); ++i)
        {
            QModelIndex ind = tm->index(i);
            int tid = tm->data(ind, TeacherModel::IdRole).toInt();
            if (tid <= 0)
                continue;

            QString surname = tm->data(ind, TeacherModel::SurnameRole).toString();
            QString name = tm->data(ind, TeacherModel::NameRole).toString();

            QString sh = surname;
            if (!name.isEmpty())
                sh += " " + name.left(1) + ".";

            teacherShort[tid] = sh;

            QVector<bool> days(DAYS, true);
            QVariantList vdays = tm->data(ind, TeacherModel::WorkingDaysRole).toList();
            if (vdays.size() == DAYS)
            {
                for (int d = 0; d < DAYS; ++d)
                    days[d] = vdays[d].toBool();
            }
            teacherDays[tid] = days;

            teacherBusy[tid] = QVector<QVector<bool>>(DAYS, QVector<bool>(PERIODS, false));
        }
    }

    QVector<QString> roomNames;
    int R = 0;

    if (rm)
    {
        R = rm->rowCount();
        roomNames.reserve(R);

        for (int i = 0; i < R; ++i)
        {
            QModelIndex ind = rm->index(i);
            QString rname = rm->data(ind, RoomModel::NameRole).toString();
            if (rname.trimmed().isEmpty())
                rname = QString::number(i + 1);
            roomNames.push_back(rname);
        }
    }

    QVector<QVector<QVector<bool>>> roomBusy;
    if (R > 0)
    {
        roomBusy = QVector<QVector<QVector<bool>>>(
            R,
            QVector<QVector<bool>>(DAYS, QVector<bool>(PERIODS, false))
            );
    }

    struct Occ
    {
        int lessonId;
        QString name;
        int teacherId;
        QVector<int> classes;
        int len;
    };

    QVector<Occ> occs;

    for (int i = 0; i < lm->rowCount(); ++i)
    {
        QModelIndex ind = lm->index(i);

        int lessonId    = lm->data(ind, LessonModel::IdRole).toInt();
        QString lname   = lm->data(ind, LessonModel::NameRole).toString();
        bool isDouble   = lm->data(ind, LessonModel::IsDoubleRole).toBool();
        int teacherId   = lm->data(ind, LessonModel::TeacherIdRole).toInt();
        int perWeek     = lm->data(ind, LessonModel::PerWeekRole).toInt();
        QVariantList clsVar = lm->data(ind, LessonModel::ClassesRole).toList();

        if (lessonId <= 0 || lname.trimmed().isEmpty())
            continue;

        if (perWeek <= 0)
            perWeek = 1;

        QVector<int> clsIdx;
        for (const QVariant &v : std::as_const(clsVar))
        {
            int cid = v.toInt();
            if (classIdToIdx.contains(cid))
                clsIdx.push_back(classIdToIdx[cid]);
        }

        if (clsIdx.isEmpty())
            continue;

        Occ base;
        base.lessonId = lessonId;
        base.name = lname;
        base.teacherId = teacherId;
        base.classes = clsIdx;
        base.len = isDouble ? 2 : 1;

        for (int k = 0; k < perWeek; ++k)
            occs.push_back(base);
    }

    std::sort(occs.begin(), occs.end(), [](const Occ &a, const Occ &b) {
        if (a.classes.size() != b.classes.size())
            return a.classes.size() > b.classes.size();
        if (a.len != b.len)
            return a.len > b.len;
        if (a.teacherId != b.teacherId)
            return a.teacherId < b.teacherId;
        return a.name < b.name;
    });

    QVector<QString> unscheduled;

    auto teacherCanDay = [&](int tid, int day) -> bool {
        if (tid <= 0) return true;
        if (!teacherDays.contains(tid)) return true;
        return teacherDays[tid][day];
    };

    auto teacherIsBusy = [&](int tid, int day, int p) -> bool {
        if (tid <= 0) return false;
        if (!teacherBusy.contains(tid)) return false;
        return teacherBusy[tid][day][p];
    };

    auto setTeacherBusy = [&](int tid, int day, int p, bool v) {
        if (tid <= 0) return;
        if (!teacherBusy.contains(tid))
            teacherBusy[tid] = QVector<QVector<bool>>(DAYS, QVector<bool>(PERIODS, false));
        teacherBusy[tid][day][p] = v;
    };

    auto roomIsBusy = [&](int ridx, int day, int p) -> bool {
        if (R <= 0) return false;
        return roomBusy[ridx][day][p];
    };

    auto setRoomBusy = [&](int ridx, int day, int p, bool v) {
        if (R <= 0) return;
        roomBusy[ridx][day][p] = v;
    };

    auto findFreeRoom = [&](int day, int start, int len) -> int {
        if (R <= 0)
            return -1;

        for (int ri = 0; ri < R; ++ri)
        {
            bool ok = true;
            for (int k = 0; k < len; ++k)
            {
                if (roomIsBusy(ri, day, start + k))
                {
                    ok = false;
                    break;
                }
            }
            if (ok)
                return ri;
        }

        return -2;
    };

    auto scoreForClass = [&](int classIdx, int day, int start, const Occ &o) -> int {
        bool occ[PERIODS];
        int beforeCount = 0;

        for (int p = 0; p < PERIODS; ++p)
        {
            occ[p] = (tableLesson[classIdx][day][p] != 0);
            if (occ[p])
                beforeCount++;
        }

        bool touchesExisting = false;
        if (beforeCount > 0)
        {
            if (start > 0 && occ[start - 1])
                touchesExisting = true;
            if (start + o.len < PERIODS && occ[start + o.len])
                touchesExisting = true;
        }

        for (int k = 0; k < o.len; ++k)
            occ[start + k] = true;

        int first = -1;
        int last = -1;
        int filled = 0;

        for (int p = 0; p < PERIODS; ++p)
        {
            if (occ[p])
            {
                if (first == -1)
                    first = p;
                last = p;
                filled++;
            }
        }

        int holes = 0;
        if (filled > 0)
            holes = (last - first + 1) - filled;

        int score = 0;

        score += holes * 100;

        if (beforeCount > 0)
        {
            if (touchesExisting)
                score -= 25;
            else
                score += 25;
        }

        int sameToday = lessonCountByDay[classIdx][day].value(o.lessonId, 0);
        if (sameToday > 0)
            score += 250 + sameToday * 150;

        if (start > 0 && tableLesson[classIdx][day][start - 1] == o.lessonId)
            score += 150;
        if (start + o.len < PERIODS && tableLesson[classIdx][day][start + o.len] == o.lessonId)
            score += 150;

        if (last >= 0)
            score += last;

        return score;
    };

    for (const Occ &o : std::as_const(occs))
    {
        const int INF = 1000000000;
        int bestScore = INF;
        int bestDay = -1;
        int bestStart = -1;
        int bestRoom = -2;

        for (int day = 0; day < DAYS; ++day)
        {
            if (!teacherCanDay(o.teacherId, day))
                continue;

            for (int p = 0; p + o.len <= PERIODS; ++p)
            {
                bool ok = true;
                for (int k = 0; k < o.len; ++k)
                {
                    if (teacherIsBusy(o.teacherId, day, p + k))
                    {
                        ok = false;
                        break;
                    }
                }
                if (!ok)
                    continue;

                for (int ci : o.classes)
                {
                    for (int k = 0; k < o.len; ++k)
                    {
                        if (tableLesson[ci][day][p + k] != 0)
                        {
                            ok = false;
                            break;
                        }
                    }
                    if (!ok)
                        break;
                }
                if (!ok)
                    continue;

                int roomChoice = findFreeRoom(day, p, o.len);
                if (roomChoice == -2)
                    continue;

                int score = 0;
                for (int ci : o.classes)
                    score += scoreForClass(ci, day, p, o);

                score += day * 4 + p;

                if (score < bestScore)
                {
                    bestScore = score;
                    bestDay = day;
                    bestStart = p;
                    bestRoom = roomChoice;
                }
            }
        }

        if (bestDay == -1)
        {
            QString miss = o.name;
            if (!o.classes.isEmpty())
            {
                QStringList cls;
                for (int ci : o.classes)
                    cls << classNames[ci];
                miss += " (" + cls.join(", ") + ")";
            }
            unscheduled.push_back(miss);
            continue;
        }

        QString tshort = teacherShort.value(o.teacherId, QString());

        QString cellText = o.name;
        if (!tshort.isEmpty())
            cellText += "\n" + tshort;
        if (bestRoom >= 0)
            cellText += "\nкаб. " + roomNames[bestRoom];

        for (int ci : o.classes)
        {
            tableText[ci][bestDay][bestStart] = cellText;
            tableLesson[ci][bestDay][bestStart] = o.lessonId;

            if (o.len == 2)
            {
                tableText[ci][bestDay][bestStart + 1] = cellText;
                tableLesson[ci][bestDay][bestStart + 1] = o.lessonId;
            }

            lessonCountByDay[ci][bestDay][o.lessonId] =
                lessonCountByDay[ci][bestDay].value(o.lessonId, 0) + 1;
        }

        for (int k = 0; k < o.len; ++k)
            setTeacherBusy(o.teacherId, bestDay, bestStart + k, true);

        if (bestRoom >= 0)
        {
            for (int k = 0; k < o.len; ++k)
                setRoomBusy(bestRoom, bestDay, bestStart + k, true);
        }
    }

    QString baseDir = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    if (baseDir.isEmpty())
        baseDir = QDir::homePath();

    QDir out(baseDir);
    if (!out.exists("Timesheet"))
        out.mkdir("Timesheet");
    out.cd("Timesheet");

    QString safeName = s->name();
    safeName.replace(QRegularExpression(R"([\\/:*?"<>|])"), "_");

    QString stamp = QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss");
    QString filePath = out.filePath(safeName + "_timetable_" + stamp + ".pdf");

    QPdfWriter writer(filePath);
    writer.setPageSize(QPageSize(QPageSize::A4));
    writer.setResolution(96);
    writer.setPageMargins(QMarginsF(10, 10, 10, 10), QPageLayout::Millimeter);

    QPainter painter(&writer);
    if (!painter.isActive())
        return QString();

    const QStringList dayNames = { "Пн", "Вт", "Ср", "Чт", "Пт", "Сб" };

    auto drawOneClass = [&](int classIdx) {
        QRect page = writer.pageLayout().paintRectPixels(writer.resolution());

        int x0 = page.left();
        int y0 = page.top();
        int w  = page.width();
        int h  = page.height();

        QRect titleRect(x0, y0, w, 34);
        painter.fillRect(titleRect, Qt::lightGray);
        painter.drawRect(titleRect);

        painter.setFont(QFont("Arial", 14, QFont::Bold));
        painter.drawText(titleRect.adjusted(10, 0, -10, 0),
                         Qt::AlignLeft | Qt::AlignVCenter,
                         s->name() + " — " + classNames[classIdx]);

        painter.setFont(QFont("Arial", 9));
        painter.drawText(QRect(x0, y0 + 40, w, 18),
                         Qt::AlignLeft | Qt::AlignVCenter,
                         "Сгенерировано: " + QDateTime::currentDateTime().toString("dd.MM.yyyy HH:mm"));

        int tableTop = y0 + 68;

        int rows = PERIODS + 1;
        int cols = DAYS + 1;

        int gridH = h - (tableTop - y0) - 18;
        int gridW = w;

        int cellH = gridH / rows;
        int cellW = gridW / cols;

        painter.setFont(QFont("Arial", 9, QFont::Bold));
        for (int c = 0; c < cols; ++c)
        {
            QRect r(x0 + c * cellW, tableTop, cellW, cellH);
            painter.fillRect(r, Qt::lightGray);
            painter.drawRect(r);

            QString txt = (c == 0) ? "#" : dayNames[c - 1];
            painter.drawText(r, Qt::AlignCenter, txt);
        }

        for (int r = 1; r < rows; ++r)
        {
            QRect numRect(x0, tableTop + r * cellH, cellW, cellH);
            painter.fillRect(numRect, Qt::lightGray);
            painter.drawRect(numRect);

            painter.setFont(QFont("Arial", 9, QFont::Bold));
            painter.drawText(numRect, Qt::AlignCenter, QString::number(r));

            painter.setFont(QFont("Arial", 8));
            for (int c = 1; c < cols; ++c)
            {
                QRect cell(x0 + c * cellW, tableTop + r * cellH, cellW, cellH);
                painter.drawRect(cell);

                QString txt = tableText[classIdx][c - 1][r - 1];
                painter.drawText(cell.adjusted(4, 4, -4, -4),
                                 Qt::AlignLeft | Qt::AlignTop | Qt::TextWordWrap,
                                 txt);
            }
        }
    };

    for (int ci = 0; ci < C; ++ci)
    {
        if (ci != 0)
            writer.newPage();

        drawOneClass(ci);
    }

    if (!unscheduled.isEmpty())
    {
        writer.newPage();
        QRect page = writer.pageLayout().paintRectPixels(writer.resolution());

        QRect titleRect(page.left(), page.top(), page.width(), 34);
        painter.fillRect(titleRect, Qt::lightGray);
        painter.drawRect(titleRect);

        painter.setFont(QFont("Arial", 14, QFont::Bold));
        painter.drawText(titleRect.adjusted(10, 0, -10, 0),
                         Qt::AlignLeft | Qt::AlignVCenter,
                         "Не удалось разместить:");

        painter.setFont(QFont("Arial", 10));
        int y = page.top() + 50;
        for (const QString &u : std::as_const(unscheduled))
        {
            painter.drawText(QRect(page.left() + 8, y, page.width() - 16, 18),
                             Qt::AlignLeft | Qt::AlignVCenter,
                             "• " + u);
            y += 18;
            if (y > page.bottom() - 20)
                break;
        }
    }

    painter.end();

    return QUrl::fromLocalFile(filePath).toString();
}

