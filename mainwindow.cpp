#include "mainwindow.h"

#include <QDebug>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), m_settings("outSmart", "LiveWatcher")
{
    createUI();

    do_print_text = false;
    do_kalman = false;

    global_time = QDateTime::currentDateTime();

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(readTime()));
    timer->start(10);

    radarStatus->setStyleSheet("background-color: rgb(255, 0, 0); color: rgb(0, 0, 0); border-width: 1px; border-style: solid; border-color: rgb(0, 0, 0);");

    cameraStatus->setStyleSheet("background-color: rgb(255, 0, 0); color: rgb(0, 0, 0); border-width: 1px; border-style: solid; border-color: rgb(0, 0, 0);");

    //make_log_path();
    //gettimeofday(&last_save, nullptr);
    //log_path = "/home/pi/Log/" + global_time.toString("yyyy-MM-dd-hh-mm-ss-zzz") + ".csv";

    initValue();

    qDebug("Try Radar Connect...");
    int try_count = 1;
    //while(!Connect_Radar())
    {
        qDebug("Retry...%d", try_count);
        try_count++;
        sleep(1);
    }
    qDebug("Radar Connection Successful");

    qDebug("Try Camera Connect...");
    try_count = 1;
    //while(!Connect_Camera())
    {
        qDebug("Retry...%d", try_count);
        try_count++;
        sleep(1);
    }
    qDebug("Camera Connection Successful");
}

MainWindow::~MainWindow()
{

}

