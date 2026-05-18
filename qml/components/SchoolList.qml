import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Basic

Page {
    id: schoolListPage
    signal showPageRequested(int pageIndex)
    signal schoolSelected(string schoolId)

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 16

        Label {
            text: "Список школ"
            font.bold: true
            font.pixelSize: 20
            Layout.alignment: Qt.AlignHCenter
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
                id: schoolList
                spacing: 10
                width: parent.width
                model: schoolModel
                clip: true

                delegate: Rectangle {
                    width: schoolList.width
                    height: 60
                    radius: 12
                    border.color: "#1976D2"
                    border.width: 1.5
                    color: "#fafafa"

                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: 8
                        spacing: 8

                        Label {
                            text: model.name
                            font.pixelSize: 16
                            Layout.fillWidth: true
                            MouseArea {
                                anchors.fill: parent
                                onClicked: {
                                    schoolSelected(index)
                                    showPageRequested(2)
                                    console.log("Нажата школа:", model.name)
                                }
                            }
                        }

                        Label {
                            text: "→"
                            font.pixelSize: 24   // увеличенный размер стрелочки
                            color: "#1976D2"
                            visible: true
                        }
                    }
                }

                Label {
                    anchors.centerIn: parent
                    font.pixelSize: 16
                    text: "У вас нет школ"
                    visible: schoolList.count === 0
                    color: "#666"
                }
            }
        }

        Button {
            text: "Добавить школу"
            Layout.alignment: Qt.AlignHCenter
            implicitWidth: 200
            implicitHeight: 44
            onClicked: {
                showPageRequested(1)
                console.log("Нажата кнопка добавления школы");
            }
            background: Rectangle {
                radius: 22
                color: "#1976D2"
                border.color: "#1976D2"
                border.width: 1
            }
            contentItem: Text {
                text: parent.text
                color: "white"
                font.pixelSize: 16
                font.bold: true
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
        }
    }
}
