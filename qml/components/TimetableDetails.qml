// TimetableDetails.qml


// TimetableDetails.qml (упрощённая версия)
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt.labs.qmlmodels  // для TableModel

Page {
    id: timetableDetailsPage
    width: 800; height: 600

    signal showPageRequested(int pageIndex)

    // Пример списка уроков
    ListModel {
        id: lessonModelExample
        ListElement { lessonId: 1; name: "Алгебра"; teacherSurname: "Иванов" }
        ListElement { lessonId: 2; name: "Геометрия"; teacherSurname: "Петров" }
        ListElement { lessonId: 3; name: "Физика"; teacherSurname: "Сидоров" }
        ListElement { lessonId: 4; name: "Химия"; teacherSurname: "Соколов" }
        ListElement { lessonId: 5; name: "Биология"; teacherSurname: "Кузнецов" }
    }

    header: ToolBar {
        RowLayout {
            anchors.fill: parent; spacing: 8
            ToolButton { text: "Назад"; onClicked: showPageRequested(2) }
            Label { text: "Расписание"; font.bold: true }
            Item { Layout.fillWidth: true }
        }
    }

    RowLayout {
        anchors.fill: parent
        spacing: 8

        // Левая часть: таблица с заголовками
        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true

            Rectangle {
                anchors.fill: parent
                color: "white"
                border.color: "#d0d0d0"
            }

            // Горизонтальный заголовок (кабинеты 1..5)
            HorizontalHeaderView {
                id: headerH
                anchors.left: parent.left
                anchors.top: parent.top
                anchors.right: parent.right
                height: 32
                syncView: tableView
                model: ListModel {
                    ListElement { name: "1" }
                    ListElement { name: "2" }
                    ListElement { name: "3" }
                    ListElement { name: "4" }
                    ListElement { name: "5" }
                }
                textRole: "name"
                clip: true
            }

            // Вертикальный заголовок (время 9:00..16:00)
            VerticalHeaderView {
                id: headerV
                anchors.left: parent.left
                anchors.top: headerH.bottom
                anchors.bottom: parent.bottom
                width: 80
                syncView: tableView
                model: ListModel {
                    ListElement { time: "9:00" }
                    ListElement { time: "10:00" }
                    ListElement { time: "11:00" }
                    ListElement { time: "12:00" }
                    ListElement { time: "13:00" }
                    ListElement { time: "14:00" }
                    ListElement { time: "15:00" }
                    ListElement { time: "16:00" }
                }
                textRole: "time"
                clip: true
            }

            // Основная таблица
            TableView {
                id: tableView
                anchors.left: headerV.right
                anchors.top: headerH.bottom
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                columnSpacing: 1
                rowSpacing: 1
                clip: true

                // Модель таблицы: 8 строк, 5 столбцов (по кабинетам)
                model: TableModel {
                    id: timetableModelExample
                    TableModelColumn { display: "col1" }
                    TableModelColumn { display: "col2" }
                    TableModelColumn { display: "col3" }
                    TableModelColumn { display: "col4" }
                    TableModelColumn { display: "col5" }
                    rows: [
                        { "col1": "", "col2": "", "col3": "", "col4": "", "col5": "" },
                        { "col1": "", "col2": "", "col3": "", "col4": "", "col5": "" },
                        { "col1": "", "col2": "", "col3": "", "col4": "", "col5": "" },
                        { "col1": "", "col2": "", "col3": "", "col4": "", "col5": "" },
                        { "col1": "", "col2": "", "col3": "", "col4": "", "col5": "" },
                        { "col1": "", "col2": "", "col3": "", "col4": "", "col5": "" },
                        { "col1": "", "col2": "", "col3": "", "col4": "", "col5": "" },
                        { "col1": "", "col2": "", "col3": "", "col4": "", "col5": "" }
                    ]
                }

                delegate: Rectangle {
                    implicitWidth: 100; implicitHeight: 50
                    border.color: "black"; border.width: 1
                    // Серый фон для пустых, голубой – если есть урок
                    color: display === "" ? "#f0f0f0" : "lightblue"

                    Text {
                        anchors.centerIn: parent
                        text: display  // 'display' содержит текст из модели TableModel
                    }

                    // Зона приёма перетаскиваемого урока
                    DropArea {
                        anchors.fill: parent
                        keys: ["lesson"]
                        onDropped: function(drop) {
                            if (drop.source && drop.source.lessonId !== undefined) {
                                // Записываем имя урока в модель таблицы
                                timetableModelExample.set(row, column, drop.source.lessonName)
                                drop.acceptProposedAction()
                            }
                        }
                    }
                }
            }
        }

        // Правая часть: список неиспользованных уроков
        Rectangle {
            Layout.preferredWidth: 200
            Layout.fillHeight: true
            color: "#f7f7f7"
            border.color: "#cccccc"
            radius: 6

            ColumnLayout {
                anchors.fill: parent
                spacing: 0

                Rectangle {
                    color: "#dddddd"
                    Layout.fillWidth: true
                    Layout.preferredHeight: 40
                    Label {
                        anchors.centerIn: parent
                        text: "Неиспользованные уроки"
                        font.bold: true
                    }
                }

                ListView {
                    id: unusedLessons
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    model: lessonModelExample
                    delegate: Rectangle {
                        id: lessonCard
                        width: ListView.view.width
                        height: 60
                        color: "#ffffff"
                        border.color: "#aaaaaa"
                        radius: 4

                        //property int lessonId: lessonId
                        //property string lessonName: name
                        //property string teacherName: teacherSurname

                        Column {
                            anchors.centerIn: parent
                            Text { text: name; font.pixelSize: 14 }
                            Text { text: teacherSurname; font.pixelSize: 12; color: "gray" }
                        }

                        // Настройка Drag для урока
                        Drag.active: dragArea.drag.active
                        Drag.hotSpot.x: dragArea.mouseX
                        Drag.hotSpot.y: dragArea.mouseY
                        Drag.keys: ["lesson"]
                        Drag.source: lessonCard

                        MouseArea {
                            id: dragArea
                            anchors.fill: parent
                            cursorShape: Qt.OpenHandCursor
                            drag.target: lessonCard  // двигаем сам элемент
                            onPressed: cursorShape = Qt.ClosedHandCursor
                            onReleased: cursorShape = Qt.OpenHandCursor
                        }
                    }
                }
            }
        }
    }
}



