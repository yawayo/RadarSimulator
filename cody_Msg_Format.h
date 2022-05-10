#pragma once
#include <QDateTime>

struct Obj_inf {

    QDateTime _DateTime;

    unsigned int ID;

    float DistLong;
    float DistLat;
    float Dist;
    float VrelLong;
    float VrelLat;
    float Speed;
    unsigned int DynProp;
    float RCS;

    unsigned int DistLat_rms;
    unsigned int DistLong_rms;
    unsigned int VrelLat_rms;
    unsigned int VrelLong_rms;
    unsigned int ArelLong_rms;
    unsigned int Orientation_rms;
    unsigned int ArelLat_rms;
    unsigned int MeasState;
    unsigned int ProbOfExist;

    float ArelLong;
    float ArelLat;
    unsigned int Class;
    float OrientationAngel;
    unsigned int MirrorProb;
    float Length;
    float Width;

    float ttc;
    int zone_index;

    bool C;
    bool D;

    bool virtual_obj;

    int index_of_infoBox;
};

// 200
struct Radar_Config {
	unsigned int MaxDistance_valid : 1;
	unsigned int SensorID_valid : 1;
	unsigned int RadarPower_valid : 1;
	unsigned int OutputType_valid : 1;
	unsigned int SendQuality_valid : 1;
	unsigned int SendExtInfo_valid : 1;
	unsigned int SortIndex_valid : 1;
	unsigned int StoreInNVM_valid : 1;
	unsigned int MaxDistance_u : 8;
	unsigned int : 6;
	unsigned int MaxDistance_d : 2;
	unsigned int : 8;
	unsigned int SensorID : 3;
	unsigned int OutputType : 2;
	unsigned int RadarPower : 3;
	unsigned int CtrlRelay_valid : 1;
	unsigned int CtrlRelay : 1;
	unsigned int SendQuality : 1;
	unsigned int SendExtInfo : 1;
	unsigned int SortIndex : 3;
	unsigned int StoreInNVM : 1;
    unsigned int RCSThreshold_valid : 1;
    unsigned int RCSThreshold : 3;
	unsigned int InvalidClusters_valid : 1;
	unsigned int : 3;
	unsigned int InvalidClusters : 8;
};
// 201
struct Radar_state {

    unsigned int : 6;
    unsigned int NVMReadStatus : 1;
    unsigned int NVMwriteStatus : 1;
    unsigned int MaxDistanceCfg_u : 8;
    unsigned int : 1;
    unsigned int VoltageError : 1;
    unsigned int TemporaryError : 1;
    unsigned int TemperatureError : 1;
    unsigned int Interference : 1;
    unsigned int PersistentError : 1;
    unsigned int MaxDistanceCfg_d : 2;
    unsigned int RadarPowerCfg_u : 2;
    unsigned int : 6;
    unsigned int SensorID : 3;
    unsigned int : 1;
    unsigned int SortIndex : 3;
    unsigned int RadarPowerCfg_d : 1;
    unsigned int : 1;
    unsigned int CtrlRelayCfg : 1;
    unsigned int OutputTypeCfg : 2;
    unsigned int SendQualityCfg : 1;
    unsigned int SendExtInfoCfg : 1;
    unsigned int MotionRxState : 2;
    unsigned int InvalidClusters : 8;
    unsigned int : 2;
    unsigned int RCSThreshold : 3;
    unsigned int : 3;
};
// 202
struct Filter_Config {
    unsigned int : 1;
    unsigned int Valid : 1;
    unsigned int Active : 1;
    unsigned int Index : 4;
    unsigned int Type : 1;
    unsigned int Min_u : 5;
    unsigned int : 3;
    unsigned int Min_d : 8;
    unsigned int Max_u : 5;
    unsigned int : 3;
    unsigned int Max_d : 8;
};
// 203
struct FilterState_Header {

    unsigned int : 3;
    unsigned int NofClusterFilterCfg : 5;
    unsigned int : 3;
    unsigned int NofObjectFilterCfg : 5;
};
// 204
struct FilterState_Config {
    unsigned int : 2;
    unsigned int Active : 1;
    unsigned int Index : 4;
    unsigned int Type : 1;
    unsigned int Min_u : 5;
    unsigned int : 3;
    unsigned int Min_d : 8;
    unsigned int Max_u : 5;
    unsigned int : 3;
    unsigned int Max_d : 8;
};
// 600
struct Cluster_Status {
    unsigned int NofClustersNear : 8;
    unsigned int NofClustersFar : 8;
    unsigned int MeasCounter_u : 8;
    unsigned int MeasCounter_d : 8;
    unsigned int : 4;
    unsigned int InterfaceVersion : 4;
};
// 60A
struct Object_Status {

    unsigned int NofObjects : 8;
    unsigned int MeasCounter_u : 8;
    unsigned int MeasCounter_d : 8;
    unsigned int : 4;
    unsigned int InterfaceVersion : 4;
};
// 60B
struct Object_General {

	unsigned int ID : 8;
	unsigned int DistLong_u : 8;
	unsigned int DistLat_u : 3;
	unsigned int DistLong_d : 5;
	unsigned int DistLat_d : 8;
	unsigned int VrelLong_u : 8;
	unsigned int VrelLat_u : 6;
	unsigned int VrelLong_d : 2;
	unsigned int DynProp : 3;
	unsigned int : 2;
	unsigned int VrelLat_d : 3;
	unsigned int RCS : 8;
};
// 60C
struct Object_Quality {

	unsigned int ID : 8;
	unsigned int DistLat_rms_u : 3;
	unsigned int DistLong_rms : 5;
	unsigned int VrelLat_rms_u : 1;
	unsigned int VrelLong_rms : 5;
	unsigned int DistLat_rms_d : 2;
	unsigned int ArelLong_rms_u : 4;
	unsigned int VrelLat_rms_d : 4;
	unsigned int Orientation_rms_u : 2;
	unsigned int ArelLat_rms : 5;
	unsigned int ArelLong_rms_d : 1;
    unsigned int MirrorProb : 3;
    unsigned int : 2;
	unsigned int Orientation_rms_d : 3;
	unsigned int : 2;
	unsigned int MeasState : 3;
	unsigned int ProbOfExist : 3;
};
//60D
struct Object_Extended {

	unsigned int ID : 8;
	unsigned int ArelLong_u : 8;
	unsigned int ArelLat_u : 5;
	unsigned int ArelLong_d : 3;
	unsigned int Class : 3;
	unsigned int : 1;
	unsigned int ArelLat_d : 4;
	unsigned int OrientationAngel_u : 8;
	unsigned int : 6;
	unsigned int OrientationAngel_d : 2;
	unsigned int Length : 8;
	unsigned int Width : 8;
};
// 700
struct SW_version_ID {

	unsigned int MajorRelease : 8;
	unsigned int MinorRelease : 8;
	unsigned int PatchLevel : 8;
	unsigned int CountryCode : 1;
	unsigned int ExtendRange : 1;
	unsigned int : 6;
};
