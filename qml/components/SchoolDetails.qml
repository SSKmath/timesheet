import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Page {
    id: schoolDetailsPage
    signal showPageRequested(int pageIndex)

    property int schoolId: -1
    property string schoolName: ""
    property var roomModel: null
    property var teacherModel: null

    onSchoolIdChanged: {
        if (schoolId >= 0) {
            var schoolData = schoolModel.get(schoolId)
            if (schoolData) {
                schoolName = schoolData.name
                roomModel = schoolModel.roomModelAt(schoolId)
                teacherModel = schoolModel.teacherModelAt(schoolId)
                appState.teacherModel = teacherModel
                console.log("Загружена школа:", schoolName)
            }
        }
    }
    header: ColumnLayout {
        spacing: 0
        ToolBar {
            Layout.fillWidth: true
            background: null
            ToolButton {
                text: "Назад"
                onClicked: {
                    showPageRequested(0)
                    console.log("Нажата кнопка назад")
                }
            }
        }

        ToolBar {
            Layout.fillWidth: true
            background: null

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 8
                anchors.rightMargin: 8

                Label {
                    text: schoolName
                    font.pointSize: 20
                    font.bold: true
                }

                Item { Layout.fillWidth: true }

                Button {
                    id: timesheetButton
                    text: "Расписание"
                    Layout.preferredWidth: 150
                    Layout.preferredHeight: 50

                    flat: true
                    hoverEnabled: true

                    readonly property color baseColor: "#1560BD"

                    background: Rectangle {
                        radius: 10
                        border.color: "#1560BD"
                        border.width: 1
                        color: {
                            if (timesheetButton.pressed)
                                return "#0D47A1"
                            else if (timesheetButton.hovered)
                                return "#1E88E5"
                            else
                                return timesheetButton.baseColor
                        }
                        Behavior on color { ColorAnimation { duration: 80 } }
                    }

                    contentItem: Text {
                        text: timesheetButton.text
                        font.pixelSize: 16
                        color: "white"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }

                    onClicked: {
                        showPageRequested(1)
                        console.log("Нажата кнопка расписания")
                    }
                }
            }
        }
    }

    RowLayout {
        anchors.fill: parent
        anchors.margins: 10
        spacing: 15

        // ---- Кабинеты ----
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: "#f8f9fa"
            radius: 12
            border.color: "#ddd"
            border.width: 1

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 12
                spacing: 12

                Label {
                    text: "Кабинеты"
                    font.bold: true
                    font.pixelSize: 18
                    color: "#333"
                }

                ScrollView {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    clip: true

                    ListView {
                        id: roomsListView
                        width: parent.width
                        height: parent.height
                        model: roomModel
                        spacing: 10
                        clip: true

                        delegate: Rectangle {
                            width: roomsListView.width
                            height: 40
                            radius: 10
                            color: mouseArea.containsMouse ? Qt.darker("#f5f5f5", 1.1) : "#f5f5f5"
                            border.color: "#79A0C1"
                            border.width: 1
//"#395778"
                            RowLayout {
                                anchors.fill: parent
                                anchors.margins: 4
                                spacing: 10

                                TextField {
                                    id: nameEditor
                                    text: name
                                    placeholderText: "Название кабинета"
                                    Layout.fillWidth: true
                                    onEditingFinished: {
                                        var ind = roomsListView.model.index(index, 0)
                                        roomsListView.model.setData(ind, text, 1)
                                        console.log("Изменено имя:", text)
                                    }
                                }

                                ComboBox {
                                    id: sizeCombo
                                    model: ["Маленький", "Большой"]
                                    currentIndex: size === "Большой" ? 1 : 0
                                    onCurrentTextChanged: {
                                        var idx = roomsListView.model.index(index, 0)
                                        roomsListView.model.setData(idx, currentText, 2)
                                        console.log("Изменён размер для", name, "->", currentText)
                                    }
                                }

                                Button {
                                    text: "🞨"
                                    onClicked: {
                                        roomModel.removeAt(index)
                                        console.log("Удаляем кабинет:", name)
                                    }
                                }
                            }
                        }
                    }
                }

                Item { Layout.fillHeight: true }

                RowLayout {
                    Layout.fillWidth: true
                    spacing: 8

                    TextField {
                        id: newRoomName
                        placeholderText: "Новое название кабинета"
                        Layout.fillWidth: true
                        onAccepted: addRoom()
                    }

                    ComboBox {
                        id: newRoomSize
                        model: ["Маленький", "Большой"]
                        currentIndex: 0
                        Layout.preferredWidth: 120
                    }

                    Button {
                        text: "Добавить"
                        onClicked: {
                            var name = newRoomName.text.trim()
                            if (name.length === 0) {
                                console.log("Имя кабинета пустое - пропускаем")
                                return
                            }
                            roomModel.appendRoom(name, newRoomSize.currentText)
                            newRoomName.text = ""
                            newRoomName.forceActiveFocus()
                            console.log("Добавлен кабинет:", name, newRoomSize.currentText)
                        }
                    }
                }
            }
        }

        // ---- Учителя ----
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: "#f8f9fa"
            radius: 12
            border.color: "#ddd"
            border.width: 1

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 12
                spacing: 12

                Label {
                    text: "Учителя"
                    font.bold: true
                    font.pixelSize: 18
                    color: "#333"
                }

                ScrollView {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    clip: true

                    ListView {
                        id: teachersListView
                        width: parent.width
                        height: parent.height
                        model: teacherModel
                        spacing: 10
                        clip: true

                        delegate: Rectangle {
                            radius: 10
                            width: parent.width
                            height: 40
                            color: mouseArea.containsMouse ? Qt.darker("#f5f5f5", 1.1) : "#f5f5f5"
                            border.color: "#79A0C1"
                            border.width: 1

                            RowLayout {
                                anchors.fill: parent
                                anchors.margins: 4
                                spacing: 8

                                Label {
                                    text: surname + " " + name.charAt(0) + ". " + patronymic.charAt(0) + "."
                                    Layout.fillWidth: true
                                    MouseArea {
                                        anchors.fill: parent
                                        onClicked: {
                                            appState.teacherIndex = index
                                            appState.teacherIsNew = false
                                            showPageRequested(3)
                                            console.log("Показать информацию об учителе")
                                        }
                                    }
                                }

                                Item { Layout.fillWidth: true }

                                Button {
                                    text: "🞨"
                                    onClicked: {
                                        teacherModel.removeAt(index)
                                        console.log("Удаляем учителя:", name)
                                    }
                                }
                            }
                        }
                    }
                }

                Item { Layout.fillHeight: true }

                RowLayout {
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignHCenter
                    Button {
                        text: "Добавить"
                        onClicked: {
                            appState.teacherIndex = -1
                            appState.teacherIsNew = true
                            showPageRequested(3)
                        }
                    }
                }
            }
        }

        // ---- Классы ----
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: "#f8f9fa"
            radius: 12
            border.color: "#ddd"
            border.width: 1

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 12
                spacing: 12

                Label {
                    text: "Классы"
                    font.bold: true
                    font.pixelSize: 18
                    color: "#333"
                }

                ScrollView {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    clip: true

                    ListView {
                        id: klassListView
                        width: parent.width
                        height: parent.height
                        model: ListModel {
                            ListElement { name: "10А" }
                            ListElement { name: "10Б" }
                        }
                        spacing: 10
                        clip: true

                        delegate: Rectangle {
                            width: parent.width
                            height: 40
                            radius: 10
                            color: mouseArea.containsMouse ? Qt.darker("#f5f5f5", 1.1) : "#f5f5f5"
                            border.color: "#79A0C1"
                            border.width: 1

                            RowLayout {
                                anchors.fill: parent
                                anchors.margins: 4
                                spacing: 8

                                Label {
                                    text: name
                                    Layout.fillWidth: true
                                    MouseArea {
                                        anchors.fill: parent
                                        onClicked: {
                                            showPageRequested(4)
                                            console.log("Показать информацию о классе")
                                        }
                                    }
                                }

                                Item { Layout.fillWidth: true }

                                Button {
                                    text: "🞨"
                                    onClicked: {
                                        console.log("Удаляем класс:", name)
                                    }
                                }
                            }
                        }
                    }
                }

                Item { Layout.fillHeight: true }

                RowLayout {
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignHCenter
                    Button {
                        text: "Добавить"
                        onClicked: {
                            console.log("Добавить класс")
                        }
                    }
                }
            }
        }
    }

    // Вспомогательная функция для добавления кабинета
    function addRoom() {
        var name = newRoomName.text.trim()
        if (name.length === 0) return
        roomModel.appendRoom(name, newRoomSize.currentText)
        newRoomName.text = ""
        newRoomName.forceActiveFocus()
        console.log("Добавлен кабинет:", name)
    }
}
