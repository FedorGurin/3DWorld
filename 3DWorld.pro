#-------------------------------------------------
#
# Project created by QtCreator 2010-12-21T22:26:56
#
#-------------------------------------------------


CONFIG +=thread
TARGET = 3DWorld
TEMPLATE = app
CONFIG   +=debug_and_release
CONFIG +=build_all
QT += gui\
    network \
    opengl \
    xml \
    widgets \
    svg
3DSLIB = 3ds
unix {

    #QMAKE_CXXFLAGS_RELEASE += -malign-double
    QGLVIEWER = QGLViewer
    unix:LIBS += -l$${3DSLIB}
    unix:LIBS += -l$${QGLVIEWER}

}
win32 {

    #QMAKE_CXXFLAGS_RELEASE += -mno-align-double
    QGLVIEWER2 = QGLViewer2
    #win32-g++:LIBS += -l$${3DSLIB}
    #win32-g++:LIBS += -l$${QGLVIEWER2}

}

CONFIG(debug, debug|release):LIBS+= "C:\MyProg\SwT\3DWorld_bpla\libsX64\QGLViewerd2.lib"
CONFIG(debug, debug|release):LIBS+= "C:\MyProg\SwT\3DWorld_bpla\libsX64\lib3ds-2_0d.lib"

CONFIG(release, debug|release):LIBS+="C:\MyProg\SwT\3DWorld_bpla\libsX64\QGLViewer2.lib"
CONFIG(release, debug|release):LIBS+="C:\MyProg\SwT\3DWorld_bpla\libsX64\lib3ds-2_0.lib"

LIBS += -lopengl32 -lglu32

#INCLUDEPATH += lib3ds \
#               libQGLViewer

SOURCES += main.cpp\
    view3dterrain.cpp \
    view3DArea.cpp \
    visual3D.cpp \
    visudp.cpp \
    formsettings.cpp \
    TemplateNASP.cpp \
    math_func.cpp

HEADERS  += \
    view3dterrain.h \
    view3DArea.h \
    visual3D.h \
    visudp.h \
    Vector3D_D.h \
    formsettings.h \
    globalData.h \
    TemplateNASP.hpp \
    math_func.h \
    Vector3D_D.h

FORMS    += \
    formsettings.ui





