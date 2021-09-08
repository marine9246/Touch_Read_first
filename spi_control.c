//.spi_control.c
/***************************************************************************//**
 @brief     SPI制御処理
 @note      参考）spi.cの内容

            void SPI_Initialize(void);
                SPIモジュール初期化
            -------------------------------------------------------
            bool SPI_Open(spi_modes_t spiUniqueConfiguration);
                SPI機能 enable
            -------------------------------------------------------
            void SPI_Close(void);
                SPI機能 Disable
            -------------------------------------------------------
            void SPI_ExchangeBlock(void *block, size_t blockSize);
                1Byteデータ交換
            -------------------------------------------------------
            void SPI_WriteBlock(void *block, size_t blockSize);
                所定バイト数データ交換
            -------------------------------------------------------
            void SPI_ReadBlock(void *block, size_t blockSize);
                所定バイト数読み出し
            -------------------------------------------------------
            void SPI_WriteByte(uint8_t byte);
                所定バイト数書き込み
            -------------------------------------------------------

            接続
                                モータドライバ(slave)(PIC)      コントローラ(master)(Q923)
            DREADY			    RC0                     →      P53
            SPI_CLOCK           RB6                     ←      P56
            SPI_MOSI            RB4(SIN)                ←      P57
            SPI_MISO            RC7(SOUT)               →      P55
            SS                  PA3                     ←      P47

 @author
 @date
*******************************************************************************/
//.include
#include "mcc_generated_files/mcc.h"
#include "MotorDrive.h"
#include "misc.h"
#include "data_types.h"
#include "spi_control.h"
//------------------------------------------------------------------------------
//.debugOption
#define     DEBUG_SPI_CTRL

//------------------------------------------------------------------------------
//..プロトタイプ宣言
void SPIControl_ExecuteCommand( void );
void SpiCmdFunc_Test( UB motor_no );
//void SpiCmdFunc_FwdPulseNormal( UB motor_no );
void SpiCmdFunc_RevPulseNormal( UB motor_no );
void SpiCmdFunc_FwdPulseWithShake( UB motor_no );
void SpiCmdFunc_SearchLoadPosition( UB motor_no );
void SpiCmdFunc_SearchBasePosition( UB motor_no );
void SpiCmdFunc_FwdPulseWithCorrect( UB motor_no );
void SetFwdPulseWithCorrectProc( UB motor_no );
void SpiCmdFunc_FwdPulsePositionCtl( UB motor_no );
void SpiCmdFunc_RevPulsePositionCtl( UB motor_no );

/**-- 触読対応 --**/
void SpiCmdFunc_StartDetectVrs( UB motor_no );
void SpiCmdFunc_StopDetectVrs( UB motor_no );
void SpiCmdFunc_EnableExcitating( UB motor_no );
void SpiCmdFunc_DisableExcitating( UB motor_no );
void SpiCmdFunc_Search1008Position( UB motor_no );
//------------------------------------------------------------------------------
//.定数
/*******************************************************************************
 @brief     コマンド定数
 @note
 @author
 @date
 ********************************************************************************/
enum ID_CMD_TYPE {
    ID_CMD_FWD_NULL = 0,
    ID_CMD_FWD_NORMAL = 1,                          // 通常の正転1発駆動
    ID_CMD_REV_NORMAL = 2,                          // 通常の逆転1発駆動
    ID_CMD_FWD_WITH_SHAKE = 3,                      // 揺動パルス付きの正転、結果は「　　」に入る
    ID_CMD_SEARCH_LOAD_POSITION = 4,                // 微調整位置を正転で探す
    ID_CMD_SEARCH_BASE_POSITION = 5,                // 基準位置を正転で探す（粗調整後の微調整検出位置）
    ID_CMD_FWD_WITH_CORRECT = 6,                    // 基準位置ズレ補正付きの運針、位置ズレ検出したら、自動補正
	ID_CMD_FWD_POSITION_CTL = 7,					// 位置管理機能付きの正転
	ID_CMD_REV_POSITION_CTL = 8,					// 位置管理機能付きの逆転
	ID_CMD_START_DETECT_VRS = 9,					// VRS検出開始
	ID_CMD_STOP_DETECT_VRS = 10,					// VRS検出中止
	ID_CMD_ENABLE_EXCITATING = 11,					// VRS検出したら、励磁パルス出力する
	ID_CMD_DISABLE_EXCITATING = 12,					// VRS検出しても、励磁パルス出力しない
	ID_CMD_SEARCH_1008_POSITION = 13,				// 基準位置検出して
    ID_CMD_TYPE_MAX
};
enum ID_CMD_MOTOR {
    ID_CMD_M0 = 0xB0,
    ID_CMD_M1 = 0xC0
};

