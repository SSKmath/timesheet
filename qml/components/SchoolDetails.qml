import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Page {
    id: schoolDtailsPage
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
            //anchors.fill: parent
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
                    //model: roomsModel
                    model: ListModel {
                       ListElement { name: "Кабинет 1"; size: "Большой" }
                       ListElement { name: "Кабинет 2"; size: "Маленький" }
                    }
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
                                    //roomsModel.set(index, {"name": text, "size": roomsModel.get(index).size})
                                    console.log("Изменено имя:", text)
                                }
                            }

                            ComboBox {
                                id: sizeCombo
                                model: ["Маленький", "Большой"]
                                currentIndex: size === "Большой" ? 1 : 0
                                onCurrentTextChanged: {
                                    //roomsModel.set(index, {"name": roomsModel.get(index).name, "size": currentText})
                                    //console.log("Изменён размер для", roomsModel.get(index).name, "->", currentText)
                                }
                            }

                            Button {
                                text: "Удалить"
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

            RowLayout {
                Layout.fillWidth: true
                spacing: 8
                Button {
                    text: "Сохранить"
                    Layout.fillWidth: true
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

                        schoolModel.addSchoolFromVariant(name, arr)

                        schoolNameField.text = ""
                        roomsModel.clear()
                        showPageRequested(0)

                        console.log("Школа успешно сохранена")
                    }
                }

                Button {
                    text: "Отмена"
                    Layout.fillWidth: true
                    onClicked: {
                        schoolNameField.text = ""
                        roomsMode.clear()
                        showPageRequested(0)
                        console.log("Отмена добавления школы")
                    }
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

    /*ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20

        Label {
            text: "Детали школы"
            font.bold: true
        }

        RowLayout {
            Label { text: "Количество классов:" }
            SpinBox { value: 10 }
        }

        Label { text: "Расписание" }
        ListView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            model: ["Понедельник: Математика", "Вторник: Информатика"]
            delegate: Label { text: modelData }
        }
    }*/
}
