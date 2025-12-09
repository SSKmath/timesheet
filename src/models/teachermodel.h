#ifndef TEACHERMODEL_H
#define TEACHERMODEL_H

#include <QAbstractListModel>
#include <QList>
#include "teacher.h"

class TeacherModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum Roles {
        SurnameRole = Qt::UserRole + 1,
        NameRole,
        PatronymicRole,
        SubjectRole,
        WorkingDaysRole
    };

    explicit TeacherModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void appendTeacher(const QString &surname,
                                   const QString &name,
                                   const QString &patronymic,
                                   const QString &subject,
                                   const QList<bool> &workingDays);
    Q_INVOKABLE void removeAt(int index);
    Q_INVOKABLE int count() const;

private:
    QList<Teacher*> m_teachers;
};

#endif // TEACHERMODEL_H
