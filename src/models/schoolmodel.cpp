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
    QVector<QString> classNames;
    classNames.reserve(cm->rowCount());

    for (int i = 0; i < cm->rowCount(); ++i)
    {
        QModelIndex ind = cm->index(i);
        int cid = cm->data(ind, ClassModel::IdRole).toInt();
        QString cname = cm->data(ind, ClassModel::NameRole).toString();
        if (cid <= 0)
            continue;

        classIdToIdx[cid] = classNames.size();
        classNames.push_back(cname);
    }

    const int C = classNames.size();
    if (C == 0)
        return QString();

    QVector<QVector<QVector<QString>>> table(
        C,
        QVector<QVector<QString>>(DAYS, QVector<QString>(PERIODS))
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
        roomBusy = QVector<QVector<QVector<bool>>>(R, QVector<QVector<bool>>(DAYS, QVector<bool>(PERIODS, false)));

    auto roomIsBusy = [&](int ridx, int day, int p) -> bool {
        if (R <= 0) return false;
        return roomBusy[ridx][day][p];
    };

    auto setRoomBusy = [&](int ridx, int day, int p, bool v) {
        if (R <= 0) return;
        roomBusy[ridx][day][p] = v;
    };


    struct Occ
    {
        QString name;
        bool isDouble;
        int teacherId;
        QVector<int> classes;
        int len;
    };

    QVector<Occ> occs;

    for (int i = 0; i < lm->rowCount(); ++i)
    {
        QModelIndex ind = lm->index(i);

        QString lname  = lm->data(ind, LessonModel::NameRole).toString();
        bool isDouble  = lm->data(ind, LessonModel::IsDoubleRole).toBool();
        int teacherId  = lm->data(ind, LessonModel::TeacherIdRole).toInt();
        QVariantList clsVar = lm->data(ind, LessonModel::ClassesRole).toList();

        if (lname.trimmed().isEmpty())
            continue;

        QVector<int> clsIdx;
        for (const QVariant &v : std::as_const(clsVar))
        {
            int cid = v.toInt();
            if (classIdToIdx.contains(cid))
                clsIdx.push_back(classIdToIdx[cid]);
        }
        if (clsIdx.isEmpty())
            continue;

        int perWeek = 1;
        QObject *lobj = lm->lessonAt(i);
        if (lobj)
        {
            QVariant pw = lobj->property("perWeek");
            if (pw.isValid())
            {
                int val = pw.toInt();
                if (val > 0)
                    perWeek = val;
            }
        }

        Occ base;
        base.name = lname;
        base.isDouble = isDouble;
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

    for (const Occ &o : std::as_const(occs))
    {
        bool placed = false;

        for (int day = 0; day < DAYS && !placed; ++day)
        {
            if (!teacherCanDay(o.teacherId, day))
                continue;

            for (int p = 0; p + o.len - 1 < PERIODS && !placed; ++p)
            {
                if (teacherIsBusy(o.teacherId, day, p)) continue;
                if (o.len == 2 && teacherIsBusy(o.teacherId, day, p + 1)) continue;

                bool ok = true;
                for (int ci : o.classes)
                {
                    if (!table[ci][day][p].isEmpty()) { ok = false; break; }
                    if (o.len == 2 && !table[ci][day][p + 1].isEmpty()) { ok = false; break; }
                }
                if (!ok) continue;

                // --- choose room ---
                int chosenRoom = -1;

                if (R > 0)
                {
                    for (int ri = 0; ri < R; ++ri)
                    {
                        if (roomIsBusy(ri, day, p)) continue;
                        if (o.len == 2 && roomIsBusy(ri, day, p + 1)) continue;
                        chosenRoom = ri;
                        break;
                    }

                    // нет свободного кабинета на это время — пробуем другой слот
                    if (chosenRoom == -1)
                        continue;
                }

                QString tshort = teacherShort.value(o.teacherId, QString());

                QString cellText = o.name;
                if (!tshort.isEmpty())
                    cellText += "\n" + tshort;

                if (chosenRoom != -1)
                    cellText += "\nкаб. " + roomNames[chosenRoom];

                for (int ci : o.classes)
                {
                    table[ci][day][p] = cellText;
                    if (o.len == 2)
                        table[ci][day][p + 1] = cellText;
                }

                setTeacherBusy(o.teacherId, day, p, true);
                if (o.len == 2)
                    setTeacherBusy(o.teacherId, day, p + 1, true);

                if (chosenRoom != -1)
                {
                    setRoomBusy(chosenRoom, day, p, true);
                    if (o.len == 2)
                        setRoomBusy(chosenRoom, day, p + 1, true);
                }

                placed = true;

            }
        }

        if (!placed)
            unscheduled.push_back(o.name);
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

        int topHeader = 70;
        int tableTop = y0 + topHeader;

        painter.setFont(QFont("Arial", 14, QFont::Bold));
        painter.drawText(QRect(x0, y0, w, 40), Qt::AlignLeft | Qt::AlignVCenter,
                         s->name() + " — " + classNames[classIdx]);

        painter.setFont(QFont("Arial", 9));
        painter.drawText(QRect(x0, y0 + 35, w, 25), Qt::AlignLeft | Qt::AlignVCenter,
                         "Сгенерировано: " + QDateTime::currentDateTime().toString("dd.MM.yyyy HH:mm"));

        int rows = PERIODS + 1;
        int cols = DAYS + 1;

        int gridH = h - topHeader - 40;
        int gridW = w;

        int cellH = gridH / rows;
        int cellW = gridW / cols;

        painter.setFont(QFont("Arial", 9));

        for (int c = 0; c < cols; ++c)
        {
            QRect r(x0 + c * cellW, tableTop, cellW, cellH);
            painter.drawRect(r);

            QString txt;
            if (c == 0) txt = "#";
            else txt = dayNames[c - 1];

            painter.drawText(r, Qt::AlignCenter, txt);
        }

        for (int r = 1; r < rows; ++r)
        {
            QRect rn(x0, tableTop + r * cellH, cellW, cellH);
            painter.drawRect(rn);
            painter.drawText(rn, Qt::AlignCenter, QString::number(r));

            for (int c = 1; c < cols; ++c)
            {
                QRect cell(x0 + c * cellW, tableTop + r * cellH, cellW, cellH);
                painter.drawRect(cell);

                QString txt = table[classIdx][c - 1][r - 1];
                painter.drawText(cell.adjusted(3, 3, -3, -3),
                                 Qt::AlignCenter | Qt::TextWordWrap,
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

        painter.setFont(QFont("Arial", 14, QFont::Bold));
        painter.drawText(QRect(page.left(), page.top(), page.width(), 40),
                         Qt::AlignLeft | Qt::AlignVCenter,
                         "Не удалось разместить:");

        painter.setFont(QFont("Arial", 10));
        int y = page.top() + 50;
        for (const QString &u : std::as_const(unscheduled))
        {
            painter.drawText(QRect(page.left(), y, page.width(), 18),
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

