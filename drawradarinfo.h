#ifndef DRAWRADARINFO_H
#define DRAWRADARINFO_H

#include <QWidget>
#include <QDebug>
#include <QPainter>
#include <QDateTime>

#include <time.h>
#include <unistd.h>
#include <iostream>
#include <thread>
#include <chrono>

#include <obj_linked_list.h>

#include <cv.h>
#include <highgui.h>

#define FPS 30
#define MAX_OBJ 100
#define INFOBOX_NUM 6


class DrawRadarInfo : public QWidget
{
public:
    DrawRadarInfo(QWidget* parent = nullptr);

    QDateTime lastFlame;
    float realFPS = 0.0;
    double minFramePerioMsec = 1000.0 / FPS;

    int MAX_CONTINUOUS;
    int MAX_DISTLONG;
    int MAX_DISTLAT;

    int Radar_ID;

    bool first_A;
    QDateTime A_DateTime;
    int ACycle = 0;

    obj_LinkedList Obj_data[MAX_OBJ];
    Obj_inf this_frame_data[MAX_OBJ];
    bool exist[MAX_OBJ];

    int LANE_NUM;
    int LANE_SPACE;
    int LANE_COUNT;

    bool infoBoxOccupied[INFOBOX_NUM * 2] = {};

    float** lanePoint;
    bool haveLanePoint;
    int sizeofLUT_X = 500, sizeofLUT_Y=500;
    IplImage* LookUpImage;
    void setLookUpImage();

    bool graphics_on = false;
    void draw_distline(QPainter* painter);
    void draw_carlane(QPainter* painter);
    void draw_obj(QPainter* painter);


protected:
    void paintEvent(QPaintEvent*) override;
};

#endif // DRAWRADARINFO_H
