/*
 * File:   MotorDrive.h
 * Author: 110202117
 *
 * Created on 2020/08/21, 17:03
 */

#ifndef MOTORDRIVE_H
#define	MOTORDRIVE_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "misc.h"
#include "data_types.h"



//.定数定義
//..一般
#define     MOTOR_NUM               2

#define		MOTOR_M0				0
#define		MOTOR_M1				1

#define     BIT_MOTOR_M0        b0000_0001
#define     BIT_MOTOR_M1        b0000_0010

#define     FOWARD              	1
#define     REVERSE             	0
//------------------------------------------------------------------------------
//..位置検出駆動タイプ
enum DRIVE_TYPE {
	DRIVE_SIMPLE = 0,				// 単純移動
	DRIVE_SEARCH_BASE_POSITION,		// 基準位置サーチ動作中
	DRIVE_CHECK_POSITION,			// 位置確認機能付きの移動
	DRIVE_CORRECT_BASE_POSITION,	// 位置ずれ検出後の位置探索移動
	DRIVE_ACT_CORRECT_POSITION,		// 位置ずれ補正移動
	DRIVE_SEARCH_1008_POSITION
};
//------------------------------------------------------------------------------
//..周波数設定用
enum FREQ_SETTING_TYPE {
	FREQ_SET_SELF_CONTROL_NORMAL,
	FREQ_SET_SELF_CONTROL_SHAKE,
	FREQ_SET_EXTERNAL_CONTROL
};

#define		SET_FREQ_200HZ			41
#define		SET_FREQ_195HZ			42
#define		SET_FREQ_191HZ			43
#define		SET_FREQ_186HZ			44
#define		SET_FREQ_182HZ			45
#define		SET_FREQ_178HZ			46
#define		SET_FREQ_174HZ			47
#define		SET_FREQ_171HZ			48
#define		SET_FREQ_167HZ			49
#define		SET_FREQ_164HZ			50
#define		SET_FREQ_161HZ			51
#define		SET_FREQ_158HZ			52
#define		SET_FREQ_155HZ			53
#define		SET_FREQ_152HZ			54
#define		SET_FREQ_149HZ			55
#define		SET_FREQ_146HZ			56
#define		SET_FREQ_144HZ			57
#define		SET_FREQ_141HZ			58
#define		SET_FREQ_139HZ			59
#define		SET_FREQ_137HZ			60
#define		SET_FREQ_134HZ			61
#define		SET_FREQ_132HZ			62
#define		SET_FREQ_130HZ			63
#define		SET_FREQ_128HZ			64
#define		SET_FREQ_126HZ			65
#define		SET_FREQ_124HZ			66
#define		SET_FREQ_122HZ			67
#define		SET_FREQ_121HZ			68
#define		SET_FREQ_119HZ			69
#define		SET_FREQ_117HZ			70
#define		SET_FREQ_115HZ			71
#define		SET_FREQ_114HZ			72
#define		SET_FREQ_112HZ			73
#define		SET_FREQ_111HZ			74
#define		SET_FREQ_109HZ			75
#define		SET_FREQ_108HZ			76
#define		SET_FREQ_106HZ			77
#define		SET_FREQ_105HZ			78
#define		SET_FREQ_104HZ			79
#define		SET_FREQ_102HZ			80
#define		SET_FREQ_101HZ			81
#define		SET_FREQ_100HZ			82
#define		SET_FREQ_99HZ			83
#define		SET_FREQ_98HZ			84
#define		SET_FREQ_96HZ			85
#define		SET_FREQ_95HZ			86
#define		SET_FREQ_94HZ			87
#define		SET_FREQ_93HZ			88
#define		SET_FREQ_92HZ			89
#define		SET_FREQ_91HZ			90
#define		SET_FREQ_90HZ			91
#define		SET_FREQ_89HZ			92
#define		SET_FREQ_88HZ			93
#define		SET_FREQ_87HZ			94
#define		SET_FREQ_86HZ			95
#define		SET_FREQ_85HZ			96
#define		SET_FREQ_84HZ			98
#define		SET_FREQ_83HZ			99
#define		SET_FREQ_82HZ			100
#define		SET_FREQ_81HZ			101
#define		SET_FREQ_80HZ			102
#define		SET_FREQ_79HZ			104
#define		SET_FREQ_78HZ			105
#define		SET_FREQ_77HZ			106
#define		SET_FREQ_76HZ			108
#define		SET_FREQ_75HZ			109
#define		SET_FREQ_74HZ			111
#define		SET_FREQ_73HZ			112
#define		SET_FREQ_72HZ			114
#define		SET_FREQ_71HZ			115
#define		SET_FREQ_70HZ			117
#define		SET_FREQ_69HZ			118
#define		SET_FREQ_68HZ			120
#define		SET_FREQ_67HZ			122
#define		SET_FREQ_66HZ			124
#define		SET_FREQ_65HZ			126
#define		SET_FREQ_64HZ			128
#define		SET_FREQ_63HZ			130
#define		SET_FREQ_62HZ			132
#define		SET_FREQ_61HZ			134
#define		SET_FREQ_60HZ			136
#define		SET_FREQ_59HZ			138
#define		SET_FREQ_58HZ			141
#define		SET_FREQ_57HZ			143
#define		SET_FREQ_56HZ			146
#define		SET_FREQ_55HZ			148
#define		SET_FREQ_54HZ			151
#define		SET_FREQ_53HZ			154
#define		SET_FREQ_52HZ			157
#define		SET_FREQ_51HZ			160
#define		SET_FREQ_50HZ			163
#define		SET_FREQ_49HZ			166
#define		SET_FREQ_48HZ			170
#define		SET_FREQ_47HZ			173
#define		SET_FREQ_46HZ			177
#define		SET_FREQ_45HZ			181
#define		SET_FREQ_44HZ			185
#define		SET_FREQ_43HZ			189
#define		SET_FREQ_42HZ			193
#define		SET_FREQ_41HZ			198
#define		SET_FREQ_40HZ			203



