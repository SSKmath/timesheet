import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Page {
    id: timetableDetailsPage
    signal showPageRequested(int pageIndex)

    readonly property var lessonModel: appState.lessonModel
    readonly property var roomModel: appState.roomModel
    readonly property var timetableModel: appState.timetableModel
    readonly property var teacherModel: appState.teacherModel

    header: ToolBar {
        ToolButton {
            text: "Назад"
            onClicked: {
                showPageRequested(2)
                console.log("Нажата кнопка назад")
            }
        }

        Label {
            text: "Расписание"
            font.pointSize: 16
            font.bold: true
            anchors.centerIn: parent
        }
    }

    RowLayout {
        anchors.fill: parent

        TableView {
            id: timetable
            Layout.fillWidth: true
            Layout.fillHeight: true

            model: timetableModel

            delegate: Rectangle {
                implicitWidth: 150
                implicitHeight: 50
                color: model.lessonName ? "lightblue" : "lightgray"
                border.color: "black"
                border.width: 1

                Text {
                    anchors.centerIn: parent
                    text: model.lessonName || ""
                }

                DropArea {
                    anchors.fill: parent
                    keys: ["lesson"]

                    onDropped: {
                        if (drop.hasText)
                        {
                            var lessonId = drop.text
                            timetableModel.setData(timetableModel.index(row, column), lessonId, TimetableModel.LessonIdRole)
                        }
                    }
                }
            }

            // rowDelegate: Rectangle {
            //     implicitHeight: 50
            //     color: "white"
            //     Text {
            //         anchors.centerIn: parent
            //         // Простой расчет: day = Math.floor(row / 8), slot = row % 8 + 1
            //         // Предполагаем 8 слотов в день
            //         text: {
            //             var days = ["Понедельник", "Вторник", "Среда", "Четверг", "Пятница"]
            //             var slotsPerDay = 8
            //             var dayIndex = Math.floor(row / slotsPerDay)
            //             var slotIndex = (row % slotsPerDay) + 1
            //             return days[dayIndex] + " " + slotIndex + "-й урок"
            //         }
            //     }
            // }

            // columnDelegate: Rectangle {
            //     implicitWidth: 150
            //     color: "white"
            //     Text {
            //         anchors.centerIn: parent
            //         // Получаем имя комнаты из roomModel по колонке (column = index комнаты)
            //         text: roomModel.data(roomModel.index(column, 0), RoomModel.NameRole) || "Комната " + (column + 1)
            //     }
            // }
        }

        ColumnLayout {
            Rectangle {
                Layout.preferredWidth: 200
                height: 56
                color: "transparent"

                Label {
                    text: "Не использованные уроки"
                    font.pointSize: 10
                    font.bold: true
                    anchors.centerIn: parent
                }
            }

            ListView {
                id: unusedLessons
                Layout.preferredWidth: 200
                Layout.fillHeight: true
                model: lessonModel
                spacing: 5

                delegate: Rectangle {
                    width: unusedLessons.width
                    height: 56
                    color: "gray"
                    radius: 5

                    property real startX: 0
                    property real startY: 0

                    Label {
                        anchors.top: parent.top
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: name
                    }

                    Label {
                        anchors.bottom: parent.bottom
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: teacherModel.teacherById(teacherId).surname
                    }

                    Behavior on x { NumberAnimation { duration: 250; easing.type: Easing.OutCubic } }
                    Behavior on y { NumberAnimation { duration: 250; easing.type: Easing.OutCubic } }

                    MouseArea {
                        anchors.fill: parent
                        drag.target: parent

                        onPressed: {
                            startX = parent.x
                            startY = parent.y

                            parent.Drag.active = true
                            parent.Drag.hotSpot.x = mouse.x
                            parent.Drag.hotSpot.y = mouse.y
                            parent.Drag.mimeData = { "text/plain": model.id }
                            parent.Drag.dragType = Drag.Internal
                            parent.Drag.keys = ["lesson"]
                            console.log("Starting drag for lesson ID:", model.id)
                        }

                        onReleased: {
                            parent.Drag.active = false

                            parent.x = startX
                            parent.y = startY
                        }

                        onCanceled: {
                            parent.Drag.active = false
                            parent.x = startX
                            parent.y = startY
                        }
                    }
                }
            }
        }
    }

    Component.onCompleted: {
        // Предполагаем, что timetableModel инициализирована с количеством комнат из roomModel
        if (timetableModel) {
            timetableModel.setRoomCount(roomModel.rowCount())
            timetableModel.setSlotCount(5 * 8)  // 5 дней x 8 слотов
        }
    }
}
