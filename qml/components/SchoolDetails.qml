import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Page {
    id: schoolDtailsPage
    signal showPageRequested(int pageIndex)

    property int schoolId: -1
    property string schoolName: ""

    onSchoolIdChanged: {
        loadSchoolData()
    }

    function loadSchoolData() {
        if (schoolId >= 0) {
            var schoolData = schoolModel.get(schoolId)
            if (schoolData) {
                schoolName = schoolData.name
                roomsModel.clear()
                var rooms = schoolData.rooms
                for (var i = 0; i < rooms.length; i++) {
                    roomsModel.append({
                        "name": rooms[i].name,
                        "size": rooms[i].size
                    })
                }
                console.log("Загружена школа:", schoolName, "с", rooms.length, "кабинетами")
            }
        }
    }

    function addRoom() {
        var name = newRoomName.text.trim()
        if (name.length === 0) {
            console.log("Имя кабинета пустое - пропускаем")
            return
        }
        roomsModel.append({ "name": name, "size": newRoomSize.currentText })
        console.log("Добавлен кабинет:", name, newRoomSize.currentText)
        newRoomName.text = ""
        newRoomName.forceActiveFocus()
    }

    header: ToolBar {
        ToolButton {
            text: "Назад"
            onClicked: {
                showPageRequested(0)
                console.log("Нажата кнопка назад")
            }
        }

        Label {
            text: "Имя школы"
            font.pointSize: 16
            font.bold: true
            anchors.centerIn: parent
        }
    }

    ListModel {
        id: roomsModel
    }

    RowLayout {
        spacing: 50

        ColumnLayout {
            anchors.margins: 16
            spacing: 12

            Label {
                text: "Кабинеты"
                font.bold: true
            }

            ScrollView {
                Layout.fillWidth: true
                Layout.preferredHeight: 240

                ListView {
                    id: roomsListView
                    width: parent.width
                    model: roomsModel
                    delegate: Rectangle {
                        width: roomsListView.width
                        height: 48
                        color: "transparent"
                        RowLayout {
                            anchors.fill: parent
                            anchors.margins: 4
                            spacing: 8

                            TextField {
                                id: nameEditor
                                text: name
                                placeholderText: "Название кабинета"
                                Layout.fillWidth: true
                                onEditingFinished: {
                                    roomsModel.setProperty(index, "name", text)
                                    console.log("Изменено имя:", text)
                                }
                            }

                            ComboBox {
                                id: sizeCombo
                                model: ["Маленький", "Большой"]
                                currentIndex: size === "Большой" ? 1 : 0
                                onCurrentTextChanged: {
                                    roomsModel.setProperty(index, "size", currentText)
                                    console.log("Изменён размер для", name, "->", currentText)
                                }
                            }

                            Button {
                                text: "Удалить"
                                onClicked: {
                                    console.log("Удаляем кабинет:", name)
                                    roomsModel.remove(index)
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
                    onClicked: addRoom()
                }
            }
        }

        ColumnLayout {
            Label {
                text: "Учителя"
                font.bold: true
            }
            ScrollView {
                Layout.fillWidth: true
                Layout.preferredHeight: 240

                ListView {
                    id: teachersListView
                    width: parent.width
                    //model: teachersModel
                    model: ListModel {
                       ListElement { name: "ФИО 1" }
                       ListElement { name: "ФИО 2" }
                    }
                    delegate: ItemDelegate {
                        width: parent.width
                        height: 60
                        //color: "transparent"
                        Label {
                            text: name
                        }
                        onClicked: {
                            showPageRequested(3)
                            console.log("Показать информацию об учителе");
                        }
                    }
                }
            }
        }

        ColumnLayout {
            Label {
                text: "Классы"
                font.bold: true
            }
            ScrollView {
                Layout.fillWidth: true
                Layout.preferredHeight: 240

                ListView {
                    id: klassListView
                    width: parent.width
                    model: ListModel {
                        ListElement { name: "10А" }
                        ListElement { name: "10Б" }
                    }
                    delegate: ItemDelegate {
                        width: parent.width
                        height: 60
                        //color: "transparent"
                        Label {
                            text: name
                        }
                        onClicked: {
                            showPageRequested(4)
                            console.log("Показать информацию о классе");
                        }
                    }
                }
            }

        }
    }
}
