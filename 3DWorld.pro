#-------------------------------------------------
#
# Project created by QtCreator 2010-12-21T22:26:56
#
#-------------------------------------------------


CONFIG +=thread console
TARGET = 3DWorld
TEMPLATE = app

QT += gui\
    network \
    opengl \
    xml \
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
    win32-g++:LIBS += -l$${3DSLIB}
    win32-g++:LIBS += -l$${QGLVIEWER2}

}

SOURCES += main.cpp\
    view3dterrain.cpp \
    view3DArea.cpp \
    visual3D.cpp \
    formstatusexitprogram.cpp \
    gl_func.cpp \
    aligmentform.cpp \   
    visudp.cpp \
    Vector3D_D.cpp \
    Matrix3D_D.cpp

HEADERS  += \
    view3dterrain.h \
    view3DArea.h \
    visual3D.h \
    formstatusexitprogram.h \
    gl_func.h \
    AligmentForm.h \   
    visudp.h \
    Vector3D_D.h \
    Matrix3D_D.h

FORMS    += \
    formstatusexitprogram.ui \
    aligmentform.ui





