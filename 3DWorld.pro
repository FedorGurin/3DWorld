#-------------------------------------------------
#
# Project created by QtCreator 2010-12-21T22:26:56
#
#-------------------------------------------------


CONFIG +=thread
TARGET = 3DWorld
TEMPLATE = app
CONFIG   +=  qt opengl warn_on thread rtti console
#CONFIG +=build_all
QT += gui\
    network \
    opengl \
    xml \
    widgets

INCLUDE_DIR = $${ROOT_DIRECTORY}
LIB_DIR = $${ROOT_DIRECTORY}/QGLViewer

#LIB_DIR = ./libs
#LIB_DIR_ABSOLUTE_PATH = $$dirname(PWD)/libs

LIBS += -lopengl32 -lglu32

LIBS *= D:/MyProg/3DWorld/QGLViewerd2.lib
#-L/home/fedor/MyProg/libQGLViewer-2.7.2/QGLViewer -lQGLViewer-qt5

unix {
        # The absolute path where the library or framework was found
        LIB_DIR_ABSOLUTE_PATH = $$dirname(PWD)/libs

        #LIB_NAME = QGLViewer-qt5
#        D:\MyProg\3DWorld\QGLViewerd2.lib

       isEmpty(QMAKE_LFLAGS_RPATH) {
                            !plugin:QMAKE_LFLAGS += -Wl,-rpath,$${LIB_DIR_ABSOLUTE_PATH}
                        }else {
                            !plugin:QMAKE_RPATHDIR *= $${LIB_DIR_ABSOLUTE_PATH}
                        }
                        LIBS *= -L$${LIB_DIR} -l$${LIB_NAME}


}


#INCLUDEPATH += QGLViewer
#INCLUDEPATH += libsX64
DEFINES += GLM_PRECISION_MEDIUMP_FLOAT


SOURCES += main.cpp\
    OBJ_Loader.cpp \
    view3DArea.cpp \
    view3DTerrain.cpp \
    visudp.cpp \
    formsettings.cpp \
    TemplateNASP.cpp \
    math_func.cpp

HEADERS  += \
    glm/glm.hpp \
    view3DArea.h \
OBJ_Loader.h \
    view3DTerrain.h \
    visudp.h \    
    formsettings.h \
    globalData.h \
    TemplateNASP.hpp \
    math_func.h
    #QGLViewer/qglviewer.h

FORMS    += \
    formsettings.ui

#DISTFILES += \
#    ../../Soft/libQGLViewer/QGLViewer/libQGLViewer-qt5.so \
#    libQGLViewer-qt5.a





