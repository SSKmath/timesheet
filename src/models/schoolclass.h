#ifndef SCHOOLCLASS_H
#define SCHOOLCLASS_H

#include <QObject>
#include <QString>

class SchoolClass : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int id READ id NOTIFY idChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)

public:
    explicit SchoolClass(int id, const QString &name, QObject *parent = nullptr);

    int id() const;

    QString name() const;
    void setName(const QString &name);

signals:
    void idChanged();
    void nameChanged();

private:
    int m_id;
    QString m_name;
};

#endif // SCHOOLCLASS_H
