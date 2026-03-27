// TimetableDetails.qml


// TimetableDetails.qml (упрощённая версия)
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt.labs.qmlmodels  // для TableModel

Page {
    id: timetableDetailsPage
    width: 800; height: 600

    signal showPageRequested(int pageIndex)

    readonly property var lessonModel: appState.lessonModel
    readonly property var roomModel: appState.roomModel
    readonly property var timetableModel: appState.timetableModel
    readonly property var teacherModel: appState.teacherModel

    property int selectLessonId: -1

    header: ToolBar {
        RowLayout {
            anchors.fill: parent; spacing: 8
            ToolButton { text: "Назад"; onClicked: showPageRequested(2) }
            Label { text: "Расписание"; font.bold: true }
            Item { Layout.fillWidth: true }
            ToolButton {
                text: "Кнопка"
                onClicked: {
                    console.log(timetableModel.rowCount());
                }
            }
        }
    }

    RowLayout {
        anchors.fill: parent
        spacing: 8

        // Левая часть: таблица с заголовками
        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true

            Rectangle {
                anchors.fill: parent
                color: "white"
                border.color: "#d0d0d0"
            }

            HorizontalHeaderView {
                id: horizontalHeader
                anchors.left: verticalHeader.right
                anchors.top: parent.top
                syncView: tableView
                clip: true
            }

            VerticalHeaderView {
                id: verticalHeader
                anchors.left: parent.left
                anchors.top: horizontalHeader.bottom
                syncView: tableView
                clip: true
            }

            TableView {
                id: tableView
                anchors.left: verticalHeader.right
                anchors.top: horizontalHeader.bottom
                anchors.right: parent.right
                anchors.bottom: parent.bottom

                clip: true
                columnSpacing: 1
                rowSpacing: 1
                model: timetableModel

                delegate: Rectangle {
                    implicitWidth: 140
                    implicitHeight: 48
                    border.width: 1
                    border.color: "#cccccc"
                    color: "white"

                    required property int row
                    required property int column
                    required property string lessonId
                    required property string lessonName

                    Text {
                        anchors.centerIn: parent
                        text: lessonName.length > 0 ? lessonName : "—"
                        wrapMode: Text.WordWrap
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                }
            }
        }

        // Правая часть: список неиспользованных уроков
        Rectangle {
            Layout.preferredWidth: 200
            Layout.fillHeight: true
            color: "#f7f7f7"
            border.color: "#cccccc"
            radius: 6

            ColumnLayout {
                anchors.fill: parent
                spacing: 0

                Rectangle {
                    color: "#dddddd"
                    Layout.fillWidth: true
                    Layout.preferredHeight: 40
                    Label {
                        anchors.centerIn: parent
                        text: "Неиспользованные уроки"
                        font.bold: true
                    }
                }

                ListView {
                    id: unusedLessons
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    model: lessonModel
                    delegate: Rectangle {
                        id: lessonCard
                        width: ListView.view.width
                        height: 60
                        color: id === selectLessonId ? "#ffcccc" : "#ffffff"
                        border.color: "#aaaaaa"
                        radius: 4

                        Column {
                            anchors.centerIn: parent
                            Text { text: name; font.pixelSize: 14 }
                            Text { text: teacherModel.teacherById(teacherId).surname; font.pixelSize: 12; color: "gray" }
                        }

                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                selectLessonId = id;
                            }
                        }
                    }
                }
            }
        }
    }
}
