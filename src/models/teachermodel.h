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
    Q_ENUM(Roles)

    explicit TeacherModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void appendTeacher(const QString &surname,
                                   const QString &name,
                                   const QString &patronymic,
                                   const QString &subject,
                                   const QList<bool> &workingDays);
    Q_INVOKABLE void removeAt(int index);
    Q_INVOKABLE int count() const;
    Q_INVOKABLE QObject *teacherAt(int index) const;

signals:
    void dataModified();

private:
    QList<Teacher*> m_teachers;
};

#endif // TEACHERMODEL_H
