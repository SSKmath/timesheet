#ifndef ROOM_H
#define ROOM_H

#include <QObject>

class Room : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString size READ size WRITE setSize NOTIFY sizeChanged)
public:
    explicit Room(QObject *parent = nullptr);
    Room(const QString &name, const QString &size, QObject *parent = nullptr);

    QString name() const;
    void setName(const QString &v);

    QString size() const;
    void setSize(const QString &v);

signals:
    void nameChanged();
    void sizeChanged();

private:
    QString m_name;
    QString m_size;
};

#endif // ROOM_H
