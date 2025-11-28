import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Page {
    id: teacherDetailsPage
    signal showPageRequested(int pageIndex)

    header: ToolBar {
        ToolButton {
            text: "Назад"
            onClicked: {
                showPageRequested(2)
                console.log("Нажата кнопка назад")
            }
        }
        Label {
            text: "Дополнительная информация"
            font.bold: true
            anchors.centerIn: parent
        }
    }
    ListModel {
        id: teacherModel
    }
    TextField {
        id: teacherName
        placeholderText: "ФИО"
        Layout.fillWidth: true

        x: 30
        y: 20
    }

    GridLayout {
        anchors.fill: parent
        anchors.margins: 50
        rows: 5
        columns: 2
        rowSpacing: 20
        columnSpacing: 10

        Item{
            Label {
                text: "Предметы"
                font.bold: true
            }
        }

        Item{

        }

        Item{
            Layout.fillWidth: true
            Layout.fillHeight: true
            TextField {
                id: teacherSubject1
                placeholderText: "Название предмета"
                Layout.fillWidth: true
            }
        }

        Item{
            Layout.fillWidth: true
            Layout.fillHeight: true
            RowLayout{

                Label{
                    text: "Классное руководство"
                }

                TextField {
                    id: classTeacher
                    placeholderText: "Классы"
                    Layout.fillWidth: true
                }
            }
        }

        Item{
            Layout.fillWidth: true
            Layout.fillHeight: true
            TextField {
                //id: teacherSubject2
                placeholderText: "Название предмета"
                Layout.fillWidth: true
            }
        }

        Item{
            Layout.fillWidth: true
            Layout.fillHeight: true
            RowLayout{

                Label{
                    text: "Личный кабинет"
                }

                TextField {
                    placeholderText: "Название кабинета"
                    Layout.fillWidth: true
                }
            }
        }

        Item{
            Layout.fillWidth: true
            Layout.fillHeight: true
            TextField {
                id: teacherSubject3
                placeholderText: "Название предмета"
                Layout.fillWidth: true
            }
        }

        Item{
            ListModel{
                id: weekDays
                ListElement{day: "Понедельник"}
                ListElement{day: "Вторник"}
                ListElement{day: "Среда"}
                ListElement{day: "Четверг"}
                ListElement{day: "Пятница"}
                ListElement{day: "Суббота"}
            }

            Layout.fillWidth: true
            Layout.fillHeight: true
            Column {
                spacing: 5

                Repeater {
                    model: weekDays

                    Row{
                        spacing: 10
                        Rectangle {
                            id: checkBox
                            width: 15
                            height: 15
                            border.color: "gray"
                            color: checked ? "lightgreen" : "white"

                            property bool checked: false

                            // Галочка (видна только когда checked = true)
                            Text {
                                text: "✓"
                                font.pixelSize: 15
                                color: "green"
                                anchors.centerIn: parent
                                visible: parent.checked
                            }

                            // Обработчик кликов
                            MouseArea {
                                anchors.fill: parent
                                onClicked: checkBox.checked = !checkBox.checked
                            }
                        }
                        Label {
                            text: day
                        }
                    }
                }
            }

        }
        Item{
            Layout.fillWidth: true
            Layout.fillHeight: true
            Button {
                text: "Добавить предмет"
                onClicked: {
                    //showPageRequested()
                    console.log("Нажата кнопка добавления предмета");
                }
            }
        }


    }



}
