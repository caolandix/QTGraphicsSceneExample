#-------------------------------------------------
#
# Project created by QtCreator 2016-03-28T10:36:59
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QTGraphicsSceneExample
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    physgraphicsscene.cpp \
    arrow.cpp \
    diagramitem.cpp \
    diagramtextitem.cpp \
    physgraphicsview.cpp

HEADERS  += mainwindow.h \
    physgraphicsscene.h \
    arrow.h \
    diagramitem.h \
    diagramtextitem.h \
    physgraphicsview.h

FORMS    += mainwindow.ui

DISTFILES += \
    images/background1.png \
    images/background2.png \
    images/background3.png \
    images/background4.png \
    images/bold.png \
    images/bringtofront.png \
    images/delete.png \
    images/floodfill.png \
    images/italic.png \
    images/linecolor.png \
    images/linepointer.png \
    images/pointer.png \
    images/sendtoback.png \
    images/textpointer.png \
    images/underline.png \
    images/vector.png \
    images/particle.png
