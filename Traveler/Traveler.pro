QT += widgets xml

CONFIG += c++17
TEMPLATE = app

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        main.cpp \
        mainwindow.cpp \
        mapview.cpp \
        photoview.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    mainwindow.h \
    mapobject.h \
    mappoint.h \
    mapregion.h \
    mapview.h \
    photoview.h

RC_ICONS = ussr.ico

DISTFILES += \
    ussr.ico
