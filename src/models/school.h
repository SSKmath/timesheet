#ifndef SCHOOL_H
#define SCHOOL_H

#include <QObject>
#include <QUuid>
#include "roommodel.h"

class School : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString id READ id CONSTANT)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QObject* roomsModel READ roomsModel CONSTANT)
public:
    explicit School(const QString &id, const QString &name, QObject *parent = nullptr);

    QString id() const;
    QString name() const;
    void setName(const QString &n);

    QObject* roomsModel() const;

signals:
    void nameChanged();
    void requestSave();

private slots:
    void saveToStorage();

private:
    QString m_id;
    QString m_name;
    RoomModel *m_rooms;
};

#endif // SCHOOL_H
