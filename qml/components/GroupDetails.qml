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
                font.pixelSize: 16
                Layout.alignment: Qt.AlignVCenter
            }

            Item { Layout.fillWidth: true }

            Label {
                text: "Классный руководитель"
                font.bold: true
                font.pixelSize: 16
                Layout.alignment: Qt.AlignVCenter
            }

            TextField {
                id: nameClassTeacher
                placeholderText: "Имя преподавателя"
                Layout.preferredWidth: 260
                Layout.alignment: Qt.AlignVCenter
                font.pixelSize: 16
                background: ovalTextFieldBackground
            }
        }

        Label {
            text: "Предметы"
            font.bold: true
            font.pixelSize: 16
        }

        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            padding: 8
            background: Rectangle {
                color: "#f5f5f5"
                radius: 8
            }

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

                    // Внешняя рамка вокруг каждого предмета
                    Rectangle {
                        anchors.fill: parent
                        radius: 12
                        border.color: "#1976D2"
                        border.width: 1.5
                        color: "#fafafa"
                    }

                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: 8
                        spacing: 8

                        // Название предмета (рамка)
                        TextField {
                            id: subjectEditor
                            text: name
                            placeholderText: "Название предмета"
                            Layout.fillWidth: true
                            font.pixelSize: 16
                            onEditingFinished: {
                                var ind = subjectListView.model.index(index, 0)
                                lessonModel.setData(ind, text, 2)
                            }
                            background: ovalTextFieldBackground
                        }

                        Label {
                            text: "Количество уроков неделю"
                            font.pixelSize: 16
                            Layout.alignment: Qt.AlignVCenter
                        }

                        // Кнопка "-" (без рамки)
                        Button {
                            text: "-"
                            Layout.preferredWidth: 20
                            Layout.preferredHeight: 34
                            enabled: perWeek > 1
                            onClicked: {
                                var ind = subjectListView.model.index(index, 0)
                                lessonModel.setData(ind, perWeek - 1, 5)
                            }
                            background: Rectangle {
                                radius: height / 2
                                color: parent.hovered ? "#ffcdd2" : "#f5f5f5"
                            }
                            contentItem: Text {
                                text: parent.text
                                font.pixelSize: 16
                                font.bold: true
                                color: "#d32f2f"
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                            }
                        }

                        Label {
                            text: perWeek
                            Layout.preferredWidth: 26
                            horizontalAlignment: Text.AlignHCenter
                            Layout.alignment: Qt.AlignVCenter
                            font.pixelSize: 16
                            font.bold: true
                        }

                        // Кнопка "+" (без рамки)
                        Button {
                            text: "+"
                            Layout.preferredWidth: 20
                            Layout.preferredHeight: 34
                            enabled: perWeek < 99
                            onClicked: {
                                var ind = subjectListView.model.index(index, 0)
                                lessonModel.setData(ind, perWeek + 1, 5)
                            }
                            background: Rectangle {
                                radius: height / 2
                                color: parent.hovered ? "#c8e6c9" : "#f5f5f5"
                            }
                            contentItem: Text {
                                text: parent.text
                                font.pixelSize: 16
                                font.bold: true
                                color: "#2e7d32"
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                            }
                        }

                        // Преподаватель (рамка)
                        ComboBox {
                            id: teacherCombo
                            model: teacherModel
                            Layout.preferredWidth: 150
                            font.pixelSize: 16

                            Component.onCompleted: {
                                currentIndex = teacherModel.indexById(teacherId)
                            }

                            displayText: {
                                if (!teacherModel)
                                    return "";
                                var teacher = teacherModel.teacherById(teacherId)
                                return teacher ? teacher.surname + " " + teacher.name[0] + ". " + teacher.patronymic[0] + "." : "Не выбран"
                            }

                            delegate: ItemDelegate {
                                width: teacherCombo.width
                                font.pixelSize: 16
                                text: model.surname + " " + (model.name ? model.name[0] + "." : "") + " " + (model.patronymic ? model.patronymic[0] + "." : "")
                            }

                            onActivated: function(comboIndex) {
                                if (currentIndex >= 0) {
                                    var selectedTeacherId = teacherModel.data(teacherModel.index(currentIndex, 0), 257)
                                    var lessonIndex = subjectListView.model.index(index, 0)
                                    lessonModel.setData(lessonIndex, selectedTeacherId, 4)
                                }
                            }
                            background: ovalComboBoxBackground
                        }

                        // Парный (рамка)
                        ComboBox {
                            id: pairCombo
                            model: ["Парный", "Одинарный"]
                            currentIndex: isDouble ? 0 : 1
                            Layout.preferredWidth: 120
                            Layout.alignment: Qt.AlignVCenter
                            font.pixelSize: 16
                            onCurrentTextChanged: {
                                var ind = subjectListView.model.index(index, 0)
                                lessonModel.setData(ind, pairCombo.currentText === "Парный", 3)
                            }
                            background: ovalComboBoxBackground
                        }

                        // Кнопка удаления (без рамки)
                        Button {
                            icon.source: "trash.png"
                            display: AbstractButton.TextBesideIcon
                            Layout.preferredWidth: 44
                            Layout.preferredHeight: 34
                            onClicked: {
                                lessonModel.removeAt(index)
                                console.log("Удаляем урок:", name)
                            }
                            background: Rectangle {
                                radius: height / 2
                                color: parent.hovered ? "#ffcdd2" : "#f5f5f5"
                            }
                            contentItem: Text {
                                text: "✕"
                                font.pixelSize: 16
                                font.bold: true
                                color: "#d32f2f"
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                            }
                        }
                    }
                }
            }
        }

        // Строка добавления нового предмета
        RowLayout {
            Layout.fillWidth: true
            spacing: 8

            // Название предмета (рамка)
            TextField {
                id: subjectAdd
                placeholderText: "Название предмета"
                Layout.fillWidth: true
                font.pixelSize: 16
                background: ovalTextFieldBackground
            }

            // Преподаватель (рамка)
            ComboBox {
                id: teacherComboAdd
                model: teacherModel
                Layout.preferredWidth: 200
                font.pixelSize: 16

                displayText: {
                    if (currentChoiseTeacher !== -1) {
                        var teacher = model.teacherAt(currentIndex)
                        return teacher.surname + " " + (teacher.name ? teacher.name[0] + "." : "") + (teacher.patronymic ? " " + teacher.patronymic[0] + "." : "")
                    }
                    return "Выберите преподавателя"
                }

                delegate: ItemDelegate {
                    width: teacherComboAdd.width
                    font.pixelSize: 16
                    text: model.surname + " " + (model.name ? model.name[0] : "") + " " + (model.patronymic ? model.patronymic[0] : "0")
                    onClicked: {
                        currentChoiseTeacher = model.id
                    }
                }
                background: ovalComboBoxBackground
            }

            // Парный (рамка)
            ComboBox {
                id: pairComboAdd
                model: ["Парный", "Одинарный"]
                Layout.preferredWidth: 100
                font.pixelSize: 16
                background: ovalComboBoxBackground
            }

            // Кнопка "Добавить" (рамка)
            Button {
                text: "Добавить"
                enabled: currentChoiseTeacher !== -1 && subjectAdd.text !== ""
                onClicked: {
                    lessonModel.appendLesson(subjectAdd.text, pairComboAdd.currentText === "Парный", currentChoiseTeacher, 1, [schoolclassModel.id])
                    subjectAdd.text = ""
                    currentChoiseTeacher = -1
                }
                background: Rectangle {
                    radius: height / 2
                    color: parent.hovered ? "#e3f2fd" : "#f5f5f5"
                    border.color: "#1976D2"
                    border.width: 1.5
                }
                contentItem: Text {
                    text: parent.text
                    font.pixelSize: 16
                    font.bold: true
                    color: "#1976D2"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }
        }

        // Нижние кнопки (без рамок, но с фоном)
        RowLayout {
            Layout.fillWidth: true
            spacing: 16

            Button {
                text: "Сохранить"
                Layout.fillWidth: true
                Layout.preferredWidth: 1
                focusPolicy: Qt.NoFocus
                flat: true

                background: Rectangle {
                    radius: height / 2
                    color: "#1976D2"
                    border.color: "#1976D2"
                    border.width: 1
                }

                contentItem: Text {
                    text: parent.text
                    color: "white"
                    font.pixelSize: 16
                    font.bold: true
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }

                onClicked: {
                    showPageRequested(2)
                }
            }

            Button {
                text: "Отмена"
                Layout.fillWidth: true
                Layout.preferredWidth: 1
                focusPolicy: Qt.NoFocus
                flat: true

                background: Rectangle {
                    radius: height / 2
                    color: "#f0f0f0"
                    border.color: "#1976D2"
                    border.width: 2
                }

                contentItem: Text {
                    text: parent.text
                    color: "#1976D2"
                    font.pixelSize: 16
                    font.bold: true
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }

                onClicked: {
                    showPageRequested(2)
                }
            }
        }
    }

    Component {
        id: ovalTextFieldBackground
        Rectangle {
            radius: height / 2
            color: parent.focus ? "#e8f0fe" : "#f5f5f5"
            border.color: "#778899"
            border.width: 1.5
        }
    }

    Component {
        id: ovalComboBoxBackground
        Rectangle {
            radius: height / 2
            color: parent.focus ? "#e8f0fe" : "#f5f5f5"
            border.color: "#778899"
            border.width: 1.5
            implicitHeight: 40
        }
    }
}
