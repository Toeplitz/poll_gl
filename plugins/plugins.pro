#-------------------------------------------------
#
# Project created by QtCreator 2014-11-15T10:19:33
#
#-------------------------------------------------

QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = poll_plugins
TEMPLATE = lib
CONFIG+= staticlib


SOURCES += \
    plugin_debug.cpp \
    plugin_firstperson_cam.cpp \
    plugin_light_tool.cpp \
    plugin_node_tool.cpp 

HEADERS  += \
    plugin_debug.h \
    plugin_firstperson_cam.h \
    plugin_light_tool.h \
    plugin_node_tool.h 


linux {

    QMAKE_CXXFLAGS += -fPIC -Wall -pedantic -g -std=c++11 -DGLM_FORCE_RADIANS
    INCLUDEPATH  += /usr/include/ /usr/include/bullet/ /usr/include/jsoncpp /usr/include/GL ../core
    LIBS += -L/home/ms/git/build-poll_qt-Desktop-Release/core -lpoll_core -lGLU -lGLEW -lassimp -lBulletCollision -lBulletDynamics -lLinearMath -ljsoncpp
}

win32 {
    QMAKE_CXXFLAGS += /MDd -DGLM_FORCE_RADIANS
    INCLUDEPATH += ../core

    # GLM
    INCLUDEPATH += $$PWD/../../../../poll_qt_external/glm

    # JSON
    LIBS += -L$$PWD/../../../../poll_qt_external/jsoncpp-src-0.5.0/build/vs71/debug/lib_json/ -ljson_vc71_libmtd
    INCLUDEPATH += $$PWD/../../../../poll_qt_external/jsoncpp-src-0.5.0/include
    DEPENDPATH += $$PWD/../../../../poll_qt_external/jsoncpp-src-0.5.0/include

    # BULLET
    LIBS += -L$$PWD/../../../../poll_qt_external/bullet-2.82-r2704/lib/Debug/ -lBulletDynamics_Debug -lBulletCollision_Debug -lConvexDecomposition_Debug -lLinearMath_Debug
    INCLUDEPATH += $$PWD/../../../../poll_qt_external/bullet-2.82-r2704/src
    DEPENDPATH += $$PWD/../../../../poll_qt_external/bullet-2.82-r2704/src

    # GLEW
    LIBS += -L"$$PWD/../../../../poll_qt_external/glew-1.11.0/lib/Debug/x64" -lglew32sd
    INCLUDEPATH += $$PWD/../../../../poll_qt_external/glew-1.11.0/include/GL
    DEPENDPATH += $$PWD/../../../../poll_qt_external/glew-1.11.0/include/GL

    # ASSIMP
    LIBS += -L$$PWD/../../../../poll_qt_external/assimp-3.1.1/bin/Debug/ -lassimpd
    LIBS += -L$$PWD/../../../../poll_qt_external/assimp-3.1.1/lib/Debug/ -lassimpd
    INCLUDEPATH += $$PWD/../../../../poll_qt_external/assimp-3.1.1/include
    DEPENDPATH += $$PWD/../../../../poll_qt_external/assimp-3.1.1/include

    LIBS += -L"C:\Users\ms\poll\core\debug" -lpoll_core
}
