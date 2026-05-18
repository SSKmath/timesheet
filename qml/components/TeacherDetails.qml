import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Basic

Page {
    id: teacherDetailsPage
    signal showPageRequested(int pageIndex)

    readonly property var model: appState.teacherModel
    readonly property int tIndex: appState.teacherIndex
    readonly property bool isNew: appState.teacherIsNew

    ListModel {
        id: weekDays
        ListElement { day: "Понедельник"; checked: false }
        ListElement { day: "Вторник"; checked: false }
        ListElement { day: "Среда"; checked: false }
        ListElement { day: "Четверг"; checked: false }
        ListElement { day: "Пятница"; checked: false }
        ListElement { day: "Суббота"; checked: false }
    }

    header: ToolBar {
        ToolButton {
            text: "Назад"
            onClicked: {
                showPageRequested(2)
                console.log("Нажата кнопка назад")
            }
        }
        Label {
            text: isNew ? "Добавить учителя" : "Информация об учителе"
            font.bold: true
            font.pixelSize: 18
            anchors.centerIn: parent
        }
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 12
        anchors.margins: 16

        // ФИО
        RowLayout {
            Layout.fillWidth: true
            spacing: 10

            TextField {
                id: teacherSurName
                placeholderText: "Фамилия"
                Layout.fillWidth: true
                font.pixelSize: 16
                background: Rectangle {
                    radius: height / 2
                    color: parent.focus ? "#e8f0fe" : "#f5f5f5"
                    border.color: "#1976D2"
                    border.width: 1.5
                }
            }
            TextField {
                id: teacherName
                placeholderText: "Имя"
                Layout.fillWidth: true
                font.pixelSize: 16
                background: Rectangle {
                    radius: height / 2
                    color: parent.focus ? "#e8f0fe" : "#f5f5f5"
                    border.color: "#1976D2"
                    border.width: 1.5
                }
            }
            TextField {
                id: teacherPatronymic
                placeholderText: "Отчество"
                Layout.fillWidth: true
                font.pixelSize: 16
                background: Rectangle {
                    radius: height / 2
                    color: parent.focus ? "#e8f0fe" : "#f5f5f5"
                    border.color: "#1976D2"
                    border.width: 1.5
                }
            }
        }

        GridLayout {
            columns: 2
            rows: 5
            rowSpacing: 20
            columnSpacing: 10
            Layout.fillWidth: true
            Layout.fillHeight: true

            // Заголовок "Предметы"
            Item {
                Layout.fillWidth: true
                Layout.fillHeight: true
                Label {
                    text: "Предметы"
                    font.bold: true
                    font.pixelSize: 16
                    anchors.left: parent.left
                    anchors.leftMargin: 4
                    anchors.verticalCenter: parent.verticalCenter
                }
            }
            Item { }

            // Первое поле предмета
            Item {
                Layout.fillWidth: true
                Layout.fillHeight: true
                TextField {
                    id: teacherSubject1
                    placeholderText: "Название предмета"
                    Layout.fillWidth: true
                    font.pixelSize: 16
                    anchors.verticalCenter: parent.verticalCenter
                    background: Rectangle {
                        radius: height / 2
                        color: parent.focus ? "#e8f0fe" : "#f5f5f5"
                        border.color: "#1976D2"
                        border.width: 1.5
                    }
                }
            }
            // Классное руководство
            Item {
                Layout.fillWidth: true
                Layout.fillHeight: true
                RowLayout {
                    anchors.fill: parent
                    spacing: 8
                    Label {
                        text: "Классное руководство"
                        font.pixelSize: 16
                    }
                    TextField {
                        id: classTeacher
                        placeholderText: "Классы"
                        Layout.fillWidth: true
                        font.pixelSize: 16
                        background: Rectangle {
                            radius: height / 2
                            color: parent.focus ? "#e8f0fe" : "#f5f5f5"
                            border.color: "#1976D2"
                            border.width: 1.5
                        }
                    }
                }
            }

            // Второе поле предмета
            Item {
                Layout.fillWidth: true
                Layout.fillHeight: true
                TextField {
                    id: teacherSubject2
                    placeholderText: "Название предмета"
                    Layout.fillWidth: true
                    font.pixelSize: 16
                    anchors.verticalCenter: parent.verticalCenter
                    background: Rectangle {
                        radius: height / 2
                        color: parent.focus ? "#e8f0fe" : "#f5f5f5"
                        border.color: "#1976D2"
                        border.width: 1.5
                    }
                }
            }
            // Личный кабинет
            Item {
                Layout.fillWidth: true
                Layout.fillHeight: true
                RowLayout {
                    anchors.fill: parent
                    spacing: 8
                    Label {
                        text: "Личный кабинет"
                        font.pixelSize: 16
                    }
                    TextField {
                        id: teacherCabinet
                        placeholderText: "Название кабинета"
                        Layout.fillWidth: true
                        font.pixelSize: 16
                        background: Rectangle {
                            radius: height / 2
                            color: parent.focus ? "#e8f0fe" : "#f5f5f5"
                            border.color: "#1976D2"
                            border.width: 1.5
                        }
                    }
                }
            }

            // Третье поле предмета
            Item {
                Layout.fillWidth: true
                Layout.fillHeight: true
                TextField {
                    id: teacherSubject3
                    placeholderText: "Название предмета"
                    Layout.fillWidth: true
                    font.pixelSize: 16
                    anchors.verticalCenter: parent.verticalCenter
                    background: Rectangle {
                        radius: height / 2
                        color: parent.focus ? "#e8f0fe" : "#f5f5f5"
                        border.color: "#1976D2"
                        border.width: 1.5
                    }
                }
            }
            // Рабочие дни
            Item {
                Layout.fillWidth: true
                Layout.fillHeight: true
                Rectangle {
                    anchors.fill: parent
                    anchors.margins: 4
                    color: "transparent"
                    radius: 8
                    Column {
                        spacing: 5
                        anchors.centerIn: parent
                        Repeater {
                            model: weekDays
                            delegate: Row {
                                spacing: 10
                                Rectangle {
                                    width: 20
                                    height: 20
                                    radius: 4
                                    border.color: "#1976D2"
                                    border.width: 1.5
                                    color: model.checked ? "#1976D2" : "white"
                                    Text {
                                        text: "✓"
                                        font.pixelSize: 14
                                        color: "white"
                                        anchors.centerIn: parent
                                        visible: model.checked
                                    }
                                    MouseArea {
                                        anchors.fill: parent
                                        onClicked: weekDays.setProperty(index, "checked", !model.checked)
                                    }
                                }
                                Label {
                                    text: day
                                    font.pixelSize: 16
                                }
                            }
                        }
                    }
                }
            }

            // Кнопка "Добавить предмет"
            Item {
                Layout.fillWidth: true
                Layout.fillHeight: true
                Button {
                    text: "Добавить предмет"
                    anchors.left: parent.left
                    anchors.leftMargin: 4
                    anchors.verticalCenter: parent.verticalCenter
                    onClicked: console.log("Нажата кнопка добавления предмета")
                    background: Rectangle {
                        radius: height / 2
                        color: parent.hovered ? "#e3f2fd" : "#f5f5f5"
                        border.color: "#1976D2"
                        border.width: 1.5
                    }
                    contentItem: Text {
                        text: parent.text
                        font.pixelSize: 16
                        font.bold: true
                        color: "#1976D2"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                }
            }
            Item { }
        }

        // Нижние кнопки
        RowLayout {
            Layout.fillWidth: true
            spacing: 16

            Button {
                id: addButton
                visible: appState.teacherIsNew === true
                text: "Добавить"
                Layout.fillWidth: true
                onClicked: {
                    var data = collectFormData()
                    console.log("Добавление учителя:", JSON.stringify(data))
                    model.appendTeacher(data.surname, data.name, data.patronymic, "subject", data.weekdays)
                    showPageRequested(2)
                }
                background: Rectangle {
                    radius: height / 2
                    color: "#1976D2"
                    border.color: "#1976D2"
                    border.width: 1
                }
                contentItem: Text {
                    text: parent.text
                    font.pixelSize: 16
                    font.bold: true
                    color: "white"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }

            Button {
                id: saveButton
                visible: appState.teacherIsNew === false
                text: "Сохранить изменения"
                Layout.fillWidth: true
                onClicked: {
                    var data = collectFormData()
                    console.log(data)
                    appState.teacherModel.setData(appState.teacherModel.index(appState.teacherIndex, 0), data.surname, 0)
                    appState.teacherModel.setData(appState.teacherModel.index(appState.teacherIndex, 0), data.name, 1)
                    appState.teacherModel.setData(appState.teacherModel.index(appState.teacherIndex, 0), data.patronymic, 2)
                    appState.teacherModel.setData(appState.teacherModel.index(appState.teacherIndex, 0), "subject", 3)
                    appState.teacherModel.setData(appState.teacherModel.index(appState.teacherIndex, 0), data.weekdays, 4)
                    showPageRequested(2)
                }
                background: Rectangle {
                    radius: height / 2
                    color: "#1976D2"
                    border.color: "#1976D2"
                    border.width: 1
                }
                contentItem: Text {
                    text: parent.text
                    font.pixelSize: 16
                    font.bold: true
                    color: "white"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }

            Button {
                text: "Отмена"
                Layout.fillWidth: true
                onClicked: showPageRequested(2)
                background: Rectangle {
                    radius: height / 2
                    color: "#f0f0f0"
                    border.color: "#1976D2"
                    border.width: 2
                }
                contentItem: Text {
                    text: parent.text
                    font.pixelSize: 16
                    font.bold: true
                    color: "#1976D2"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }
        }
    }

    // Загрузка данных
    onVisibleChanged: {
        if (visible) {
            if (appState.teacherIsNew) {
                teacherSurName.text = ""
                teacherName.text = ""
                teacherPatronymic.text = ""
                for (var i = 0; i < weekDays.count; ++i)
                    weekDays.setProperty(i, "checked", false)
            } else {
                var teacherObj = appState.teacherModel.teacherAt(appState.teacherIndex);
                console.log(teacherObj.workingDays)
                teacherSurName.text = teacherObj.surname
                teacherName.text = teacherObj.name
                teacherPatronymic.text = teacherObj.patronymic
                if (teacherObj.workingDays) {
                    for (var j = 0; j < weekDays.count; ++j)
                        weekDays.setProperty(j, "checked", teacherObj.workingDays[j])
                }
            }
        }
    }

    function collectFormData() {
        var weekdaysArr = []
        for (var i = 0; i < weekDays.count; ++i)
            weekdaysArr.push(weekDays.get(i).checked)
        return {
            surname: teacherSurName.text,
            name: teacherName.text,
            patronymic: teacherPatronymic.text,
            weekdays: weekdaysArr
        }
    }
}