void MainWindow::createUI()
{
    //Total Layout
    total_layout = new QHBoxLayout;


    // Display Image
    AI_Display_layout = new QVBoxLayout;
    AI_Display_Img = new QLabel; AI_Display_Img->setMinimumSize(64, 384); AI_Display_Img->setMaximumSize(64, 384);
    AI_Display_spacer = new QSpacerItem(30, 0, QSizePolicy::Fixed, QSizePolicy::Expanding);

    QPixmap off_Img("C:/Users/ODYSSEY/Desktop/build-PTZ_TEST-Desktop_Qt_6_2_3_MinGW_64_bit-Debug/debug/img/Display_ON.png");
    AI_Display_Img->setPixmap(off_Img);
    AI_Display_Img->setScaledContents(true);

    AI_Display_layout->addWidget(AI_Display_Img);
    AI_Display_layout->addSpacerItem(AI_Display_spacer);

    // Main Contents
    main_Contents_layout = new QVBoxLayout;


    // Button
    button_layout = new QHBoxLayout;

    radarGroup = new QGroupBox("Radar");
    Connectbtn_Radar = new QPushButton("Connect");
    Disconnectbtn_Radar = new QPushButton("Disconnect"); Disconnectbtn_Radar->setEnabled(false);
    QHBoxLayout* radarGroup_layout = new QHBoxLayout;
    connect(Connectbtn_Radar, SIGNAL(clicked()), SLOT(on_Connectbtn_Radar_clicked()));
    connect(Disconnectbtn_Radar, SIGNAL(clicked()), SLOT(on_Disconnectbtn_Radar_clicked()));
    radarGroup->setMaximumSize(300,100);

    radarGroup_layout->addWidget(Connectbtn_Radar);
    radarGroup_layout->addWidget(Disconnectbtn_Radar);
    radarGroup->setLayout(radarGroup_layout);


    cameraGroup = new QGroupBox("Camera");
    Connectbtn_Camera = new QPushButton("Connect");
    Disconnectbtn_Camera = new QPushButton("Disconnect"); Disconnectbtn_Camera->setEnabled(false);
    QHBoxLayout* cameraGroup_layout = new QHBoxLayout;
    connect(Connectbtn_Camera, SIGNAL(clicked()), SLOT(on_Connectbtn_Camera_clicked()));
    connect(Disconnectbtn_Camera, SIGNAL(clicked()), SLOT(on_Disconnectbtn_Camera_clicked()));
    cameraGroup->setMaximumSize(300,100);

    cameraGroup_layout->addWidget(Connectbtn_Camera);
    cameraGroup_layout->addWidget(Disconnectbtn_Camera);
    cameraGroup->setLayout(cameraGroup_layout);


    serialGroup = new QGroupBox("Serial");
    Connectbtn_Serial = new QPushButton("Connect");
    Disconnectbtn_Serial = new QPushButton("Disconnect"); Disconnectbtn_Serial->setEnabled(false);
    QHBoxLayout* serialGroup_layout = new QHBoxLayout;
    connect(Connectbtn_Serial, SIGNAL(clicked()), SLOT(on_Connectbtn_Serial_clicked()));
    connect(Disconnectbtn_Serial, SIGNAL(clicked()), SLOT(on_Disconnectbtn_Serial_clicked()));
    serialGroup->setMaximumSize(300,100);

    serialGroup_layout->addWidget(Connectbtn_Serial);
    serialGroup_layout->addWidget(Disconnectbtn_Serial);
    serialGroup->setLayout(serialGroup_layout);


    settingGroup = new QGroupBox("Setting");
    Settingbtn = new QPushButton("Setting");
    QHBoxLayout* settingGroup_layout = new QHBoxLayout;
    connect(Settingbtn, SIGNAL(clicked()), SLOT(on_Settingbtn_clicked()));
    settingGroup->setMaximumSize(300,100);

    settingGroup_layout->addWidget(Settingbtn);
    settingGroup->setLayout(settingGroup_layout);

    button_layout_spacer = new QSpacerItem(0, 30, QSizePolicy::Expanding, QSizePolicy::Fixed);

    Logo = new QLabel;
    QPixmap logo("C:/Users/ODYSSEY/Desktop/build-PTZ_TEST-Desktop_Qt_6_2_3_MinGW_64_bit-Debug/debug/img/HBrain.png");
    Logo->setPixmap(logo);
    Logo->setScaledContents(true);


    button_layout->addWidget(radarGroup);
    button_layout->addWidget(cameraGroup);
    button_layout->addWidget(serialGroup);
    button_layout->addWidget(settingGroup);
    button_layout->addSpacerItem(button_layout_spacer);
    button_layout->addWidget(Logo);


    // Main Panel
    page = new QStackedWidget;

    // Main page
    main_page = new QWidget;
    main_page_layout = new QHBoxLayout;

    Display_Radar = new DrawRadarInfo; Display_Radar->setStyleSheet("background-color: rgb(0, 0, 0);");

    main_page_layout2 = new QVBoxLayout;
    Display_Camera = new QFrame; Display_Camera->setStyleSheet("background-color: rgb(0, 0, 0);");
    radarTable = new QTableView;

    main_page_layout2->addWidget(Display_Camera);
    main_page_layout2->addWidget(radarTable);
    main_page_layout2->setStretch(0, 1);
    main_page_layout2->setStretch(1, 1);


    main_page_layout3 = new QVBoxLayout;

    basicSetting_layout = new QVBoxLayout;
    radarID_label = new QLabel("Radar ID");
    radarID_edit = new QLineEdit;
    maxDist_label = new QLabel("MAX DIST");
    maxDist_edit = new QLineEdit;
    minDist_label = new QLabel("MIN DIST");
    minDist_edit = new QLineEdit;
    cameraIP_label = new QLabel("IP");
    cameraIP_edit = new QLineEdit;
    cameraPort_label = new QLabel("Port");
    cameraPort_edit = new QLineEdit;
    cameraID_label = new QLabel("ID");
    cameraID_edit = new QLineEdit;
    cameraPW_label = new QLabel("PW");
    cameraPW_edit = new QLineEdit; cameraPW_edit->setEchoMode(QLineEdit::Password);

    basicSetting_spacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding);

    SaveSetting_btn = new QPushButton("SAVE");

    basicSetting_layout->addWidget(radarID_label);
    basicSetting_layout->addWidget(radarID_edit);
    basicSetting_layout->addWidget(maxDist_label);
    basicSetting_layout->addWidget(maxDist_edit);
    basicSetting_layout->addWidget(minDist_label);
    basicSetting_layout->addWidget(minDist_edit);
    basicSetting_layout->addWidget(cameraIP_label);
    basicSetting_layout->addWidget(cameraIP_edit);
    basicSetting_layout->addWidget(cameraPort_label);
    basicSetting_layout->addWidget(cameraPort_edit);
    basicSetting_layout->addWidget(cameraID_label);
    basicSetting_layout->addWidget(cameraID_edit);
    basicSetting_layout->addWidget(cameraPW_label);
    basicSetting_layout->addWidget(cameraPW_edit);
    basicSetting_layout->addSpacerItem(basicSetting_spacer);
    basicSetting_layout->addWidget(SaveSetting_btn);


    PDStatus = new QLabel; PDStatus->setStyleSheet("background-color: rgb(0, 0, 0);");
    QPixmap PDstatusImg("C:/Users/ODYSSEY/Desktop/build-PTZ_TEST-Desktop_Qt_6_2_3_MinGW_64_bit-Debug/debug/img/PD_OUT.png");
    PDStatus->setPixmap(PDstatusImg);
    PDStatus->setScaledContents(true);

    main_page_layout3->addLayout(basicSetting_layout);
    main_page_layout3->addWidget(PDStatus);
    main_page_layout3->setStretch(0, 1);
    main_page_layout3->setStretch(1, 1);

    main_page_layout->addWidget(Display_Radar);
    main_page_layout->addLayout(main_page_layout2);
    main_page_layout->addLayout(main_page_layout3);
    main_page_layout->setStretch(0, 1);
    main_page_layout->setStretch(1, 1);
    main_page_layout->setStretch(2, 1);

    main_page->setLayout(main_page_layout);


    // Radar Setting page
    radarSetting_page = new QWidget;
    radarSetting_page_layout = new QHBoxLayout;

    radarSetting_layout = new QVBoxLayout;

    radar_title_layout = new QHBoxLayout;
    radar_title = new QLabel("Radar"); radar_title->setFont(QFont("맑은 고딕", 20, QFont::Bold));
    radar_title_spacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Fixed);
    writebtn_radarSetting = new QPushButton("Write");

    radar_title_layout->addWidget(radar_title);
    radar_title_layout->addSpacerItem(radar_title_spacer);
    radar_title_layout->addWidget(writebtn_radarSetting);


    radarSetting_value_layout = new QHBoxLayout;

    read_radarSetting_layout = new QVBoxLayout;

    read_NVMReadStatus_layout = new QHBoxLayout;
    read_NVMReadStatus_label = new QLabel("NVMReadStatus");
    read_NVMReadStatus_value = new QLabel("-"); read_NVMReadStatus_value->setAlignment(Qt::AlignRight);
    read_NVMReadStatus_layout->addWidget(read_NVMReadStatus_label);
    read_NVMReadStatus_layout->addWidget(read_NVMReadStatus_value);
    read_NVMReadStatus_layout->setStretch(0, 1);
    read_NVMReadStatus_layout->setStretch(1, 1);

    read_NVMWriteStatus_layout = new QHBoxLayout;
    read_NVMWriteStatus_label = new QLabel("NVMWriteStatus");
    read_NVMWriteStatus_value = new QLabel("-"); read_NVMWriteStatus_value->setAlignment(Qt::AlignRight);
    read_NVMWriteStatus_layout->addWidget(read_NVMWriteStatus_label);
    read_NVMWriteStatus_layout->addWidget(read_NVMWriteStatus_value);
    read_NVMWriteStatus_layout->setStretch(0, 1);
    read_NVMWriteStatus_layout->setStretch(1, 1);

    read_MaxDistanceCfg_layout = new QHBoxLayout;
    read_MaxDistanceCfg_label = new QLabel("MaxDistanceCfg");
    read_MaxDistanceCfg_value = new QLabel("-"); read_MaxDistanceCfg_value->setAlignment(Qt::AlignRight);
    read_MaxDistanceCfg_layout->addWidget(read_MaxDistanceCfg_label);
    read_MaxDistanceCfg_layout->addWidget(read_MaxDistanceCfg_value);
    read_MaxDistanceCfg_layout->setStretch(0, 1);
    read_MaxDistanceCfg_layout->setStretch(1, 1);

    read_PersistentError_layout = new QHBoxLayout;
    read_PersistentError_label = new QLabel("Persistent Error");
    read_PersistentError_value = new QLabel("-"); read_PersistentError_value->setAlignment(Qt::AlignRight);
    read_PersistentError_layout->addWidget(read_PersistentError_label);
    read_PersistentError_layout->addWidget(read_PersistentError_value);
    read_PersistentError_layout->setStretch(0, 1);
    read_PersistentError_layout->setStretch(1, 1);

    read_Interference_layout = new QHBoxLayout;
    read_Interference_label = new QLabel("Interference");
    read_Interference_value = new QLabel("-"); read_Interference_value->setAlignment(Qt::AlignRight);
    read_Interference_layout->addWidget(read_Interference_label);
    read_Interference_layout->addWidget(read_Interference_value);
    read_Interference_layout->setStretch(0, 1);
    read_Interference_layout->setStretch(1, 1);

    read_TemperatureError_layout = new QHBoxLayout;
    read_TemperatureError_label = new QLabel("Temperature Error");
    read_TemperatureError_value = new QLabel("-"); read_TemperatureError_value->setAlignment(Qt::AlignRight);
    read_TemperatureError_layout->addWidget(read_TemperatureError_label);
    read_TemperatureError_layout->addWidget(read_TemperatureError_value);
    read_TemperatureError_layout->setStretch(0, 1);
    read_TemperatureError_layout->setStretch(1, 1);

    read_TemporaryError_layout = new QHBoxLayout;
    read_TemporaryError_label = new QLabel("Temporary Error");
    read_TemporaryError_value = new QLabel("-"); read_TemporaryError_value->setAlignment(Qt::AlignRight);
    read_TemporaryError_layout->addWidget(read_TemporaryError_label);
    read_TemporaryError_layout->addWidget(read_TemporaryError_value);
    read_TemporaryError_layout->setStretch(0, 1);
    read_TemporaryError_layout->setStretch(1, 1);

    read_VoltageError_layout = new QHBoxLayout;
    read_VoltageError_label = new QLabel("Voltage Error");
    read_VoltageError_value = new QLabel("-"); read_VoltageError_value->setAlignment(Qt::AlignRight);
    read_VoltageError_layout->addWidget(read_VoltageError_label);
    read_VoltageError_layout->addWidget(read_VoltageError_value);
    read_VoltageError_layout->setStretch(0, 1);
    read_VoltageError_layout->setStretch(1, 1);

    read_SensorID_layout = new QHBoxLayout;
    read_SensorID_label = new QLabel("Sensor ID");
    read_SensorID_value = new QLabel("-"); read_SensorID_value->setAlignment(Qt::AlignRight);
    read_SensorID_layout->addWidget(read_SensorID_label);
    read_SensorID_layout->addWidget(read_SensorID_value);
    read_SensorID_layout->setStretch(0, 1);
    read_SensorID_layout->setStretch(1, 1);

    read_SortIndex_layout = new QHBoxLayout;
    read_SortIndex_label = new QLabel("SortIndex");
    read_SortIndex_value = new QLabel("-"); read_SortIndex_value->setAlignment(Qt::AlignRight);
    read_SortIndex_layout->addWidget(read_SortIndex_label);
    read_SortIndex_layout->addWidget(read_SortIndex_value);
    read_SortIndex_layout->setStretch(0, 1);
    read_SortIndex_layout->setStretch(1, 1);

    read_RadarPowerCfg_layout = new QHBoxLayout;
    read_RadarPowerCfg_label = new QLabel("RadarPowerCfg");
    read_RadarPowerCfg_value = new QLabel("-"); read_RadarPowerCfg_value->setAlignment(Qt::AlignRight);
    read_RadarPowerCfg_layout->addWidget(read_RadarPowerCfg_label);
    read_RadarPowerCfg_layout->addWidget(read_RadarPowerCfg_value);
    read_RadarPowerCfg_layout->setStretch(0, 1);
    read_RadarPowerCfg_layout->setStretch(1, 1);

    read_CtrlRelayCfg_layout = new QHBoxLayout;
    read_CtrlRelayCfg_label = new QLabel("CtrlRelayCfg");
    read_CtrlRelayCfg_value = new QLabel("-"); read_CtrlRelayCfg_value->setAlignment(Qt::AlignRight);
    read_CtrlRelayCfg_layout->addWidget(read_CtrlRelayCfg_label);
    read_CtrlRelayCfg_layout->addWidget(read_CtrlRelayCfg_value);
    read_CtrlRelayCfg_layout->setStretch(0, 1);
    read_CtrlRelayCfg_layout->setStretch(1, 1);

    read_OutputTypeCfg_layout = new QHBoxLayout;
    read_OutputTypeCfg_label = new QLabel("OutputTypeCfg");
    read_OutputTypeCfg_value = new QLabel("-"); read_OutputTypeCfg_value->setAlignment(Qt::AlignRight);
    read_OutputTypeCfg_layout->addWidget(read_OutputTypeCfg_label);
    read_OutputTypeCfg_layout->addWidget(read_OutputTypeCfg_value);
    read_OutputTypeCfg_layout->setStretch(0, 1);
    read_OutputTypeCfg_layout->setStretch(1, 1);

    read_SendQualityCfg_layout = new QHBoxLayout;
    read_SendQualityCfg_label = new QLabel("SendQualityCfg");
    read_SendQualityCfg_value = new QLabel("-"); read_SendQualityCfg_value->setAlignment(Qt::AlignRight);
    read_SendQualityCfg_layout->addWidget(read_SendQualityCfg_label);
    read_SendQualityCfg_layout->addWidget(read_SendQualityCfg_value);
    read_SendQualityCfg_layout->setStretch(0, 1);
    read_SendQualityCfg_layout->setStretch(1, 1);

    read_SendExtInfoCfg_layout = new QHBoxLayout;
    read_SendExtInfoCfg_label = new QLabel("SendExtInfoCfg");
    read_SendExtInfoCfg_value = new QLabel("-"); read_SendExtInfoCfg_value->setAlignment(Qt::AlignRight);
    read_SendExtInfoCfg_layout->addWidget(read_SendExtInfoCfg_label);
    read_SendExtInfoCfg_layout->addWidget(read_SendExtInfoCfg_value);
    read_SendExtInfoCfg_layout->setStretch(0, 1);
    read_SendExtInfoCfg_layout->setStretch(1, 1);

    read_MotionRxState_layout = new QHBoxLayout;
    read_MotionRxState_label = new QLabel("MotionRxState");
    read_MotionRxState_value = new QLabel("-"); read_MotionRxState_value->setAlignment(Qt::AlignRight);
    read_MotionRxState_layout->addWidget(read_MotionRxState_label);
    read_MotionRxState_layout->addWidget(read_MotionRxState_value);
    read_MotionRxState_layout->setStretch(0, 1);
    read_MotionRxState_layout->setStretch(1, 1);

    read_RCSThreshold_layout = new QHBoxLayout;
    read_RCSThreshold_label = new QLabel("RCS Threshold");
    read_RCSThreshold_value = new QLabel("-"); read_RCSThreshold_value->setAlignment(Qt::AlignRight);
    read_RCSThreshold_layout->addWidget(read_RCSThreshold_label);
    read_RCSThreshold_layout->addWidget(read_RCSThreshold_value);
    read_RCSThreshold_layout->setStretch(0, 1);
    read_RCSThreshold_layout->setStretch(1, 1);

    read_InvalidClusters_layout = new QHBoxLayout;
    read_InvalidClusters_label = new QLabel("InvalidClusters");
    read_InvalidClusters_value = new QLabel("-"); read_InvalidClusters_value->setAlignment(Qt::AlignRight);
    read_InvalidClusters_layout->addWidget(read_InvalidClusters_label);
    read_InvalidClusters_layout->addWidget(read_InvalidClusters_value);
    read_InvalidClusters_layout->setStretch(0, 1);
    read_InvalidClusters_layout->setStretch(1, 1);

    read_radarSetting_layout->addLayout(read_NVMReadStatus_layout);
    read_radarSetting_layout->addLayout(read_NVMWriteStatus_layout);
    read_radarSetting_layout->addLayout(read_MaxDistanceCfg_layout);
    read_radarSetting_layout->addLayout(read_PersistentError_layout);
    read_radarSetting_layout->addLayout(read_Interference_layout);
    read_radarSetting_layout->addLayout(read_TemperatureError_layout);
    read_radarSetting_layout->addLayout(read_TemporaryError_layout);
    read_radarSetting_layout->addLayout(read_VoltageError_layout);
    read_radarSetting_layout->addLayout(read_SensorID_layout);
    read_radarSetting_layout->addLayout(read_SortIndex_layout);
    read_radarSetting_layout->addLayout(read_RadarPowerCfg_layout);
    read_radarSetting_layout->addLayout(read_CtrlRelayCfg_layout);
    read_radarSetting_layout->addLayout(read_OutputTypeCfg_layout);
    read_radarSetting_layout->addLayout(read_SendQualityCfg_layout);
    read_radarSetting_layout->addLayout(read_SendExtInfoCfg_layout);
    read_radarSetting_layout->addLayout(read_MotionRxState_layout);
    read_radarSetting_layout->addLayout(read_RCSThreshold_layout);
    read_radarSetting_layout->addLayout(read_InvalidClusters_layout);
    read_radarSetting_layout->setStretch(0, 1);
    read_radarSetting_layout->setStretch(1, 1);
    read_radarSetting_layout->setStretch(2, 1);
    read_radarSetting_layout->setStretch(3, 1);
    read_radarSetting_layout->setStretch(4, 1);
    read_radarSetting_layout->setStretch(5, 1);
    read_radarSetting_layout->setStretch(6, 1);
    read_radarSetting_layout->setStretch(7, 1);
    read_radarSetting_layout->setStretch(8, 1);
    read_radarSetting_layout->setStretch(9, 1);
    read_radarSetting_layout->setStretch(10, 1);
    read_radarSetting_layout->setStretch(11, 1);
    read_radarSetting_layout->setStretch(12, 1);
    read_radarSetting_layout->setStretch(13, 1);
    read_radarSetting_layout->setStretch(14, 1);
    read_radarSetting_layout->setStretch(15, 1);
    read_radarSetting_layout->setStretch(16, 1);
    read_radarSetting_layout->setStretch(17, 1);


    write_radarSetting_layout = new QVBoxLayout;

    write_MaxDistance_layout = new QHBoxLayout;
    write_MaxDistance_label = new QCheckBox("MaxDistance");
    write_MaxDistance_value = new QLineEdit;
    write_MaxDistance_layout->addWidget(write_MaxDistance_label);
    write_MaxDistance_layout->addWidget(write_MaxDistance_value);
    write_MaxDistance_layout->setStretch(0, 1);
    write_MaxDistance_layout->setStretch(1, 1);

    write_SensorID_layout = new QHBoxLayout;
    write_SensorID_label = new QCheckBox("Sensor ID");
    write_SensorID_value = new QComboBox; write_SensorID_value->addItems({"0", "1", "2", "3", "4", "5", "6", "7"});
    write_SensorID_layout->addWidget(write_SensorID_label);
    write_SensorID_layout->addWidget(write_SensorID_value);
    write_SensorID_layout->setStretch(0, 1);
    write_SensorID_layout->setStretch(1, 1);

    write_OutputType_layout = new QHBoxLayout;
    write_OutputType_label = new QCheckBox("OutputType");
    write_OutputType_value = new QComboBox; write_OutputType_value->addItems({"none", "send objects", "send clusters"});
    write_OutputType_layout->addWidget(write_OutputType_label);
    write_OutputType_layout->addWidget(write_OutputType_value);
    write_OutputType_layout->setStretch(0, 1);
    write_OutputType_layout->setStretch(1, 1);

    write_RadarPower_layout = new QHBoxLayout;
    write_RadarPower_label = new QCheckBox("RadarPower");
    write_RadarPower_value = new QComboBox; write_RadarPower_value->addItems({"standard", "-3dB Tx gain", "-6dB Tx gain", "-9dB Tx gain"});
    write_RadarPower_layout->addWidget(write_RadarPower_label);
    write_RadarPower_layout->addWidget(write_RadarPower_value);
    write_RadarPower_layout->setStretch(0, 1);
    write_RadarPower_layout->setStretch(1, 1);

    write_CtrlRelay_layout = new QHBoxLayout;
    write_CtrlRelay_label = new QCheckBox("CtrlRelay");
    write_CtrlRelay_value = new QComboBox; write_CtrlRelay_value->addItems({"inactive", "active"});
    write_CtrlRelay_layout->addWidget(write_CtrlRelay_label);
    write_CtrlRelay_layout->addWidget(write_CtrlRelay_value);
    write_CtrlRelay_layout->setStretch(0, 1);
    write_CtrlRelay_layout->setStretch(1, 1);

    write_SendQuality_layout = new QHBoxLayout;
    write_SendQuality_label = new QCheckBox("SendQuality");
    write_SendQuality_value = new QComboBox; write_SendQuality_value->addItems({"inactive", "active"});
    write_SendQuality_layout->addWidget(write_SendQuality_label);
    write_SendQuality_layout->addWidget(write_SendQuality_value);
    write_SendQuality_layout->setStretch(0, 1);
    write_SendQuality_layout->setStretch(1, 1);

    write_SendExtInfo_layout = new QHBoxLayout;
    write_SendExtInfo_label = new QCheckBox("SendExtInfo");
    write_SendExtInfo_value = new QComboBox; write_SendExtInfo_value->addItems({"inactive", "active"});
    write_SendExtInfo_layout->addWidget(write_SendExtInfo_label);
    write_SendExtInfo_layout->addWidget(write_SendExtInfo_value);
    write_SendExtInfo_layout->setStretch(0, 1);
    write_SendExtInfo_layout->setStretch(1, 1);

    write_SortIndex_layout = new QHBoxLayout;
    write_SortIndex_label = new QCheckBox("SortIndex");
    write_SortIndex_value = new QComboBox; write_SortIndex_value->addItems({"no sorting", "sorted by range", "sorted by RCS"});
    write_SortIndex_layout->addWidget(write_SortIndex_label);
    write_SortIndex_layout->addWidget(write_SortIndex_value);
    write_SortIndex_layout->setStretch(0, 1);
    write_SortIndex_layout->setStretch(1, 1);

    write_StoreInNVM_layout = new QHBoxLayout;
    write_StoreInNVM_label = new QCheckBox("StoreInNVM");
    write_StoreInNVM_value = new QComboBox; write_StoreInNVM_value->addItems({"inactive", "active"});
    write_StoreInNVM_layout->addWidget(write_StoreInNVM_label);
    write_StoreInNVM_layout->addWidget(write_StoreInNVM_value);
    write_StoreInNVM_layout->setStretch(0, 1);
    write_StoreInNVM_layout->setStretch(1, 1);

    write_RCSThreshold_layout = new QHBoxLayout;
    write_RCSThreshold_label = new QCheckBox("RCSThreshold");
    write_RCSThreshold_value = new QComboBox; write_RCSThreshold_value->addItems({"Standard", "High sensitivity"});
    write_RCSThreshold_layout->addWidget(write_RCSThreshold_label);
    write_RCSThreshold_layout->addWidget(write_RCSThreshold_value);
    write_RCSThreshold_layout->setStretch(0, 1);
    write_RCSThreshold_layout->setStretch(1, 1);

    write_InvalidClusters_layout = new QHBoxLayout;
    write_InvalidClusters_label = new QCheckBox("InvalidClusters");
    write_InvalidClusters_value = new QComboBox; write_InvalidClusters_value->addItems({"Disable", "Enable all invalid clusters", "Enable low RCS dynamic", "Enable low RCS static", "Enable invalid range rate", "Enable range < 1m", "Enable ego mirror", "Enable wrapped stationary"});
    write_InvalidClusters_layout->addWidget(write_InvalidClusters_label);
    write_InvalidClusters_layout->addWidget(write_InvalidClusters_value);
    write_InvalidClusters_layout->setStretch(0, 1);
    write_InvalidClusters_layout->setStretch(1, 1);

    write_radarSetting_layout->addLayout(write_MaxDistance_layout);
    write_radarSetting_layout->addLayout(write_SensorID_layout);
    write_radarSetting_layout->addLayout(write_OutputType_layout);
    write_radarSetting_layout->addLayout(write_RadarPower_layout);
    write_radarSetting_layout->addLayout(write_CtrlRelay_layout);
    write_radarSetting_layout->addLayout(write_SendQuality_layout);
    write_radarSetting_layout->addLayout(write_SendExtInfo_layout);
    write_radarSetting_layout->addLayout(write_SortIndex_layout);
    write_radarSetting_layout->addLayout(write_StoreInNVM_layout);
    write_radarSetting_layout->addLayout(write_RCSThreshold_layout);
    write_radarSetting_layout->addLayout(write_InvalidClusters_layout);
    write_radarSetting_layout->setStretch(0, 1);
    write_radarSetting_layout->setStretch(1, 1);
    write_radarSetting_layout->setStretch(2, 1);
    write_radarSetting_layout->setStretch(3, 1);
    write_radarSetting_layout->setStretch(4, 1);
    write_radarSetting_layout->setStretch(5, 1);
    write_radarSetting_layout->setStretch(6, 1);
    write_radarSetting_layout->setStretch(7, 1);
    write_radarSetting_layout->setStretch(8, 1);
    write_radarSetting_layout->setStretch(9, 1);
    write_radarSetting_layout->setStretch(10, 1);


    radarSetting_value_layout->addLayout(read_radarSetting_layout);
    radarSetting_value_layout->addLayout(write_radarSetting_layout);
    radarSetting_value_layout->setStretch(0, 1);
    radarSetting_value_layout->setStretch(1, 1);

    radarSetting_layout->addLayout(radar_title_layout);
    radarSetting_layout->addLayout(radarSetting_value_layout);
    radarSetting_layout->setStretch(0, 1);
    radarSetting_layout->setStretch(1, 20);


    // Filter Setting page
    filterSetting_layout = new QVBoxLayout;

    filter_title_layout = new QHBoxLayout;
    filter_title = new QLabel("Filter"); filter_title->setFont(QFont("맑은 고딕", 20, QFont::Bold));
    filter_title_spacer = new QSpacerItem(0, 30, QSizePolicy::Expanding, QSizePolicy::Fixed);
    writebtn_filterSetting = new QPushButton("Write");

    filter_title_layout->addWidget(filter_title);
    filter_title_layout->addSpacerItem(filter_title_spacer);
    filter_title_layout->addWidget(writebtn_filterSetting);

    filterSetting_value_layout = new QHBoxLayout;


    read_filterSetting_layout = new QVBoxLayout;

    read_MinMax_layout = new QHBoxLayout;
    read_Empty_label = new QLabel;
    read_Min_label = new QLabel("Min"); read_Min_label->setFont(QFont("맑은 고딕", 10, QFont::Bold)); read_Min_label->setAlignment(Qt::AlignRight);
    read_Max_label = new QLabel("Max"); read_Max_label->setFont(QFont("맑은 고딕", 10, QFont::Bold)); read_Max_label->setAlignment(Qt::AlignRight);
    read_MinMax_layout->addWidget(read_Empty_label);
    read_MinMax_layout->addWidget(read_Min_label);
    read_MinMax_layout->addWidget(read_Max_label);
    read_MinMax_layout->setStretch(0, 1);
    read_MinMax_layout->setStretch(1, 1);
    read_MinMax_layout->setStretch(2, 1);

    read_NofObj_layout = new QHBoxLayout;
    read_NofObj_label = new QLabel("NofObj");
    read_NofObj_min = new QLabel("-"); read_NofObj_min->setAlignment(Qt::AlignRight);
    read_NofObj_max = new QLabel("-"); read_NofObj_max->setAlignment(Qt::AlignRight);
    read_NofObj_layout->addWidget(read_NofObj_label);
    read_NofObj_layout->addWidget(read_NofObj_min);
    read_NofObj_layout->addWidget(read_NofObj_max);
    read_NofObj_layout->setStretch(0, 1);
    read_NofObj_layout->setStretch(1, 1);
    read_NofObj_layout->setStretch(2, 1);

    read_Distance_layout = new QHBoxLayout;
    read_Distance_label = new QLabel("Distance");
    read_Distance_min = new QLabel("-"); read_Distance_min->setAlignment(Qt::AlignRight);
    read_Distance_max = new QLabel("-"); read_Distance_max->setAlignment(Qt::AlignRight);
    read_Distance_layout->addWidget(read_Distance_label);
    read_Distance_layout->addWidget(read_Distance_min);
    read_Distance_layout->addWidget(read_Distance_max);
    read_Distance_layout->setStretch(0, 1);
    read_Distance_layout->setStretch(1, 1);
    read_Distance_layout->setStretch(2, 1);

    read_Azimuth_layout = new QHBoxLayout;
    read_Azimuth_label = new QLabel("Azimuth");
    read_Azimuth_min = new QLabel("-"); read_Azimuth_min->setAlignment(Qt::AlignRight);
    read_Azimuth_max = new QLabel("-"); read_Azimuth_max->setAlignment(Qt::AlignRight);
    read_Azimuth_layout->addWidget(read_Azimuth_label);
    read_Azimuth_layout->addWidget(read_Azimuth_min);
    read_Azimuth_layout->addWidget(read_Azimuth_max);
    read_Azimuth_layout->setStretch(0, 1);
    read_Azimuth_layout->setStretch(1, 1);
    read_Azimuth_layout->setStretch(2, 1);

    read_VrelOncome_layout = new QHBoxLayout;
    read_VrelOncome_label = new QLabel("VrelOncome");
    read_VrelOncome_min = new QLabel("-"); read_VrelOncome_min->setAlignment(Qt::AlignRight);
    read_VrelOncome_max = new QLabel("-"); read_VrelOncome_max->setAlignment(Qt::AlignRight);
    read_VrelOncome_layout->addWidget(read_VrelOncome_label);
    read_VrelOncome_layout->addWidget(read_VrelOncome_min);
    read_VrelOncome_layout->addWidget(read_VrelOncome_max);
    read_VrelOncome_layout->setStretch(0, 1);
    read_VrelOncome_layout->setStretch(1, 1);
    read_VrelOncome_layout->setStretch(2, 1);

    read_VrelDepart_layout = new QHBoxLayout;
    read_VrelDepart_label = new QLabel("VrelDepart");
    read_VrelDepart_min = new QLabel("-"); read_VrelDepart_min->setAlignment(Qt::AlignRight);
    read_VrelDepart_max = new QLabel("-"); read_VrelDepart_max->setAlignment(Qt::AlignRight);
    read_VrelDepart_layout->addWidget(read_VrelDepart_label);
    read_VrelDepart_layout->addWidget(read_VrelDepart_min);
    read_VrelDepart_layout->addWidget(read_VrelDepart_max);
    read_VrelDepart_layout->setStretch(0, 1);
    read_VrelDepart_layout->setStretch(1, 1);
    read_VrelDepart_layout->setStretch(2, 1);

    read_RCS_layout = new QHBoxLayout;
    read_RCS_label = new QLabel("RCS");
    read_RCS_min = new QLabel("-"); read_RCS_min->setAlignment(Qt::AlignRight);
    read_RCS_max = new QLabel("-"); read_RCS_max->setAlignment(Qt::AlignRight);
    read_RCS_layout->addWidget(read_RCS_label);
    read_RCS_layout->addWidget(read_RCS_min);
    read_RCS_layout->addWidget(read_RCS_max);
    read_RCS_layout->setStretch(0, 1);
    read_RCS_layout->setStretch(1, 1);
    read_RCS_layout->setStretch(2, 1);

    read_Lifetime_layout = new QHBoxLayout;
    read_Lifetime_label = new QLabel("Lifetime");
    read_Lifetime_min = new QLabel("-"); read_Lifetime_min->setAlignment(Qt::AlignRight);
    read_Lifetime_max = new QLabel("-"); read_Lifetime_max->setAlignment(Qt::AlignRight);
    read_Lifetime_layout->addWidget(read_Lifetime_label);
    read_Lifetime_layout->addWidget(read_Lifetime_min);
    read_Lifetime_layout->addWidget(read_Lifetime_max);
    read_Lifetime_layout->setStretch(0, 1);
    read_Lifetime_layout->setStretch(1, 1);
    read_Lifetime_layout->setStretch(2, 1);

    read_Size_layout = new QHBoxLayout;
    read_Size_label = new QLabel("Size");
    read_Size_min = new QLabel("-"); read_Size_min->setAlignment(Qt::AlignRight);
    read_Size_max = new QLabel("-"); read_Size_max->setAlignment(Qt::AlignRight);
    read_Size_layout->addWidget(read_Size_label);
    read_Size_layout->addWidget(read_Size_min);
    read_Size_layout->addWidget(read_Size_max);
    read_Size_layout->setStretch(0, 1);
    read_Size_layout->setStretch(1, 1);
    read_Size_layout->setStretch(2, 1);

    read_ProbExists_layout = new QHBoxLayout;
    read_ProbExists_label = new QLabel("ProbExists");
    read_ProbExists_min = new QLabel("-"); read_ProbExists_min->setAlignment(Qt::AlignRight);
    read_ProbExists_max = new QLabel("-"); read_ProbExists_max->setAlignment(Qt::AlignRight);
    read_ProbExists_layout->addWidget(read_ProbExists_label);
    read_ProbExists_layout->addWidget(read_ProbExists_min);
    read_ProbExists_layout->addWidget(read_ProbExists_max);
    read_ProbExists_layout->setStretch(0, 1);
    read_ProbExists_layout->setStretch(1, 1);
    read_ProbExists_layout->setStretch(2, 1);

    read_Y_layout = new QHBoxLayout;
    read_Y_label = new QLabel("Y");
    read_Y_min = new QLabel("-"); read_Y_min->setAlignment(Qt::AlignRight);
    read_Y_max = new QLabel("-"); read_Y_max->setAlignment(Qt::AlignRight);
    read_Y_layout->addWidget(read_Y_label);
    read_Y_layout->addWidget(read_Y_min);
    read_Y_layout->addWidget(read_Y_max);
    read_Y_layout->setStretch(0, 1);
    read_Y_layout->setStretch(1, 1);
    read_Y_layout->setStretch(2, 1);

    read_X_layout = new QHBoxLayout;
    read_X_label = new QLabel("X");
    read_X_min = new QLabel("-"); read_X_min->setAlignment(Qt::AlignRight);
    read_X_max = new QLabel("-"); read_X_max->setAlignment(Qt::AlignRight);
    read_X_layout->addWidget(read_X_label);
    read_X_layout->addWidget(read_X_min);
    read_X_layout->addWidget(read_X_max);
    read_X_layout->setStretch(0, 1);
    read_X_layout->setStretch(1, 1);
    read_X_layout->setStretch(2, 1);

    read_VYRightLeft_layout = new QHBoxLayout;
    read_VYRightLeft_label = new QLabel("VYRightLeft");
    read_VYRightLeft_min = new QLabel("-"); read_VYRightLeft_min->setAlignment(Qt::AlignRight);
    read_VYRightLeft_max = new QLabel("-"); read_VYRightLeft_max->setAlignment(Qt::AlignRight);
    read_VYRightLeft_layout->addWidget(read_VYRightLeft_label);
    read_VYRightLeft_layout->addWidget(read_VYRightLeft_min);
    read_VYRightLeft_layout->addWidget(read_VYRightLeft_max);
    read_VYRightLeft_layout->setStretch(0, 1);
    read_VYRightLeft_layout->setStretch(1, 1);
    read_VYRightLeft_layout->setStretch(2, 1);

    read_VXOncome_layout = new QHBoxLayout;
    read_VXOncome_label = new QLabel("VXOncome");
    read_VXOncome_min = new QLabel("-"); read_VXOncome_min->setAlignment(Qt::AlignRight);
    read_VXOncome_max = new QLabel("-"); read_VXOncome_max->setAlignment(Qt::AlignRight);
    read_VXOncome_layout->addWidget(read_VXOncome_label);
    read_VXOncome_layout->addWidget(read_VXOncome_min);
    read_VXOncome_layout->addWidget(read_VXOncome_max);
    read_VXOncome_layout->setStretch(0, 1);
    read_VXOncome_layout->setStretch(1, 1);
    read_VXOncome_layout->setStretch(2, 1);

    read_VYLeftRight_layout = new QHBoxLayout;
    read_VYLeftRight_label = new QLabel("VYLeftRight");
    read_VYLeftRight_min = new QLabel("-"); read_VYLeftRight_min->setAlignment(Qt::AlignRight);
    read_VYLeftRight_max = new QLabel("-"); read_VYLeftRight_max->setAlignment(Qt::AlignRight);
    read_VYLeftRight_layout->addWidget(read_VYLeftRight_label);
    read_VYLeftRight_layout->addWidget(read_VYLeftRight_min);
    read_VYLeftRight_layout->addWidget(read_VYLeftRight_max);
    read_VYLeftRight_layout->setStretch(0, 1);
    read_VYLeftRight_layout->setStretch(1, 1);
    read_VYLeftRight_layout->setStretch(2, 1);

    read_VXDepart_layout = new QHBoxLayout;
    read_VXDepart_label = new QLabel("VXDepart");
    read_VXDepart_min = new QLabel("-"); read_VXDepart_min->setAlignment(Qt::AlignRight);
    read_VXDepart_max = new QLabel("-"); read_VXDepart_max->setAlignment(Qt::AlignRight);
    read_VXDepart_layout->addWidget(read_VXDepart_label);
    read_VXDepart_layout->addWidget(read_VXDepart_min);
    read_VXDepart_layout->addWidget(read_VXDepart_max);
    read_VXDepart_layout->setStretch(0, 1);
    read_VXDepart_layout->setStretch(1, 1);
    read_VXDepart_layout->setStretch(2, 1);


    read_filterSetting_layout->addLayout(read_MinMax_layout);
    read_filterSetting_layout->addLayout(read_NofObj_layout);
    read_filterSetting_layout->addLayout(read_Distance_layout);
    read_filterSetting_layout->addLayout(read_Azimuth_layout);
    read_filterSetting_layout->addLayout(read_VrelOncome_layout);
    read_filterSetting_layout->addLayout(read_VrelDepart_layout);
    read_filterSetting_layout->addLayout(read_RCS_layout);
    read_filterSetting_layout->addLayout(read_Lifetime_layout);
    read_filterSetting_layout->addLayout(read_Size_layout);
    read_filterSetting_layout->addLayout(read_ProbExists_layout);
    read_filterSetting_layout->addLayout(read_Y_layout);
    read_filterSetting_layout->addLayout(read_X_layout);
    read_filterSetting_layout->addLayout(read_VYRightLeft_layout);
    read_filterSetting_layout->addLayout(read_VXOncome_layout);
    read_filterSetting_layout->addLayout(read_VYLeftRight_layout);
    read_filterSetting_layout->addLayout(read_VXDepart_layout);
    read_filterSetting_layout->setStretch(0, 1);
    read_filterSetting_layout->setStretch(1, 1);
    read_filterSetting_layout->setStretch(2, 1);
    read_filterSetting_layout->setStretch(3, 1);
    read_filterSetting_layout->setStretch(4, 1);
    read_filterSetting_layout->setStretch(5, 1);
    read_filterSetting_layout->setStretch(6, 1);
    read_filterSetting_layout->setStretch(7, 1);
    read_filterSetting_layout->setStretch(8, 1);
    read_filterSetting_layout->setStretch(9, 1);
    read_filterSetting_layout->setStretch(10, 1);
    read_filterSetting_layout->setStretch(11, 1);
    read_filterSetting_layout->setStretch(12, 1);
    read_filterSetting_layout->setStretch(13, 1);
    read_filterSetting_layout->setStretch(14, 1);
    read_filterSetting_layout->setStretch(15, 1);


    write_filterSetting_layout = new QVBoxLayout;


    write_MinMaxActive_layout = new QHBoxLayout;
    write_Empty_label = new QLabel;
    write_Min_label = new QLabel("Min"); write_Min_label->setFont(QFont("맑은 고딕", 10, QFont::Bold)); write_Min_label->setAlignment(Qt::AlignRight);
    write_Max_label = new QLabel("Max"); write_Max_label->setFont(QFont("맑은 고딕", 10, QFont::Bold)); write_Max_label->setAlignment(Qt::AlignRight);
    write_Active_label = new QLabel("Active"); write_Active_label->setFont(QFont("맑은 고딕", 10, QFont::Bold)); write_Active_label->setAlignment(Qt::AlignRight);
    write_MinMaxActive_layout->addWidget(write_Empty_label);
    write_MinMaxActive_layout->addWidget(write_Min_label);
    write_MinMaxActive_layout->addWidget(write_Max_label);
    write_MinMaxActive_layout->addWidget(write_Active_label);
    write_MinMaxActive_layout->setStretch(0, 5);
    write_MinMaxActive_layout->setStretch(1, 5);
    write_MinMaxActive_layout->setStretch(2, 5);
    write_MinMaxActive_layout->setStretch(3, 1);

    write_NofObj_layout = new QHBoxLayout;
    write_NofObj_valid = new QCheckBox("NofObj");
    write_NofObj_min = new QLineEdit;
    write_NofObj_max = new QLineEdit;
    write_NofObj_active = new QCheckBox;
    write_NofObj_layout->addWidget(write_NofObj_valid);
    write_NofObj_layout->addWidget(write_NofObj_min);
    write_NofObj_layout->addWidget(write_NofObj_max);
    write_NofObj_layout->addWidget(write_NofObj_active);
    write_NofObj_layout->setStretch(0, 5);
    write_NofObj_layout->setStretch(1, 5);
    write_NofObj_layout->setStretch(2, 5);
    write_NofObj_layout->setStretch(3, 1);

    write_Distance_layout = new QHBoxLayout;
    write_Distance_valid = new QCheckBox("Distance");
    write_Distance_min = new QLineEdit;
    write_Distance_max = new QLineEdit;
    write_Distance_active = new QCheckBox;
    write_Distance_layout->addWidget(write_Distance_valid);
    write_Distance_layout->addWidget(write_Distance_min);
    write_Distance_layout->addWidget(write_Distance_max);
    write_Distance_layout->addWidget(write_Distance_active);
    write_Distance_layout->setStretch(0, 5);
    write_Distance_layout->setStretch(1, 5);
    write_Distance_layout->setStretch(2, 5);
    write_Distance_layout->setStretch(3, 1);

    write_Azimuth_layout = new QHBoxLayout;
    write_Azimuth_valid = new QCheckBox("Azimuth");
    write_Azimuth_min = new QLineEdit;
    write_Azimuth_max = new QLineEdit;
    write_Azimuth_active = new QCheckBox;
    write_Azimuth_layout->addWidget(write_Azimuth_valid);
    write_Azimuth_layout->addWidget(write_Azimuth_min);
    write_Azimuth_layout->addWidget(write_Azimuth_max);
    write_Azimuth_layout->addWidget(write_Azimuth_active);
    write_Azimuth_layout->setStretch(0, 5);
    write_Azimuth_layout->setStretch(1, 5);
    write_Azimuth_layout->setStretch(2, 5);
    write_Azimuth_layout->setStretch(3, 1);

    write_VrelOncome_layout = new QHBoxLayout;
    write_VrelOncome_valid = new QCheckBox("VrelOncome");
    write_VrelOncome_min = new QLineEdit;
    write_VrelOncome_max = new QLineEdit;
    write_VrelOncome_active = new QCheckBox;
    write_VrelOncome_layout->addWidget(write_VrelOncome_valid);
    write_VrelOncome_layout->addWidget(write_VrelOncome_min);
    write_VrelOncome_layout->addWidget(write_VrelOncome_max);
    write_VrelOncome_layout->addWidget(write_VrelOncome_active);
    write_VrelOncome_layout->setStretch(0, 5);
    write_VrelOncome_layout->setStretch(1, 5);
    write_VrelOncome_layout->setStretch(2, 5);
    write_VrelOncome_layout->setStretch(3, 1);

    write_VrelDepart_layout = new QHBoxLayout;
    write_VrelDepart_valid = new QCheckBox("VrelDepart");
    write_VrelDepart_min = new QLineEdit;
    write_VrelDepart_max = new QLineEdit;
    write_VrelDepart_active = new QCheckBox;
    write_VrelDepart_layout->addWidget(write_VrelDepart_valid);
    write_VrelDepart_layout->addWidget(write_VrelDepart_min);
    write_VrelDepart_layout->addWidget(write_VrelDepart_max);
    write_VrelDepart_layout->addWidget(write_VrelDepart_active);
    write_VrelDepart_layout->setStretch(0, 5);
    write_VrelDepart_layout->setStretch(1, 5);
    write_VrelDepart_layout->setStretch(2, 5);
    write_VrelDepart_layout->setStretch(3, 1);

    write_RCS_layout = new QHBoxLayout;
    write_RCS_valid = new QCheckBox("RCS");
    write_RCS_min = new QLineEdit;
    write_RCS_max = new QLineEdit;
    write_RCS_active = new QCheckBox;
    write_RCS_layout->addWidget(write_RCS_valid);
    write_RCS_layout->addWidget(write_RCS_min);
    write_RCS_layout->addWidget(write_RCS_max);
    write_RCS_layout->addWidget(write_RCS_active);
    write_RCS_layout->setStretch(0, 5);
    write_RCS_layout->setStretch(1, 5);
    write_RCS_layout->setStretch(2, 5);
    write_RCS_layout->setStretch(3, 1);

    write_Lifetime_layout = new QHBoxLayout;
    write_Lifetime_valid = new QCheckBox("Lifetime");
    write_Lifetime_min = new QLineEdit;
    write_Lifetime_max = new QLineEdit;
    write_Lifetime_active = new QCheckBox;
    write_Lifetime_layout->addWidget(write_Lifetime_valid);
    write_Lifetime_layout->addWidget(write_Lifetime_min);
    write_Lifetime_layout->addWidget(write_Lifetime_max);
    write_Lifetime_layout->addWidget(write_Lifetime_active);
    write_Lifetime_layout->setStretch(0, 5);
    write_Lifetime_layout->setStretch(1, 5);
    write_Lifetime_layout->setStretch(2, 5);
    write_Lifetime_layout->setStretch(3, 1);

    write_Size_layout = new QHBoxLayout;
    write_Size_valid = new QCheckBox("Size");
    write_Size_min = new QLineEdit;
    write_Size_max = new QLineEdit;
    write_Size_active = new QCheckBox;
    write_Size_layout->addWidget(write_Size_valid);
    write_Size_layout->addWidget(write_Size_min);
    write_Size_layout->addWidget(write_Size_max);
    write_Size_layout->addWidget(write_Size_active);
    write_Size_layout->setStretch(0, 5);
    write_Size_layout->setStretch(1, 5);
    write_Size_layout->setStretch(2, 5);
    write_Size_layout->setStretch(3, 1);

    write_ProbExists_layout = new QHBoxLayout;
    write_ProbExists_valid = new QCheckBox("ProbExists");
    write_ProbExists_min = new QComboBox; write_ProbExists_min->addItems({"0%", "25%", "50%", "75%", "90%", "99%", "99.9%", "100%"});
    write_ProbExists_max = new QComboBox; write_ProbExists_max->addItems({"0%", "25%", "50%", "75%", "90%", "99%", "99.9%", "100%"});
    write_ProbExists_active = new QCheckBox;
    write_ProbExists_layout->addWidget(write_ProbExists_valid);
    write_ProbExists_layout->addWidget(write_ProbExists_min);
    write_ProbExists_layout->addWidget(write_ProbExists_max);
    write_ProbExists_layout->addWidget(write_ProbExists_active);
    write_ProbExists_layout->setStretch(0, 5);
    write_ProbExists_layout->setStretch(1, 5);
    write_ProbExists_layout->setStretch(2, 5);
    write_ProbExists_layout->setStretch(3, 1);

    write_Y_layout = new QHBoxLayout;
    write_Y_valid = new QCheckBox("Y");
    write_Y_min = new QLineEdit;
    write_Y_max = new QLineEdit;
    write_Y_active = new QCheckBox;
    write_Y_layout->addWidget(write_Y_valid);
    write_Y_layout->addWidget(write_Y_min);
    write_Y_layout->addWidget(write_Y_max);
    write_Y_layout->addWidget(write_Y_active);
    write_Y_layout->setStretch(0, 5);
    write_Y_layout->setStretch(1, 5);
    write_Y_layout->setStretch(2, 5);
    write_Y_layout->setStretch(3, 1);

    write_X_layout = new QHBoxLayout;
    write_X_valid = new QCheckBox("X");
    write_X_min = new QLineEdit;
    write_X_max = new QLineEdit;
    write_X_active = new QCheckBox;
    write_X_layout->addWidget(write_X_valid);
    write_X_layout->addWidget(write_X_min);
    write_X_layout->addWidget(write_X_max);
    write_X_layout->addWidget(write_X_active);
    write_X_layout->setStretch(0, 5);
    write_X_layout->setStretch(1, 5);
    write_X_layout->setStretch(2, 5);
    write_X_layout->setStretch(3, 1);

    write_VYRightLeft_layout = new QHBoxLayout;
    write_VYRightLeft_valid = new QCheckBox("VYRightLeft");
    write_VYRightLeft_min = new QLineEdit;
    write_VYRightLeft_max = new QLineEdit;
    write_VYRightLeft_active = new QCheckBox;
    write_VYRightLeft_layout->addWidget(write_VYRightLeft_valid);
    write_VYRightLeft_layout->addWidget(write_VYRightLeft_min);
    write_VYRightLeft_layout->addWidget(write_VYRightLeft_max);
    write_VYRightLeft_layout->addWidget(write_VYRightLeft_active);
    write_VYRightLeft_layout->setStretch(0, 5);
    write_VYRightLeft_layout->setStretch(1, 5);
    write_VYRightLeft_layout->setStretch(2, 5);
    write_VYRightLeft_layout->setStretch(3, 1);

    write_VXOncome_layout = new QHBoxLayout;
    write_VXOncome_valid = new QCheckBox("VXOncome");
    write_VXOncome_min = new QLineEdit;
    write_VXOncome_max = new QLineEdit;
    write_VXOncome_active = new QCheckBox;
    write_VXOncome_layout->addWidget(write_VXOncome_valid);
    write_VXOncome_layout->addWidget(write_VXOncome_min);
    write_VXOncome_layout->addWidget(write_VXOncome_max);
    write_VXOncome_layout->addWidget(write_VXOncome_active);
    write_VXOncome_layout->setStretch(0, 5);
    write_VXOncome_layout->setStretch(1, 5);
    write_VXOncome_layout->setStretch(2, 5);
    write_VXOncome_layout->setStretch(3, 1);

    write_VYLeftRight_layout = new QHBoxLayout;
    write_VYLeftRight_valid = new QCheckBox("VYLeftRight");
    write_VYLeftRight_min = new QLineEdit;
    write_VYLeftRight_max = new QLineEdit;
    write_VYLeftRight_active = new QCheckBox;
    write_VYLeftRight_layout->addWidget(write_VYLeftRight_valid);
    write_VYLeftRight_layout->addWidget(write_VYLeftRight_min);
    write_VYLeftRight_layout->addWidget(write_VYLeftRight_max);
    write_VYLeftRight_layout->addWidget(write_VYLeftRight_active);
    write_VYLeftRight_layout->setStretch(0, 5);
    write_VYLeftRight_layout->setStretch(1, 5);
    write_VYLeftRight_layout->setStretch(2, 5);
    write_VYLeftRight_layout->setStretch(3, 1);

    write_VXDepart_layout = new QHBoxLayout;
    write_VXDepart_valid = new QCheckBox("VXDepart");
    write_VXDepart_min = new QLineEdit;
    write_VXDepart_max = new QLineEdit;
    write_VXDepart_active = new QCheckBox;
    write_VXDepart_layout->addWidget(write_VXDepart_valid);
    write_VXDepart_layout->addWidget(write_VXDepart_min);
    write_VXDepart_layout->addWidget(write_VXDepart_max);
    write_VXDepart_layout->addWidget(write_VXDepart_active);
    write_VXDepart_layout->setStretch(0, 5);
    write_VXDepart_layout->setStretch(1, 5);
    write_VXDepart_layout->setStretch(2, 5);
    write_VXDepart_layout->setStretch(3, 1);


    write_filterSetting_layout->addLayout(write_MinMaxActive_layout);
    write_filterSetting_layout->addLayout(write_NofObj_layout);
    write_filterSetting_layout->addLayout(write_Distance_layout);
    write_filterSetting_layout->addLayout(write_Azimuth_layout);
    write_filterSetting_layout->addLayout(write_Azimuth_layout);
    write_filterSetting_layout->addLayout(write_VrelDepart_layout);
    write_filterSetting_layout->addLayout(write_RCS_layout);
    write_filterSetting_layout->addLayout(write_Lifetime_layout);
    write_filterSetting_layout->addLayout(write_Size_layout);
    write_filterSetting_layout->addLayout(write_ProbExists_layout);
    write_filterSetting_layout->addLayout(write_Y_layout);
    write_filterSetting_layout->addLayout(write_X_layout);
    write_filterSetting_layout->addLayout(write_VYRightLeft_layout);
    write_filterSetting_layout->addLayout(write_VXOncome_layout);
    write_filterSetting_layout->addLayout(write_VYLeftRight_layout);
    write_filterSetting_layout->addLayout(write_VXDepart_layout);
    write_filterSetting_layout->setStretch(0, 1);
    write_filterSetting_layout->setStretch(1, 1);
    write_filterSetting_layout->setStretch(2, 1);
    write_filterSetting_layout->setStretch(3, 1);
    write_filterSetting_layout->setStretch(4, 1);
    write_filterSetting_layout->setStretch(5, 1);
    write_filterSetting_layout->setStretch(6, 1);
    write_filterSetting_layout->setStretch(7, 1);
    write_filterSetting_layout->setStretch(8, 1);
    write_filterSetting_layout->setStretch(9, 1);
    write_filterSetting_layout->setStretch(10, 1);
    write_filterSetting_layout->setStretch(11, 1);
    write_filterSetting_layout->setStretch(12, 1);
    write_filterSetting_layout->setStretch(13, 1);
    write_filterSetting_layout->setStretch(14, 1);
    write_filterSetting_layout->setStretch(15, 1);




    filterSetting_value_layout->addLayout(read_filterSetting_layout);
    filterSetting_value_layout->addLayout(write_filterSetting_layout);
    filterSetting_value_layout->setStretch(0, 1);
    filterSetting_value_layout->setStretch(1, 1);


    filterSetting_layout->addLayout(filter_title_layout);
    filterSetting_layout->addLayout(filterSetting_value_layout);
    filterSetting_layout->setStretch(0, 1);
    filterSetting_layout->setStretch(1, 20);

    radarSetting_page_layout->addLayout(radarSetting_layout);
    radarSetting_page_layout->addLayout(filterSetting_layout);
    radarSetting_layout->setStretch(0, 1);
    radarSetting_layout->setStretch(1, 1);

    radarSetting_page->setLayout(radarSetting_page_layout);

    page->insertWidget(0, main_page);
    page->insertWidget(1, radarSetting_page);


    // Show Status Bar
    statusBar_layout = new QHBoxLayout;
    realTime = new QLabel;
    statusBar_layout_spacer = new QSpacerItem(0, 40, QSizePolicy::Expanding, QSizePolicy::Fixed);
    status_layout = new QHBoxLayout;
    radarStatus = new QLabel;
    cameraStatus = new QLabel;

    statusBar_layout->setSpacing(0);

    realTime->setAlignment(Qt::AlignCenter);
    radarStatus->setAlignment(Qt::AlignCenter);
    cameraStatus->setAlignment(Qt::AlignCenter);

    radarStatus->setText("Radar");
    cameraStatus->setText("Camera");
    radarStatus->setFrameShape(QFrame::Box);
    cameraStatus->setFrameShape(QFrame::Box);
    radarStatus->setLineWidth(1);
    cameraStatus->setLineWidth(1);

    realTime->setMaximumSize(300, 40);
    radarStatus->setMaximumSize(100, 40);
    cameraStatus->setMaximumSize(100, 40);

    status_layout->addWidget(radarStatus);
    status_layout->addWidget(cameraStatus);
    status_layout->setStretch(0, 1);
    status_layout->setStretch(1, 1);

    statusBar_layout->addWidget(realTime);
    statusBar_layout->addSpacerItem(statusBar_layout_spacer);
    statusBar_layout->addLayout(status_layout);


    main_Contents_layout->setContentsMargins(0, 0, 0, 0);
    main_Contents_layout->addLayout(button_layout);
    main_Contents_layout->addWidget(page);
    main_Contents_layout->addLayout(statusBar_layout);
    main_Contents_layout->setStretch(0, 1);
    main_Contents_layout->setStretch(1, 30);
    main_Contents_layout->setStretch(2, 1);

    total_layout->setContentsMargins(0, 0, 0, 0);
    total_layout->addLayout(AI_Display_layout);
    total_layout->addLayout(main_Contents_layout);
    total_layout->setStretch(0, 1);
    total_layout->setStretch(1, 20);

    QWidget * window = new QWidget();
    window->setLayout(total_layout);
    window->setMinimumSize(1280, 720);
    setCentralWidget(window);
}

