import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Page {
    id: schoolDtailsPage
    signal showPageRequested(int pageIndex)

    property int schoolId: -1
    property string schoolName: ""
    property var roomModel: null
    property var teacherModel: null
    property var classModel: null
    property var lessonModel: null

    onSchoolIdChanged: {
        if (schoolId >= 0) {
            var schoolData = schoolModel.get(schoolId)
            if (schoolData) {
                schoolName = schoolData.name
                roomModel = schoolModel.roomModelAt(schoolId)
                teacherModel = schoolModel.teacherModelAt(schoolId)
                appState.teacherModel = teacherModel
                classModel = schoolModel.classModelAt(schoolId)
                appState.classModel = classModel
                lessonModel = schoolModel.lessonModelAt(schoolId)
                appState.lessonModel = lessonModel
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

        ToolButton {
            text: "Сгенерировать"
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            onClicked: {
                if (schoolId < 0) return
                var url = schoolModel.generateTimetablePdf(schoolId)
                console.log("PDF url:", url)
                if (url && url.length > 0)
                    Qt.openUrlExternally(url)
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
            Layout.alignment: Qt.AlignTop
            anchors.margins: 8

            Label {
                text: "Кабинеты"
                font.bold: true
            }

            ScrollView {
                Layout.fillWidth: true

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

            Item { // Костыль
                Layout.fillHeight: true
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
            Layout.alignment: Qt.AlignTop
            anchors.margins: 8

            Label {
                text: "Учителя"
                font.bold: true
            }

            ScrollView {
                Layout.fillWidth: true

                ListView {
                    id: teachersListView
                    width: parent.width
                    model: teacherModel
                    delegate: Rectangle {
                        width: parent.width
                        height: 48
                        color: "transparent"

                        RowLayout {
                            anchors.fill: parent
                            anchors.margins: 4
                            spacing: 8

                            Label {
                                text: surname + " " + name[0] + ". " + patronymic[0] + "."

                                MouseArea {
                                    anchors.fill: parent
                                    onClicked: {
                                        appState.teacherIndex = index
                                        appState.teacherIsNew = false
                                        showPageRequested(3)
                                        console.log("Показать информацию об учителе");
                                    }
                                }
                            }

                            Item { // Костыль, Label и Button у левой и правой границы соответственно
                                Layout.fillWidth: true
                            }

                            Button {
                                text: "Удалить"
                                onClicked: {
                                    teacherModel.removeAt(index)
                                    console.log("Удаляем учителя:", name)
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
                    onClicked: {
                        appState.teacherIndex = -1
                        appState.teacherIsNew = true
                        showPageRequested(3)
                    }
                }
            }
        }

        ColumnLayout {
            Layout.fillWidth: true
            Layout.preferredWidth: 1
            Layout.fillHeight: true
            Layout.alignment: Qt.AlignTop
            anchors.margins: 8

            Label {
                text: "Классы"
                font.bold: true
            }
            ScrollView {
                Layout.fillWidth: true

                ListView {
                    id: klassListView
                    width: parent.width
                    model: classModel
                    delegate: Rectangle {
                        width: parent.width
                        height: 48
                        color: "transparent"

                        RowLayout {
                            anchors.fill: parent
                            anchors.margins: 4
                            spacing: 8

                            Label {
                                id: nameClass
                                text: name

                                MouseArea {
                                    anchors.fill: parent
                                    onClicked: {
                                        showPageRequested(4)
                                        appState.schoolclassModel = classModel.classAt(index)
                                        console.log("Показать информацию о классе");
                                    }
                                }
                            }

                            Item { // Костыль, Label и Button у левой и правой границы соответственно
                                Layout.fillWidth: true
                            }

                            Button {
                                text: "Удалить"
                                onClicked: {
                                    classModel.removeAt(index)
                                    console.log("Удаляем класс:", name)
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
                spacing: 8
                Layout.alignment: Qt.AlignHCenter

                TextField {
                    id: newClassName
                    placeholderText: "Название класса"
                    Layout.fillWidth: true
                    onAccepted: addClass()
                }

                Button {
                    text: "Добавить"
                    onClicked: {
                        var name = newClassName.text.trim()
                        if (name.length === 0) {
                            console.log("Имя класса пустое - пропускаем")
                            return
                        }
                        classModel.appendClass(name)
                        newClassName.text = ""
                        newClassName.forceActiveFocus()
                        console.log("Добавлен класс:", name)
                    }
                }
            }
        }
    }
}