#define     CMD_M0_FWD_NORMAL               ( ID_CMD_M0 + ID_CMD_FWD_NORMAL )                   	// 通常の正転1発駆動
#define     CMD_M0_REV_NORMAL               ( ID_CMD_M0 + ID_CMD_REV_NORMAL )                   	// 通常の逆転1発駆動
#define     CMD_M0_FWD_WITH_SHAKE           ( ID_CMD_M0 + ID_CMD_FWD_WITH_SHAKE )               	// 揺動パルス付きの正転、結果は「　　」に入る
#define     CMD_M0_SEARCH_LOAD_POSITION     ( ID_CMD_M0 + ID_CMD_SEARCH_LOAD_POSITION )         	// 微調整位置を正転で探す
#define     CMD_M0_SEARCH_BASE_POSITION     ( ID_CMD_M0 + ID_CMD_SEARCH_BASE_POSITION )         	// 基準位置を正転で探す（粗調整後の微調整検出位置）
#define     CMD_M0_FWD_WITH_CORRECT         ( ID_CMD_M0 + ID_CMD_FWD_WITH_CORRECT )             	// 基準位置ズレ補正付きの運針、位置ズレ検出したら、自動補正
#define     CMD_M0_FWD_POSITION_CTL         ( ID_CMD_M0 + ID_CMD_FWD_POSITION_CTL )             	//
#define     CMD_M0_REV_POSITION_CTL         ( ID_CMD_M0 + ID_CMD_REV_POSITION_CTL )             	//
#define     CMD_M0_START_DETECT_VRS			( ID_CMD_M0 + ID_CMD_START_DETECT_VRS )					//
#define     CMD_M0_STOP_DETECT_VRS			( ID_CMD_M0 + ID_CMD_STOP_DETECT_VRS  )					//
#define     CMD_M0_ENABLE_EXCITATING 		( ID_CMD_M0 + ID_CMD_ENABLE_EXCITATING )				//
#define     CMD_M0_DISABLE_EXCITATING 		( ID_CMD_M0 + ID_CMD_DISABLE_EXCITATING )				//
#define     CMD_M0_SEARCH_1008_POSITION		( ID_CMD_M0 + ID_CMD_SEARCH_1008_POSITION )				//

#define     CMD_M1_FWD_NORMAL               ( ID_CMD_M1 + ID_CMD_FWD_NORMAL )                   	// 通常の正転1発駆動
#define     CMD_M1_REV_NORMAL               ( ID_CMD_M1 + ID_CMD_REV_NORMAL )                   	// 通常の逆転1発駆動
#define     CMD_M1_FWD_WITH_SHAKE           ( ID_CMD_M1 + ID_CMD_FWD_WITH_SHAKE )               	// 揺動パルス付きの正転、結果は「　　」に入る
#define     CMD_M1_SEARCH_LOAD_POSITION     ( ID_CMD_M1 + ID_CMD_SEARCH_LOAD_POSITION )         	// 微調整位置を正転で探す
#define     CMD_M1_SEARCH_BASE_POSITION     ( ID_CMD_M1 + ID_CMD_SEARCH_BASE_POSITION )         	// 基準位置を正転で探す（粗調整後の微調整検出位置）
#define     CMD_M1_FWD_WITH_CORRECT         ( ID_CMD_M1 + ID_CMD_FWD_WITH_CORRECT )             	// 基準位置ズレ補正付きの運針、位置ズレ検出したら、自動補正
#define     CMD_M1_FWD_POSITION_CTL         ( ID_CMD_M1 + ID_CMD_FWD_POSITION_CTL )             	//
#define     CMD_M1_REV_POSITION_CTL         ( ID_CMD_M1 + ID_CMD_REV_POSITION_CTL )             	//
#define     CMD_M1_START_DETECT_VRS			( ID_CMD_M1 + ID_CMD_START_DETECT_VRS )					//
#define     CMD_M1_STOP_DETECT_VRS			( ID_CMD_M1 + ID_CMD_STOP_DETECT_VRS  )					//
#define     CMD_M1_ENABLE_EXCITATING 		( ID_CMD_M1 + ID_CMD_ENABLE_EXCITATING )				//
#define     CMD_M1_DISABLE_EXCITATING 		( ID_CMD_M1 + ID_CMD_DISABLE_EXCITATING )				//
#define     CMD_M1_SEARCH_1008_POSITION		( ID_CMD_M1 + ID_CMD_SEARCH_1008_POSITION )				//

