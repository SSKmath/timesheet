// TimetableModel.h
#ifndef TIMETABLEMODEL_H
#define TIMETABLEMODEL_H

#include <QAbstractTableModel>
#include <QList>

struct LessonAssignment {
    QString lessonId;
    QString lessonName;
};

class TimetableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit TimetableModel(QObject *parent = nullptr);

    // Роли данных для QML (добавлены через Q_ENUM для доступа из QML)
    enum Roles {
        LessonIdRole = Qt::UserRole + 1,
        LessonNameRole
    };
    Q_ENUM(Roles)

    // Базовые переопределения QAbstractTableModel:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // Для редактирования ячеек
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    // Переопределяем roleNames для имен ролей в QML
    QHash<int, QByteArray> roleNames() const override;

    // Переопределяем headerData для заголовков столбцов/строк
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Инвокабельные методы для настройки таблицы
    Q_INVOKABLE void setRoomCount(int count);  // установить количество кабинетов (столбцов)
    Q_INVOKABLE void setSlotCount(int count);  // установить количество слотов (строк)
    Q_INVOKABLE void setRoomModel(QObject *roomModel); // указать модель кабинетов (для имен)

private:
    int m_roomCount;   // число кабинетов (столбцов)
    int m_slotCount;   // число временных слотов (строк)
    QList<LessonAssignment> m_cells; // плоский массив размера (m_roomCount * m_slotCount)
    QObject *m_roomModel; // указатель на существующую модель кабинетов (RoomModel)
};

#endif // TIMETABLEMODEL_H
