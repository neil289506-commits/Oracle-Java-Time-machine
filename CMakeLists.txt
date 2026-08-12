cmake_minimum_required(VERSION 3.21)
project(OracleJavaTimeMachine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# 指向本機 Qt 6.11.0 MSVC2022_64 安裝路徑（可依實際路徑覆寫，例如：
# cmake -B build -DCMAKE_PREFIX_PATH="C:/Qt/6.11.0/msvc2022_64"）
if(WIN32 AND NOT DEFINED CMAKE_PREFIX_PATH)
    set(CMAKE_PREFIX_PATH "C:/Qt/6.11.0/msvc2022_64")
endif()

find_package(Qt6 6.11 REQUIRED COMPONENTS Core Gui Widgets)

if(MSVC)
    add_compile_options(/utf-8)
endif()

set(PROJECT_SOURCES
    src/main.cpp
    src/mainwindow.cpp
    src/mainwindow.h
    src/environmentchecker.cpp
    src/environmentchecker.h
    resources/resource.qrc
)

if(WIN32)
    list(APPEND PROJECT_SOURCES resources/app.rc)
endif()

qt_add_executable(OracleJavaTimeMachine
    WIN32
    ${PROJECT_SOURCES}
)

target_link_libraries(OracleJavaTimeMachine PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

set_target_properties(OracleJavaTimeMachine PROPERTIES
    WIN32_EXECUTABLE ON
    OUTPUT_NAME "OracleJavaTimeMachine"
)

# 方便開發時直接執行，自動部署 Qt 相依 DLL（僅 Windows + windeployqt 存在時）
if(WIN32)
    find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${Qt6_DIR}/../../../bin")
    if(WINDEPLOYQT_EXECUTABLE)
        add_custom_command(TARGET OracleJavaTimeMachine POST_BUILD
            COMMAND "${WINDEPLOYQT_EXECUTABLE}" --no-compiler-runtime "$<TARGET_FILE:OracleJavaTimeMachine>"
            COMMENT "Running windeployqt..."
        )
    endif()
endif()
