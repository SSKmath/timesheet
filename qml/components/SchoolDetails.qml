import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Page {
    id: schoolDtailsPage
    signal showPageRequested(int pageIndex)

    property int schoolId: -1
    property string schoolName: ""
    property var roomModel: null

    onSchoolIdChanged: {
        if (schoolId >= 0) {
            var schoolData = schoolModel.get(schoolId)
            if (schoolData) {
                schoolName = schoolData.name
                roomModel = schoolModel.roomsModelAt(schoolId)
                console.log(roomModel) // undefined
                console.log("Загружена школа:", schoolName, "с комнатами из C++ модели")
            }
        }
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
            text: schoolName
            font.pointSize: 16
            font.bold: true
            anchors.centerIn: parent
        }
    }

    RowLayout {
        anchors.fill: parent
        spacing: 10

        ColumnLayout {
            Layout.fillWidth: true
            Layout.preferredWidth: 1
            Layout.fillHeight: true
            //anchors.margins: 16
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
                    model: roomModel
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
                                text: "Удалить"
                                onClicked: {
                                    roomModel.removeAt(index)
                                    console.log("Удаляем кабинет:", name)
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

        ColumnLayout {
            Layout.fillWidth: true
            Layout.preferredWidth: 1
            Layout.fillHeight: true

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
            Layout.fillWidth: true
            Layout.preferredWidth: 1
            Layout.fillHeight: true

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
