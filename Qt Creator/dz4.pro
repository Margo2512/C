QT       += core
QT += datavisualization
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
QT += widgets
CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    plot.cpp

HEADERS += \
    mainwindow.h \
    plot.h

FORMS += \
    mainwindow.ui

macx: LIBS += -L/Users/margaritakadocnikova/Qt/6.5.1/macos/lib/cmake/QtDataVisualization

macx: {
    target.path = /Users/margaritakadocnikova/Qt/6.5.1/macos/lib/cmake/QtDataVisualization
}

!isEmpty(target.path): INSTALLS += target
