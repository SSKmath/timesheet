#ifndef LESSONMODEL_H
#define LESSONMODEL_H

#include <QAbstractListModel>
#include <QList>
#include "lesson.h"

class LessonModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit LessonModel(QObject *parent = nullptr);

    enum Roles {
        IdRole = Qt::UserRole + 1,
        NameRole,
        IsDoubleRole,
        TeacherIdRole,
        PerWeekRole,
        ClassesRole
    };

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index,
                  int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index,
                 const QVariant &value,
                 int role = Qt::EditRole) override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void appendLesson(const QString &name,
                                  bool isDouble,
                                  int teacherId,
                                  int perWeek,
                                  const QList<int> &classes);

    void appendLessonWithId(int id,
                            const QString &name,
                            bool isDouble,
                            int teacherId,
                            int perWeek,
                            const QList<int> &classes);

    Q_INVOKABLE void removeAt(int index);
    Q_INVOKABLE int count() const;
    Q_INVOKABLE QObject *lessonAt(int index) const;

signals:
    void dataModified();

private:
    QList<Lesson*> m_lessons;
    int m_nextId;
};

#endif // LESSONMODEL_H