/*import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Page {
    id: timetableDetailsPage

    signal showPageRequested(int pageIndex)

    readonly property var lessonModel: appState.lessonModel
    readonly property var roomModel: appState.roomModel
    readonly property var timetableModel: appState.timetableModel
    readonly property var teacherModel: appState.teacherModel

    function lessonIsPlaced(lessonId) {
        if (timetableModel && typeof timetableModel.isLessonPlaced === "function") {
            return timetableModel.isLessonPlaced(lessonId)
        }
        return false
    }

    function placeLesson(lessonId, row, column) {
        if (!timetableModel)
            return

        if (typeof timetableModel.assignLesson === "function") {
            timetableModel.assignLesson(row, column, lessonId)
            return
        }

        console.warn("timetableModel.assignLesson(row, column, lessonId) is not implemented")
    }

    header: ToolBar {
        RowLayout {
            anchors.fill: parent
            spacing: 8

            ToolButton {
                text: "Назад"
                onClicked: showPageRequested(2)
            }

            Label {
                text: "Расписание"
                font.bold: true
            }

            Item { Layout.fillWidth: true }
        }
    }

    RowLayout {
        anchors.fill: parent
        spacing: 8

        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true

            Rectangle {
                anchors.fill: parent
                color: "white"
                border.color: "#d0d0d0"
            }

            Text {
                anchors.centerIn: parent
                visible: !timetableModel
                text: "Расписание не загружено"
                color: "gray"
            }

            VerticalHeaderView {
                id: headerV
                syncView: tableView
                // можно явно задать модель с временами, например ListModel с полем "time"
                anchors.left: parent.left
                anchors.top: headerH.bottom
                anchors.bottom: parent.bottom
                width: 120
                clip: true
            }

            HorizontalHeaderView {
                id: headerH
                syncView: tableView
                model: roomModel            // модель с кабинетами
                textRole: "name"            // роль, содержащая название кабинета
                anchors.left: headerV.right
                anchors.top: parent.top
                anchors.right: parent.right
                height: 32
                clip: true
            }

            TableView {
                id: tableView
                anchors.left: headerV.right
                anchors.top: headerH.bottom
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                columnSpacing: 1
                rowSpacing: 1
                clip: true
                model: timetableModel

                delegate: Rectangle {
                    required property int row
                    required property int column
                    required property string lessonName

                    implicitWidth: 150
                    implicitHeight: 50
                    border.color: "black"
                    border.width: 1
                    color: lessonName !== "" ? "lightblue" : "lightgray"

                    Text {
                        anchors.centerIn: parent
                        text: lessonName
                        wrapMode: Text.WordWrap
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }

                    DropArea {
                        anchors.fill: parent
                        keys: ["lesson"]

                        onDropped: function(drop) {
                            if (drop.source && drop.source.lessonId !== undefined) {
                                placeLesson(drop.source.lessonId, row, column)
                                drop.acceptProposedAction()
                            }
                        }
                    }
                }
            }
        }

        Rectangle {
            Layout.preferredWidth: 260
            Layout.fillHeight: true
            radius: 8
            color: "#f7f7f7"
            border.color: "#cccccc"

            ColumnLayout {
                anchors.fill: parent
                spacing: 0

                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 40
                    color: "#dddddd"
                    radius: 8

                    Label {
                        anchors.centerIn: parent
                        text: "Неиспользованные уроки"
                        font.bold: true
                    }
                }

                ListView {
                    id: unusedLessons
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    spacing: 6
                    clip: true
                    model: lessonModel

                    delegate: Rectangle {
                        id: lessonCard

                        required property int id
                        required property string name
                        required property int teacherId

                        width: ListView.view.width
                        height: visible ? 60 : 0
                        radius: 6
                        color: "lightgray"
                        border.color: "#aaaaaa"

                        property int lessonId: id

                        visible: !lessonIsPlaced(id)
                        opacity: visible ? 1 : 0

                        Column {
                            anchors.centerIn: parent
                            spacing: 2

                            Text {
                                text: name
                                font.pixelSize: 14
                                horizontalAlignment: Text.AlignHCenter
                            }

                            Text {
                                text: teacherModel.teacherById(teacherId).surname
                                font.pixelSize: 12
                                color: "darkgray"
                                horizontalAlignment: Text.AlignHCenter
                            }
                        }

                        Drag.active: dragArea.drag.active
                        Drag.hotSpot.x: dragArea.mouseX
                        Drag.hotSpot.y: dragArea.mouseY
                        Drag.keys: ["lesson"]
                        Drag.source: lessonCard

                        MouseArea {
                            id: dragArea
                            anchors.fill: parent

                            cursorShape: Qt.OpenHandCursor

                            drag.target: lessonCard   // ← КРИТИЧЕСКАЯ СТРОКА!

                            onPressed: cursorShape = Qt.ClosedHandCursor
                            onReleased: cursorShape = Qt.OpenHandCursor
                        }
                    }
                }
            }
        }
    }

    Component.onCompleted: {
        if (timetableModel && roomModel) {
            if (typeof timetableModel.setRoomModel === "function")
                timetableModel.setRoomModel(roomModel)

            if (typeof roomModel.count === "number" && typeof timetableModel.setRoomCount === "function")
                timetableModel.setRoomCount(roomModel.count)

            if (typeof timetableModel.setSlotCount === "function")
                timetableModel.setSlotCount(5 * 8)

            tableView.forceLayout();
        }
    }
}*/
