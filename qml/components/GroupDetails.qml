import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Page {
    id: teacherDetailsPage
    signal showPageRequested(int pageIndex)

    readonly property var schoolclassModel: appState.schoolclassModel
    readonly property var lessonModel: appState.lessonModel

    header: ToolBar {
        ToolButton {
            text: "Назад"
            onClicked: {
                showPageRequested(2)
                console.log("Нажата кнопка назад")
            }
        }
    }

    ColumnLayout {
        anchors.fill: parent
        //spacing: 10

        RowLayout {
            Label {
                //anchors.margins: 8
                text: schoolclassModel === null ? "error" : schoolclassModel.name
                font.bold: true
            }

            Item { // Костыль
                Layout.fillWidth: true
            }

            Label {
                //anchors.margins: 8
                text: "Классный руководитель"
                font.bold: true
            }

            TextField{
                id: nameClassTeacher
                //text:
                placeholderText: "Имя преподавателя"
                //Layout.fillWidth: true
            }
        }

        Label {
            //anchors.margins: 8
            text: "Предметы"
            font.bold: true
        }

        ColumnLayout {
            Layout.fillWidth: true
            Layout.preferredWidth: 1
            Layout.fillHeight: true
            Layout.alignment: Qt.AlignTop
            anchors.margins: 8

            ScrollView {
                Layout.fillWidth: true

                ListView {
                    id: subjectListView
                    width: parent.width
                    model: lessonModel
                    delegate: Rectangle {
                        width: subjectListView.width
                        height: 48
                        color: "transparent"
                        RowLayout {
                            anchors.fill: parent
                            anchors.margins: 4
                            spacing: 8

                            TextField {
                                id: subjectEditor
                                //text: subject
                                text: "text"
                                placeholderText: "Название предмета"
                                Layout.fillWidth: true
                                onEditingFinished: {
                                    //var ind = roomsListView.model.index(index, 0)
                                    //roomsListView.model.setData(ind, text, 1)
                                    //console.log("Изменено имя:", text)
                                }
                            }

                            TextField {
                                id: nameEditor
                                text: name
                                placeholderText: "Имя преподавателя"
                                Layout.fillWidth: true
                                onEditingFinished: {
                                    //var ind = roomsListView.model.index(index, 0)
                                    //roomsListView.model.setData(ind, text, 1)
                                    //console.log("Изменено имя:", text)
                                }
                            }

                            Label {
                                text: "Количество уроков неделю"
                            }

                            Button {
                                text: "-"
                                onClicked: {
                                    subjectCount--;
                                }
                            }

                            Label {
                                text: subjectCount
                            }

                            Button {
                                text: "+"
                                onClicked: {
                                    subjectCount++;
                                }
                            }



                            ComboBox {
                                id: teacherCombo
                                model: ["Учитель 1", " Учитель 2"]
                                currentIndex: size === "Учетель 1" ? 1 : 0
                                onCurrentTextChanged: {
                                    //var idx = roomsListView.model.index(index, 0)
                                    //roomsListView.model.setData(idx, currentText, 2)
                                    //console.log("Изменена парность", name, "->", currentText)
                                }
                            }

                            ComboBox {
                                id: pairCombo
                                model: ["Парный", "Одинарный"]
                                currentIndex: size === "Парный" ? 1 : 0
                                onCurrentTextChanged: {
                                    //var idx = roomsListView.model.index(index, 0)
                                    //roomsListView.model.setData(idx, currentText, 2)
                                    //console.log("Изменена парность", name, "->", currentText)
                                }
                            }

                            Button {
                                icon.source: "trash.png"
                                display: AbstractButton.TextBesideIcon
                                onClicked: {
                                    roomModel.removeAt(index)
                                    console.log("Удаляем кабинет:", name)
                                }
                            }
                        }
                    }
                }
            }


        //Item { // Костыль, Label и Button у левой и правой границы соответственно
            //Layout.fillWidth: true
        //}

                            /*Button {
                                text: "Удалить"
                                onClicked: {
                                    //roomModel.removeAt(index)
                                    console.log("Удаляем предмет:", name)
                                }
                            }
                        }
                    }
                }
            }

            Item { // Костыль, Button снизу
                Layout.fillHeight: true
            }

            RowLayout {
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignHCenter
                Button {
                    text: "Добавить"
                }
            }
            */
        }
    }
}