void MainWindow::read_Setting_ini(QString path)
{
    QSettings* setting = new QSettings(path, QSettings::IniFormat);
    QString key;

    key = "RADAR/";
    LOWER_SPEED = setting->value(key + "LOWER_SPEED", 0).toInt();
    UPPER_SPEED = setting->value(key + "UPPER_SPEED", 150).toInt();
}

void MainWindow::writeSettings()
{
    m_settings.beginGroup("/Settings");

    m_settings.setValue("/RadarID", radarID_edit->text()); Display_Radar->Radar_ID = radarID_edit->text().toInt();
    m_settings.setValue("/minDist", minDist_edit->text()); minDist = minDist_edit->text().toInt();
    m_settings.setValue("/maxDist", maxDist_edit->text()); maxDist = maxDist_edit->text().toInt();

    m_settings.setValue("/cameraIP", cameraIP_edit->text()); cameraIP = cameraIP_edit->text();
    m_settings.setValue("/cameraPort", cameraPort_edit->text()); cameraPort = cameraPort_edit->text();
    m_settings.setValue("/cameraID", cameraID_edit->text()); cameraID = cameraID_edit->text();
    m_settings.setValue("/cameraPW", cameraPW_edit->text()); cameraPW = cameraPW_edit->text();

    m_settings.endGroup();
}

