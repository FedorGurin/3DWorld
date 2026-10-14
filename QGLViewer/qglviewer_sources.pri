# Сборка libQGLViewer из исходников, лежащих в репозитории, вместе с проектом.
# Так заголовки и код библиотеки всегда одной версии (2.8) и не зависят
# от версии системного пакета libqglviewer.

# экспорт снимков в векторные форматы (VRender) проекту не нужен
DEFINES *= NO_VECTORIAL_RENDER

INCLUDEPATH *= $$PWD

HEADERS += \
    $$PWD/qglviewer.h \
    $$PWD/camera.h \
    $$PWD/manipulatedFrame.h \
    $$PWD/manipulatedCameraFrame.h \
    $$PWD/frame.h \
    $$PWD/constraint.h \
    $$PWD/keyFrameInterpolator.h \
    $$PWD/mouseGrabber.h \
    $$PWD/quaternion.h \
    $$PWD/vec.h \
    $$PWD/domUtils.h \
    $$PWD/config.h

SOURCES += \
    $$PWD/qglviewer.cpp \
    $$PWD/camera.cpp \
    $$PWD/manipulatedFrame.cpp \
    $$PWD/manipulatedCameraFrame.cpp \
    $$PWD/frame.cpp \
    $$PWD/saveSnapshot.cpp \
    $$PWD/constraint.cpp \
    $$PWD/keyFrameInterpolator.cpp \
    $$PWD/mouseGrabber.cpp \
    $$PWD/quaternion.cpp \
    $$PWD/vec.cpp

FORMS += $$PWD/ImageInterface.ui
