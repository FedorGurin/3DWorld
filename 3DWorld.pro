#-------------------------------------------------
#
# Project created by QtCreator 2010-12-21T22:26:56
#
#-------------------------------------------------


CONFIG +=thread
TARGET = 3DWorld
TEMPLATE = app

QT += gui\
    network \
    opengl \
    xml \
    svg

SOURCES += main.cpp\
    view3dterrain.cpp \
    view3DArea.cpp \
    visual3D.cpp \
    gl_func.cpp \
    visudp.cpp \
    Vector3D_D.cpp \
    Matrix3D_D.cpp \
    formsettings.cpp \
    TemplateNASP.cpp

HEADERS  += \
    view3dterrain.h \
    view3DArea.h \
    visual3D.h \
    gl_func.h \
    visudp.h \
    Vector3D_D.h \
    Matrix3D_D.h \
    formsettings.h \
    globalData.h \
    TemplateNASP.hpp

FORMS    += \
    formsettings.ui





