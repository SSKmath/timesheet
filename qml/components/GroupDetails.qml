import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Page {
    id: classDetailsPage
    signal showPageRequested(int pageIndex)

    // --- Поддержка внешнего appState (из контекста) или создание заглушки ---
    property var actualAppState: (typeof appState !== 'undefined') ? appState : null

    QtObject {
        id: internal
        property var appState: actualAppState ? actualAppState : dummyAppState
    }

    // Заглушка для работы без внешнего appState
    QtObject {
        id: dummyAppState
        property ListModel teachersModel: ListModel {
            ListElement { name: "Иванов И.И." }
            ListElement { name: "Петрова А.С." }
            ListElement { name: "Сидоров В.П." }
        }
        property bool isNewClass: true
        property int classIndex: 0
    }

    // Свойства для управления режимом (добавление/редактирование)
    readonly property bool isNew: internal.appState.isNewClass !== undefined ? internal.appState.isNewClass : true
    readonly property int classIndex: internal.appState.classIndex !== undefined ? internal.appState.classIndex : 0

    // Модель предметов (пустая, без предустановленных данных)
    ListModel {
        id: subjectsModel
    }

    // Модель для выбора учителя (используется только для классного руководителя)
    property var teachersModel: internal.appState.teachersModel

    header: ToolBar {
        ToolButton {
            text: "Назад"
            onClicked: {
                showPageRequested(2)
                console.log("Нажата кнопка назад")
            }
        }
        Label {
            text: isNew ? "Добавить класс" : "Информация о классе"
            font.bold: true
            anchors.centerIn: parent
        }
    }

    // Основной контент с отступами от краёв
    Item {
        anchors.fill: parent
        anchors.margins: 16

        ColumnLayout {
            anchors.fill: parent
            spacing: 16

            // Верхняя строка: название класса и классный руководитель
            GridLayout {
                columns: 2
                columnSpacing: 16
                rowSpacing: 8
                Layout.fillWidth: true

                Label { text: "Название класса" }
                TextField {
                    id: classNameField
                    placeholderText: "Например, 10А"
                    Layout.fillWidth: true
                    background: Rectangle {
                        radius: height / 2
                        color: parent.focus ? "#e8f0fe" : "#f5f5f5"
                        border.color: "#1976D2"
                        border.width: 1.5
                    }
                }

                Label { text: "Классный руководитель" }
                ComboBox {
                    id: classTeacherCombo
                    model: teachersModel
                    textRole: "name"
                    Layout.fillWidth: true
                    background: Rectangle {
                        radius: height / 2
                        color: parent.focus ? "#e8f0fe" : "#f5f5f5"
                        border.color: "#1976D2"
                        border.width: 1.5
                        implicitHeight: 20
                    }
                    delegate: ItemDelegate {
                        text: model.name
                        font.pixelSize: 14
                        highlighted: ListView.isCurrentItem
                    }
                }
            }

            // Заголовок раздела предметов
            Label {
                text: "Предметы"
                font.bold: true
                font.pixelSize: 16
            }

            // Прокручиваемая область со списком предметов (серый фон)
            ScrollView {
                Layout.fillWidth: true
                Layout.fillHeight: true
                padding: 8
                background: Rectangle {
                    color: "#f5f5f5"
                    radius: 8
                }

                ListView {
                    id: subjectsListView
                    model: subjectsModel
                    spacing: 8
                    delegate: Rectangle {
                        width: subjectsListView.width
                        height: 70
                        radius: 12
                        border.color: "#1976D2"
                        border.width: 1.5
                        color: "#ddd"

                        RowLayout {
                            anchors.fill: parent
                            anchors.margins: 8
                            spacing: 8

                            // Название предмета
                            TextField {
                                text: model.subjectName
                                placeholderText: "Предмет"
                                Layout.fillWidth: true
                                onEditingFinished: subjectsModel.setProperty(index, "subjectName", text)
                                background: Rectangle {
                                    radius: height / 2
                                    color: parent.focus ? "#e8f0fe" : "#f5f5f5"
                                    border.color: "#1976D2"
                                    border.width: 1.5
                                }
                            }

                            // Учитель (текстовое поле)
                            TextField {
                                text: model.teacherName
                                placeholderText: "ФИО учителя"
                                Layout.preferredWidth: 140
                                onEditingFinished: subjectsModel.setProperty(index, "teacherName", text)
                                background: Rectangle {
                                    radius: height / 2
                                    color: parent.focus ? "#e8f0fe" : "#f5f5f5"
                                    border.color: "#1976D2"
                                    border.width: 1.5
                                }
                            }

                            // Кастомный счётчик количества уроков в неделю
                            RowLayout {
                                spacing: 4
                                Layout.preferredWidth: 100

                                Button {
                                    text: "-"
                                    focusPolicy: Qt.NoFocus
                                    flat: true
                                    implicitWidth: 30
                                    implicitHeight: 30
                                    onClicked: {
                                        var val = model.hoursPerWeek
                                        if (val > 1) {
                                            subjectsModel.setProperty(index, "hoursPerWeek", val - 1)
                                        }
                                    }
                                    background: Rectangle {
                                        radius: height / 2
                                        color: parent.hovered ? "#ffcdd2" : "#f5f5f5"
                                        border.color: "#d32f2f"
                                        border.width: 1
                                    }
                                    contentItem: Text {
                                        text: parent.text
                                        color: "#d32f2f"
                                        font.bold: true
                                        horizontalAlignment: Text.AlignHCenter
                                        verticalAlignment: Text.AlignVCenter
                                    }
                                }

                                Label {
                                    text: model.hoursPerWeek
                                    font.bold: true
                                    horizontalAlignment: Text.AlignHCenter
                                    verticalAlignment: Text.AlignVCenter
                                    Layout.preferredWidth: 30
                                }

                                Button {
                                    text: "+"
                                    focusPolicy: Qt.NoFocus
                                    flat: true
                                    implicitWidth: 30
                                    implicitHeight: 30
                                    onClicked: {
                                        var val = model.hoursPerWeek
                                        if (val < 10) {
                                            subjectsModel.setProperty(index, "hoursPerWeek", val + 1)
                                        }
                                    }
                                    background: Rectangle {
                                        radius: height / 2
                                        color: parent.hovered ? "#c8e6c9" : "#f5f5f5"
                                        border.color: "#2e7d32"
                                        border.width: 1
                                    }
                                    contentItem: Text {
                                        text: parent.text
                                        color: "#2e7d32"
                                        font.bold: true
                                        horizontalAlignment: Text.AlignHCenter
                                        verticalAlignment: Text.AlignVCenter
                                    }
                                }
                            }

                            // Тип пары (парный/одинарный)
                            ComboBox {
                                model: ["Одинарный", "Парный"]
                                currentIndex: model.isPair ? 1 : 0
                                Layout.preferredWidth: 100
                                onCurrentTextChanged: subjectsModel.setProperty(index, "isPair", currentIndex === 1)
                                background: Rectangle {
                                    radius: height / 2
                                    color: parent.focus ? "#e8f0fe" : "#f5f5f5"
                                    border.color: "#1976D2"
                                    border.width: 1.5
                                    implicitHeight: 40
                                }
                            }

                            // Кнопка удаления предмета
                            Button {
                                text: "✕"
                                focusPolicy: Qt.NoFocus
                                flat: true
                                Layout.preferredWidth: 30
                                onClicked: subjectsModel.remove(index)
                                background: Rectangle {
                                    radius: height / 2
                                    color: parent.hovered ? "#ffcdd2" : "#f5f5f5"
                                    border.color: "#d32f2f"
                                    border.width: 1
                                }
                                contentItem: Text {
                                    text: parent.text
                                    color: "#d32f2f"
                                    font.bold: true
                                    horizontalAlignment: Text.AlignHCenter
                                    verticalAlignment: Text.AlignVCenter
                                }
                            }
                        }
                    }
                }
            }

            // Кнопка добавления нового предмета
            Button {
                text: "+ Добавить предмет"
                Layout.alignment: Qt.AlignHCenter
                focusPolicy: Qt.NoFocus
                flat: true
                onClicked: subjectsModel.append({
                    subjectName: "",
                    teacherName: "",
                    hoursPerWeek: 1,
                    isPair: false
                })
                background: Rectangle {
                    radius: height / 2
                    color: parent.hovered ? "#e3f2fd" : "#f5f5f5"
                    border.color: "#1976D2"
                    border.width: 1.5
                }
                contentItem: Text {
                    text: parent.text
                    color: "#1976D2"
                    font.bold: true
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }

            // Нижние кнопки (Сохранить / Отмена)
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
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        font.pixelSize: 16
                        font.bold: true
                    }

                    onClicked: {
                        // Собираем данные и сохраняем (заглушка)
                        console.log("Сохранение класса:", classNameField.text)
                        for(var i = 0; i < subjectsModel.count; i++) {
                            console.log("Предмет:", subjectsModel.get(i).subjectName,
                                        "Учитель:", subjectsModel.get(i).teacherName)
                        }
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
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        font.pixelSize: 16
                        font.bold: true
                    }

                    onClicked: {
                        showPageRequested(2)
                        console.log("Отмена редактирования класса")
                    }
                }
            }
        }
    }
}
