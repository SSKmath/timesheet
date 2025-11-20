import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "components"

ApplicationWindow {
    id: root
    visible: true
    width: 800
    height: 600
    title: "timesheet"

    property int currentPage: 0

    SchoolList {
        id: schoolListPage
        visible: currentPage === 0
        anchors.fill: parent
        onShowPageRequested: (pageIndex) => showPage(pageIndex)
    }

    AddSchool {
        id: addSchoolPage
        visible: currentPage === 1
        anchors.fill: parent
        onShowPageRequested: (pageIndex) => showPage(pageIndex)
    }

    SchoolDetails {
        id: schoolDetails
        visible: currentPage === 2
        anchors.fill: parent
        onShowPageRequested: (pageIndex) => showPage(pageIndex)
    }

    function showPage(pageIndex) {
        currentPage = pageIndex
    }
}
