import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Page {
    id: teacherDetailsPage
    signal showPageRequested(int pageIndex)

    readonly property var model: appState.teacherModel
    readonly property int tIndex: appState.teacherIndex
    readonly property bool isNew: appState.teacherIsNew

    ListModel {
        id: weekDays
        ListElement{day: "Понедельник"; checked: false}
        ListElement{day: "Вторник"; checked: false}
        ListElement{day: "Среда"; checked: false}
        ListElement{day: "Четверг"; checked: false}
        ListElement{day: "Пятница"; checked: false}
        ListElement{day: "Суббота"; checked: false}
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
            anchors.centerIn: parent
        }
    }

    RowLayout {
        anchors.margins: 8
        anchors.left: parent.left
        anchors.right: parent.right
        spacing: 10

        TextField {
            id: teacherSurName
            placeholderText: "Фамилия"
            Layout.fillWidth: true
        }

        TextField {
            id: teacherName
            placeholderText: "Имя"
            Layout.fillWidth: true
        }

        TextField {
            id: teacherPatronymic
            placeholderText: "Отчество"
            Layout.fillWidth: true
        }
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

        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Column {
                spacing: 5

                Repeater {
                    id: weekRepeater
                    model: weekDays

                    Row {
                        spacing: 10
                        Rectangle {
                            id: checkBox
                            width: 15
                            height: 15
                            border.color: "gray"
                            color: model.checked ? "lightgreen" : "white"

                            property bool checked: false

                            Text {
                                text: "✓"
                                font.pixelSize: 15
                                color: "green"
                                anchors.centerIn: parent
                                visible: model.checked
                            }

                            MouseArea {
                                anchors.fill: parent
                                onClicked: {
                                    weekDays.setProperty(index, "checked", !model.checked)
                                }
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

    RowLayout {
        Layout.fillWidth: true
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: 16

        Button {
            id: addButton
            visible: appState.teacherIsNew === true
            text: "Добавить"
            onClicked: {
                var data = collectFormData()
                console.log("Добавление учителя:", JSON.stringify(data))
                model.appendTeacher(data.surname, data.name, data.patronymic, "subject", data.weekdays)
                showPageRequested(2)
            }
        }

        Button {
            id: saveButton
            visible: appState.teacherIsNew === false
            text: "Сохранить изменения"
            onClicked: {
                var data = collectFormData()
                console.log(data)
                appState.teacherModel.setData(appState.teacherModel.index(appState.teacherIndex, 0), data.surname, 0)
                appState.teacherModel.setData(appState.teacherModel.index(appState.teacherIndex, 0), data.name, 1)
                appState.teacherModel.setData(appState.teacherModel.index(appState.teacherIndex, 0), data.patronymic, 2) // жёсткие костыли,
                appState.teacherModel.setData(appState.teacherModel.index(appState.teacherIndex, 0), "subject", 3)       // я не понимаю, почему
                appState.teacherModel.setData(appState.teacherModel.index(appState.teacherIndex, 0), data.weekdays, 4)   // роли не работают
                showPageRequested(2)
            }
        }
    }

    onVisibleChanged: {
        if (visible) {
            if (appState.teacherIsNew)
            {
                teacherSurName.text = ""
                teacherName.text = ""
                teacherPatronymic.text = ""
                for (var i = 0; i < weekDays.count; ++i) {
                    weekDays.setProperty(i, "checked", false)
                }
            }
            else
            {
                var teacherObj = appState.teacherModel.teacherAt(appState.teacherIndex);
                console.log(teacherObj.workingDays)
                teacherSurName.text = teacherObj.surname
                teacherName.text = teacherObj.name
                teacherPatronymic.text = teacherObj.patronymic
                for (var j = 0; j < weekDays.count; ++j) {
                    weekDays.setProperty(j, "checked", teacherObj.workingDays[j])
                }
            }
        }
    }

    function collectFormData() {
        var weekdaysArr = []
        for (var i = 0; i < weekDays.count; ++i) {
            weekdaysArr.push(weekDays.get(i).checked)
        }

        return {
            surname: teacherSurName.text,
            name: teacherName.text,
            patronymic: teacherPatronymic.text,
            weekdays: weekdaysArr
        }
    }
}
