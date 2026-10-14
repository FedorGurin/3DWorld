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


# Каталог с библиотеками для Windows (QGLViewer2.lib/QGLViewerd2.lib)
LIB_DIR = $$PWD/libs

INCLUDEPATH *= $$PWD
DEPENDPATH  *= $$PWD

unix {
        # В Linux используется системная библиотека libQGLViewer
        # (пакет libqglviewer-dev-qt5 в Debian/Ubuntu)
        LIBS *= -lQGLViewer-qt5
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
    dataPath.h \
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





