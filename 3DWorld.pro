#-------------------------------------------------
#
# Project created by QtCreator 2010-12-21T22:26:56
#
#-------------------------------------------------


TARGET = 3DWorld
TEMPLATE = app
CONFIG += qt opengl thread rtti console
#CONFIG +=build_all
QT += xml opengl widgets gui network


ROOT_DIRECTORY = ./..

LIB_DIR_ABSOLUTE_PATH = $${ROOT_DIRECTORY}/libs
INCLUDE_DIR = $${ROOT_DIRECTORY}
LIB_DIR = D:\MyProg\3DWorld\libs

INCLUDEPATH *= $${INCLUDE_DIR}
DEPENDPATH  *= $${INCLUDE_DIR}
#LIBS *= -L/home/fedor/MyProg/libQGLViewer-2.7.2/QGLViewer -lQGLViewer-qt5

unix {
        # The absolute path where the library or framework was found
        LIB_DIR_ABSOLUTE_PATH = $$dirname(PWD)/libs

        LIB_NAME = QGLViewer-qt5

       isEmpty(QMAKE_LFLAGS_RPATH) {
                            !plugin:QMAKE_LFLAGS += -Wl,-rpath,$${LIB_DIR_ABSOLUTE_PATH}
                        }else {
                            !plugin:QMAKE_RPATHDIR *= $${LIB_DIR_ABSOLUTE_PATH}
                        }
                        LIBS *= -L$${LIB_DIR} -l$${LIB_NAME}


}
win32 {
        # Seems to be needed for Visual Studio with Intel compiler
        DEFINES *= WIN32

        # Use native OpenGL drivers with Qt5.5
        # No longer implicit since the ANGLE driver is now an alternative
        LIBS += -lopengl32 -lglu32

        isEmpty( QGLVIEWER_STATIC ) {
                CONFIG(debug, debug|release) {
                        LIBS *= -L$${LIB_DIR} -lQGLViewerd2
                } else {
                        LIBS *= -L$${LIB_DIR} -lQGLViewer2
                }
        } else {
                DEFINES *= QGLVIEWER_STATIC
                CONFIG(debug, debug|release) {
                        LIBS *= $${LIB_DIR}/libQGLViewerd2.a
                } else {
                        LIBS *= $${LIB_DIR}/libQGLViewer2.a
                }
        }
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
    Vector3D_D.h \
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





