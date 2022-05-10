#include "drawradarinfo.h"

DrawRadarInfo::DrawRadarInfo(QWidget* parent) : QWidget (parent)
{

}


void DrawRadarInfo::setLookUpImage()
{
    LookUpImage = cvCreateImage(cvSize(sizeofLUT_X, sizeofLUT_Y), IPL_DEPTH_8U, 1);
    cvSetZero(LookUpImage);
    CvPoint Lane_segments_cv[20][20][4];

    int i, j;
    for(i = 0; i < LANE_NUM; i++)
    {
        for(j = 0; j < LANE_COUNT - 1; j++)
        {
            Lane_segments_cv[i][j][0] = cvPoint(lanePoint[i][j] * (sizeofLUT_X/2), sizeofLUT_Y - (j + 0.5) * (sizeofLUT_Y / LANE_COUNT));
            Lane_segments_cv[i][j][1] = cvPoint(lanePoint[i+1][j] * (sizeofLUT_X/2), sizeofLUT_Y - (j + 0.5) * (sizeofLUT_Y / LANE_COUNT));
            Lane_segments_cv[i][j][2] = cvPoint(lanePoint[i+1][j+1] * (sizeofLUT_X/2), sizeofLUT_Y - (j + 1.5) * (sizeofLUT_Y / LANE_COUNT));
            Lane_segments_cv[i][j][3] = cvPoint(lanePoint[i][j+1] * (sizeofLUT_X/2), sizeofLUT_Y - (j + 1.5) * (sizeofLUT_Y / LANE_COUNT));

            cvFillConvexPoly(LookUpImage, Lane_segments_cv[i][j], 4, cvScalar(((i * (LANE_COUNT - 1)) + j + 1)*1));
        }
    }
    //cvShowImage("test", LookUpImage);
}

void DrawRadarInfo::paintEvent(QPaintEvent*)
{
    if(graphics_on)
    {
        QPainter painter(this);
        painter.setCompositionMode(QPainter::CompositionMode_Source);

        draw_distline(&painter);
        if(haveLanePoint)
            draw_carlane(&painter);
        draw_obj(&painter);
    }

    update();

    QDateTime thisFlame = QDateTime::currentDateTime();
    int diff_msec = lastFlame.msecsTo(thisFlame);
    int waitMsec = minFramePerioMsec - diff_msec;
    if(waitMsec > 0)
        std::this_thread::sleep_for(std::chrono::milliseconds(waitMsec));

    int diff_now = lastFlame.msecsTo(QDateTime::currentDateTime());

    realFPS = 1000 / ((diff_now > 0) ? diff_now : 1);

    lastFlame = QDateTime::currentDateTime();
}

void DrawRadarInfo::draw_distline(QPainter* painter)
{
    QFont font = painter->font() ;
    font.setPointSize(width() / 80);
    painter->setFont(font);

    int i;

    //painter->setPen(QPen(QColor::fromRgbF(0, 1, 0, 1), 1));

    for(i=0; i<LANE_COUNT; i++)
    {
        painter->setPen(QPen(Qt::yellow, 1));
        painter->drawLine(width() / 4, height() - (height() / 20) - ((float(LANE_SPACE * i) / MAX_DISTLONG) * (height() * (1.0 - (2.0 / 20)))),
                          (width() * 3) / 4, height() - (height() / 20) - ((float(LANE_SPACE * i) / MAX_DISTLONG) * (height() * (1.0 - (2.0 / 20)))));


        painter->setPen(Qt::white);
        painter->drawText(((width() * 3) / 4)-30, height() - (height() / 20) - ((float(LANE_SPACE * i) / MAX_DISTLONG) * (height() * (1.0 - (2.0 / 20)))) - 2, QString::number(LANE_SPACE * i) + "m");

    }
    painter->drawText(width() * (16.0 / 20.0), height() / 30.0, "60A : " + QString::number(ACycle) + "ms  FPS : " + QString::number(realFPS, 'f', 1));
}
void DrawRadarInfo::draw_carlane(QPainter* painter)
{
    QFont font = painter->font() ;
    font.setPointSize(height() / 100);
    painter->setFont(font);

    painter->setPen(Qt::white);
    int i, j;
    for(i=0; i<LANE_NUM + 1; i++)
    {
        for(j=0; j<LANE_COUNT-1; j++)
        {
            painter->drawLine((width()/4) + lanePoint[i][j] * (width()/4), height() - (height() / 20) - ((float(LANE_SPACE * j) / MAX_DISTLONG) * (height() * (1.0 - (2.0 / 20)))),
                              (width()/4) + lanePoint[i][j+1] * (width()/4), height() - (height() / 20) - ((float(LANE_SPACE * (j + 1)) / MAX_DISTLONG) * (height() * (1.0 - (2.0 / 20)))));
        }
    }
}

