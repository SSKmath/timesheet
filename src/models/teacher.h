#ifndef TEACHER_H
#define TEACHER_H

#include <QObject>

class Teacher : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString surname READ surname WRITE setSurname NOTIFY surnameChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString patronymic READ patronymic WRITE setPatronymic NOTIFY patronymicChanged)
    Q_PROPERTY(QString subject READ subject WRITE setSubject NOTIFY subjectChanged)
public:
    explicit Teacher(QObject *parent = nullptr);
    Teacher(const QString &surname, const QString &size, const QString &patronymic, const QString &subject, QObject *parent = nullptr);

    QString surname() const;
    void setSurname(const QString &v);

    QString name() const;
    void setName(const QString &v);

    QString patronymic() const;
    void setPatronymic(const QString &v);

    QString subject() const;
    void setSubject(const QString &v);

signals:
    void surnameChanged();
    void nameChanged();
    void patronymicChanged();
    void subjectChanged();

private:
    QString m_surname;
    QString m_name;
    QString m_patronymic;
    QString m_subject;
};

#endif // TEACHER_H