void MainWindow::readSettings() {

    m_settings.beginGroup("/Settings");

    radarID_edit->setText(m_settings.value("/RadarID", "0").toString()); Display_Radar->Radar_ID = radarID_edit->text().toInt();
    minDist_edit->setText(m_settings.value("/minDist", "0").toString()); minDist = minDist_edit->text().toInt();
    maxDist_edit->setText(m_settings.value("/maxDist", "150").toString()); maxDist = maxDist_edit->text().toInt();

    cameraIP_edit->setText(m_settings.value("/cameraIP", "192.168.1.30").toString()); cameraIP = cameraIP_edit->text();
    cameraPort_edit->setText(m_settings.value("/cameraPort", "554").toString()); cameraPort = cameraPort_edit->text();
    cameraID_edit->setText(m_settings.value("/cameraID", "admin").toString()); cameraID = cameraID_edit->text();
    cameraPW_edit->setText(m_settings.value("/cameraPW", "admin").toString()); cameraPW = cameraPW_edit->text();

    m_settings.endGroup();
}

void MainWindow::closeEvent(QCloseEvent * event)
{
    Disconnect_Radar();
    Disconnect_Camera();
    exit(0);
}

void MainWindow::readTime()
{
    global_time = QDateTime::currentDateTime();
    realTime->setText(global_time.toString("   yyyy/MM/dd hh:mm:ss.zzz"));
}
void MainWindow::initValue()
{
    InitializeValue_Radar();
    InitializeValue_Camera();

    readSettings();
    read_Setting_ini("/home/pi/Simulator_Setting.ini");

    storage_make_empty();
}

