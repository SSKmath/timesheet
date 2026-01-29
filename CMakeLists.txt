cmake_minimum_required(VERSION 3.16)

project(timesheet VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Quick)
find_package(Qt6 REQUIRED COMPONENTS Core)
find_package(Qt6 REQUIRED COMPONENTS Core)
find_package(Qt6 REQUIRED COMPONENTS Core)

qt_standard_project_setup(REQUIRES 6.8)

qt_add_executable(apptimesheet
    main.cpp
)

qt_add_qml_module(apptimesheet
    URI timesheet
    VERSION 1.0
    QML_FILES
        qml/Main.qml
        qml/components/SchoolList.qml
        qml/components/AddSchool.qml
        qml/components/SchoolDetails.qml
        qml/components/TeacherDetails.qml
        qml/components/GroupDetails.qml
    RESOURCES
        qml/components/trash.png
    SOURCES
        src/models/room.h src/models/room.cpp
        src/models/roommodel.h src/models/roommodel.cpp
        src/models/school.h src/models/school.cpp
        src/models/schoolmodel.h src/models/schoolmodel.cpp
        src/storage/schoolstorage.h src/storage/schoolstorage.cpp
        src/models/teacher.h src/models/teacher.cpp
        src/models/teachermodel.h src/models/teachermodel.cpp
        SOURCES src/models/lesson.h src/models/lesson.cpp
        SOURCES src/models/lessonmodel.h src/models/lessonmodel.cpp
        SOURCES src/models/schoolclass.h src/models/schoolclass.cpp
        SOURCES src/models/classmodel.h src/models/classmodel.cpp
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(apptimesheet PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.apptimesheet
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(apptimesheet
    PRIVATE Qt6::Quick
)
target_link_libraries(apptimesheet PRIVATE Qt6::Core)
target_link_libraries(apptimesheet PRIVATE Qt6::Core)
target_link_libraries(apptimesheet PRIVATE Qt6::Core)

include(GNUInstallDirs)
install(TARGETS apptimesheet
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
