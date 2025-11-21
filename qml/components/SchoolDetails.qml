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
    }

    ColumnLayout {
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
    }
}
