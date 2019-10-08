#-------------------------------------------------
#
# Project created by QtCreator 2011-01-12T21:37:57
#
#-------------------------------------------------

QT       += core gui widgets

TARGET = PdfCrop
TEMPLATE = app

LIBS    += -lpoppler-qt5

SOURCES += main.cpp\
           mainwindow.cpp \
           displaywidget.cpp

HEADERS += mainwindow.h \
            displaywidget.h

FORMS   += mainwindow.ui

RESOURCES += resources.qrc

OTHER_FILES += cropper.tex