//------------------------------------------------------------------------------
//.extern変数
extern UB motorBitOperation;
extern UB motorNoOperation;
extern UB reqCorrectBasePosBit;							// 針位置補正要求
extern UB reqCorrectFinePoBit;
extern UB actCorrectDriveBit;
extern UB handPositionActiveBit;						// 0:針位置無効→位置検出が未了、1:針位置有効→位置検出が終了
//------------------------------------------------------------------------------
//.extern関数
extern void  SetMotorBitMotorNo( UB motor_no );

extern void SetSearchBasePosition( UB motor_no, UB sw );
extern void SetCheckLoadPosition( UB motor_no, UB sw );

extern void SetAfterMotorStopFunc( UB motor_no, void ( * func )( UB motor_no ) );
extern void SetAfterDetectLoadFunc( UB motor_no, void ( * func )( UB motor_no ) );

extern void SetMotorDirection(UB motor_no, UB dir);
extern void SetMotorStepCounter(UB motor_no, H steps);
extern void SetMotorAddShakePulse(UB motor_no, UB sw);
extern void SetMotorFrequency(UB motor_no, UB type, UB set_freq );
extern void SetMotorStart(UB motor_no);
extern void SetMotorPositionSearchStart( UB motor_no );
extern void CalcMotorFwdStepCounter( UB motor_no );
extern void VerifyHandPosition_LoadPosition( UB motor_no );
extern UB 	GetIsMotorActive( UB motor_no );
extern void WatchMotorDriveFinish( void );

extern void CalcMotorStepCounter( UB motor_no );

extern void UpdateTargetPosition( UB motor_no, H steps );
extern void SetMotorStop( UB motor_no );

extern void StartPulseOutM0( void );
extern void StartPulseOutM1( void );

extern void TMR2_Interrupt( void );
extern void InitPulsePositionTable( UB motor_no );

extern void SetDriveType( UB motor_no, UB drive_type );

extern void DeactivateHandPosition( UB motor_no );

extern void SetMotorStartSPK( void );
extern void SetMotorStopSPK( void );
extern void SetMotorEnableExcitating( UB motor_no );
extern void SetMotorDisableExcitating( UB motor_no );
//------------------------------------------------------------------------------
#ifdef	__cplusplus
}
#endif

#endif	/* MOTORDRIVE_H */

