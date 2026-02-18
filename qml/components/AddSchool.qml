import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Page {
    id: addSchoolPage
    signal showPageRequested(int pageIndex)

    header: ToolBar {
        ToolButton {
            text: "Назад"
            onClicked: {
                showPageRequested(0)
                console.log("Нажата кнопка назад")
            }
        }

        Label {
            text: "Добавить школу"
            font.bold: true
            anchors.centerIn: parent
        }
    }

    ListModel {
        id: roomsModel
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 12

        TextField {
            id: schoolNameField
            placeholderText: "Название школы"
            Layout.fillWidth: true
            background: Rectangle {
                radius: height / 2
                color: parent.focus ? "#e8f0fe" : "#f5f5f5"
                border.color: "#1976D2"
                border.width: 1.5
            }
        }

        Label {
            text: "Кабинеты"
            font.bold: true
        }
        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            padding: 8
            background: Rectangle {
                color: "#f5f5f5"
                radius: 8
            }

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
                            background: Rectangle {
                                radius: height / 2
                                color: parent.focus ? "#e8f0fe" : "#f5f5f5"
                                border.color: "#1976D2"
                                border.width: 1.5
                            }
                            onEditingFinished: {
                                roomsModel.set(index, {"name": text, "size": roomsModel.get(index).size})
                                console.log("Изменено имя:", text)
                            }
                        }

                        ComboBox {
                            id: sizeCombo
                            model: ["Маленький", "Большой"]
                            currentIndex: size === "Большой" ? 1 : 0
                            onCurrentTextChanged: {
                                roomsModel.set(index, {"name": roomsModel.get(index).name, "size": currentText})
                                console.log("Изменён размер для", roomsModel.get(index).name, "->", currentText)
                            }
                        }

                        Button {
                            text: "🞨"
                            onClicked: {
                                console.log("Удаляем кабинет:", roomsModel.get(index).name)
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
                background: Rectangle {
                    radius: height / 2
                    color: parent.focus ? "#e8f0fe" : "#f5f5f5"
                    border.color: "#1976D2"
                    border.width: 1.5
                }
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
                background: Rectangle {
                    color: {
                        if (parent.pressed) return "#b0b0b0"
                        if (parent.hovered) return "#c0c0c0"
                        return "#e0e0e0"
                    }
                    border.color: parent.hovered ? "#999" : "#ccc"
                    border.width: 1
                }
                onClicked: addRoom()
            }
        }

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
                    var name = schoolNameField.text.trim()
                    if (name.length === 0) {
                        console.log("Имя школы пустое, сохранение отменено")
                        return
                    }

                    var arr = []
                    for (var i = 0; i < roomsModel.count; ++i) {
                        var cur = roomsModel.get(i)
                        if (!cur.name || cur.name.trim().length === 0)
                            continue
                        arr.push({"name": cur.name, "size": cur.size ? cur.size : "Маленький"})
                    }

                    schoolModel.addSchoolFromVariant(name, arr, [])

                    schoolNameField.text = ""
                    roomsModel.clear()
                    showPageRequested(0)

                    console.log("Школа успешно сохранена")
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
                    schoolNameField.text = ""
                    roomsModel.clear()
                    showPageRequested(0)
                    console.log("Отмена добавления школы")
                }
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
}
