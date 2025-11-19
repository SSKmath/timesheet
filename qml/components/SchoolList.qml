import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Page {
    id: schoolListPage
    signal showPageRequested(int pageIndex)

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20

        Label {
            text: "Список школ"
            font.bold: true
            Layout.alignment: Qt.AlignHCenter
        }

        ListView {
            id: schoolList
            Layout.fillWidth: true
            Layout.fillHeight: true
            model: ListModel {
                ListElement { name: "Школа 1" }
                ListElement { name: "Школа 2" }
            }
            delegate: ItemDelegate {
                text: model.name
                width: parent.width
                onClicked: {
                    showPageRequested(2)
                    console.log("Нажата школа");
                }
            }

            Label {
                anchors.centerIn: parent
                text: "У вас нет школ"
                visible: schoolList.count === 0
            }
        }

        Button {
            text: "Добавить школу"
            Layout.alignment: Qt.AlignHCenter
            onClicked: {
                showPageRequested(1)
                console.log("Нажата кнопка добавления школы");
            }
        }
    }
}
