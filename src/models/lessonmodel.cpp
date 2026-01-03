#include "lessonmodel.h"
#include <utility>

LessonModel::LessonModel(QObject *parent)
    : QAbstractListModel(parent),
    m_nextId(1)
{
}

int LessonModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_lessons.count();
}

QVariant LessonModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_lessons.count())
        return QVariant();

    Lesson *l = m_lessons.at(index.row());
    if (!l)
        return QVariant();

    switch (role) {
    case IdRole:        return l->id();
    case NameRole:      return l->name();
    case IsDoubleRole:  return l->isDouble();
    case TeacherIdRole: return l->teacherId();
    case ClassesRole:
    {
        QVariantList cls;
        for (int id : l->classes())
            cls << id;
        return cls;
    }
    default:
        return QVariant();
    }
}

bool LessonModel::setData(const QModelIndex &index,
                          const QVariant &value,
                          int role)
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_lessons.count())
        return false;

    Lesson *l = m_lessons.at(index.row());
    if (!l)
        return false;

    switch (role) {
    case 0:
        l->setName(value.toString());
        break;
    case 1:
        l->setIsDouble(value.toBool());
        break;
    case 2:
        l->setTeacherId(value.toInt());
        break;
    case 3:
    {
        if (!value.canConvert<QVariantList>())
            return false;

        QVariantList list = value.toList();
        QList<int> ids;
        ids.reserve(list.size());

        for (const QVariant &v : std::as_const(list))
            ids.append(v.toInt());

        l->setClasses(ids);
        break;
    }

    default:
        return false;
    }

    emit dataChanged(index, index, { role });
    emit dataModified();
    return true;
}


QHash<int, QByteArray> LessonModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[IdRole]        = "id";
    roles[NameRole]      = "name";
    roles[IsDoubleRole]  = "isDouble";
    roles[TeacherIdRole] = "teacherId";
    roles[ClassesRole]   = "classes";
    return roles;
}

void LessonModel::appendLesson(const QString &name,
                               bool isDouble,
                               int teacherId,
                               const QList<int> &classes)
{
    if (name.trimmed().isEmpty() || teacherId <= 0)
        return;

    int id = m_nextId++;
    appendLessonWithId(id, name, isDouble, teacherId, classes);
}

void LessonModel::appendLessonWithId(int id,
                                     const QString &name,
                                     bool isDouble,
                                     int teacherId,
                                     const QList<int> &classes)
{
    if (name.trimmed().isEmpty() || teacherId <= 0)
        return;

    const int row = m_lessons.count();
    beginInsertRows(QModelIndex(), row, row);

    Lesson *l = new Lesson(id, name, isDouble, teacherId, classes, this);
    m_lessons.append(l);

    endInsertRows();

    if (id >= m_nextId)
        m_nextId = id + 1;

    emit dataModified();
}

void LessonModel::removeAt(int index)
{
    if (index < 0 || index >= m_lessons.count())
        return;

    beginRemoveRows(QModelIndex(), index, index);
    Lesson *l = m_lessons.takeAt(index);
    endRemoveRows();

    if (l)
        l->deleteLater();

    emit dataModified();
}

int LessonModel::count() const
{
    return m_lessons.count();
}

QObject *LessonModel::lessonAt(int index) const
{
    if (index < 0 || index >= m_lessons.size())
        return nullptr;
    return m_lessons.at(index);
}
