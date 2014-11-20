#-------------------------------------------------
#
# Project created by QtCreator 2014-11-15T10:19:33
#
#-------------------------------------------------

TARGET = poll_core
TEMPLATE = lib
CONFIG+= staticlib

SOURCES += \
    animated.cpp \
    armature.cpp \
    assets.cpp \
    bone.cpp \
    camera.cpp \
    config.cpp \
    console.cpp \
    glcontext.cpp \
    gldebug.cpp \
    glshader.cpp \
    image.cpp \
    keyframe.cpp \
    light.cpp \
    material.cpp \
    mesh.cpp \
    model.cpp \
    node.cpp \
    physics.cpp \
    physics_char_cont.cpp \
    physics_rigidbody.cpp \
    poll.cpp \
    poll_plugin.cpp \
    raycast.cpp \
    scene.cpp \
    text.cpp \
    texture.cpp \
    ui.cpp \
    utils.cpp

HEADERS  += \
    animated.h \
    armature.h \
    assets.h \
    bone.h \
    camera.h \
    config.h \
    console.h \
    glcontext.h \
    gldebug.h \
    gldefaults.h \
    glshader.h \
    image.h \
    keyframe.h \
    light.h \
    material.h \
    mesh.h \
    model.h \
    node.h \
    physics.h \
    physics_char_cont.h \
    physics_rigidbody.h \
    poll.h \
    poll_plugin.h \
    raycast.h \
    scene.h \
    text.h \
    texture.h \
    ui.h \
    utils.h


linux {

    QMAKE_CXXFLAGS += -fPIC -Wall -pedantic -g -std=c++11 -DGLM_FORCE_RADIANS
    INCLUDEPATH  += /usr/include/ /usr/include/bullet/ /usr/include/jsoncpp /usr/include/GL ./src
    LIBS += -lGLU -lGLEW -lassimp -lBulletCollision -lBulletDynamics -lLinearMath -ljsoncpp
}

win32 {
    QMAKE_CXXFLAGS += /MDd -DGLM_FORCE_RADIANS

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
}
