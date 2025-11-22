#ifndef SCHOOLMODEL_H
#define SCHOOLMODEL_H

#include <QAbstractListModel>
#include <QVariantMap>
#include <QList>
#include "school.h"
#include "../storage/schoolstorage.h"

class SchoolModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum Roles {
        IdRole = Qt::UserRole + 1,
        NameRole,
        RoomsModelRole
    };

    explicit SchoolModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void addSchoolFromVariant(const QString &name, const QVariantList &rooms);
    Q_INVOKABLE void removeSchool(int index);
    Q_INVOKABLE QVariantMap get(int index) const;
    Q_INVOKABLE int count() const;

private:
    QList<School*> m_schools;
    SchoolStorage *m_storage;
};

#endif // SCHOOLMODEL_H