bool MainWindow::read_CarLane_ini(QString path)
{
    QSettings* setting = new QSettings(path, QSettings::IniFormat);

    QString key = "LANE/";
    Display_Radar->MAX_DISTLONG = setting->value(key + "MAX_DISTLONG", 150).toInt();
    Display_Radar->MAX_DISTLAT = setting->value(key + "MAX_DISTLAT", 10).toInt();
    Display_Radar->LANE_NUM = setting->value(key + "LANE_NUM", 1).toInt();
    Display_Radar->LANE_SPACE = setting->value(key + "LANE_SPACE", 25).toInt();
    Display_Radar->LANE_COUNT = int(Display_Radar->MAX_DISTLONG / Display_Radar->LANE_SPACE) + 1;

    int i;
    Display_Radar->lanePoint = new float*[Display_Radar->LANE_NUM + 1];
    for(i=0; i<(Display_Radar->LANE_NUM + 1); i++)
        Display_Radar->lanePoint[i] = new float[Display_Radar->LANE_COUNT];

    key = "POINT/";
    for(int i=0; i<(Display_Radar->LANE_NUM + 1); i++)
    {
        for(int j=0; j<Display_Radar->LANE_COUNT; j++)
        {
            QString count = QString::number(i*Display_Radar->LANE_COUNT + j);
            Display_Radar->lanePoint[i][j] = setting->value(key + count).toFloat() + 1.000f;
        }
    }

    Display_Radar->setLookUpImage();

    Display_Radar->haveLanePoint = true;

    delete setting;
}

