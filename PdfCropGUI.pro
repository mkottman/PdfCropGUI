#-------------------------------------------------
#
# Project created by QtCreator 2011-01-12T21:37:57
#
#-------------------------------------------------

QT       += core gui

TARGET = PdfCrop
TEMPLATE = app

INCLUDEPATH  += /usr/include/poppler/qt4
LIBS         += -L/usr/lib -lpoppler-qt4

SOURCES += main.cpp\
        mainwindow.cpp \
    displaywidget.cpp

HEADERS  += mainwindow.h \
    displaywidget.h

FORMS    += mainwindow.ui

RESOURCES += \
    resources.qrc

OTHER_FILES += \
    cropper.tex
