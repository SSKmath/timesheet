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
            spacing: 0
            Layout.fillWidth: true
            Layout.fillHeight: true
            model: schoolModel
            delegate: ItemDelegate {
                text: model.name
                font.pointSize: 18
                width: parent.width

                onClicked: {
                    showPageRequested(2)
                    console.log("Нажата школа");
                }
            }

            Label {
                anchors.centerIn: parent
                font.pointSize: 18
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
