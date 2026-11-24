#-------------------------------------------------
#
# Project created by QtCreator 2017-01-09T10:03:27
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ShowImage
TEMPLATE = app

QMAKE_LIBS_OPENGL = 
DEFINES += QT_NO_DEBUG_OUTPUT QT_NO_OPENGL
#QMAKE_CXXFLAGS_RELEASE += -mssse3
LIBS += -lrt -lpthread
LIBS += -L../../../lib -lMVSDK -lVideoRender
LIBS += -L../../../lib -llog4cpp -lImageConvert -liImageProcessing64 -lImageSave -lusb-1.0 -lTinyXml -lcompress_decode
LIBS += -L../../../lib/GenICam/bin -lGCBase_gcc485_v3_0 -lGenApi_gcc485_v3_0 -lLog_gcc485_v3_0 -llog4cpp_gcc485_v3_0 -lNodeMapData_gcc485_v3_0 -lXmlParser_gcc485_v3_0 -lMathParser_gcc485_v3_0

INCLUDEPATH = ../../../include

SOURCES += src/main.cpp\
           src/CammerWidget.cpp\
           src/form.cpp

HEADERS  += src/CammerWidget.h\
			src/MessageQue.h\
			src/form.h

FORMS    += src/cammerwidget.ui \
			src/form.ui

DESTDIR = ./bin	






