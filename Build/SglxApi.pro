#-------------------------------------------------
#
# Project created by QtCreator 2020-07-23T22:35:43
#
#-------------------------------------------------

QT       -= gui

TARGET = SglxApi
TEMPLATE = lib
DESTDIR = C:/Users/labadmin/Desktop/SGLBUILD/FIXU/SpikeGLX-CPP-SDK/API

DEFINES += SGLXAPI_LIBRARY

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

HEADERS += \
        Comm.h          \
        NetClient.h     \
        SglxApi.h       \
        Socket.h

SOURCES += \
        Comm.cpp        \
        NetClient.cpp   \
        SglxApi.cpp     \
        Socket.cpp

defineReplace(copyToDir) {
    files = $$1
    DIR   = $$2
    LINK  =
    for( FILE, files ) {
        LINK += $$QMAKE_COPY $$shell_path($${PWD}/$$FILE) $$shell_path($$DIR) $$escape_expand(\\n\\t)
    }
    return($$LINK)
}

win32 {
    LIBS    += -lWs2_32
    QMAKE_POST_LINK += $$copyToDir( SglxApi.h SglxCppClient.h SglxCppClient.cpp, $${DESTDIR} )
}

unix {
    target.path = /usr/lib
    INSTALLS += target
}
