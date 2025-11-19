import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Page {
    id: addSchoolPage
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
            text: "Добавить школу"
            font.bold: true
        }

        TextField {
            placeholderText: "Название школы"
            Layout.fillWidth: true
        }

        TextField {
            placeholderText: "Адрес"
            Layout.fillWidth: true
        }

        Button {
            text: "Сохранить"
            onClicked: {
                showPageRequested(0)
                console.log("Нажата кнопка сохранить")
            }
        }
    }
}
