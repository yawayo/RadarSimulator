QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    crc16.cpp \
    drawradarinfo.cpp \
    main.cpp \
    mainwindow.cpp \
    PCANBasicClass.cpp \
    obj_linked_list.cpp \
    serialclass.cpp \

HEADERS += \
    HCNetSDK.h \
    crc16.h \
    drawradarinfo.h \
    mainwindow.h \
    PCANBasic.h \
    PCANBasicClass.h \
    cody_Msg_Format.h \
    obj_linked_list.h \
    serialclass.h \

LIBS += D:/OpenCV/opencv_build/bin/libopencv_calib3d3415.dll
LIBS += D:/OpenCV/opencv_build/bin/libopencv_core3415.dll
LIBS += D:/OpenCV/opencv_build/bin/libopencv_dnn3415.dll
LIBS += D:/OpenCV/opencv_build/bin/libopencv_features2d3415.dll
LIBS += D:/OpenCV/opencv_build/bin/libopencv_flann3415.dll
LIBS += D:/OpenCV/opencv_build/bin/libopencv_imgproc3415.dll
LIBS += D:/OpenCV/opencv_build/bin/libopencv_ml3415.dll
LIBS += D:/OpenCV/opencv_build/bin/libopencv_objdetect3415.dll
LIBS += D:/OpenCV/opencv_build/bin/libopencv_photo3415.dll
LIBS += D:/OpenCV/opencv_build/bin/libopencv_shape3415.dll
LIBS += D:/OpenCV/opencv_build/bin/libopencv_stitching3415.dll
LIBS += D:/OpenCV/opencv_build/bin/libopencv_superres3415.dll
LIBS += D:/OpenCV/opencv_build/bin/libopencv_video3415.dll
LIBS += D:/OpenCV/opencv_build/bin/libopencv_videostab3415.dll

LIBS += -LC:/Users/ODYSSEY/Desktop/PTZTest/LIB/ -lHCCore -lHCNetSDK -lPlayCtrl -lHCAlarm -lHCGeneralCfgMgr -lHCPreview

LIBS += -Wl,--verbose

DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += _QT_
DEFINES += _CFTEST_
DEFINES += OPT_WINOS=1

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32:CONFIG(release, debug|release): LIBS += -LD:/OpenCV/opencv/build/x64/vc15/lib/ -lopencv_world3415
else:win32:CONFIG(debug, debug|release): LIBS += -LD:/OpenCV/opencv/build/x64/vc15/lib/ -lopencv_world3415d
else:unix: LIBS += -LD:/OpenCV/opencv/build/x64/vc15/lib/ -lopencv_world3415


INCLUDEPATH += D:/OpenCV/opencv/build/include
INCLUDEPATH += D:/OpenCV/opencv/build/include/opencv
INCLUDEPATH += D:/OpenCV/opencv/build/include/opencv2
