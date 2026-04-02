import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt.labs.qmlmodels

Page {
    id: timetableDetailsPage
    width: 800
    height: 600

    signal showPageRequested(int pageIndex)

    readonly property var lessonModel: appState.lessonModel
    readonly property var roomModel: appState.roomModel
    readonly property var timetableModel: appState.timetableModel
    readonly property var teacherModel: appState.teacherModel
    readonly property var classModel: appState.classModel

    // Сейчас выбранный урок из списка
    property string selectedLessonId: ""
    property string selectedLessonName: ""

    header: ToolBar {
        RowLayout {
            anchors.fill: parent
            spacing: 8

            ToolButton {
                text: "Назад"
                onClicked: showPageRequested(2)
            }

            Label {
                text: "Расписание"
                font.bold: true
            }

            Item { Layout.fillWidth: true }
        }
    }

    RowLayout {
        anchors.fill: parent
        spacing: 8

        // Левая часть: таблица
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
                    color: lessonName.length > 0 ? "#eef7ff" : "white"

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

                    Text {
                        text: lessonId ? teacherModel.teacherById(lessonModel.idTeacher(lessonId)).surname : ""
                    }

                    Text {
                        text: lessonId ? classModel.classById(lessonModel.idClass(lessonId)).name : ""
                        anchors.top: parent.top
                        anchors.right: parent.right
                    }

                    MouseArea {
                        anchors.fill: parent
                        acceptedButtons: Qt.LeftButton | Qt.RightButton

                        onClicked: function(mouse) {
                            if (mouse.button === Qt.LeftButton) {
                                if (selectedLessonId !== "") {
                                    if (timetableModel.placeLesson(row, column,
                                                                   selectedLessonId,
                                                                   selectedLessonName)) {
                                        selectedLessonId = ""
                                        selectedLessonName = ""
                                    }
                                }
                            } else if (mouse.button === Qt.RightButton) {
                                timetableModel.clearLesson(row, column)
                            }
                        }
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

                    Button {
                        anchors.centerIn: parent
                        text: "Генерировать"
                        onClicked: timetableModel.generate()
                    }
                }

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
                    clip: true
                    model: lessonModel

                    delegate: Rectangle {
                        id: lessonCard
                        width: ListView.view.width

                        required property string id
                        required property string name
                        required property int teacherId

                        property bool used: timetableModel.lessonUsageRevision >= 0
                                            && timetableModel.isLessonUsed(id)

                        required property var classes

                        height: used ? 0 : 60
                        visible: !used
                        opacity: selectedLessonId === id ? 0.75 : 1.0
                        color: selectedLessonId === id ? "#ffcccc" : "#ffffff"
                        border.color: "#aaaaaa"
                        radius: 4

                        Column {
                            anchors.centerIn: parent
                            spacing: 2

                            Text {
                                text: name
                                font.pixelSize: 14
                            }

                            Text {
                                text: teacherModel.teacherById(teacherId).surname
                                font.pixelSize: 12
                                color: "gray"
                            }

                            Text {
                                text: classes.length > 0 ? classModel.classAt(classes[0] - 1).name : ""
                                font.pixelSize: 12
                            }
                        }

                        MouseArea {
                            anchors.fill: parent
                            enabled: !used
                            onClicked: {
                                selectedLessonId = id
                                selectedLessonName = name
                            }
                        }
                    }
                }
            }
        }
    }
}
