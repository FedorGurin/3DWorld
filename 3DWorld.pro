#-------------------------------------------------
#
# Project created by QtCreator 2010-12-21T22:26:56
#
#-------------------------------------------------


CONFIG +=thread
TARGET = 3DWorld
TEMPLATE = app
CONFIG   +=debug
CONFIG +=build_all
QT += gui\
    network \
    opengl \
    xml \
    widgets \
    svg

#INCLUDEPATH += QGLViewer
#INCLUDEPATH += libsX64

INCLUDEPATH *= /home/fedor/Soft/libQGLViewer
LIBS *= -L/home/fedor/Soft/libQGLViewer/QGLViewer -lQGLViewer-qt5

#CONFIG(debug, debug|release):LIBS+= F:\MyProg\SwT\3DWorld_bpla\libsX64\QGLViewerd2.lib
#CONFIG(debug, debug|release):LIBS+= F:\MyProg\SwT\3DWorld_bpla\libsX64\lib3ds-2_0d.lib

#CONFIG(release, debug|release):LIBS+= F:\MyProg\SwT\3DWorld_bpla\libsX64\QGLViewer2.lib
#CONFIG(release, debug|release):LIBS+= F:\MyProg\SwT\3DWorld_bpla\libsX64\lib3ds-2_0.lib

#CONFIG(debug, debug|release):LIBS+= /home/fedor/Soft/libQGLViewer-2.7.1/QGLViewer/libQGLViewer-qt5.so
#CONFIG(debug, debug|release):LIBS+= D:\fromWork\JOB\diskF\SwT\3DWorld\libsX64\lib3ds-2_0d.lib

#CONFIG(release, debug|release):LIBS+= D:\fromWork\JOB\diskF\SwT\3DWorld\libsX64\QGLViewer2.lib
#CONFIG(release, debug|release):LIBS+= D:\fromWork\JOB\diskF\SwT\3DWorld\libsX64\lib3ds-2_0.lib

#LIBS += -lopengl32 -lglu32

SOURCES += main.cpp\
    OBJ_Loader.cpp \
    view3DArea.cpp \
    view3DTerrain.cpp \
    visual3D.cpp \
    visudp.cpp \
    formsettings.cpp \
    TemplateNASP.cpp \
    math_func.cpp

HEADERS  += \
    view3DArea.h \
OBJ_Loader.h \
    view3DTerrain.h \
    visual3D.h \
    visudp.h \
    Vector3D_D.h \
    formsettings.h \
    globalData.h \
    TemplateNASP.hpp \
    math_func.h \
    Vector3D_D.h
    #QGLViewer/qglviewer.h

FORMS    += \
    formsettings.ui





