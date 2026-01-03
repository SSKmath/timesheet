#ifndef TEACHER_H
#define TEACHER_H

#include <QObject>
#include <QList>

class Teacher : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString surname READ surname WRITE setSurname NOTIFY surnameChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString patronymic READ patronymic WRITE setPatronymic NOTIFY patronymicChanged)
    Q_PROPERTY(QString subject READ subject WRITE setSubject NOTIFY subjectChanged)
    Q_PROPERTY(QList<bool> workingDays READ workingDays WRITE setWorkingDays NOTIFY workingDaysChanged)

public:
    explicit Teacher(QObject *parent = nullptr);

    Teacher(int id,
            const QString &surname,
            const QString &name,
            const QString &patronymic,
            const QString &subject,
            QObject *parent = nullptr);

    int id() const;

    QString surname() const;
    void setSurname(const QString &v);

    QString name() const;
    void setName(const QString &v);

    QString patronymic() const;
    void setPatronymic(const QString &v);

    QString subject() const;
    void setSubject(const QString &v);

    QList<bool> workingDays() const;
    void setWorkingDays(const QList<bool> &days);

    bool worksOnDay(int index) const;
    void setWorksOnDay(int index, bool v);

signals:
    void surnameChanged();
    void nameChanged();
    void patronymicChanged();
    void subjectChanged();
    void workingDaysChanged();

private:
    int m_id = -1;
    QString m_surname;
    QString m_name;
    QString m_patronymic;
    QString m_subject;
    QList<bool> m_workingDays;
};

#endif // TEACHER_H
