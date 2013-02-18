#-------------------------------------------------
#
# Project created by QtCreator 2013-01-29T16:33:46
#
#-------------------------------------------------

QT       += core gui
QT       += opengl

# necessary to keep OS X from building an app bundle
CONFIG   -= app_bundle

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Terrain
TEMPLATE = app


SOURCES += \
    renderwidget.cpp \
    main.cpp \
    rw_input.cpp

HEADERS  += \
    renderwidget.h

OTHER_FILES += \
    quad.vert \
    Sky.frag \
    Sky.vert \
    Ground.vert \
    Ground.frag \
    flow.frag \
    Water.vert \
    Water.frag \
    clouds.frag \
    terrain.frag \
    WaterFast.vert \
    WaterFast.frag \
    GroundNoCaustic.frag

INCLUDEPATH += .\
