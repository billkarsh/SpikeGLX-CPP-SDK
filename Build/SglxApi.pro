#-------------------------------------------------
#
# Project created by QtCreator 2020-07-23T22:35:43
#
#-------------------------------------------------

QT -= gui

TARGET = SglxApi
TEMPLATE = lib

win32 {
    DESTDIR = C:/Users/labadmin/Desktop/SGLBUILD/FIXU/SpikeGLX-CPP-SDK/Windows/API
}

unix {
    DESTDIR = /home/billkarsh/Code/SpikeGLX-CPP-SDK/Linux/API
}

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
        C_SglxApi.h         \
        Comm.h              \
        NetClient.h         \
        SglxApi.h           \
        SglxCppClient.h     \
        Socket.h

SOURCES += \
        C_SglxApi.cpp       \
        Comm.cpp            \
        NetClient.cpp       \
        SglxApi.cpp         \
        SglxCppClient.cpp   \
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
    CONFIG          += skip_target_version_ext
    LIBS            += -lWs2_32
    QMAKE_CXXFLAGS  += -ffunction-sections -fdata-sections -flto
    QMAKE_LFLAGS    += -Wl,--no-whole-archive -Wl,--gc-sections -flto
    QMAKE_LFLAGS    += -static -static-libgcc -static-libstdc++
}

unix {
    CONFIG += unversioned_libname
#    target.path = /usr/lib
#    INSTALLS += target
}

QMAKE_POST_LINK += $$copyToDir( C_SglxApi.h SglxApi.h SglxCppClient.h SglxCppClient.cpp, $${DESTDIR} )
QMAKE_POST_LINK += strip --strip-unneeded $${DESTDIR}/$${TARGET}.dll

QMAKE_TARGET_COMPANY = Bill Karsh
QMAKE_TARGET_PRODUCT = SglxApi
QMAKE_TARGET_DESCRIPTION = SpikeGLX Com Link
QMAKE_TARGET_COPYRIGHT = (c) 2025, Bill Karsh, All rights reserved
VERSION = 1.6.0


