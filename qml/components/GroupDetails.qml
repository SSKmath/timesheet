import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Page {
    id: teacherDetailsPage
    signal showPageRequested(int pageIndex)

    readonly property var schoolclassModel: appState.schoolclassModel
    readonly property var lessonModel: appState.lessonModel
    readonly property var teacherModel: appState.teacherModel
    readonly property var classModel: appState.classModel
    property int currentChoiseTeacher: -1

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
                    height: visible ? 56 : 0
                    color: "transparent"
                    visible: classes.includes(schoolclassModel.id)

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
                                var ind = subjectListView.model.index(index, 0)
                                lessonModel.setData(ind, text, 2)
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
                            enabled: perWeek > 1
                            onClicked: {
                                var ind = subjectListView.model.index(index, 0)
                                lessonModel.setData(ind, perWeek - 1, 5) // Всё ещё не понимаю, как работают роли
                            }
                        }

                        Label {
                            text: perWeek
                            Layout.preferredWidth: 26
                            horizontalAlignment: Text.AlignHCenter
                            Layout.alignment: Qt.AlignVCenter
                        }

                        Button {
                            text: "+"
                            Layout.preferredWidth: 34
                            Layout.preferredHeight: 34
                            enabled: perWeek < 99
                            onClicked: {
                                var ind = subjectListView.model.index(index, 0)
                                lessonModel.setData(ind, perWeek + 1, 5) // Всё ещё не понимаю, как работают роли
                            }
                        }

                        ComboBox {
                            id: teacherCombo
                            model: teacherModel
                            Layout.preferredWidth: 170

                            Component.onCompleted: {
                                currentIndex = teacherModel.indexById(teacherId)
                            }

                            displayText: {
                                var teacher = teacherModel.teacherById(teacherId)
                                return teacher ? teacher.surname + " " + teacher.name[0] + ". " + teacher.patronymic[0] + "." : "Не выбран"
                            }

                            delegate: ItemDelegate {
                                width: teacherCombo.width
                                text: model.surname + " " + (model.name ? model.name[0] + "." : "") + " " + (model.patronymic ? model.patronymic[0] + "." : "")
                            }

                            onActivated: function(comboIndex) {
                                if (currentIndex >= 0) {
                                    var selectedTeacherId = teacherModel.data(teacherModel.index(currentIndex, 0), 257)
                                    var lessonIndex = subjectListView.model.index(index, 0)
                                    lessonModel.setData(lessonIndex, selectedTeacherId, 4)
                                }
                            }
                        }

                        ComboBox {
                            id: pairCombo
                            model: ["Парный", "Одинарный"]
                            currentIndex: isDouble ? 0 : 1
                            Layout.preferredWidth: 140
                            Layout.alignment: Qt.AlignVCenter
                            onCurrentTextChanged: {
                                var ind = subjectListView.model.index(index, 0)
                                lessonModel.setData(ind, pairCombo.currentText === "Парный", 3)
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
                model: teacherModel
                Layout.preferredWidth: 170

                displayText: {
                    if (currentChoiseTeacher !== -1) {
                        var teacher = model.teacherAt(currentIndex)
                        return teacher.surname + " " + (teacher.name ? teacher.name[0] + "." : "") +(teacher.patronymic ? " " + teacher.patronymic[0] + "." : "")
                    }
                    return "Выберите преподавателя" // почему-то это никогда не выводится
                }

                delegate: ItemDelegate {
                    width: teacherComboAdd.width
                    text: model.surname + " " + (model.name ? model.name[0] : "") + " " + (model.patronymic ? model.patronymic[0] : "0")
                    onClicked: {
                        currentChoiseTeacher = model.id
                    }
                }
            }

            ComboBox {
                id: pairComboAdd
                model: ["Парный", "Одинарный"]
                Layout.preferredWidth: 140
            }

            Button {
                text: "Добавить"
                enabled: currentChoiseTeacher !== -1 && subjectAdd.text !== ""
                onClicked: {
                    lessonModel.appendLesson(subjectAdd.text, pairComboAdd.currentText === "Парный", currentChoiseTeacher, 1, [schoolclassModel.id])
                    subjectAdd.text = ""
                    currentChoiseTeacher = -1
                }
            }
        }
    }
}