void MainWindow::storage_make_empty()
{

}
bool MainWindow::InitializeValue_Radar()
{
    m_Handle = PCAN_USBBUS1;
    m_Baudrate = PCAN_BAUD_500K;
    m_Type = PCAN_TYPE_ISA;
    m_Port = 0x0100;
    m_Interrupt = 0x3;

    startRadar = false;

    read_CarLane_ini("/home/pi/LaneInfo.ini");

    return true;
}

void MainWindow::on_Connectbtn_Radar_clicked()
{
    Connect_Radar();
}

void MainWindow::on_Disconnectbtn_Radar_clicked()
{
    Disconnect_Radar();
}
bool MainWindow::Connect_Radar()
{
    if(!startRadar)
    {
        Connectbtn_Radar->setEnabled(false);

        m_PCANDevice = new PCANBasicClass();
        TPCANStatus stsResult;
        stsResult = m_PCANDevice->Initialize(m_Handle, m_Baudrate, m_Type, m_Port, m_Interrupt);
        if(stsResult != PCAN_ERROR_OK)
        {
            qDebug() << "PCAN Initialize ERROR";
            return false;
        }
        else
        {
//            startRadar = true;
//            RadarConnection->setStyleSheet("background-color: rgb(0, 255, 0);");
//            pthread_create(&radarThread, nullptr, MainWindow::callreadCANMessageFunc, this);

//            setFilterbtn->click();

//            Traffictimer = new QTimer(this);
//            connect(Traffictimer, SIGNAL(timeout()), this, SLOT(traffic_save_timer()));
//            Traffictimer->start(10000);
//            return true;
        }
    }

    return true;
}
bool MainWindow::Disconnect_Radar()
{
    startRadar = false;
    TPCANStatus stsResult = m_PCANDevice->Uninitialize(m_Handle);
    if(stsResult != PCAN_ERROR_OK)
    {
        qDebug() << "UnInit ERROR";
        return false;
    }
    else
    {
//        Traffictimer->stop();
//        for(int i=0; i<MAX_OBJ; i++)
//        {
//            ui->showObjInfo->Obj_data[i].remove_all();
//            memset(ui->showObjInfo->exist, 0, sizeof(bool) * MAX_OBJ);
//            memset(ui->showObjInfo->this_frame_data, 0, sizeof(Obj_inf) * MAX_OBJ);
//            memset(ui->showObjInfo->infoBoxOccupied, 0, sizeof(bool) * INFOBOX_NUM * 2);

//            ui->showObjInfo->ACycle = 0;
//            ui->RadarConnection->setStyleSheet("background-color: rgb(255, 0, 0);");
//        }
    }
}

