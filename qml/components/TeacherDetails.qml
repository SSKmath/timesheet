import QtQuick 6.0
import QtQuick.Controls 6.0
import QtQuick.Layouts 6.0
import QtQml.Models 6.0

Page {
    id: teacherDetailsPage
    signal showPageRequested(int pageIndex)

    // --- Поддержка внешнего appState (из контекста) или создание заглушки ---
    property var actualAppState: (typeof appState !== 'undefined') ? appState : null

    QtObject {
        id: internal
        property var appState: actualAppState ? actualAppState : dummyAppState
    }

    // Заглушка для работы без внешнего appState
    QtObject {
        id: dummyAppState
        property ListModel teacherModel: ListModel {}
        property int teacherIndex: 0
        property bool teacherIsNew: false  // сразу покажем тестового учителя

        function appendTeacher(surname, name, patronymic, mainSubject, additionalSubject, classLeadership, cabinet, workingDays) {
            teacherModel.append({
                surname: surname,
                name: name,
                patronymic: patronymic,
                mainSubject: mainSubject,
                additionalSubject: additionalSubject,
                classLeadership: classLeadership,
                cabinet: cabinet,
                workingDays: workingDays
            });
        }

        function teacherAt(index) {
            if (index >= 0 && index < teacherModel.count)
                return teacherModel.get(index);
            return null;
        }

        function updateTeacher(index, surname, name, patronymic, mainSubject, additionalSubject, classLeadership, cabinet, workingDays) {
            if (index >= 0 && index < teacherModel.count) {
                var item = teacherModel.get(index);
                item.surname = surname;
                item.name = name;
                item.patronymic = patronymic;
                item.mainSubject = mainSubject;
                item.additionalSubject = additionalSubject;
                item.classLeadership = classLeadership;
                item.cabinet = cabinet;
                item.workingDays = workingDays;
                teacherModel.set(index, item);
            }
        }
    }

    // Добавляем тестового учителя в заглушку
    Component.onCompleted: {
        if (!actualAppState) {
            dummyAppState.teacherModel.append({
                surname: "Иванов",
                name: "Иван",
                patronymic: "Иванович",
                mainSubject: "Математика",
                additionalSubject: "Физика",
                classLeadership: "10А",
                cabinet: "301",
                workingDays: [false, true, true, true, false, false]
            });
            dummyAppState.teacherIndex = 0;
        }
    }

    // Для удобства внутри кода используем internal.appState
    readonly property var model: internal.appState.teacherModel
    readonly property int tIndex: internal.appState.teacherIndex
    readonly property bool isNew: internal.appState.teacherIsNew

    // Модель дней недели
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
            anchors.centerIn: parent
        }
    }

    // Основной контейнер
    Item {
        id: mainContent
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: buttonRow.top
        anchors.margins: 16

        ColumnLayout {
            anchors.fill: parent
            spacing: 20

            // Блок ФИО
            ColumnLayout {
                spacing: 8
                Layout.fillWidth: true

                Label {
                    text: "Информация об учителе"
                    font.bold: true
                    font.pixelSize: 18
                    color: "#333"
                }

                RowLayout {
                    id: fioRow
                    Layout.fillWidth: true
                    spacing: 10

                    TextField {
                        id: teacherSurName
                        placeholderText: "Фамилия"
                        Layout.fillWidth: true
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
                        background: Rectangle {
                            radius: height / 2
                            color: parent.focus ? "#e8f0fe" : "#f5f5f5"
                            border.color: "#1976D2"
                            border.width: 1.5
                        }
                    }
                }
            }

            // Две колонки + отступ справа
            RowLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true
                spacing: 20

                // Левая колонка: Предметы и нагрузка
                ColumnLayout {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    Layout.preferredWidth: 3
                    spacing: 12

                    Label {
                        text: "Предметы и нагрузка"
                        font.bold: true
                        font.pixelSize: 18
                        color: "#333"
                    }

                    // Основной предмет
                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 8
                        Label {
                            text: "Основной предмет"
                            color: "#333"
                        }
                        TextField {
                            id: teacherSubject1
                            placeholderText: "Введите предмет"
                            Layout.fillWidth: true
                            background: Rectangle {
                                radius: height / 2
                                color: parent.focus ? "#e8f0fe" : "#f5f5f5"
                                border.color: "#1976D2"
                                border.width: 1.5
                            }
                        }
                    }

                    // Дополнительный предмет
                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 8
                        Label {
                            text: "Дополнительный предмет"
                            color: "#333"
                        }
                        TextField {
                            id: teacherSubject3
                            placeholderText: "Введите предмет"
                            Layout.fillWidth: true
                            background: Rectangle {
                                radius: height / 2
                                color: parent.focus ? "#e8f0fe" : "#f5f5f5"
                                border.color: "#1976D2"
                                border.width: 1.5
                            }
                        }
                    }

                    // Классное руководство
                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 8
                        Label {
                            text: "Классное руководство"
                            color: "#333"
                        }
                        TextField {
                            id: classTeacher
                            placeholderText: "Классы (например, 7А, 7Б)"
                            Layout.fillWidth: true
                            background: Rectangle {
                                radius: height / 2
                                color: parent.focus ? "#e8f0fe" : "#f5f5f5"
                                border.color: "#1976D2"
                                border.width: 1.5
                            }
                        }
                    }

                    // Личный кабинет
                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 8
                        Label {
                            text: "Личный кабинет"
                            color: "#333"
                        }
                        TextField {
                            id: teacherCabinet
                            placeholderText: "Номер или название"
                            Layout.fillWidth: true
                            background: Rectangle {
                                radius: height / 2
                                color: parent.focus ? "#e8f0fe" : "#f5f5f5"
                                border.color: "#1976D2"
                                border.width: 1.5
                            }
                        }
                    }

                    // Кнопка "Добавить предмет" (на всю ширину)
                    Button {
                        text: "Добавить предмет"
                        Layout.fillWidth: true
                        Layout.topMargin: 8
                        focusPolicy: Qt.NoFocus
                        flat: true

                        background: Rectangle {
                            radius: height / 2
                            color: {
                                if (parent.pressed) return "#b0b0b0"
                                if (parent.hovered) return "#c0c0c0"
                                return "#e0e0e0"
                            }
                            border.color: parent.hovered ? "#999" : "#ccc"
                            border.width: 1
                        }

                        contentItem: Text {
                            text: parent.text
                            color: "#333"
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                            font.pixelSize: 14
                        }

                        onClicked: console.log("Нажата кнопка добавления предмета")
                    }

                    // Заполнитель
                    Item { Layout.fillHeight: true }
                }

                // Правая колонка: Рабочие дни
                ColumnLayout {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    Layout.preferredWidth: 2
                    spacing: 8

                    Label {
                        text: "Рабочие дни"
                        font.bold: true
                        font.pixelSize: 18
                        color: "#333"
                    }

                    Column {
                        spacing: 5
                        Repeater {
                            model: weekDays
                            delegate: Row {
                                spacing: 10
                                Rectangle {
                                    width: 15
                                    height: 15
                                    border.color: "gray"
                                    color: model.checked ? "lightgreen" : "white"
                                    Text {
                                        text: "✓"
                                        font.pixelSize: 15
                                        color: "green"
                                        anchors.centerIn: parent
                                        visible: model.checked
                                    }
                                    MouseArea {
                                        anchors.fill: parent
                                        onClicked: weekDays.setProperty(index, "checked", !model.checked)
                                    }
                                }
                                Label { text: day }
                            }
                        }
                    }

                    Item { Layout.fillHeight: true }
                }

                // Пустой элемент справа (отступ)
                Item {
                    Layout.preferredWidth: 40
                    Layout.fillHeight: true
                }
            }
        }
    }

    // Нижние кнопки
    RowLayout {
        id: buttonRow
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: 16
        spacing: 16

        Button {
            id: actionButton
            text: isNew ? "Добавить" : "Сохранить изменения"
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
                var data = collectFormData()
                if (isNew) {
                    internal.appState.appendTeacher(
                        data.surname,
                        data.name,
                        data.patronymic,
                        data.mainSubject,
                        data.additionalSubject,
                        data.classLeadership,
                        data.cabinet,
                        data.weekdays
                    );
                } else {
                    internal.appState.updateTeacher(
                        tIndex,
                        data.surname,
                        data.name,
                        data.patronymic,
                        data.mainSubject,
                        data.additionalSubject,
                        data.classLeadership,
                        data.cabinet,
                        data.weekdays
                    );
                }
                showPageRequested(2)
            }
        }

        Button {
            text: "Отменить"
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
                showPageRequested(2)
                console.log("Отмена")
            }
        }
    }

    // Загрузка данных при открытии
    onVisibleChanged: {
        if (visible) {
            if (isNew) {
                teacherSurName.text = ""
                teacherName.text = ""
                teacherPatronymic.text = ""
                teacherSubject1.text = ""
                teacherSubject3.text = ""
                classTeacher.text = ""
                teacherCabinet.text = ""
                for (var i = 0; i < weekDays.count; ++i)
                    weekDays.setProperty(i, "checked", false)
            } else {
                var teacherObj = internal.appState.teacherAt(tIndex)
                if (teacherObj) {
                    teacherSurName.text = teacherObj.surname || ""
                    teacherName.text = teacherObj.name || ""
                    teacherPatronymic.text = teacherObj.patronymic || ""
                    teacherSubject1.text = teacherObj.mainSubject || ""
                    teacherSubject3.text = teacherObj.additionalSubject || ""
                    classTeacher.text = teacherObj.classLeadership || ""
                    teacherCabinet.text = teacherObj.cabinet || ""
                    var days = teacherObj.workingDays || []
                    for (var j = 0; j < weekDays.count; ++j) {
                        weekDays.setProperty(j, "checked", days[j] || false)
                    }
                }
            }
        }
    }

    // Сбор данных формы
    function collectFormData() {
        var weekdaysArr = []
        for (var i = 0; i < weekDays.count; ++i)
            weekdaysArr.push(weekDays.get(i).checked)
        return {
            surname: teacherSurName.text,
            name: teacherName.text,
            patronymic: teacherPatronymic.text,
            mainSubject: teacherSubject1.text,
            additionalSubject: teacherSubject3.text,
            classLeadership: classTeacher.text,
            cabinet: teacherCabinet.text,
            weekdays: weekdaysArr
        }
    }
}
