import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Page {
    id: schoolListPage
    signal showPageRequested(int pageIndex)
    signal schoolSelected(string schoolId)

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 20

        Label {
            text: "Список школ"
            font.bold: true
            font.pixelSize: 24
            Layout.alignment: Qt.AlignHCenter
            color: "#333"
        }

        ListView {
            id: schoolList
            Layout.fillWidth: true
            Layout.fillHeight: true
            model: schoolModel
            spacing: 10
            clip: true

            delegate: Rectangle {
                id: delegateRect
                width: schoolList.width
                height: 70
                radius: 12
                color: mouseArea.containsMouse ? Qt.darker("#f5f5f5", 1.1) : "#f5f5f5"
                border.color: "#0D47A1"
                border.width: 1

                Behavior on color {
                    ColorAnimation { duration: 80 }
                }

                Text {
                    text: model.name
                    font.pixelSize: 18
                    color: "#333"
                    anchors.left: parent.left
                    anchors.leftMargin: 20
                    anchors.verticalCenter: parent.verticalCenter
                }

                MouseArea {
                    id: mouseArea
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: {
                        schoolSelected(index)
                        showPageRequested(2)
                        console.log("Нажата школа:", model.name)
                    }
                }
            }

            Label {
                anchors.centerIn: parent
                font.pixelSize: 18
                text: "У вас пока нет школ"
                color: "#999"
                visible: schoolList.count === 0
            }
        }


        Button {
            id: addSchoolButton
            text: "Добавить школу"
            Layout.alignment: Qt.AlignHCenter
            Layout.preferredWidth: 200
            Layout.preferredHeight: 50

            flat: true                      // отключаем стандартное оформление
            hoverEnabled: true

            readonly property color baseColor: "#1976D2"

            background: Rectangle {
                radius: 12
                border.color: "#0D47A1"
                border.width: 1

                // Цвета заданы явно, без Qt.lighter для надёжности
                color: {
                    if (addSchoolButton.pressed)
                        return "#0D47A1"        // темнее базового
                    else if (addSchoolButton.hovered)
                        return "#1E88E5"        // светлее базового (подобрано вручную)
                    else
                        return addSchoolButton.baseColor
                }

                Behavior on color {
                    ColorAnimation { duration: 80 }
                }
            }

            contentItem: Text {
                text: addSchoolButton.text
                font.pixelSize: 18
                color: "white"
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }

            onClicked: {
                showPageRequested(1)
                console.log("Нажата кнопка добавления школы");
            }
        }
    }
}
