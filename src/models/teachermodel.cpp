#include "teachermodel.h"

TeacherModel::TeacherModel(QObject *parent) : QAbstractListModel(parent)  {}

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

QHash<int, QByteArray> TeacherModel::roleNames() const
{
    QHash<int, QByteArray> roles;
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

    const int ind = m_teachers.count();
    beginInsertRows(QModelIndex(), ind, ind);

    Teacher *t = new Teacher(surname, name, patronymic, subject, this);
    t->setWorkingDays(workingDays);

    m_teachers.append(t);
    endInsertRows();
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
}

int TeacherModel::count() const
{
    return m_teachers.count();
}
