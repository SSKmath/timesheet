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

    QtObject {
        id: appState
        property var roomModel: nul
        property var teacherModel: null
        property int teacherIndex: -1
        property bool teacherIsNew: false
        property var classModel: null
        property var schoolclassModel: null
        property var lessonModel: null
    }

    property int currentPage: 0

    SchoolList {
        id: schoolListPage
        visible: currentPage === 0
        anchors.fill: parent
        onShowPageRequested: (pageIndex) => showPage(pageIndex)
        onSchoolSelected: (schoolId) => schoolDetails.schoolId = schoolId
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

    TeacherDetails {
        id: teacherDetails
        visible: currentPage === 3
        anchors.fill: parent
        onShowPageRequested: (pageIndex) => showPage(pageIndex)
    }

    GroupDetails {
        id: groupDetails
        visible: currentPage === 4
        anchors.fill: parent
        onShowPageRequested: (pageIndex) => showPage(pageIndex)
    }

    TimetableDetails {
        id: timetableDetails
        visible: currentPage === 5
        anchors.fill: parent
        onShowPageRequested: (pageIndex) => showPage(pageIndex)
    }

    function showPage(pageIndex) {
        currentPage = pageIndex
    }
}
