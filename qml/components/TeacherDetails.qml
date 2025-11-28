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
            Label {
                text: "Классы в которых преподает учитель"
                font.bold: true
            }
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
                text: "8:"
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
                text: "8:"
                }

                TextField {
                    placeholderText: "Классы"
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
            Layout.fillWidth: true
            Layout.fillHeight: true
            RowLayout{

                Label{
                text: "8:"
                }

                TextField {
                    placeholderText: "Классы"
                    Layout.fillWidth: true
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