void *MainWindow::callReadFunc_Radar(void *func)
{
    (static_cast<MainWindow *>(func))->readCANMessage();
    return nullptr;
}
void MainWindow::readCANMessage()
{
    TPCANStatus stsResult;
    stsResult = m_PCANDevice->SetValue(m_Handle, PCAN_RECEIVE_EVENT, &thread_radar, sizeof(thread_radar));

    TPCANMsg msg;
    TPCANTimestamp time;

    while(startRadar)
    {
        stsResult = m_PCANDevice->Read(m_Handle, &msg, &time);

        if(stsResult != PCAN_ERROR_QRCVEMPTY)
            ClassifyingEachCANMessage(msg);
    }
}
void MainWindow::ClassifyingEachCANMessage(TPCANMsg msg)
{
    if((msg.ID & 0xF0F) == 0x60A)
    {
        Set_Obj_A(msg);
    }
    else if((msg.ID & 0xF0F) == 0x60B)
    {
        Set_Obj_B(msg);
    }
    else if((msg.ID & 0xF0F) == 0x60C)
    {
        Set_Obj_C(msg);
    }
    else if((msg.ID & 0xF0F) == 0x60D)
    {
        Set_Obj_D(msg);
    }
}
void MainWindow::Set_Obj_A(TPCANMsg msg)
{
    Object_Status os;
    memcpy(&os, msg.DATA, sizeof(Object_Status));

    //qDebug() << "(60A) Num of Obj : " << os.NofObjects;

    if(static_cast<int>(msg.ID) == (0x60A + (Display_Radar->Radar_ID * 0x10)))
    {
        Display_Radar->ACycle = Display_Radar->A_DateTime.msecsTo(global_time);
        Display_Radar->A_DateTime = global_time;
        Display_Radar->first_A = true;

        N_car = os.NofObjects;
        N_car_now = 0;
        count4Kalman = 0;

        memset(Display_Radar->exist, 0, sizeof(bool) * MAX_OBJ);
        memset(Display_Radar->this_frame_data, 0, sizeof(Obj_inf) * MAX_OBJ);

        if(do_kalman)
        {
            if(N_car == 0)
            {
                for(int i=0; i<MAX_OBJ; i++)
                {
                    if(!Display_Radar->Obj_data[i].isEmpty())
                    {
                        Obj_inf last_data = Display_Radar->Obj_data[i].getLast();
                        QTime RealTime = QTime::currentTime();
                        if(((last_data._DateTime.secsTo(global_time)) ) >= KEEP_SEC)
                        {
                            if(last_data.index_of_infoBox != 0)
                                Display_Radar->infoBoxOccupied[last_data.index_of_infoBox - 1] = false;
                            Display_Radar->Obj_data[i].remove_all();
                            cvReleaseKalman(&kalman_d[i]);
                            cvReleaseKalman(&kalman_v[i]);
                        }
                        else if(Display_Radar->Obj_data[i].getsize() >= 2)
                        {
                            Obj_inf pred_data = last_data;
                            cvKalmanPredict(kalman_d[i], 0);
                            cvKalmanPredict(kalman_v[i], 0);

                            QPointF last_dist(pred_data.DistLat, pred_data.DistLong);
                            QPointF last_vrel(pred_data.VrelLat, pred_data.VrelLong);

                            pred_data.DistLat = kalman_d[i]->state_pre->data.fl[0];
                            pred_data.DistLong = kalman_d[i]->state_pre->data.fl[1];
                            pred_data.VrelLat = kalman_v[i]->state_pre->data.fl[0];
                            pred_data.VrelLong = kalman_v[i]->state_pre->data.fl[1];
                            pred_data.Dist = sqrt((pred_data.DistLong * pred_data.DistLong) + (pred_data.DistLat * pred_data.DistLat));
                            pred_data.Speed = 3.6 * sqrt((pred_data.VrelLong * pred_data.VrelLong) + (pred_data.VrelLat * pred_data.VrelLat));
                            if(pred_data.Speed != 0.0)
                                pred_data.ttc = pred_data.Dist/pred_data.Speed;
                            else
                                pred_data.ttc = 100;

                            int x = ((pred_data.DistLat - Display_Radar->MAX_DISTLAT) * -1)
                                    * (Display_Radar->LookUpImage->width / (Display_Radar->MAX_DISTLAT * 2));
                            int y = Display_Radar->LookUpImage->height
                                    - ((0.5 * (Display_Radar->LookUpImage->height / Display_Radar->LANE_COUNT))
                                       + (pred_data.DistLong / Display_Radar->MAX_DISTLONG)
                                       * (Display_Radar->LookUpImage->height / Display_Radar->LANE_COUNT)
                                       * (Display_Radar->LANE_COUNT - 1));

                            pred_data.virtual_obj = true;

                            if((x >= 0) && (x < Display_Radar->LookUpImage->width) && (y >= 0) && (y < Display_Radar->LookUpImage->height))
                            {
                                if(Display_Radar->haveLanePoint)
                                {
                                    int index = (x + (y * Display_Radar->LookUpImage->widthStep));

                                    pred_data.zone_index = Display_Radar->LookUpImage->imageData[index];

                                    if(pred_data.zone_index != 0)
                                    {
                                        memcpy(&Display_Radar->this_frame_data[i], &pred_data, sizeof(Obj_inf));
                                        Display_Radar->exist[i] = true;
                                    }
                                }

                                CvMat* measurement_d = cvCreateMat(2, 1, CV_32FC1);
                                CvMat* measurement_v = cvCreateMat(2, 1, CV_32FC1);
                                measurement_d->data.fl[0] = 4 * kalman_d[i]->state_pre->data.fl[0] - 3 * last_dist.x();
                                measurement_d->data.fl[1] = 4 * kalman_d[i]->state_pre->data.fl[1] - 3 * last_dist.y();
                                measurement_v->data.fl[0] = 4 * kalman_v[i]->state_pre->data.fl[0] - 3 * last_vrel.x();
                                measurement_v->data.fl[1] = 4 * kalman_v[i]->state_pre->data.fl[1] - 3 * last_vrel.y();

                                memcpy(kalman_d[i]->measurement_matrix->data.fl, H, sizeof(H));
                                memcpy(kalman_v[i]->measurement_matrix->data.fl, H, sizeof(H));
                                cvKalmanCorrect(kalman_d[i], measurement_d);
                                cvKalmanCorrect(kalman_v[i], measurement_v);

                                cvReleaseMat(&measurement_d);
                                cvReleaseMat(&measurement_v);
                            }
                        }
                    }
                }
            }
        }
        else
        {
            for(int i=0; i<MAX_OBJ; i++)
            {
                if(!Display_Radar->Obj_data[i].isEmpty())
                {
                    Obj_inf last_data = Display_Radar->Obj_data[i].getLast();
                    if(((last_data._DateTime.secsTo(global_time)) ) >= KEEP_SEC)
                    {
                        if(last_data.index_of_infoBox != 0)
                            Display_Radar->infoBoxOccupied[last_data.index_of_infoBox - 1] = false;
                        Display_Radar->Obj_data[i].remove_all();
                    }
                }
            }
        }
    }
}
void MainWindow::Set_Obj_B(TPCANMsg msg)
{
    Object_General og;
    memcpy(&og, msg.DATA, sizeof(Object_General));

    //qDebug() << "(60B) Obj ID : " << og.ID;

    Obj_inf now_data = {Display_Radar->A_DateTime, };
    //memset(&now_data, 0, sizeof(Obj_inf));
    //now_data._DateTime = Display_Radar->A_DateTime;

    if(static_cast<int>(msg.ID) == (0x60B + (Display_Radar->Radar_ID * 0x10)))
    {
        if(Display_Radar->first_A)
        {
            int i;
            if(do_kalman)
            {
                for(i=count4Kalman; i<og.ID; i++)
                {
                    if(!Display_Radar->Obj_data[i].isEmpty())
                    {
                        Obj_inf last_data = Display_Radar->Obj_data[i].getLast();
                        if(((last_data._DateTime.secsTo(global_time)) ) >= KEEP_SEC)
                        {
                            if(last_data.index_of_infoBox != 0)
                                Display_Radar->infoBoxOccupied[last_data.index_of_infoBox - 1] = false;
                            Display_Radar->Obj_data[i].remove_all();
                            cvReleaseKalman(&kalman_d[i]);
                            cvReleaseKalman(&kalman_v[i]);
                        }
                        else if(Display_Radar->Obj_data[i].getsize() >= 2)
                        {
                            Obj_inf pred_data = last_data;
                            cvKalmanPredict(kalman_d[i], 0);
                            cvKalmanPredict(kalman_v[i], 0);

                            QPointF last_dist(pred_data.DistLat, pred_data.DistLong);
                            QPointF last_vrel(pred_data.VrelLat, pred_data.VrelLong);

                            pred_data.DistLat = kalman_d[i]->state_pre->data.fl[0];
                            pred_data.DistLong = kalman_d[i]->state_pre->data.fl[1];
                            pred_data.VrelLat = kalman_v[i]->state_pre->data.fl[0];
                            pred_data.VrelLong = kalman_v[i]->state_pre->data.fl[1];
                            pred_data.Dist = sqrt((pred_data.DistLong * pred_data.DistLong) + (pred_data.DistLat * pred_data.DistLat));
                            pred_data.Speed = 3.6 * sqrt((pred_data.VrelLong * pred_data.VrelLong) + (pred_data.VrelLat * pred_data.VrelLat));
                            if(pred_data.Speed != 0.0)
                                pred_data.ttc = pred_data.Dist/pred_data.Speed;
                            else
                                pred_data.ttc = 100;

                            int x = ((pred_data.DistLat - Display_Radar->MAX_DISTLAT) * -1)
                                    * (Display_Radar->LookUpImage->width / (Display_Radar->MAX_DISTLAT * 2));
                            int y = Display_Radar->LookUpImage->height
                                    - ((0.5 * (Display_Radar->LookUpImage->height / Display_Radar->LANE_COUNT))
                                       + (pred_data.DistLong / Display_Radar->MAX_DISTLONG)
                                       * (Display_Radar->LookUpImage->height / Display_Radar->LANE_COUNT)
                                       * (Display_Radar->LANE_COUNT - 1));

                            pred_data.virtual_obj = true;

                            if((x >= 0) && (x < Display_Radar->LookUpImage->width) && (y >= 0) && (y < Display_Radar->LookUpImage->height))
                            {
                                if(Display_Radar->haveLanePoint)
                                {
                                    int index = (x + (y * Display_Radar->LookUpImage->widthStep));

                                    pred_data.zone_index = Display_Radar->LookUpImage->imageData[index];

                                    if(pred_data.zone_index != 0)
                                    {
                                        memcpy(&Display_Radar->this_frame_data[i], &pred_data, sizeof(Obj_inf));
                                        Display_Radar->exist[i] = true;
                                    }
                                }

                                CvMat* measurement_d = cvCreateMat(2, 1, CV_32FC1);
                                CvMat* measurement_v = cvCreateMat(2, 1, CV_32FC1);
                                measurement_d->data.fl[0] = 4 * kalman_d[i]->state_pre->data.fl[0] - 3 * last_dist.x();
                                measurement_d->data.fl[1] = 4 * kalman_d[i]->state_pre->data.fl[1] - 3 * last_dist.y();
                                measurement_v->data.fl[0] = 4 * kalman_v[i]->state_pre->data.fl[0] - 3 * last_vrel.x();
                                measurement_v->data.fl[1] = 4 * kalman_v[i]->state_pre->data.fl[1] - 3 * last_vrel.y();

                                memcpy(kalman_d[i]->measurement_matrix->data.fl, H, sizeof(H));
                                memcpy(kalman_v[i]->measurement_matrix->data.fl, H, sizeof(H));
                                cvKalmanCorrect(kalman_d[i], measurement_d);
                                cvKalmanCorrect(kalman_v[i], measurement_v);

                                cvReleaseMat(&measurement_d);
                                cvReleaseMat(&measurement_v);
                            }
                        }
                    }
                }
            }
            count4Kalman = og.ID + 1;
            N_car_now++;


            now_data.ID = og.ID;
            now_data.DistLong = ((((og.DistLong_u << 5) + og.DistLong_d) * 0.2) -500);
            now_data.DistLat = ((((og.DistLat_u << 8) + og.DistLat_d) * 0.2) - 204.6);
            now_data.VrelLong = ((((og.VrelLong_u << 2) + og.VrelLong_d) * 0.25) - 128);
            now_data.VrelLat = ((((og.VrelLat_u << 3) + og.VrelLat_d) * 0.25) - 64);
            now_data.DynProp = og.DynProp;
            now_data.RCS = ((og.RCS * 0.5) - 64.0);
            now_data.Dist = sqrt((now_data.DistLong * now_data.DistLong) + (now_data.DistLat * now_data.DistLat));
            now_data.Speed = 3.6 * sqrt((now_data.VrelLong * now_data.VrelLong) + (now_data.VrelLat * now_data.VrelLat));
            if(now_data.Speed >= 1)
                now_data.ttc = now_data.Dist/now_data.Speed;
            else
                now_data.ttc = 100;

            if(do_kalman)
            {
                if(Display_Radar->Obj_data[og.ID].getsize() >= 2)
                {
                    //Obj_inf last_data = Display_Radar->Obj_data[og.ID].getLast();
                    //if(now_data.DistLong <= last_data.DistLong)
                    {
                        cvKalmanPredict(kalman_d[og.ID], 0);
                        cvKalmanPredict(kalman_v[og.ID], 0);

                        CvMat* measurement_d = cvCreateMat(2, 1, CV_32FC1);
                        CvMat* measurement_v = cvCreateMat(2, 1, CV_32FC1);
                        measurement_d->data.fl[0] = now_data.DistLat;
                        measurement_d->data.fl[1] = now_data.DistLong;
                        measurement_v->data.fl[0] = now_data.VrelLat;
                        measurement_v->data.fl[1] = now_data.VrelLong;

                        now_data.DistLat    = kalman_d[og.ID]->state_pre->data.fl[0];
                        now_data.DistLong   = kalman_d[og.ID]->state_pre->data.fl[1];
                        now_data.VrelLat    = kalman_v[og.ID]->state_pre->data.fl[0];
                        now_data.VrelLong   = kalman_v[og.ID]->state_pre->data.fl[1];

                        memcpy(kalman_d[og.ID]->measurement_matrix->data.fl, H, sizeof(H));
                        memcpy(kalman_v[og.ID]->measurement_matrix->data.fl, H, sizeof(H));
                        cvKalmanCorrect(kalman_d[og.ID], measurement_d);
                        cvKalmanCorrect(kalman_v[og.ID], measurement_v);

                        cvReleaseMat(&measurement_d);
                        cvReleaseMat(&measurement_v);
                    }
                    //else
                    //    Display_Radar->Obj_data[og.ID].remove_all();
                }
            }

            if((LOWER_SPEED <= now_data.Speed) && (UPPER_SPEED >= now_data.Speed))
            {
                now_data._DateTime = Display_Radar->A_DateTime;

                if(Display_Radar->haveLanePoint)
                {
                    int x = ((now_data.DistLat - Display_Radar->MAX_DISTLAT) * -1)
                            * (Display_Radar->LookUpImage->width / (Display_Radar->MAX_DISTLAT * 2));
                    int y = Display_Radar->LookUpImage->height
                            - ((0.5 * (Display_Radar->LookUpImage->height / Display_Radar->LANE_COUNT))
                               + (now_data.DistLong / Display_Radar->MAX_DISTLONG)
                               * (Display_Radar->LookUpImage->height / Display_Radar->LANE_COUNT)
                               * (Display_Radar->LANE_COUNT - 1));

                    if((x >= 0) && (x < Display_Radar->LookUpImage->width) && (y >= 0) && (y < Display_Radar->LookUpImage->height))
                    {
                        int index = (x + (y * Display_Radar->LookUpImage->widthStep));
                        now_data.zone_index = Display_Radar->LookUpImage->imageData[index];

                        if(now_data.zone_index != 0)
                        {
                            if(Display_Radar->Obj_data[og.ID].isEmpty())
                            {
                                if(do_kalman)
                                {
                                    kalman_d[og.ID] = cvCreateKalman(4, 2, 0);
                                    memcpy(kalman_d[og.ID]->transition_matrix->data.fl, A, sizeof(A));
                                    memcpy(kalman_d[og.ID]->measurement_matrix->data.fl, H, sizeof(H));
                                    cvSetIdentity(kalman_d[og.ID]->process_noise_cov, cvRealScalar(1e-6));
                                    cvSetIdentity(kalman_d[og.ID]->measurement_noise_cov, cvRealScalar(0.000006));
                                    cvSetIdentity(kalman_d[og.ID]->error_cov_post, cvRealScalar(1e+10));

                                    kalman_v[og.ID] = cvCreateKalman(4, 2, 0);
                                    memcpy(kalman_v[og.ID]->transition_matrix->data.fl, A, sizeof(A));
                                    memcpy(kalman_v[og.ID]->measurement_matrix->data.fl, H, sizeof(H));
                                    cvSetIdentity(kalman_v[og.ID]->process_noise_cov, cvRealScalar(1e-6));
                                    cvSetIdentity(kalman_v[og.ID]->measurement_noise_cov, cvRealScalar(0.000006));
                                    cvSetIdentity(kalman_v[og.ID]->error_cov_post, cvRealScalar(1e+10));
                                }

                                for(i=0; i<INFOBOX_NUM * 2; i++)
                                {
                                    if(!Display_Radar->infoBoxOccupied[i])
                                    {
                                        now_data.index_of_infoBox = i + 1;
                                        Display_Radar->infoBoxOccupied[i] = true;
                                        break;
                                    }
                                }
                            }
                            else
                            {
                                Obj_inf last_data = Display_Radar->Obj_data[og.ID].getLast();

                                now_data.index_of_infoBox = Display_Radar->Obj_data[og.ID].getLast().index_of_infoBox;
                            }

                            memcpy(&Display_Radar->this_frame_data[og.ID], &now_data, sizeof(Obj_inf));
                            Display_Radar->exist[og.ID] = true;
                        }
                    }
                }
            }
            if(do_kalman)
            {
                if(N_car == N_car_now)
                {
                    for(i=og.ID + 1; i<MAX_OBJ; i++)
                    {
                        if(!Display_Radar->Obj_data[i].isEmpty())
                        {
                            Obj_inf last_data = Display_Radar->Obj_data[i].getLast();
                            if(((last_data._DateTime.secsTo(global_time)) ) >= KEEP_SEC)
                            {
                                if(last_data.index_of_infoBox != 0)
                                    Display_Radar->infoBoxOccupied[last_data.index_of_infoBox - 1] = false;
                                Display_Radar->Obj_data[i].remove_all();
                                cvReleaseKalman(&kalman_d[i]);
                                cvReleaseKalman(&kalman_v[i]);
                            }
                            else if(Display_Radar->Obj_data[i].getsize() >= 2)
                            {
                                Obj_inf pred_data = last_data;
                                cvKalmanPredict(kalman_d[i], 0);
                                cvKalmanPredict(kalman_v[i], 0);

                                QPointF last_dist(pred_data.DistLat, pred_data.DistLong);
                                QPointF last_vrel(pred_data.VrelLat, pred_data.VrelLong);

                                pred_data.DistLat = kalman_d[i]->state_pre->data.fl[0];
                                pred_data.DistLong = kalman_d[i]->state_pre->data.fl[1];
                                pred_data.VrelLat = kalman_v[i]->state_pre->data.fl[0];
                                pred_data.VrelLong = kalman_v[i]->state_pre->data.fl[1];
                                pred_data.Dist = sqrt((pred_data.DistLong * pred_data.DistLong) + (pred_data.DistLat * pred_data.DistLat));
                                pred_data.Speed = 3.6 * sqrt((pred_data.VrelLong * pred_data.VrelLong) + (pred_data.VrelLat * pred_data.VrelLat));
                                if(pred_data.Speed != 0.0)
                                    pred_data.ttc = pred_data.Dist/pred_data.Speed;
                                else
                                    pred_data.ttc = 100;

                                int x = ((pred_data.DistLat - Display_Radar->MAX_DISTLAT) * -1)
                                        * (Display_Radar->LookUpImage->width / (Display_Radar->MAX_DISTLAT * 2));
                                int y = Display_Radar->LookUpImage->height
                                        - ((0.5 * (Display_Radar->LookUpImage->height / Display_Radar->LANE_COUNT))
                                           + (pred_data.DistLong / Display_Radar->MAX_DISTLONG)
                                           * (Display_Radar->LookUpImage->height / Display_Radar->LANE_COUNT)
                                           * (Display_Radar->LANE_COUNT - 1));

                                pred_data.virtual_obj = true;

                                if((x >= 0) && (x < Display_Radar->LookUpImage->width) && (y >= 0) && (y < Display_Radar->LookUpImage->height))
                                {
                                    if(Display_Radar->haveLanePoint)
                                    {
                                        int index = (x + (y * Display_Radar->LookUpImage->widthStep));

                                        pred_data.zone_index = Display_Radar->LookUpImage->imageData[index];

                                        if(pred_data.zone_index != 0)
                                        {
                                            memcpy(&Display_Radar->this_frame_data[i], &pred_data, sizeof(Obj_inf));
                                            Display_Radar->exist[i] = true;
                                        }
                                    }

                                    CvMat* measurement_d = cvCreateMat(2, 1, CV_32FC1);
                                    CvMat* measurement_v = cvCreateMat(2, 1, CV_32FC1);
                                    measurement_d->data.fl[0] = 4 * kalman_d[i]->state_pre->data.fl[0] - 3 * last_dist.x();
                                    measurement_d->data.fl[1] = 4 * kalman_d[i]->state_pre->data.fl[1] - 3 * last_dist.y();
                                    measurement_v->data.fl[0] = 4 * kalman_v[i]->state_pre->data.fl[0] - 3 * last_vrel.x();
                                    measurement_v->data.fl[1] = 4 * kalman_v[i]->state_pre->data.fl[1] - 3 * last_vrel.y();

                                    memcpy(kalman_d[i]->measurement_matrix->data.fl, H, sizeof(H));
                                    memcpy(kalman_v[i]->measurement_matrix->data.fl, H, sizeof(H));
                                    cvKalmanCorrect(kalman_d[i], measurement_d);
                                    cvKalmanCorrect(kalman_v[i], measurement_v);

                                    cvReleaseMat(&measurement_d);
                                    cvReleaseMat(&measurement_v);
                                }
                            }
                        }
                    }
                }
                N_car_now = 0;
                count4Kalman = 0;
            }
        }
    }
}
void MainWindow::Set_Obj_C(TPCANMsg msg)
{
    Object_Quality oq;
    memcpy(&oq, msg.DATA, sizeof(Object_Quality));

    //qDebug() << "(60C) Obj ID : " << oq.ID;

    if((int)msg.ID == (0x60C + (Display_Radar->Radar_ID * 0x10)))
    {
        if(Display_Radar->exist[oq.ID])
        {
            if(Display_Radar->first_A)
            {
                if(!Display_Radar->this_frame_data[oq.ID].C)
                {
                    Display_Radar->this_frame_data[oq.ID].DistLong_rms = oq.DistLong_rms;
                    Display_Radar->this_frame_data[oq.ID].DistLat_rms = ((oq.DistLat_rms_u << 2) + oq.DistLat_rms_d);
                    Display_Radar->this_frame_data[oq.ID].VrelLong_rms = oq.VrelLong_rms;
                    Display_Radar->this_frame_data[oq.ID].VrelLat_rms = ((oq.VrelLat_rms_u << 4) + oq.VrelLat_rms_d);
                    Display_Radar->this_frame_data[oq.ID].ArelLong_rms = ((oq.ArelLong_rms_u << 1) + oq.ArelLong_rms_d);
                    Display_Radar->this_frame_data[oq.ID].ArelLat_rms = oq.ArelLat_rms;
                    Display_Radar->this_frame_data[oq.ID].Orientation_rms = ((oq.Orientation_rms_u << 3) + oq.Orientation_rms_d);
                    Display_Radar->this_frame_data[oq.ID].MirrorProb = oq.MirrorProb;
                    Display_Radar->this_frame_data[oq.ID].MeasState = oq.MeasState;
                    Display_Radar->this_frame_data[oq.ID].ProbOfExist = oq.ProbOfExist;
                    Display_Radar->this_frame_data[oq.ID].C = true;
                }
            }
        }
    }
}
void MainWindow::Set_Obj_D(TPCANMsg msg)
{
    Object_Extended oe;
    memcpy(&oe, msg.DATA, sizeof(Object_Extended));

    //qDebug() << "(60D) Obj ID : " << oe.ID;

    if(int(msg.ID) == (0x60D + (Display_Radar->Radar_ID * 0x10)))
    {
        if(log_work)
        {
            if(startRadar)
            {
                bool header = false;
                if(int(last_save_obj.time().minute() / 10) != (global_time.time().minute() / 10))
                {
                    last_save_obj = global_time;
                    log_obj_PATH = log_DIR + "/OBJ/" + last_save_obj.toString("yyyy-MM-dd-HH-mm-ss-zzz") + ".csv";
                    header = true;
                }
                QFile file(log_obj_PATH);
                file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text);
                QTextStream out(&file);

                if(header)
                    out << "Time, ID, DistLat, DistLong, DIST, VrelLat, VrelLong, VREL, DynProp, RCS, ProbofExist, MirrorProp, ArelLat, ArelLong, Orientation, Class, Length, Width/n";

                out << Display_Radar->this_frame_data[oe.ID]._DateTime.toString("yyyy/MM/dd hh:mm:ss.zzz") << ",";
                out << Display_Radar->this_frame_data[oe.ID].ID << ",";
                out << Display_Radar->this_frame_data[oe.ID].DistLat << ",";
                out << Display_Radar->this_frame_data[oe.ID].DistLong << ",";
                out << Display_Radar->this_frame_data[oe.ID].Dist << ",";
                out << Display_Radar->this_frame_data[oe.ID].VrelLat << ",";
                out << Display_Radar->this_frame_data[oe.ID].VrelLong << ",";
                out << Display_Radar->this_frame_data[oe.ID].Speed << ",";
                out << Display_Radar->this_frame_data[oe.ID].DynProp << ",";
                out << Display_Radar->this_frame_data[oe.ID].RCS << ",";
                out << Display_Radar->this_frame_data[oe.ID].ProbOfExist << ",";
                out << Display_Radar->this_frame_data[oe.ID].MirrorProb << ",";
                out << Display_Radar->this_frame_data[oe.ID].ArelLat << ",";
                out << Display_Radar->this_frame_data[oe.ID].ArelLong << ",";
                out << Display_Radar->this_frame_data[oe.ID].OrientationAngel << ",";
                out << Display_Radar->this_frame_data[oe.ID].Class << ",";
                out << Display_Radar->this_frame_data[oe.ID].Length << ",";
                out << Display_Radar->this_frame_data[oe.ID].Width << "/n";

                file.close();
            }
        }

        if(do_kalman)
        {
            for(int i=count4Kalman; i<oe.ID; i++)
            {
                if(Display_Radar->exist[i])
                {
                    if(log_work)
                    {
                    }
                    Display_Radar->Obj_data[i].addLast(Display_Radar->this_frame_data[i]);
                    Display_Radar->exist[i] = false;
                }
            }
        }
        count4Kalman = oe.ID + 1;
        N_car_now++;

        if(Display_Radar->exist[oe.ID])
        {
            if(Display_Radar->first_A)
            {
                if(!Display_Radar->this_frame_data[oe.ID].D)
                {
                    Display_Radar->this_frame_data[oe.ID].ArelLong = ((((oe.ArelLong_u << 3) + oe.ArelLong_d) * 0.01) - 10);
                    Display_Radar->this_frame_data[oe.ID].ArelLat = ((((oe.ArelLat_u << 3) + oe.ArelLat_d) * 0.01) - 2.5);
                    Display_Radar->this_frame_data[oe.ID].Class = oe.Class;
                    Display_Radar->this_frame_data[oe.ID].OrientationAngel = ((((oe.OrientationAngel_u << 2) + oe.OrientationAngel_d) * 0.4) - 180);
                    Display_Radar->this_frame_data[oe.ID].Length = (oe.Length * 0.2);
                    Display_Radar->this_frame_data[oe.ID].Width = (oe.Width * 0.2);
                    Display_Radar->this_frame_data[oe.ID].D = true;

                    if(log_work)
                    {
                    }
                    Display_Radar->Obj_data[oe.ID].addLast(Display_Radar->this_frame_data[oe.ID]);
                    Display_Radar->exist[oe.ID] = false;
                }
            }
        }
        if(do_kalman)
        {
            if(N_car == N_car_now)
            {
                for(int i=oe.ID + 1; i<MAX_OBJ; i++)
                {
                    if(Display_Radar->exist[i])
                    {
                        if(log_work)
                        {
                        }
                        Display_Radar->Obj_data[i].addLast(Display_Radar->this_frame_data[i]);
                        Display_Radar->exist[i] = false;
                    }
                }
            }
        }
    }
}