void DrawRadarInfo::draw_obj(QPainter* painter)
{

    int i;
    for(i=0; i<MAX_OBJ; i++)
    {
        if(!Obj_data[i].isEmpty())
        {
            painter->setBrush(QBrush(Qt::NoBrush));

            Obj_inf item = Obj_data[i].getLast();

            int obj_x = ((item.DistLat / 2 - MAX_DISTLAT) * -1) * (float(width()) / (MAX_DISTLAT * 2));
            int obj_y = height() - (height() / 20) - ((item.DistLong / MAX_DISTLONG) * (height() * (1.0 - (2.0 / 20.0))));
            int info_x = int((item.index_of_infoBox - 1) / INFOBOX_NUM) * (3 * (width() / 4));
            int info_y = (height() / 20) + ((item.index_of_infoBox - 1) % INFOBOX_NUM) * ((height() * (19.0 / 20.0)) / INFOBOX_NUM);

            QRect rect(obj_x-7, obj_y-7, height() / 50, height() / 50);

            painter->setPen(Qt::green);
            painter->setBrush(QBrush(Qt::green));

            painter->drawEllipse((width() / 2) * (0.5 + int((item.index_of_infoBox - 1) / INFOBOX_NUM)) - (width() / 200), info_y + (height() / (INFOBOX_NUM * 2)) - (width() / 200), width() / 100, width() / 100);

            if(item.virtual_obj)
                painter->setBrush(QBrush(Qt::NoBrush));

            painter->drawRect(rect);

            painter->setBrush(QBrush(Qt::NoBrush));

            QFont font = painter->font() ;
            font.setPointSize(height() / 100);
            painter->setFont(font);

            if((item.index_of_infoBox > 0) && (item.index_of_infoBox <= (INFOBOX_NUM * 2)))
            {
                // 4각형
                QRect rect(info_x, info_y, width() / 4, ((height() * (19.0 / 20.0)) / INFOBOX_NUM) - 1);
                painter->drawRect(rect);

                painter->drawLine(obj_x, obj_y, (width() / 2) * (0.5 + int((item.index_of_infoBox - 1) / INFOBOX_NUM)), info_y + ((height() * (19.0 / 20.0)) / (INFOBOX_NUM * 2)));
                painter->drawText(info_x + (width() / 100), info_y + (float((height() * (19.0 / 20.0))) / (50.0 / 1.0)), "ID       : " + QString::number(item.ID));
                painter->drawText(info_x + (width() / 100), info_y + (float((height() * (19.0 / 20.0))) / (50.0 / 2.0)), "SPEED    : " + QString::number(item.Speed,'f',1));
                painter->drawText(info_x + (width() / 100), info_y + (float((height() * (19.0 / 20.0))) / (50.0 / 3.0)), "DISTLAT  : " + QString::number(item.DistLat,'f',1));
                painter->drawText(info_x + (width() / 100), info_y + (float((height() * (19.0 / 20.0))) / (50.0 / 4.0)), "DISTLONG : " + QString::number(item.DistLong,'f',1));
                painter->drawText(info_x + (width() / 100), info_y + (float((height() * (19.0 / 20.0))) / (50.0 / 5.0)), "RCS      : " + QString::number(item.DistLong,'f',1));
                painter->drawText(info_x + (width() / 100), info_y + (float((height() * (19.0 / 20.0))) / (50.0 / 6.0)), "ProbOfExist : " + QString::number(item.ProbOfExist));
                painter->drawText(info_x + (width() / 100), info_y + (float((height() * (19.0 / 20.0))) / (50.0 / 7.0)), "Zone Index : " + QString::number(item.zone_index));
                painter->drawText(info_x + (width() / 100), info_y + (float((height() * (19.0 / 20.0))) / (50.0 / 8.0)), "Obj : " + QString::number(Obj_data[i].getsize()));


                /*
                // 5각형
                if(((item.index_of_infoBox - 1) / INFOBOX_NUM) == 0)
                {
                    painter->drawLine(info_x, info_y, info_x + (width() / 5), info_y);
                    painter->drawLine(info_x, info_y, info_x, info_y + ((height() / INFOBOX_NUM) - 1));
                    painter->drawLine(info_x, info_y + ((height() / INFOBOX_NUM) - 1), info_x + (width() / 5), info_y + ((height() / INFOBOX_NUM) - 1));

                    painter->drawLine(info_x + (width() / 5), info_y, (width() / 2) * 0.5, info_y + (height() / (INFOBOX_NUM * 2)));
                    painter->drawLine(info_x + (width() / 5), info_y + ((height() / INFOBOX_NUM) - 1), (width() / 2) * 0.5, info_y + (height() / (INFOBOX_NUM * 2)));

                    painter->drawText(info_x + (width() / 100), info_y + (float(height()) / (50.0 / 1.0)), "ID       : " + QString::number(item.ID));
                    painter->drawText(info_x + (width() / 100), info_y + (float(height()) / (50.0 / 2.0)), "SPEED    : " + QString::number(item.Speed,'f',1));
                    painter->drawText(info_x + (width() / 100), info_y + (float(height()) / (50.0 / 3.0)), "DISTLAT  : " + QString::number(item.DistLat,'f',1));
                    painter->drawText(info_x + (width() / 100), info_y + (float(height()) / (50.0 / 4.0)), "DISTLONG : " + QString::number(item.DistLong,'f',1));
                    painter->drawText(info_x + (width() / 100), info_y + (float(height()) / (50.0 / 5.0)), "RCS      : " + QString::number(item.DistLong,'f',1));
                    painter->drawText(info_x + (width() / 100), info_y + (float(height()) / (50.0 / 6.0)), "ProbOfExist : " + QString::number(item.ProbOfExist));

                    painter->drawLine(obj_x, obj_y, (width() / 2) * 0.5, info_y + (height() / (INFOBOX_NUM * 2)));
                }
                else
                {
                    painter->drawLine(info_x + (width() / 20), info_y, info_x + (width() / 4), info_y);
                    painter->drawLine(info_x + (width() / 4), info_y, info_x + (width() / 4), info_y + ((height() / INFOBOX_NUM) - 1));
                    painter->drawLine(info_x + (width() / 20), info_y + ((height() / INFOBOX_NUM) - 1), info_x + (width() / 4), info_y + ((height() / INFOBOX_NUM) - 1));

                    painter->drawLine(info_x + (width() / 20), info_y, (width() / 2) * 1.5, info_y + (height() / (INFOBOX_NUM * 2)));
                    painter->drawLine(info_x + (width() / 20), info_y + ((height() / INFOBOX_NUM) - 1), (width() / 2) * 1.5, info_y + (height() / (INFOBOX_NUM * 2)));

                    painter->drawText(info_x + (width() / 100) + (width() / 20), info_y + (float(height()) / (50.0 / 1.0)), "ID       : " + QString::number(item.ID));
                    painter->drawText(info_x + (width() / 100) + (width() / 20), info_y + (float(height()) / (50.0 / 2.0)), "SPEED    : " + QString::number(item.Speed,'f',1));
                    painter->drawText(info_x + (width() / 100) + (width() / 20), info_y + (float(height()) / (50.0 / 3.0)), "DISTLAT  : " + QString::number(item.DistLat,'f',1));
                    painter->drawText(info_x + (width() / 100) + (width() / 20), info_y + (float(height()) / (50.0 / 4.0)), "DISTLONG : " + QString::number(item.DistLong,'f',1));
                    painter->drawText(info_x + (width() / 100) + (width() / 20), info_y + (float(height()) / (50.0 / 5.0)), "RCS      : " + QString::number(item.DistLong,'f',1));
                    painter->drawText(info_x + (width() / 100) + (width() / 20), info_y + (float(height()) / (50.0 / 6.0)), "ProbOfExist : " + QString::number(item.ProbOfExist));

                    painter->drawLine(obj_x, obj_y, (width() / 2) * 1.5, info_y + (height() / (INFOBOX_NUM * 2)));
                }
                */
            }
            else
            {
                painter->drawText(obj_x + 10, obj_y, "[" + QString::number(item.ID) + "] " + QString::number(item.Speed));
            }
        }
    }
}
