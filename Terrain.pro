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
    rw_input.cpp \
    shaderprogram.cpp \
    gl/gl_core_4_0.c

HEADERS  += \
    renderwidget.h \
    shaderprogram.h \
    gl/gl_core_4_0.h

OTHER_FILES += \
    Sky.frag \
    Sky.vert \
    Ground.vert \
    Ground.frag \
    flow.frag \
    Water.frag \
    clouds.frag \
    terrain.frag \
    WaterFast.frag \
    GroundNoCaustic.frag \
    Water.tesse \
    Ground.tesse \
    tesselate.vert \
    Ground.tessc \
    Water.tessc \
    quad.geom \
    quad.vert

INCLUDEPATH += .\
