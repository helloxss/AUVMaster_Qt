#-------------------------------------------------
#
# Project created by QtCreator 2018-06-10T00:50:22
#
#-------------------------------------------------

QT       += core gui
QT += serialport
QT += charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = AUVMaster
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    controlpane.cpp \
    logpane.cpp \
    opencvpane.cpp \
    chartpane.cpp

HEADERS  += mainwindow.h \
    controlpane.h \
    logpane.h \
    opencvpane.h \
    defines.h \
    chartpane.h

FORMS    += mainwindow.ui \
    controlpane.ui

#Opencv----
INCLUDEPATH += D:\opencv\build\include\

CONFIG(debug, debug|release): {
LIBS += -LD:/opencv/build/x64/vc12/lib \
-lopencv_core249d \
-lopencv_imgproc249d \
-lopencv_highgui249d \
-lopencv_ml249d \
-lopencv_video249d \
-lopencv_features2d249d \
-lopencv_calib3d249d \
-lopencv_objdetect249d \
-lopencv_contrib249d \
-lopencv_legacy249d \
-lopencv_flann249d
} else:CONFIG(release, debug|release): {
LIBS += -LD:/opencv/build/x64/vc12/lib \
-lopencv_core249 \
-lopencv_imgproc249 \
-lopencv_highgui249 \
-lopencv_ml249 \
-lopencv_video249 \
-lopencv_features2d249 \
-lopencv_calib3d249 \
-lopencv_objdetect249 \
-lopencv_contrib249 \
-lopencv_legacy249 \
-lopencv_flann249
}