#define		SPI_SLAVE_INT					LATC0
//------------------------------------------------------------------------------
//.変数
static UB recieveCommand;

static void ( * const SpiCmmandFunction[ ID_CMD_TYPE_MAX ] )( UB motor_no ) = {
#ifdef DEBUG_SPI_CTRL
    SpiCmdFunc_Test,
#else
    NULL_PTR,                               /**-- 0 --**/
#endif

    SpiCmdFunc_FwdPulseNormal,              /**-- 1 --**/
    SpiCmdFunc_RevPulseNormal,              /**-- 2 --**/
    SpiCmdFunc_FwdPulseWithShake,           /**-- 3 --**/
    SpiCmdFunc_SearchLoadPosition,          /**-- 4 --**/
    SpiCmdFunc_SearchBasePosition,          /**-- 5 --**/
    SpiCmdFunc_FwdPulseWithCorrect,         /**-- 6 --**/
    SpiCmdFunc_FwdPulsePositionCtl,         /**-- 7 --**/
    SpiCmdFunc_RevPulsePositionCtl,         /**-- 8 --**/

	SpiCmdFunc_StartDetectVrs,				/**-- 9 --**/
	SpiCmdFunc_StopDetectVrs,				/**-- A --**/
	SpiCmdFunc_EnableExcitating,			/**-- B --**/
	SpiCmdFunc_DisableExcitating,			/**-- C --**/
	SpiCmdFunc_Search1008Position			/**-- D --**/
};
//------------------------------------------------------------------------------
//.関数
//.SPIControl_RecieveComannd
/***************************************************************************//**
 @brief     コマンド受信
 @note      コマンド受け付けるまで（SPI割り込み入るまで）ベタ待ち
            ベタ待ち中でも、モータ駆動が必要なときは、
            TMR2_Interruptでモータは駆動する

            最大駆動周波数200Hzで2モータなので、
            5msの区間で最大2回しかコマンドこない
            （コマンドタイミングが少し早かったとして、合計4コマンド）
            同じモータに対する命令は1ms以上空けること
            データはゆっくりさばいて問題ない


 @author
 @date
*******************************************************************************/
void SPIControl_RecieveComannd( void )
{
    while ( 1 ) {
        /**-- 1byte読み出し --**/
        //      SPI_ReadBlock( &recieveCommand, 1 );            // recieveCommandに保存

        if ( PIR1bits.SSP1IF ) {
            PIR1bits.SSP1IF = 0;
            recieveCommand = SPI_ReadByte( );
            SPIControl_ExecuteCommand( ); // コマンド実行
        }

        WatchMotorDriveFinish( );
    }
}
//------------------------------------------------------------------------------
//.SPIControl_ExecuteCommand
/*******************************************************************************
 @brief     コマンドの内容に応じて、モータ駆動処理を実行する
 @note      モータ駆動中の場合は、停止後に実行する。
 @author
 @date
 ********************************************************************************/