bool MainWindow::InitializeValue_Camera()
{
    startCamera = false;

    return true;
}
void MainWindow::on_Connectbtn_Camera_clicked()
{
    Connect_Camera();
}
void MainWindow::on_Disconnectbtn_Camera_clicked()
{
    Disconnect_Camera();
}


bool MainWindow::Connect_Camera()
{
    if(!startCamera)
    {
        Connectbtn_Camera->setEnabled(false);

        ////////////

        Connectbtn_Camera->setEnabled(false);
        Disconnectbtn_Camera->setEnabled(true);
        cameraStatus->setStyleSheet("background-color: rgb(0, 255, 0); color: rgb(0, 0, 0); border-width: 1px; border-style: solid; border-color: rgb(0, 0, 0);");
        startCamera = true;
    }

    return true;
}

bool MainWindow::Disconnect_Camera()
{
    if(startCamera)
    {
        ////////////

        Connectbtn_Camera->setEnabled(true);
        Disconnectbtn_Camera->setEnabled(false);
        cameraStatus->setStyleSheet("background-color: rgb(255, 0, 0); color: rgb(0, 0, 0); border-width: 1px; border-style: solid; border-color: rgb(0, 0, 0);");
        startCamera = false;
    }

    return true;
}
void MainWindow::on_Connectbtn_Serial_clicked()
{

}

void MainWindow::on_Disconnectbtn_Serial_clicked()
{

}

void MainWindow::on_Settingbtn_clicked()
{
    if(page->currentIndex() == 0)
    {
        page->setCurrentIndex(1);
        Settingbtn->setText("Main");
    }
    else
    {
        page->setCurrentIndex(0);
        Settingbtn->setText("Setting");
    }/*
    qDebug() << write_MinMaxActive_layout->geometry();
    qDebug() << write_NofObj_layout->geometry();
    qDebug() << write_Distance_layout->geometry();
    qDebug() << write_Azimuth_layout->geometry();
    qDebug() << write_Azimuth_layout->geometry();*/
}

void MainWindow::on_SaveSetting_btn_clicked()
{
    writeSettings();
}
