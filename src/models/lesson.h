#ifndef LESSON_H
#define LESSON_H

#include <QObject>
#include <QList>
#include <QString>

class Lesson : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int id READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(bool isDouble READ isDouble WRITE setIsDouble NOTIFY isDoubleChanged)
    Q_PROPERTY(int teacherId READ teacherId WRITE setTeacherId NOTIFY teacherIdChanged)
    Q_PROPERTY(QList<int> classes READ classes WRITE setClasses NOTIFY classesChanged)

public:
    explicit Lesson(QObject *parent = nullptr);
    Lesson(int id,
           const QString &name,
           bool isDouble,
           int teacherId,
           const QList<int> &classes,
           QObject *parent = nullptr);

    int id() const;
    void setId(int id);

    QString name() const;
    void setName(const QString &name);

    bool isDouble() const;
    void setIsDouble(bool v);

    int teacherId() const;
    void setTeacherId(int id);

    QList<int> classes() const;
    void setClasses(const QList<int> &c);

signals:
    void idChanged();
    void nameChanged();
    void isDoubleChanged();
    void teacherIdChanged();
    void classesChanged();

private:
    int m_id;
    QString m_name;
    bool m_isDouble;
    int m_teacherId;
    QList<int> m_classes;
};

#endif // LESSON_H
