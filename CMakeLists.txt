cmake_minimum_required(VERSION 3.10)
project(MiniGitGUI)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 QUIET COMPONENTS Widgets)
if(Qt6_FOUND)
    set(QT_VERSION Qt6)
else()
    find_package(Qt5 REQUIRED COMPONENTS Widgets)
    set(QT_VERSION Qt5)
endif()

add_executable(minigit_gui main.cpp MainWindow.cpp Repository.cpp Utils.cpp)
add_executable(minigit_smoke_tests tests/RepositorySmoke.cpp Repository.cpp Utils.cpp)

target_link_libraries(minigit_gui ${QT_VERSION}::Widgets)

target_include_directories(minigit_gui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(minigit_smoke_tests PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

enable_testing()
add_test(NAME minigit_smoke_tests COMMAND minigit_smoke_tests)

set_target_properties(minigit_gui PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/dist"
)

if(WIN32 AND QT_VERSION STREQUAL "Qt6")
    get_filename_component(QT6_PREFIX_DIR "${Qt6_DIR}/../../.." ABSOLUTE)
    find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${QT6_PREFIX_DIR}/bin")

    if(WINDEPLOYQT_EXECUTABLE)
        add_custom_command(TARGET minigit_gui POST_BUILD
            COMMAND "${WINDEPLOYQT_EXECUTABLE}"
                --no-translations
                --no-compiler-runtime
                "$<TARGET_FILE:minigit_gui>"
            COMMENT "Deploying Qt runtime for minigit_gui"
        )
    else()
        message(WARNING "windeployqt was not found; the built GUI may miss Qt DLLs.")
    endif()
endif()
