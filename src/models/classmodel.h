#ifndef CLASSMODEL_H
#define CLASSMODEL_H

#include <QAbstractListModel>
#include <QList>
#include "schoolclass.h"

class ClassModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum Roles {
        IdRole = Qt::UserRole + 1,
        NameRole
    };

    explicit ClassModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index,
                  int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void appendClass(const QString &name);
    Q_INVOKABLE void removeAt(int index);
    Q_INVOKABLE int count() const;
    Q_INVOKABLE QObject *classAt(int index) const;
    Q_INVOKABLE QObject *classById(int id) const;

    void appendClassWithId(int id, const QString &name);

signals:
    void dataModified();

private:
    QList<SchoolClass*> m_classes;
    int m_nextId;
};

#endif // CLASSMODEL_H
