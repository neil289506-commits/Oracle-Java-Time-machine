QT       += core gui widgets

TARGET = OracleJavaTimeMachine
TEMPLATE = app
CONFIG += c++17

win32-msvc {
    QMAKE_CXXFLAGS += /utf-8
}

# 圖標設定
exists(app.ico) {
    RC_ICONS = app.ico
    RESOURCES += resource.qrc
}

SOURCES += main.cpp \
           mainwindow.cpp
HEADERS += mainwindow.h