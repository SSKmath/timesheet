#ifndef SCHOOLSTORAGE_H
#define SCHOOLSTORAGE_H

#include <QObject>
#include <QVariantMap>
#include <QVariantList>
#include <QString>
#include <QByteArray>
#include <QList>

class School;

class SchoolStorage : public QObject
{
    Q_OBJECT
public:
    explicit SchoolStorage(QObject *parent = nullptr);

    QString storageDirectory() const;

    bool saveSchool(const QVariantMap &schoolData);
    bool saveSchool(class School *school);

    QVariantMap loadSchool(const QString &id) const;

    QList<QVariantMap> loadAllSchools() const;

    bool removeSchool(const QString &id);

    bool writeJsonToFile(const QString &path, const QByteArray &json);

private:
    QString m_dir;
};

#endif // SCHOOLSTORAGE_H
