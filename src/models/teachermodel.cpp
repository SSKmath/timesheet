#include "teachermodel.h"
#include <utility>

TeacherModel::TeacherModel(QObject *parent) : QAbstractListModel(parent), m_nextId(1)  {}

int TeacherModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_teachers.count();
}

QVariant TeacherModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_teachers.count())
        return QVariant();
    Teacher *t = m_teachers.at(index.row());

    switch (role) {
    case IdRole:         return t->id();
    case SurnameRole:    return t->surname();
    case NameRole:       return t->name();
    case PatronymicRole: return t->patronymic();
    case SubjectRole:    return t->subject();
    case WorkingDaysRole:
    {
        QVariantList days;
        for (bool d : t->workingDays())
            days << d;
        return days;
    };
    default: return QVariant();
    }
}

bool TeacherModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_teachers.count())
        return false;
    Teacher *t = m_teachers.at(index.row());
    if (!t)
        return false;

    switch (role) {
    case 0:
        t->setSurname(value.toString());
        break;
    case 1:
        t->setName(value.toString());
        break;
    case 2:                                        // жёсткие костыли, не понимаю, почему роли не работают
        t->setPatronymic(value.toString());
        break;
    case 4:
    {
        if (!value.canConvert<QVariantList>())
            return false;

        QVariantList list = value.toList();
        QList<bool> boolList;
        boolList.reserve(list.size());

        for (const QVariant &v : std::as_const(list))
            boolList.append(v.toBool());

        t->setWorkingDays(boolList);
        break;
    }
    default:
        return false;
    }

    emit dataChanged(index, index, {role});
    emit dataModified();
    return true;
}

QHash<int, QByteArray> TeacherModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[IdRole]          = "id";
    roles[SurnameRole]     = "surname";
    roles[NameRole]        = "name";
    roles[PatronymicRole]  = "patronymic";
    roles[SubjectRole]     = "subject";
    roles[WorkingDaysRole] = "workingDays";
    return roles;
}

void TeacherModel::appendTeacher(const QString &surname,
                                 const QString &name,
                                 const QString &patronymic,
                                 const QString &subject,
                                 const QList<bool> &workingDays)
{
    if (surname.isEmpty() || name.isEmpty() || patronymic.isEmpty() ||
        subject.isEmpty() || workingDays.size() != 6)
        return;

    int id = m_nextId++;
    appendTeacherWithId(id, surname, name, patronymic, subject, workingDays);
}

void TeacherModel::appendTeacherWithId(int id,
                                       const QString &surname,
                                       const QString &name,
                                       const QString &patronymic,
                                       const QString &subject,
                                       const QList<bool> &workingDays)
{
    if (surname.isEmpty() || name.isEmpty() || patronymic.isEmpty() ||
        subject.isEmpty() || workingDays.size() != 6)
        return;

    const int ind = m_teachers.count();
    beginInsertRows(QModelIndex(), ind, ind);

    Teacher *t = new Teacher(id, surname, name, patronymic, subject, this);
    t->setWorkingDays(workingDays);

    m_teachers.append(t);
    endInsertRows();

    if (id >= m_nextId)
        m_nextId = id + 1;

    emit dataModified();
}

void TeacherModel::removeAt(int index)
{
    if (index < 0 || index >= m_teachers.count())
        return;
    beginRemoveRows(QModelIndex(), index, index);
    Teacher *t = m_teachers.takeAt(index);
    endRemoveRows();
    if (t)
        t->deleteLater();

    emit dataModified();
}

int TeacherModel::count() const
{
    return m_teachers.count();
}

QObject* TeacherModel::teacherAt(int index) const
{
    if (index < 0 || index >= m_teachers.size())
        return nullptr;
    return m_teachers.at(index);
}

QObject* TeacherModel::teachetById(int id) const
{
    for (Teacher *tp : m_teachers)
    {
        if (tp->id() == id)
            return tp;
    }
    // потом написать, что будет, если не нашёлся учитель
}