void SPIControl_ExecuteCommand( void )
{
    UB motor_select;
    UB cmd_type;

    motor_select = recieveCommand;
    motor_select &= 0xF0;

    cmd_type = recieveCommand;
    cmd_type &= 0x0F;

    if ( ID_CMD_M0 == motor_select ) {
        motor_select = MOTOR_M0;
    } else if ( ID_CMD_M1 == motor_select ) {
        motor_select = MOTOR_M1;
    } else {
        motor_select = 0xFF;
    }

	if ( ( ID_CMD_TYPE_MAX > cmd_type ) && ( 0xFF != motor_select ) ) {
		if ( ID_CMD_FWD_WITH_CORRECT == cmd_type ) {
			/**-- 補正付き駆動はモータ駆動中でも実行する --**/
			    SpiCmmandFunction[ cmd_type ]( motor_select );
		} else {
			/**-- 補正付き駆動以外 --**/
			if ( GetIsMotorActive( motor_select ) ) {
			    /**-- モータ駆動中である→何もしない --**/
			    SetAfterMotorStopFunc( motor_select, SpiCmmandFunction[ cmd_type ] );
			} else {
			    /**-- モータ駆動中でない→すぐにモータ駆動開始 --**/
			    SpiCmmandFunction[ cmd_type ]( motor_select );
			}
		}
	}
}
//------------------------------------------------------------------------------
//..SpiCmdFunc_Test
/***************************************************************************//**
 @brief     動作テスト用
 @note
 @author
 @date
*******************************************************************************/
void SpiCmdFunc_Test( UB motor_no )
{



}
//------------------------------------------------------------------------------
//..SpiCmdFunc_FwdPulseNormal(1)
/*******************************************************************************
 @brief     正転1発駆動(0xB1,0xC1)
 @note		ゼロマッチ修正のイメージ、針位置カウンタ更新しない
 @author
 @date
 ********************************************************************************/
void SpiCmdFunc_FwdPulseNormal( UB motor_no )
{
	SetDriveType( motor_no, DRIVE_SIMPLE );

    SetMotorDirection( motor_no, FOWARD );          // 正転セット

    SetMotorStepCounter( motor_no, 1 );             // 駆動ステップ数1発

    SetMotorFrequency( motor_no, FREQ_SET_EXTERNAL_CONTROL, NA );

    SetMotorStart( motor_no );                      // 駆動開始
}
//------------------------------------------------------------------------------
//..SpiCmdFunc_RevPulseNormal(2)
/*******************************************************************************
 @brief     逆転1発駆動(0xB2,0xC2)
 @note		ゼロマッチ修正のイメージ、針位置カウンタ更新しない
 @author
 @date
 ********************************************************************************/
void SpiCmdFunc_RevPulseNormal( UB motor_no )
{
	SetDriveType( motor_no, DRIVE_SIMPLE );

    SetMotorDirection( motor_no, REVERSE ); 		// 逆転セット

    SetMotorStepCounter( motor_no, 1 ); 			// 駆動ステップ数1発

    SetMotorFrequency( motor_no, FREQ_SET_EXTERNAL_CONTROL, NA );

    SetMotorStart( motor_no ); 						// 駆動開始
}
//------------------------------------------------------------------------------
//..未作成SpiCmdFunc_FwdPulseWithShake(3)
/*******************************************************************************
 @brief     揺動パルス付き正転1発駆動(0xB3,0xC3)
 @note		ゼロマッチ修正のイメージ、針位置カウンタ更新しない
 @author
 @date
 ********************************************************************************/
void SpiCmdFunc_FwdPulseWithShake( UB motor_no )
{

}
//------------------------------------------------------------------------------
//..未作成SpiCmdFunc_SearchLoadPosition(4)
/*******************************************************************************
 @brief     微調整位置検出(0xB4,0xC4)
 @note      負荷を検出したら止まる。針位置カウンタ更新しない
 @author
 @date
 ********************************************************************************/
void SpiCmdFunc_SearchLoadPosition( UB motor_no )
{

}
//------------------------------------------------------------------------------
//..SpiCmdFunc_SearchBasePosition(5)
/*******************************************************************************
 @brief     基準位置検出(0xB5,0xC5)
 @note      基準位置は、粗調整後の微調整位置とする
 @author
 @date
 ********************************************************************************/
