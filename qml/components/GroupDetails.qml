import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Page {
    id: teacherDetailsPage
    signal showPageRequested(int pageIndex)

    readonly property var schoolclassModel: appState.schoolclassModel
    readonly property var lessonModel: appState.lessonModel

    header: ToolBar {
        RowLayout {
            anchors.fill: parent
            anchors.margins: 6
            spacing: 8

            ToolButton {
                text: "Назад"
                onClicked: {
                    showPageRequested(2)
                    console.log("Нажата кнопка назад")
                }
            }

            Item { Layout.fillWidth: true }
        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 12

        RowLayout {
            Layout.fillWidth: true
            spacing: 12

            Label {
                text: schoolclassModel === null ? "error" : schoolclassModel.name
                font.bold: true
                Layout.alignment: Qt.AlignVCenter
            }

            Item { Layout.fillWidth: true }

            Label {
                text: "Классный руководитель"
                font.bold: true
                Layout.alignment: Qt.AlignVCenter
            }

            TextField {
                id: nameClassTeacher
                placeholderText: "Имя преподавателя"
                Layout.preferredWidth: 260
                Layout.alignment: Qt.AlignVCenter
            }
        }

        Label {
            text: "Предметы"
            font.bold: true
        }

        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true

            ListView {
                id: subjectListView
                width: parent.width
                model: lessonModel
                clip: true
                spacing: 8

                delegate: Rectangle {
                    width: subjectListView.width
                    height: 56
                    color: "transparent"

                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: 8
                        spacing: 8

                        TextField {
                            id: subjectEditor
                            text: name
                            placeholderText: "Название предмета"
                            Layout.fillWidth: true
                            onEditingFinished: {
                                // оставлено без изменений (логика редактирования будет добавлена позже)
                            }
                        }

                        Label {
                            text: "Количество уроков неделю"
                            Layout.alignment: Qt.AlignVCenter
                        }

                        Button {
                            text: "-"
                            Layout.preferredWidth: 34
                            Layout.preferredHeight: 34
                            onClicked: {
                                subjectCount--;
                            }
                        }

                        Label {
                            text: "4"
                            Layout.preferredWidth: 26
                            horizontalAlignment: Text.AlignHCenter
                            Layout.alignment: Qt.AlignVCenter
                        }

                        Button {
                            text: "+"
                            Layout.preferredWidth: 34
                            Layout.preferredHeight: 34
                            onClicked: {
                                subjectCount++;
                            }
                        }

                        ComboBox {
                            id: teacherCombo
                            model: ["Шелест О. В.", " Учитель 2"]
                            currentIndex: size === "Учетель 1" ? 1 : 0
                            Layout.preferredWidth: 170
                            Layout.alignment: Qt.AlignVCenter
                            onCurrentTextChanged: {
                                // оставлено без изменений (логика будет добавлена позже)
                            }
                        }

                        ComboBox {
                            id: pairCombo
                            model: ["Парный", "Одинарный"]
                            currentIndex: size === "Парный" ? 1 : 0
                            Layout.preferredWidth: 140
                            Layout.alignment: Qt.AlignVCenter
                            onCurrentTextChanged: {
                                // оставлено без изменений (логика будет добавлена позже)
                            }
                        }

                        Button {
                            icon.source: "trash.png"
                            display: AbstractButton.TextBesideIcon
                            Layout.preferredWidth: 44
                            Layout.preferredHeight: 34
                            onClicked: {
                                lessonModel.removeAt(index)
                                console.log("Удаляем урок:", name)
                            }
                        }
                    }
                }
            }
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: 8

            TextField {
                id: subjectAdd
                placeholderText: "Название предмета"
                Layout.fillWidth: true
            }

            ComboBox {
                id: teacherComboAdd
                model: ["Шелест О. В.", " Учитель 2"]
                currentIndex: size === "Учетель 1" ? 1 : 0
                Layout.preferredWidth: 170
            }

            ComboBox {
                id: pairComboAdd
                model: ["Парный", "Одинарный"]
                currentIndex: size === "Парный" ? 1 : 0
                Layout.preferredWidth: 140
            }

            Button {
                text: "Добавить"
                onClicked: {
                    lessonModel.appendLesson(subjectAdd.text, false, 1, [])
                }
            }
        }
    }
}
