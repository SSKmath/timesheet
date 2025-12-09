#ifndef ROOMMODEL_H
#define ROOMMODEL_H

#include <QAbstractListModel>
#include <QList>
#include "room.h"

class RoomModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum Roles {
        NameRole = Qt::UserRole + 1,
        SizeRole
    };
    Q_ENUM(Roles)

    explicit RoomModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void appendRoom(const QString &name, const QString &size);
    Q_INVOKABLE void removeAt(int index);
    Q_INVOKABLE int count() const;

signals:
    void dataModified();

private:
    QList<Room*> m_rooms;
};

#endif // ROOMMODEL_H