void SpiCmdFunc_SearchBasePosition( UB motor_no )
{
//	H recieve_offset;

	/**-- 基準位置オフセットを受信 --**/
//	SPI_ReadBlock( ( UB* )&recieve_offset, 2 );

	DeactivateHandPosition( motor_no );

	/**-- 駆動開始 --**/
	SetDriveType( motor_no, DRIVE_SEARCH_BASE_POSITION );

    SetMotorDirection( motor_no, FOWARD );      // 正転セット

    SetMotorStepCounter( motor_no, 0 );     	// 駆動ステップ未設定

    SetMotorFrequency( motor_no, FREQ_SET_SELF_CONTROL_SHAKE, SET_FREQ_60HZ ); //

    SetMotorPositionSearchStart( motor_no );         // 駆動開始
}
//------------------------------------------------------------------------------
//..SpiCmdFunc_FwdPulseWithCorrect(6)
/*******************************************************************************
 @brief     位置補正付きの正転1発駆動(0xB6,0xC6)
 @note      位置が期待値と違った場合、強制的に位置補正を行う
			針位置管理機能付きの正転駆動
			対象のモータが駆動中は一旦停止させてから実行させる
			→停止後処理に登録する
 @author
 @date
 ********************************************************************************/
void SpiCmdFunc_FwdPulseWithCorrect( UB motor_no )
{
	UB motor_bit;

	motor_bit = ( UB )( 0x01 << motor_no );

	if ( GetIsMotorActive( motor_no ) ) {
		/**-- モータ駆動中 → モータ停止要求 & 停止後処理を登録--**/
		SetMotorStop( motor_no );
		SetAfterMotorStopFunc( motor_no, SetFwdPulseWithCorrectProc );
	} else {
		/**-- モータ駆動中ではない --**/
		SetFwdPulseWithCorrectProc( motor_no );
	}
}
//------------------------------------------------------------------------------
//...SetFwdPulseWithCorrectProc
/*******************************************************************************
 @brief		位置補正付きの正転1発駆動命令本体
 @note		targetPositionを+1づゝする

 			目標位置を現在位置と比較して、差分計算し、
 			正逆回転判別して、駆動させる
 @author
 @date
********************************************************************************/
void SetFwdPulseWithCorrectProc( UB motor_no )
{
    UB motor_bit = ( UB )( 0x01 << motor_no );

    if ( 0 == ( actCorrectDriveBit & motor_bit ) ) {
		/**-- 補正駆動中以外 --**/
    	SetDriveType( motor_no, DRIVE_CHECK_POSITION );
	    SetMotorFrequency( motor_no, FREQ_SET_EXTERNAL_CONTROL, NA );
    }

	UpdateTargetPosition( motor_no, 1 );			// 目標位置更新
	CalcMotorStepCounter( motor_no );				// 目標位置までの駆動パルス数算出＆回転方向セット

	/**-- 負荷位置を見つけたときの処理を追加 --**/
	if ( ( reqCorrectBasePosBit | actCorrectDriveBit ) & motor_bit ) {
		/**-- 位置再検出中 --**/
		/**-- 負荷発見コールバックは変更しない --**/
	} else {
		/**-- 通常動作中 --**/
		SetAfterDetectLoadFunc( motor_no, VerifyHandPosition_LoadPosition );
	}

	/**-- 駆動スタート --**/
    SetMotorStart( motor_no );                      // 駆動開始
}
//------------------------------------------------------------------------------
//..SpiCmdFunc_FwdPulsePositionCtl(7)
/*******************************************************************************
 @brief     正転1発駆動(0xB7,0xC7)
 @note		正転1発駆動し針位置カウンタも更新する
 @author
 @date
 ********************************************************************************/
