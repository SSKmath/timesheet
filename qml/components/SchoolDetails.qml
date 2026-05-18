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

    function loadSchoolData() {
        if (schoolId < 0)
            return

        var schoolData = schoolModel.get(schoolId)
        if (!schoolData)
            return

        schoolName = schoolData.name
        roomModel = schoolModel.roomModelAt(schoolId)
        teacherModel = schoolModel.teacherModelAt(schoolId)
        classModel = schoolModel.classModelAt(schoolId)
        lessonModel = schoolModel.lessonModelAt(schoolId)

        appState.roomModel = roomModel
        appState.teacherModel = teacherModel
        appState.classModel = classModel
        appState.lessonModel = lessonModel

        appState.timetableModel.setRoomModel(roomModel)
        appState.timetableModel.setLessonModel(lessonModel)
        appState.timetableModel.setTeacherModel(teacherModel)
        appState.timetableModel.setRoomCount(roomModel ? roomModel.rowCount() : 0)
        appState.timetableModel.setSlotCount(6 * 8)

        console.log("Загружена школа:", schoolName, "с комнатами из C++ модели")
    }

    onVisibleChanged: {
        if (visible)
            loadSchoolData()
    }

    onSchoolIdChanged: loadSchoolData()

    header: ToolBar {
        ToolButton {
            text: "Назад"
            onClicked: {
                appState.roomModel = null
                appState.teacherModel = null
                appState.classModel = null
                appState.lessonModel = null

                appState.timetableModel.setRoomModel(null)
                appState.timetableModel.setLessonModel(null)
                appState.timetableModel.setRoomCount(0)

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

        Button {
            text: "Расписание"
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            onClicked: {
                showPageRequested(5)
            }
            background: Rectangle {
                radius: height / 2
                color: "#1976D2"
                border.color: "#1976D2"
                border.width: 1
            }
            contentItem: Text {
                text: parent.text
                color: "white"
                font.pixelSize: 14
                font.bold: true
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
        }
    }

    RowLayout {
        anchors.fill: parent
        spacing: 10
        anchors.margins: 8

        // ----- Кабинеты -----
        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.alignment: Qt.AlignTop
            spacing: 12

            Label {
                text: "Кабинеты"
                font.bold: true
                font.pixelSize: 16
            }

            ScrollView {
                Layout.fillWidth: true
                Layout.fillHeight: true
                clip: true
                padding: 8
                background: Rectangle {
                    color: "#f5f5f5"
                    radius: 8
                }

                ListView {
                    id: roomsListView
                    width: parent.width
                    model: roomModel
                    spacing: 8
                    clip: true
                    delegate: Rectangle {
                        width: roomsListView.width
                        height: 56
                        radius: 12
                        border.color: "#1976D2"
                        border.width: 1.5
                        color: "#fafafa"

                        RowLayout {
                            anchors.fill: parent
                            anchors.margins: 8
                            spacing: 8

                            TextField {
                                id: nameEditor
                                text: name
                                placeholderText: "Название кабинета"
                                Layout.fillWidth: true
                                font.pixelSize: 16
                                onEditingFinished: {
                                    var ind = roomsListView.model.index(index, 0)
                                    roomsListView.model.setData(ind, text, 1)
                                    console.log("Изменено имя:", text)
                                }
                                background: Rectangle {
                                    radius: height / 2
                                    color: parent.focus ? "#e8f0fe" : "#f5f5f5"
                                    border.color: "#c0c0c0"
                                    border.width: 1.5
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
                                background: Rectangle {
                                    radius: height / 5
                                    color: parent.focus ? "#e8f0fe" : "#f5f5f5"
                                    border.color: "#c0c0c0"
                                    border.width: 1.5
                                    implicitHeight: 25
                                }
                                font.pixelSize: 16
                            }

                            Button {
                                text: "✕"
                                onClicked: {
                                    roomModel.removeAt(index)
                                    console.log("Удаляем кабинет:", name)
                                }
                                background: Rectangle {
                                    radius: height / 2
                                    color: parent.hovered ? "#ffcdd2" : "#f5f5f5"
                                    border.color: "#d32f2f"
                                    border.width: 1
                                }
                                contentItem: Text {
                                    text: parent.text
                                    font.pixelSize: 14
                                    font.bold: true
                                    color: "#d32f2f"
                                    horizontalAlignment: Text.AlignHCenter
                                    verticalAlignment: Text.AlignVCenter
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
                    font.pixelSize: 16
                    onAccepted: addRoom()
                    background: Rectangle {
                        radius: height / 2
                        color: parent.focus ? "#e8f0fe" : "#f5f5f5"
                        border.color: "#1976D2"
                        border.width: 1.5
                    }
                }

                ComboBox {
                    id: newRoomSize
                    model: ["Маленький", "Большой"]
                    currentIndex: 0
                    Layout.preferredWidth: 120
                    font.pixelSize: 16
                    background: Rectangle {
                        radius: height / 4
                        color: parent.focus ? "#c0c0c0" : "#f5f5f5"
                        border.color: "#778899"
                        border.width: 1.5
                        implicitHeight: 25
                    }
                }

                Button {
                    text: "Добавить"
                    onClicked: {
                        var name = newRoomName.text.trim()
                        if (name.length === 0) return
                        roomModel.appendRoom(name, newRoomSize.currentText)
                        newRoomName.text = ""
                        newRoomName.forceActiveFocus()
                        console.log("Добавлен кабинет:", name, newRoomSize.currentText)
                    }
                    background: Rectangle {
                        radius: 5
                        color: "#1976D2"
                        border.color: "#1976D2"
                        border.width: 1
                    }
                    contentItem: Text {
                        text: parent.text
                        color: "white"
                        font.pixelSize: 14
                        font.bold: true
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                }
            }
        }

        // ----- Учителя -----
        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.alignment: Qt.AlignTop
            spacing: 12

            Label {
                text: "Учителя"
                font.bold: true
                font.pixelSize: 16
            }

            ScrollView {
                Layout.fillWidth: true
                Layout.fillHeight: true
                clip: true
                padding: 8
                background: Rectangle {
                    color: "#f5f5f5"
                    radius: 8
                }

                ListView {
                    id: teachersListView
                    width: parent.width
                    model: teacherModel
                    spacing: 8
                    clip: true
                    delegate: Rectangle {
                        width: teachersListView.width
                        height: 56
                        radius: 12
                        border.color: "#1976D2"
                        border.width: 1.5
                        color: "#fafafa"

                        RowLayout {
                            anchors.fill: parent
                            anchors.margins: 8
                            spacing: 8

                            Label {
                                text: surname + " " + name[0] + ". " + patronymic[0] + "."
                                font.pixelSize: 16
                                Layout.fillWidth: true
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

                            Item { Layout.fillWidth: true }

                            Button {
                                text: "✕"
                                onClicked: {
                                    teacherModel.removeAt(index)
                                    console.log("Удаляем учителя:", name)
                                }
                                background: Rectangle {
                                    radius: height / 2
                                    color: parent.hovered ? "#ffcdd2" : "#f5f5f5"
                                    border.color: "#d32f2f"
                                    border.width: 1
                                }
                                contentItem: Text {
                                    text: parent.text
                                    font.pixelSize: 14
                                    font.bold: true
                                    color: "#d32f2f"
                                    horizontalAlignment: Text.AlignHCenter
                                    verticalAlignment: Text.AlignVCenter
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
                    background: Rectangle {
                        radius: 5
                        color: "#1976D2"
                        border.color: "#1976D2"
                        border.width: 1
                    }
                    contentItem: Text {
                        text: parent.text
                        color: "white"
                        font.pixelSize: 14
                        font.bold: true
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                }
            }
        }

        // ----- Классы -----
        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.alignment: Qt.AlignTop
            spacing: 12

            Label {
                text: "Классы"
                font.bold: true
                font.pixelSize: 16
            }

            ScrollView {
                Layout.fillWidth: true
                Layout.fillHeight: true
                clip: true
                padding: 8
                background: Rectangle {
                    color: "#f5f5f5"
                    radius: 8
                }

                ListView {
                    id: klassListView
                    width: parent.width
                    model: classModel
                    spacing: 8
                    clip: true
                    delegate: Rectangle {
                        width: klassListView.width
                        height: 56
                        radius: 12
                        border.color: "#1976D2"
                        border.width: 1.5
                        color: "#fafafa"

                        RowLayout {
                            anchors.fill: parent
                            anchors.margins: 8
                            spacing: 8

                            Label {
                                id: nameClass
                                text: name
                                font.pixelSize: 16
                                Layout.fillWidth: true
                                MouseArea {
                                    anchors.fill: parent
                                    onClicked: {
                                        showPageRequested(4)
                                        appState.schoolclassModel = classModel.classAt(index)
                                        console.log("Показать информацию о классе");
                                    }
                                }
                            }

                            Item { Layout.fillWidth: true }

                            Button {
                                text: "✕"
                                onClicked: {
                                    classModel.removeAt(index)
                                    console.log("Удаляем класс:", name)
                                }
                                background: Rectangle {
                                    radius: height / 2
                                    color: parent.hovered ? "#ffcdd2" : "#f5f5f5"
                                    border.color: "#d32f2f"
                                    border.width: 1
                                }
                                contentItem: Text {
                                    text: parent.text
                                    font.pixelSize: 14
                                    font.bold: true
                                    color: "#d32f2f"
                                    horizontalAlignment: Text.AlignHCenter
                                    verticalAlignment: Text.AlignVCenter
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
                    id: newClassName
                    placeholderText: "Название класса"
                    Layout.fillWidth: true
                    font.pixelSize: 16
                    onAccepted: addClass()
                    background: Rectangle {
                        radius: height / 2
                        color: parent.focus ? "#e8f0fe" : "#f5f5f5"
                        border.color: "#1976D2"
                        border.width: 1.5
                    }
                }

                Button {
                    text: "Добавить"
                    onClicked: {
                        var name = newClassName.text.trim()
                        if (name.length === 0) return
                        classModel.appendClass(name)
                        newClassName.text = ""
                        newClassName.forceActiveFocus()
                        console.log("Добавлен класс:", name)
                    }
                    background: Rectangle {
                        radius: 5
                        color: "#1976D2"
                        border.color: "#1976D2"
                        border.width: 1
                    }
                    contentItem: Text {
                        text: parent.text
                        color: "white"
                        font.pixelSize: 14
                        font.bold: true
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                }
            }
        }
    }
}