void SpiCmdFunc_FwdPulsePositionCtl( UB motor_no )
{
	SetDriveType( motor_no, DRIVE_SIMPLE );

	UpdateTargetPosition( motor_no, 1 );			// 目標位置更新
	CalcMotorStepCounter( motor_no );				// 目標位置までの駆動パルス数算出＆回転方向セット
													// 現在位置更新

    SetMotorFrequency( motor_no, FREQ_SET_EXTERNAL_CONTROL, NA );

    SetMotorStart( motor_no );                      // 駆動開始
}
//------------------------------------------------------------------------------
//..SpiCmdFunc_RevPulsePositionCtl(8)
/*******************************************************************************
 @brief     逆転1発駆動(0xB8,0xC8)
 @note		逆転1発駆動し針位置カウンタも更新する
 @author
 @date
 ********************************************************************************/
void SpiCmdFunc_RevPulsePositionCtl( UB motor_no )
{
	SetDriveType( motor_no, DRIVE_SIMPLE );

	UpdateTargetPosition( motor_no, -1 );			// 目標位置更新
	CalcMotorStepCounter( motor_no );				// 目標位置までの駆動パルス数算出＆回転方向セット
													// 現在位置更新

    SetMotorFrequency( motor_no, FREQ_SET_EXTERNAL_CONTROL, NA );

    SetMotorStart( motor_no ); 						// 駆動開始
}
//------------------------------------------------------------------------------
//..SpiCmdFunc_StartDetectVrs(9)
/***************************************************************************//**
 @brief		VRS検出開始(0xB9,0xC9)
 @note
 @author
 @date
*******************************************************************************/
void SpiCmdFunc_StartDetectVrs( UB motor_no )
{
	SetMotorStartSPK();
}
//------------------------------------------------------------------------------
//..SpiCmdFunc_StopDetectVrs(10)
/***************************************************************************//**
 @brief		VRS検出停止(0xBA,0xCA)
 @note
 @author
 @date
*******************************************************************************/
void SpiCmdFunc_StopDetectVrs( UB motor_no )
{
	SetMotorStopSPK();
}
//------------------------------------------------------------------------------
//..SpiCmdFunc_EnableExcitating(11)
/***************************************************************************//**
 @brief		VRS検出したときに励磁する設定(0xBB,0xCB)
 @note
 @author
 @date
*******************************************************************************/
void SpiCmdFunc_EnableExcitating( UB motor_no )
{
	SetMotorEnableExcitating( motor_no );
}
//------------------------------------------------------------------------------
//..SpiCmdFunc_DisableExcitating(12)
/***************************************************************************//**
 @brief		VRS検出したときに励磁しない設定(0xBC,0xCC)
 @note
 @author
 @date
*******************************************************************************/
void SpiCmdFunc_DisableExcitating( UB motor_no )
{
	SetMotorDisableExcitating( motor_no );
}
//------------------------------------------------------------------------------
//..SpiCmdFunc_SearchBasePosition(13)
/*******************************************************************************
 @brief     基準位置検出後10：08に移動する
 @note
 @author
 @date
 ********************************************************************************/
void SpiCmdFunc_Search1008Position( UB motor_no )
{
//	H recieve_offset;

	/**-- 基準位置オフセットを受信 --**/
//	SPI_ReadBlock( ( UB* )&recieve_offset, 2 );

	DeactivateHandPosition( motor_no );

	/**-- 駆動開始 --**/
	SetDriveType( motor_no, DRIVE_SEARCH_1008_POSITION );

    SetMotorDirection( motor_no, FOWARD );      // 正転セット

    SetMotorStepCounter( motor_no, 0 );     	// 駆動ステップ未設定

    SetMotorFrequency( motor_no, FREQ_SET_SELF_CONTROL_SHAKE, SET_FREQ_60HZ ); //

    SetMotorPositionSearchStart( motor_no );         // 駆動開始
}
//------------------------------------------------------------------------------
//.SPIControl_SetNotification
/*******************************************************************************
 @brief		完了通知
 @note
 @author
 @date
********************************************************************************/
void SPIControl_SetNotification( UB motor_no )
{
	if ( ( BIT_MOTOR_M0 + BIT_MOTOR_M1 )== handPositionActiveBit ) {

	    SPI_SLAVE_INT = 1;


		__delay_us(1000);


	    SPI_SLAVE_INT = 0;

		__delay_us(1000);

	}
}
//------------------------------------------------------------------------------

