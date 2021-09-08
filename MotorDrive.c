//.filename MotorDrive.c
#include "mcc.h"
#include "MotorDrive.h"
#include "misc.h"
#include "data_types.h"
#include "spi_control.h"
/***************************************************************************//**
 @brief     TMR2割り込みでモータを駆動させる
 @note      MOTORポート割当

    RAx
    |bit7   |bit6   |bit5   |bit4   |bit3   |bit2   |bit1   |bit0   |
    |-      |-      |RA5    |RA4    |-      |RA2    |RA1    |RA0    |
    |-      |-      |M0_2   |M0_1   |-      |-      |M0_3   |-      |

    RBx
    |bit7   |bit6   |bit5   |bit4   |bit3   |bit2   |bit1   |bit0   |
    |RB7    |RB6    |RB5    |RB4    |-      |-      |-      |-      |
    |M1_1   |SCK    |M1_2   |SDI    |-      |-      |-      |-      |


    RCx
    |bit7   |bit6   |bit5   |bit4   |bit3   |bit2   |bit1   |bit0   |
    |RC7    |RC6    |RC5    |RC4    |RC3    |RC2    |RC1    |RC0    |
    |SDO    |SS     |M0_VD  |M1_VD  |M1_4   |M1_3   |M0_4   |REQ    |

 @author
 @date
*******************************************************************************/
//.DebugOption
//..RC4出力切り替え
//#define		DEBUG_TIMER2            // TIMER2処理時間測定用
//#define		DEBUG_VRS0			  	// 負荷検出場所
//#define		DEBUG_CORRECT_POSITION	// 位置補正デバッグ

//#define		DEBUG_TEST_GEAR

#define     TOUCH_TEST          //触読トルク測定用デバッグOPTION　通常：コメントアウト
/*******************************************************************************
このFWは、トルク測定用に触読ONSWが入力されると、励磁ONとなる。
その際、SPK有り無しを励磁有り無しSWで選択できる
********************************************************************************/
//.定数
/**-- M0出力ポート --**/
/**-- 検出抵抗は無操作 --**/
#define     BIT_PORTA_M0_ALL    b0011_0010
#define     BIT_PORTB_M0_ALL    b0000_0000
#define     BIT_PORTC_M0_ALL    b0000_0010

#define     BIT_PORTA_M0_OUT1   b0001_0000
#define     BIT_PORTB_M0_OUT1   b0000_0000
#define     BIT_PORTC_M0_OUT1   b0000_0000

#define     BIT_PORTA_M0_OUT2   b0010_0000
#define     BIT_PORTB_M0_OUT2   b0000_0000
#define     BIT_PORTC_M0_OUT2   b0000_0000

#define     BIT_PORTA_M0_OUT3   b0000_0010
#define     BIT_PORTB_M0_OUT3   b0000_0000
#define     BIT_PORTC_M0_OUT3   b0000_0000

#define     BIT_PORTA_M0_OUT4   b0000_0000
#define     BIT_PORTB_M0_OUT4   b0000_0000
#define     BIT_PORTC_M0_OUT4   b0000_0010

/**-- M1出力ポート --**/
/**-- 検出抵抗は無操作 --**/
#define     BIT_PORTA_M1_ALL    b0000_0000
#define     BIT_PORTB_M1_ALL    b1010_0000
#define     BIT_PORTC_M1_ALL    b0000_1100

#define     BIT_PORTA_M1_OUT1   b0000_0000
#define     BIT_PORTB_M1_OUT1   b1000_0000
#define     BIT_PORTC_M1_OUT1   b0000_0000

#define     BIT_PORTA_M1_OUT2   b0000_0000
#define     BIT_PORTB_M1_OUT2   b0010_0000
#define     BIT_PORTC_M1_OUT2   b0000_0000

#define     BIT_PORTA_M1_OUT3   b0000_0000
#define     BIT_PORTB_M1_OUT3   b0000_0000
#define     BIT_PORTC_M1_OUT3   b0000_0100

#define     BIT_PORTA_M1_OUT4   b0000_0000
#define     BIT_PORTB_M1_OUT4   b0000_0000
#define     BIT_PORTC_M1_OUT4   b0000_1000

/*  入出力レジスタTRISX　*/
/**-- M0入出力切り替え --**/
#define     BIT_TRISA_M0_ALL    b0000_0010
#define     BIT_TRISB_M0_ALL    b0000_0000
#define     BIT_TRISC_M0_ALL    b0010_0010

#define     BIT_TRISA_M0_DRV    b0000_0000
#define     BIT_TRISB_M0_DRV    b0000_0000
#define     BIT_TRISC_M0_DRV    b0010_0000

#define     BIT_TRISA_M0_O3_SHT b0000_0000
#define     BIT_TRISB_M0_O3_SHT b0000_0000
#define     BIT_TRISC_M0_O3_SHT b0000_0000

#define     BIT_TRISA_M0_O3_OPN b0000_0010
#define     BIT_TRISB_M0_O3_OPN b0000_0000
#define     BIT_TRISC_M0_O3_OPN b0000_0000

#define     BIT_TRISA_M0_O4_SHT b0000_0000
#define     BIT_TRISB_M0_O4_SHT b0000_0000
#define     BIT_TRISC_M0_O4_SHT b0000_0000

#define     BIT_TRISA_M0_O4_OPN b0000_0000
#define     BIT_TRISB_M0_O4_OPN b0000_0000
#define     BIT_TRISC_M0_O4_OPN b0000_0010
//++++++++++++++++++++++++++Touch Read++++++++++++++++++++++++++++++++++++++
/* 触読用******************************************************************/
#define     BIT_TRISA_M0_SHT    (BIT_TRISA_M0_O3_SHT | BIT_TRISA_M0_O4_SHT)
#define     BIT_TRISB_M0_SHT    (BIT_TRISB_M0_O3_SHT | BIT_TRISB_M0_O4_SHT)
#define     BIT_TRISC_M0_SHT    (BIT_TRISC_M0_O3_SHT | BIT_TRISC_M0_O4_SHT)

#define     BIT_PORTA_OUT1   (BIT_PORTA_M0_OUT1 | BIT_PORTA_M1_OUT1)
#define     BIT_PORTB_OUT1   (BIT_PORTB_M0_OUT1 | BIT_PORTB_M1_OUT1)
#define     BIT_PORTC_OUT1   (BIT_PORTC_M0_OUT1 | BIT_PORTC_M1_OUT1)
/**************************************************************************/
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

/*  入出力レジスタTRISX　*/
/**-- M1入出力切り替え --**/
#define     BIT_TRISA_M1_ALL    b0000_0000
#define     BIT_TRISB_M1_ALL    b0000_0000
#define     BIT_TRISC_M1_ALL    b0001_1100

#define     BIT_TRISA_M1_DRV    b0000_0000
#define     BIT_TRISB_M1_DRV    b0000_0000
#define     BIT_TRISC_M1_DRV    b0001_0000

#define     BIT_TRISA_M1_O3_SHT b0000_0000
#define     BIT_TRISB_M1_O3_SHT b0000_0000
#define     BIT_TRISC_M1_O3_SHT b0000_0000

#define     BIT_TRISA_M1_O3_OPN b0000_0000
#define     BIT_TRISB_M1_O3_OPN b0000_0000
#define     BIT_TRISC_M1_O3_OPN b0000_0100

#define     BIT_TRISA_M1_O4_SHT b0000_0000
#define     BIT_TRISB_M1_O4_SHT b0000_0000
#define     BIT_TRISC_M1_O4_SHT b0000_0000

#define     BIT_TRISA_M1_O4_OPN b0000_0000
#define     BIT_TRISB_M1_O4_OPN b0000_0000
#define     BIT_TRISC_M1_O4_OPN b0000_1000
//++++++++++++++++++++++++++Touch Read++++++++++++++++++++++++++++++++++++++
/* 触読用******************************************************************/
#define     BIT_TRISA_M1_SHT    (BIT_TRISA_M1_O3_SHT | BIT_TRISA_M1_O4_SHT)
#define     BIT_TRISB_M1_SHT    (BIT_TRISB_M1_O3_SHT | BIT_TRISB_M1_O4_SHT)
#define     BIT_TRISC_M1_SHT    (BIT_TRISC_M1_O3_SHT | BIT_TRISC_M1_O4_SHT)

/* 触読用Time設定値*/
#define     EXCITATING_OFF_MSK_TIME  82      //励磁OFF後のマスク時間(=10msec)
#define     EXCITATING_TIME  8197        //励磁ON継続時間(=1sec)
/* 触読用　インジケータLED*/
#define     BIT_PORTA_LED_ON    b0000_0100

#define     CHANGE_POSITION_OPEN    1       //変化ポイント　OPEN
#define     CHANGE_POSITION_SHORT   0       //変化ポイント　SHORT

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**************************************************************************/
/* コンパレータ設定値 */
//Comparator1
#define     CMP1_BASE           b0001_0100
#define     CMP1_ON             b1000_0000
#define     CMP1_VCOMP_BASE     b1001_0000
#define     M0_O3_CMP           b0000_0000
#define     M0_O4_CMP           b0000_0001
//Comparator2
#define     CMP2_BASE           b0001_0100
#define     CMP2_ON             b1000_0000
#define     CMP2_VCOMP_BASE     b1001_0000
#define     M1_O3_CMP           b0000_0010
#define     M1_O4_CMP           b0000_0011



//..M0
//...M0_SHAKE 揺動パルス+SPK
//....POL0
//.....P0(OUT1)
#define     BIT_PORTA_M0_SHAKE_POL0_P0  BIT_PORTA_M0_OUT1
#define     BIT_PORTB_M0_SHAKE_POL0_P0  BIT_PORTB_M0_OUT1
#define     BIT_PORTC_M0_SHAKE_POL0_P0  BIT_PORTC_M0_OUT1
//.....SPK(OPEN OUT4)
#define     BIT_TRISA_M0_SPK_POL0_OPN   BIT_TRISA_M0_O4_OPN
#define     BIT_TRISB_M0_SPK_POL0_OPN   BIT_TRISB_M0_O4_OPN
#define     BIT_TRISC_M0_SPK_POL0_OPN   BIT_TRISC_M0_O4_OPN
//.....SPK(SHORT OUT4)
#define     BIT_TRISA_M0_SPK_POL0_SHT   BIT_TRISA_M0_O4_SHT
#define     BIT_TRISB_M0_SPK_POL0_SHT   BIT_TRISB_M0_O4_SHT
#define     BIT_TRISC_M0_SPK_POL0_SHT   BIT_TRISC_M0_O4_SHT
//------------------------------------------------------------------------------
//....POL1
//.....P0(OUT2)
#define     BIT_PORTA_M0_SHAKE_POL1_P0  BIT_PORTA_M0_OUT2
#define     BIT_PORTB_M0_SHAKE_POL1_P0  BIT_PORTB_M0_OUT2
#define     BIT_PORTC_M0_SHAKE_POL1_P0  BIT_PORTC_M0_OUT2
//.....SPK(OPEN OUT3)
#define     BIT_TRISA_M0_SPK_POL1_OPN   BIT_TRISA_M0_O3_OPN
#define     BIT_TRISB_M0_SPK_POL1_OPN   BIT_TRISB_M0_O3_OPN
#define     BIT_TRISC_M0_SPK_POL1_OPN   BIT_TRISC_M0_O3_OPN
//.....SPK(SHORT OUT3)
#define     BIT_TRISA_M0_SPK_POL1_SHT   BIT_TRISA_M0_O3_SHT
#define     BIT_TRISB_M0_SPK_POL1_SHT   BIT_TRISB_M0_O3_SHT
#define     BIT_TRISC_M0_SPK_POL1_SHT   BIT_TRISC_M0_O3_SHT
//------------------------------------------------------------------------------
//..M1
//...SHAKE　揺動パルス+SPK
//....POL0
//.....P0(OUT1)
#define     BIT_PORTA_M1_SHAKE_POL0_P0  BIT_PORTA_M1_OUT1
#define     BIT_PORTB_M1_SHAKE_POL0_P0  BIT_PORTB_M1_OUT1
#define     BIT_PORTC_M1_SHAKE_POL0_P0  BIT_PORTC_M1_OUT1
//.....SPK(OPEN OUT4)
#define     BIT_TRISA_M1_SPK_POL0_OPN   BIT_TRISA_M1_O4_OPN
#define     BIT_TRISB_M1_SPK_POL0_OPN   BIT_TRISB_M1_O4_OPN
#define     BIT_TRISC_M1_SPK_POL0_OPN   BIT_TRISC_M1_O4_OPN
//.....SPK(SHORT OUT4)
#define     BIT_TRISA_M1_SPK_POL0_SHT   BIT_TRISA_M1_O4_SHT
#define     BIT_TRISB_M1_SPK_POL0_SHT   BIT_TRISB_M1_O4_SHT
#define     BIT_TRISC_M1_SPK_POL0_SHT   BIT_TRISC_M1_O4_SHT
//------------------------------------------------------------------------------
//....POL1
//.....P0(OUT2)
#define     BIT_PORTA_M1_SHAKE_POL1_P0  BIT_PORTA_M1_OUT2
#define     BIT_PORTB_M1_SHAKE_POL1_P0  BIT_PORTB_M1_OUT2
#define     BIT_PORTC_M1_SHAKE_POL1_P0  BIT_PORTC_M1_OUT2
//.....SPK(OPEN OUT3)
#define     BIT_TRISA_M1_SPK_POL1_OPN   BIT_TRISA_M1_O3_OPN
#define     BIT_TRISB_M1_SPK_POL1_OPN   BIT_TRISB_M1_O3_OPN
#define     BIT_TRISC_M1_SPK_POL1_OPN   BIT_TRISC_M1_O3_OPN
//.....SPK(SHORT OUT3)
#define     BIT_TRISA_M1_SPK_POL1_SHT   BIT_TRISA_M1_O3_SHT
#define     BIT_TRISB_M1_SPK_POL1_SHT   BIT_TRISB_M1_O3_SHT
#define     BIT_TRISC_M1_SPK_POL1_SHT   BIT_TRISC_M1_O3_SHT
//------------------------------------------------------------------------------
//..M0
//...FWD
//....POL0
//.....P1(OUT1)
#define     BIT_PORTA_M0_FWD_POL0_P1    BIT_PORTA_M0_OUT1
#define     BIT_PORTB_M0_FWD_POL0_P1    BIT_PORTB_M0_OUT1
#define     BIT_PORTC_M0_FWD_POL0_P1    BIT_PORTC_M0_OUT1
//.....P2(OUT3)
#define     BIT_PORTA_M0_FWD_POL0_P2    BIT_PORTA_M0_OUT3
#define     BIT_PORTB_M0_FWD_POL0_P2    BIT_PORTB_M0_OUT3
#define     BIT_PORTC_M0_FWD_POL0_P2    BIT_PORTC_M0_OUT3
//------------------------------------------------------------------------------
//....POL1
//.....P1(OUT2)
#define     BIT_PORTA_M0_FWD_POL1_P1    BIT_PORTA_M0_OUT2
#define     BIT_PORTB_M0_FWD_POL1_P1    BIT_PORTB_M0_OUT2
#define     BIT_PORTC_M0_FWD_POL1_P1    BIT_PORTC_M0_OUT2
//.....P2(OUT4)
#define     BIT_PORTA_M0_FWD_POL1_P2    BIT_PORTA_M0_OUT4
#define     BIT_PORTB_M0_FWD_POL1_P2    BIT_PORTB_M0_OUT4
#define     BIT_PORTC_M0_FWD_POL1_P2    BIT_PORTC_M0_OUT4
//------------------------------------------------------------------------------
//...REV
//....POL0
//.....P1(OUT4)
#define     BIT_PORTA_M0_REV_POL0_P1    BIT_PORTA_M0_OUT4
#define     BIT_PORTB_M0_REV_POL0_P1    BIT_PORTB_M0_OUT4
#define     BIT_PORTC_M0_REV_POL0_P1    BIT_PORTC_M0_OUT4
//.....P2(OUT2)
#define     BIT_PORTA_M0_REV_POL0_P2    BIT_PORTA_M0_OUT2
#define     BIT_PORTB_M0_REV_POL0_P2    BIT_PORTB_M0_OUT2
#define     BIT_PORTC_M0_REV_POL0_P2    BIT_PORTC_M0_OUT2
//------------------------------------------------------------------------------
//....POL1
//.....P1(OUT3)
#define     BIT_PORTA_M0_REV_POL1_P1    BIT_PORTA_M0_OUT3
#define     BIT_PORTB_M0_REV_POL1_P1    BIT_PORTB_M0_OUT3
#define     BIT_PORTC_M0_REV_POL1_P1    BIT_PORTC_M0_OUT3
//.....P2(OUT1)
#define     BIT_PORTA_M0_REV_POL1_P2    BIT_PORTA_M0_OUT1
#define     BIT_PORTB_M0_REV_POL1_P2    BIT_PORTB_M0_OUT1
#define     BIT_PORTC_M0_REV_POL1_P2    BIT_PORTC_M0_OUT1
//------------------------------------------------------------------------------
//..M1
//...FWD
//....POL0
//.....P1(OUT1)
#define     BIT_PORTA_M1_FWD_POL0_P1    BIT_PORTA_M1_OUT1
#define     BIT_PORTB_M1_FWD_POL0_P1    BIT_PORTB_M1_OUT1
#define     BIT_PORTC_M1_FWD_POL0_P1    BIT_PORTC_M1_OUT1
//.....P2(OUT3)
#define     BIT_PORTA_M1_FWD_POL0_P2    BIT_PORTA_M1_OUT3
#define     BIT_PORTB_M1_FWD_POL0_P2    BIT_PORTB_M1_OUT3
#define     BIT_PORTC_M1_FWD_POL0_P2    BIT_PORTC_M1_OUT3
//------------------------------------------------------------------------------
//....POL1
//.....P1(OUT2)
#define     BIT_PORTA_M1_FWD_POL1_P1    BIT_PORTA_M1_OUT2
#define     BIT_PORTB_M1_FWD_POL1_P1    BIT_PORTB_M1_OUT2
#define     BIT_PORTC_M1_FWD_POL1_P1    BIT_PORTC_M1_OUT2
//.....P2(OUT4)
#define     BIT_PORTA_M1_FWD_POL1_P2    BIT_PORTA_M1_OUT4
#define     BIT_PORTB_M1_FWD_POL1_P2    BIT_PORTB_M1_OUT4
#define     BIT_PORTC_M1_FWD_POL1_P2    BIT_PORTC_M1_OUT4
//------------------------------------------------------------------------------
//...REV
//....POL0
//.....P1(OUT4)
#define     BIT_PORTA_M1_REV_POL0_P1    BIT_PORTA_M1_OUT4
#define     BIT_PORTB_M1_REV_POL0_P1    BIT_PORTB_M1_OUT4
#define     BIT_PORTC_M1_REV_POL0_P1    BIT_PORTC_M1_OUT4
//.....P2(OUT2)
#define     BIT_PORTA_M1_REV_POL0_P2    BIT_PORTA_M1_OUT2
#define     BIT_PORTB_M1_REV_POL0_P2    BIT_PORTB_M1_OUT2
#define     BIT_PORTC_M1_REV_POL0_P2    BIT_PORTC_M1_OUT2
//------------------------------------------------------------------------------
//....POL1
//.....P1(OUT3)
#define     BIT_PORTA_M1_REV_POL1_P1    BIT_PORTA_M1_OUT3
#define     BIT_PORTB_M1_REV_POL1_P1    BIT_PORTB_M1_OUT3
#define     BIT_PORTC_M1_REV_POL1_P1    BIT_PORTC_M1_OUT3
//.....P2(OUT1)
#define     BIT_PORTA_M1_REV_POL1_P2    BIT_PORTA_M1_OUT1
#define     BIT_PORTB_M1_REV_POL1_P2    BIT_PORTB_M1_OUT1
#define     BIT_PORTC_M1_REV_POL1_P2    BIT_PORTC_M1_OUT1
//------------------------------------------------------------------------------
//..パルス出力設定値
/*******************************************************************************
 @brief     Positionカウンタテーブル、
            mxPulsePositionCtがこのテーブルの値に一致したら、
            関数ポインタ実行しパルス切り替え
 @note
 @author
 @date
 ********************************************************************************/
//...mxPulsePositionCt
#ifdef DEBUG_TEST_GEAR
//	#define     WIDTH_PULSE_P0              5           // P0パルス幅                        4:0.488ms / 5:0.610ms / 6:0.732ms
#define     WIDTH_PULSE_P0              4           // P0パルス幅                        4:0.488ms / 5:0.610ms / 6:0.732ms
#else
#define     WIDTH_PULSE_P0              5           // P0パルス幅(1号機)                 4:0.488ms / 5:0.610ms / 6:0.732ms
//	#define     WIDTH_PULSE_P0              5           // P0パルス幅(2号機)                 4:0.488ms / 5:0.610ms / 6:0.732ms
#endif

#define     WIDTH_SPK_START_WAIT            4           // P0のあとのSPK開始待ち時間         4:0.488ms
#define     WIDTH_SPK_ON                    1           // SPK_ON幅                          1:0.122ms
#define     WIDTH_SPK_OFF                   3           // SPK_OFF幅                         3:0.366ms
#define     NUMBER_OF_REPEAT_SPK            10          // SPK本数                          10:4.880ms
#define     WIDTH_PULSE_P1                  6           // P1パルス幅                        6:0.732ms
#define     WIDTH_PULSE_P2                  16          // P2パルス幅                       16:1.952ms
#define     WIDTH_PULSE_OFF_MIN             7           // P2のあとの最低OFF確保時間         7:0.854ms ( 200Hzよりも早い間隔のみを確保 )
#define     WIDTH_PULSE_OFF_DEF             19          // P2のあとの標準OFF確保時間        19:2.318ms

#define     WIDTH_SHAKE_PULSE_ALL           ( POS_SHAKE_INIT_DATA + WIDTH_PULSE_P0 + WIDTH_SPK_START_WAIT + ( WIDTH_SPK_ON + WIDTH_SPK_OFF ) * NUMBER_OF_REPEAT_SPK )

#define     POS_P0_START                    1                           // 必ず1以上の数
#define     POS_P1_START                    ( 1 + WIDTH_SHAKE_PULSE_ALL )

#define     POS_SHAKE_INIT_DATA             ( POS_P0_START - 1 )        // POS_P0_STARTの一つ前にする（初めの1カウントは半端な時間になるため）
#define     POS_P1P2_INIT_DATA              ( POS_P1_START - 1 )        // POS_P1_STARTの一つ前にする（初めの1カウントは半端な時間になるため）


#define     POS_PULSE_INTERVAL_FINISH_MIN   ( POS_P1_START + WIDTH_PULSE_P1 + WIDTH_PULSE_P2 + WIDTH_PULSE_OFF_MIN )
#define     POS_PULSE_INTERVAL_FINISH_DEF   ( POS_P1_START + WIDTH_PULSE_P1 + WIDTH_PULSE_P2 + WIDTH_PULSE_OFF_DEF )

//#define     PULSE_CHANGE_ARRAY_LENGTH     ( 6 + NUMBER_OF_REPEAT_SPK * 2 )
#define     PULSE_CHANGE_ARRAY_LENGTH       ( 5 + NUMBER_OF_REPEAT_SPK * 2 )
// 6 = ( POS_P0_START, POS_P0_FINISH, POS_P1_START, POS_P2_START, POS_OFF_START, POS_FINISH )

#define     PULSE_CHANGE_ARRAY_LAST         ( PULSE_CHANGE_ARRAY_LENGTH - 1 )

enum PULSE_CHANGE_POSITION {
    PULSE_CHANGE_START_P0 = (0 + POS_P0_START),
    PULSE_CHANGE_STOP_P0 = (PULSE_CHANGE_START_P0 + WIDTH_PULSE_P0),
    PULSE_CHANGE_START_WAITE = (PULSE_CHANGE_STOP_P0 + WIDTH_SPK_START_WAIT),
    PULSE_CHANGE_START_SPK_ON_1 = (PULSE_CHANGE_START_WAITE + WIDTH_SPK_ON),
    PULSE_CHANGE_START_SPK_OFF_1 = (PULSE_CHANGE_START_SPK_ON_1 + WIDTH_SPK_OFF),
    PULSE_CHANGE_START_SPK_ON_2 = (PULSE_CHANGE_START_SPK_OFF_1 + WIDTH_SPK_ON),
    PULSE_CHANGE_START_SPK_OFF_2 = (PULSE_CHANGE_START_SPK_ON_2 + WIDTH_SPK_OFF),
    PULSE_CHANGE_START_SPK_ON_3 = (PULSE_CHANGE_START_SPK_OFF_2 + WIDTH_SPK_ON),
    PULSE_CHANGE_START_SPK_OFF_3 = (PULSE_CHANGE_START_SPK_ON_3 + WIDTH_SPK_OFF),
    PULSE_CHANGE_START_SPK_ON_4 = (PULSE_CHANGE_START_SPK_OFF_3 + WIDTH_SPK_ON),
    PULSE_CHANGE_START_SPK_OFF_4 = (PULSE_CHANGE_START_SPK_ON_4 + WIDTH_SPK_OFF),
    PULSE_CHANGE_START_SPK_ON_5 = (PULSE_CHANGE_START_SPK_OFF_4 + WIDTH_SPK_ON),
    PULSE_CHANGE_START_SPK_OFF_5 = (PULSE_CHANGE_START_SPK_ON_5 + WIDTH_SPK_OFF),
    PULSE_CHANGE_START_SPK_ON_6 = (PULSE_CHANGE_START_SPK_OFF_5 + WIDTH_SPK_ON),
    PULSE_CHANGE_START_SPK_OFF_6 = (PULSE_CHANGE_START_SPK_ON_6 + WIDTH_SPK_OFF),
    PULSE_CHANGE_START_SPK_ON_7 = (PULSE_CHANGE_START_SPK_OFF_6 + WIDTH_SPK_ON),
    PULSE_CHANGE_START_SPK_OFF_7 = (PULSE_CHANGE_START_SPK_ON_7 + WIDTH_SPK_OFF),
    PULSE_CHANGE_START_SPK_ON_8 = (PULSE_CHANGE_START_SPK_OFF_7 + WIDTH_SPK_ON),
    PULSE_CHANGE_START_SPK_OFF_8 = (PULSE_CHANGE_START_SPK_ON_8 + WIDTH_SPK_OFF),
    PULSE_CHANGE_START_SPK_ON_9 = (PULSE_CHANGE_START_SPK_OFF_8 + WIDTH_SPK_ON),
    PULSE_CHANGE_START_SPK_OFF_9 = (PULSE_CHANGE_START_SPK_ON_9 + WIDTH_SPK_OFF),
    PULSE_CHANGE_START_SPK_ON_10 = (PULSE_CHANGE_START_SPK_OFF_9 + WIDTH_SPK_ON),
    PULSE_CHANGE_START_SPK_OFF_10 = (PULSE_CHANGE_START_SPK_ON_10 + WIDTH_SPK_OFF),
    PULSE_CHANGE_START_P1 = (PULSE_CHANGE_START_SPK_OFF_10 + WIDTH_PULSE_P1),
    PULSE_CHANGE_START_P2 = (PULSE_CHANGE_START_P1 + WIDTH_PULSE_P2),
    PULSE_CHANGE_OFF = (PULSE_CHANGE_START_P2 + WIDTH_PULSE_OFF_DEF)
};

//...ID_PULSE_OUT
//------------------------------------------------------------------------------

enum ID_PULSE_OUT_STEP {
    ID_PULSE_OUT_STEP_P0 = 0,
    ID_PULSE_OUT_STEP_P0_OFF = 1,
    ID_PULSE_OUT_STEP_SPK = 2,
    ID_PULSE_OUT_STEP_P1 = (ID_PULSE_OUT_STEP_SPK + 2 * NUMBER_OF_REPEAT_SPK), // 22
    ID_PULSE_OUT_STEP_P2 = (ID_PULSE_OUT_STEP_P1 + 1), // 23
    ID_PULSE_OUT_STEP_OFF = (ID_PULSE_OUT_STEP_P1 + 2), // 24
    ID_PULSE_OUT_STEP_WAIT = (ID_PULSE_OUT_STEP_P1 + 3), // 25
    ID_PULSE_OUT_STEP_MAX = (ID_PULSE_OUT_STEP_P1 + 4) // 26
};
//------------------------------------------------------------------------------
//...mxPulseOutStep
#define     OUTSTEP_SHAKE_INIT_DATA     ID_PULSE_OUT_STEP_P0
#define     OUTSTEP_P1P2_INIT_DATA      ID_PULSE_OUT_STEP_P1
//------------------------------------------------------------------------------
//..位置検出設定値
//...検出状態ID

enum ID_SEARCH_STAT {
    ID_SEARCH_FINE_POS_1ST = 0, // 最初の微調整位置サーチ
    ID_SEARCH_FINE_ROUGH_POS, // 微調整・粗調整の位置サーチ
    ID_SEARCH_MAX
};
//------------------------------------------------------------------------------
//...検出結果ID

enum ID_SEARCH_RESULT {
    ID_SEARCH_RESULT_NULL = 0, // 結果なし（前の負荷の続きとか）
    ID_SEARCH_RESULT_NEW_POS, // 新規位置検出
    ID_SEARCH_RESULT_CONT, // 前の続き
    ID_SEARCH_RESULT_RETRY, // 位置検出失敗→やり直し
    ID_SEARCH_RESULT_SUCCESS, // 位置検出成功
    ID_SEARCH_RESULT_ERR // 位置検出失敗→タイムオーバー
};
//------------------------------------------------------------------------------
//...検出した負荷の種別
#define MARK_DETECT_LOAD_TYPE_CONT      10

enum ID_DETECT_LOAD_TYPE {
    ID_DETECT_LOAD_NULL = 0, // 検出なし
    ID_DETECT_LOAD_FINE = 1, // 微調整位置を検出
    ID_DETECT_LOAD_ROUGH = 2, // 粗調整位置を検出
    ID_DETECT_LOAD_FINE_CONT = (ID_DETECT_LOAD_FINE + MARK_DETECT_LOAD_TYPE_CONT), // 微調整位置続きを検出
    ID_DETECT_LOAD_ROUGH_CONT = (ID_DETECT_LOAD_ROUGH + MARK_DETECT_LOAD_TYPE_CONT), // 粗調整位置続きを検出
    ID_DETECT_LOAD_MAX
};
//------------------------------------------------------------------------------
//...検出セッティング
#define     NUMBER_OF_FINE_POSITION     8           // 1周で8箇所微調整位置は現れる
#define     NUMBER_OF_ROUGH_POSITION    1           // 1周で1箇所粗調整位置は現れる

#define     MAX_FINE_DETECTION_NUMBER   2           // 1つの微調整位置は連続2箇所まで負荷が現れても可とする
#define     MAX_ROUGH_DETECTION_NUMBER  9           // 1つの粗調整位置は連続5箇所まで負荷が現れても可とする

#define     MAX_INTERVAL_FINE_CONT      2           // 微調整位置の連続許容条件
#define     MAX_INTERVAL_ROUGH_CONT     3           // 粗調整位置の連続許容条件

#define     THRESHOLD_FINE_DIFF_UPPER   47          // 微調整位置は46step以下離れていると仮定する
#define     THRESHOLD_FINE_DIFF_LOWER   43          // 微調整位置は44step以上離れていると仮定する
// 微調整位置は44,45,46を考慮しておく
// つまり負荷位置の差が44未満の箇所は粗調位置と判定する
#define     THRESHOLD_ROUGH_DIFF_UPPER  2           // 粗調整位置は2stepより多く離れて検出されない

#define     MAX_SEARCH_ROTATION         3           // 検出にかける最大回転数

#define     MAX_LOAD_POSITION_ARRAY_LENGTH      ( ( ( NUMBER_OF_FINE_POSITION * MAX_FINE_DETECTION_NUMBER ) + ( NUMBER_OF_ROUGH_POSITION * MAX_ROUGH_DETECTION_NUMBER ) ) - 2 )         // 負荷位置許容数


#define     MAX_STEPS_M0                360
#define     MAX_STEPS_M1                360

static const H TblMaxSteps[ MOTOR_NUM ] = {
    MAX_STEPS_M0,
    MAX_STEPS_M1
};

static const H TblMaxSearchSteps[ MOTOR_NUM ] = {
    MAX_STEPS_M0 * 3,
    MAX_STEPS_M1 * 3
};
#ifdef DEBUG_TEST_GEAR
//	#define     OFFSET_M0_BASE_POSITION       212         // 基準位置
//	#define     OFFSET_M1_BASE_POSITION        6        // 基準位置

#define     OFFSET_M0_BASE_POSITION        0         // 基準位置
#define     OFFSET_M1_BASE_POSITION        0 	      // 基準位置
#else
/*1号機*/
//	#define     OFFSET_M0_BASE_POSITION        293         // 基準位置
//	#define     OFFSET_M1_BASE_POSITION        294        // 基準位置

#define     OFFSET_M0_BASE_POSITION        -24      // 基準位置
#define     OFFSET_M1_BASE_POSITION        -16        // 基準位置

/*2号機*/
//	#define     OFFSET_M0_BASE_POSITION        12         // 基準位置
//	#define     OFFSET_M1_BASE_POSITION        35        // 基準位置

#endif
//------------------------------------------------------------------------------
//.プロトタイプ宣言
void SetPulseOutLATxWork_M0_FWD_POL0_P1(void);
void SetPulseOutLATxWork_M0_FWD_POL0_P2(void);
void SetPulseOutLATxWork_M0_FWD_POL0_OFF(void);
void SetPulseOutLATxWork_M0_FWD_POL1_P1(void);
void SetPulseOutLATxWork_M0_FWD_POL1_P2(void);
void SetPulseOutLATxWork_M0_FWD_POL1_OFF(void);
void SetPulseOutLATxWork_M0_REV_POL0_P1(void);
void SetPulseOutLATxWork_M0_REV_POL0_P2(void);
void SetPulseOutLATxWork_M0_REV_POL0_OFF(void);
void SetPulseOutLATxWork_M0_REV_POL1_P1(void);
void SetPulseOutLATxWork_M0_REV_POL1_P2(void);
void SetPulseOutLATxWork_M0_REV_POL1_OFF(void);

void SetPulseOutLATxWork_M1_FWD_POL0_P1(void);
void SetPulseOutLATxWork_M1_FWD_POL0_P2(void);
void SetPulseOutLATxWork_M1_FWD_POL0_OFF(void);
void SetPulseOutLATxWork_M1_FWD_POL1_P1(void);
void SetPulseOutLATxWork_M1_FWD_POL1_P2(void);
void SetPulseOutLATxWork_M1_FWD_POL1_OFF(void);
void SetPulseOutLATxWork_M1_REV_POL0_P1(void);
void SetPulseOutLATxWork_M1_REV_POL0_P2(void);
void SetPulseOutLATxWork_M1_REV_POL0_OFF(void);
void SetPulseOutLATxWork_M1_REV_POL1_P1(void);
void SetPulseOutLATxWork_M1_REV_POL1_P2(void);
void SetPulseOutLATxWork_M1_REV_POL1_OFF(void);

void SetPulseOutLATxWork_M0_SHAKE_POL0_P0(void);
void SetPulseOutLATxWork_M0_SHAKE_POL0_OFF(void);
void SetDataTRISxWork_M0_SPK_POL0_SHORT(void);
void SetDataTRISxWork_M0_SPK_POL0_OPEN(void);
void SetPulseOutLATxWork_M0_SHAKE_POL1_P0(void);
void SetPulseOutLATxWork_M0_SHAKE_POL1_OFF(void);
void SetDataTRISxWork_M0_SPK_POL1_SHORT(void);
void SetDataTRISxWork_M0_SPK_POL1_OPEN(void);

void SetPulseOutLATxWork_M1_SHAKE_POL0_P0(void);
void SetPulseOutLATxWork_M1_SHAKE_POL0_OFF(void);
void SetDataTRISxWork_M1_SPK_POL0_SHORT(void);
void SetDataTRISxWork_M1_SPK_POL0_OPEN(void);
void SetPulseOutLATxWork_M1_SHAKE_POL1_P0(void);
void SetPulseOutLATxWork_M1_SHAKE_POL1_OFF(void);
void SetDataTRISxWork_M1_SPK_POL1_SHORT(void);
void SetDataTRISxWork_M1_SPK_POL1_OPEN(void);

void ComparatorOff(UB motor_no);
void ComparatorOn(UB motor_no);
void VrsCheck(UB motor_no);

void InitPositionSearch(UB motor_no);
void AfterDetectedLoadPosition(UB motor_no);
void ActivateHandPosition(UB motor_no);
void MoveTargetPosition(UB motor_no);
void DetermineBasePosition(UB motor_no);
void UpdataHandPosition(UB motor_no);
void SearchFinePosition(UB motor_no);
void SaveLoadPosition(UB motor_no);
void PriorUpdateCorrentPosition(UB motor_no);
void VerifyHandPosition_LoadPosition(UB motor_no);

void CorrectPosition(H *pos, H max);
void CorrectHandPositionDrive(UB motor_no);
void UpdateCurrentPositionByFinePosition(UB motor_no);

//++++++++++++++++++++++++++Touch Read++++++++++++++++++++++++++++++++++++++
void SpkComparatorOff(void);
void SpkComparatorOn(void);
void SpkVrsCheck(void);
void ExcitatingOutOff(void);
void ExcitatingOutOn(void);
void SetDataTRISxWork_SPK_OPEN(void);
void SetDataTRISxWork_SPK_SHORT(void);
void SetOutPutLATX(void);
void SetExcitatingOutOffData(void);
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//.テーブルデータ
#if 0
static UB TblM0PulseChangePosition[ PULSE_CHANGE_ARRAY_LENGTH ];
static UB TblM1PulseChangePosition[ PULSE_CHANGE_ARRAY_LENGTH ];
#endif
static const UB TblM0PulseChangePosition[ PULSE_CHANGE_ARRAY_LENGTH ] = {
    PULSE_CHANGE_START_P0, // P0開始タイミング		0
    PULSE_CHANGE_STOP_P0, // P0終了タイミング     1
    PULSE_CHANGE_START_WAITE, // SPK1開始タイミング   2
    PULSE_CHANGE_START_SPK_ON_1, // SPK1終了タイミング   3
    PULSE_CHANGE_START_SPK_OFF_1, // SPK2開始タイミング   4
    PULSE_CHANGE_START_SPK_ON_2, // SPK2終了タイミング   5
    PULSE_CHANGE_START_SPK_OFF_2, // SPK3開始タイミング   6
    PULSE_CHANGE_START_SPK_ON_3, // SPK3終了タイミング   7
    PULSE_CHANGE_START_SPK_OFF_3, // SPK4開始タイミング   8
    PULSE_CHANGE_START_SPK_ON_4, // SPK4終了タイミング   9
    PULSE_CHANGE_START_SPK_OFF_4, // SPK5開始タイミング   10
    PULSE_CHANGE_START_SPK_ON_5, // SPK5終了タイミング   11
    PULSE_CHANGE_START_SPK_OFF_5, // SPK6開始タイミング   12
    PULSE_CHANGE_START_SPK_ON_6, // SPK6終了タイミング   13
    PULSE_CHANGE_START_SPK_OFF_6, // SPK7開始タイミング   14
    PULSE_CHANGE_START_SPK_ON_7, // SPK7終了タイミング   15
    PULSE_CHANGE_START_SPK_OFF_7, // SPK8開始タイミング   16
    PULSE_CHANGE_START_SPK_ON_8, // SPK8終了タイミング   17
    PULSE_CHANGE_START_SPK_OFF_8, // SPK9開始タイミング   18
    PULSE_CHANGE_START_SPK_ON_9, // SPK9終了タイミング   19
    PULSE_CHANGE_START_SPK_OFF_9, // SPK10開始タイミング  20
    PULSE_CHANGE_START_SPK_ON_10, // SPK10終了タイミング  21
    PULSE_CHANGE_START_SPK_OFF_10, // P1開始タイミング     22
    PULSE_CHANGE_START_P1, // P2開始タイミング     23
    PULSE_CHANGE_START_P2 // OFF開始タイミング    24
};
//------------------------------------------------------------------------------
static const UB TblM1PulseChangePosition[ PULSE_CHANGE_ARRAY_LENGTH ] = {
    PULSE_CHANGE_START_P0, // P0開始タイミング		0
    PULSE_CHANGE_STOP_P0, // P0終了タイミング     1
    PULSE_CHANGE_START_WAITE, // SPK1開始タイミング   2
    PULSE_CHANGE_START_SPK_ON_1, // SPK1終了タイミング   3
    PULSE_CHANGE_START_SPK_OFF_1, // SPK2開始タイミング   4
    PULSE_CHANGE_START_SPK_ON_2, // SPK2終了タイミング   5
    PULSE_CHANGE_START_SPK_OFF_2, // SPK3開始タイミング   6
    PULSE_CHANGE_START_SPK_ON_3, // SPK3終了タイミング   7
    PULSE_CHANGE_START_SPK_OFF_3, // SPK4開始タイミング   8
    PULSE_CHANGE_START_SPK_ON_4, // SPK4終了タイミング   9
    PULSE_CHANGE_START_SPK_OFF_4, // SPK5開始タイミング   10
    PULSE_CHANGE_START_SPK_ON_5, // SPK5終了タイミング   11
    PULSE_CHANGE_START_SPK_OFF_5, // SPK6開始タイミング   12
    PULSE_CHANGE_START_SPK_ON_6, // SPK6終了タイミング   13
    PULSE_CHANGE_START_SPK_OFF_6, // SPK7開始タイミング   14
    PULSE_CHANGE_START_SPK_ON_7, // SPK7終了タイミング   15
    PULSE_CHANGE_START_SPK_OFF_7, // SPK8開始タイミング   16
    PULSE_CHANGE_START_SPK_ON_8, // SPK8終了タイミング   17
    PULSE_CHANGE_START_SPK_OFF_8, // SPK9開始タイミング   18
    PULSE_CHANGE_START_SPK_ON_9, // SPK9終了タイミング   19
    PULSE_CHANGE_START_SPK_OFF_9, // SPK10開始タイミング  20
    PULSE_CHANGE_START_SPK_ON_10, // SPK10終了タイミング  21
    PULSE_CHANGE_START_SPK_OFF_10, // P1開始タイミング     22
    PULSE_CHANGE_START_P1, // P2開始タイミング     23
    PULSE_CHANGE_START_P2 // OFF開始タイミング    24
};
//------------------------------------------------------------------------------
//.変数
//.その他
UB motorBitOperation;
UB motorNoOperation;
//..駆動パルス生成
static UB TblM0PulseChangePositionLast;
static UB TblM1PulseChangePositionLast;

static UB pulseOutLATA; // モータ端子からpulse出力するときのポートデータ
static UB pulseOutLATB;
static UB pulseOutLATC;

static UB pulseOutLATAWork_M0; // M0モータ端子からpulse出力するときのポートデータ
static UB pulseOutLATBWork_M0;
static UB pulseOutLATCWork_M0;

static UB pulseOutLATAWork_M1; // M1モータ端子からpulse出力するときのポートデータ
static UB pulseOutLATBWork_M1;
static UB pulseOutLATCWork_M1;

static UB dataTRISA; // ポートの入出力設定データ（最終値）
static UB dataTRISB;
static UB dataTRISC;

static UB dataTRISAWork_M0; // ポートの入出力データWork
static UB dataTRISBWork_M0;
static UB dataTRISCWork_M0;

static UB dataTRISAWork_M1; // ポートの入出力データWork
static UB dataTRISBWork_M1;
static UB dataTRISCWork_M1;

static UB m0PulsePositionCt; // パルスインターバル終了までのカウント0〜255
static UB m1PulsePositionCt; // パルスインターバル終了までのカウント0〜255

static UB m0PulsePositionCtWork; // 上記のワーク
static UB m1PulsePositionCtWork; //

static UB m0PulseChangePosition; // pulsePositionCtがこの値のときに、pulseOutM0LATxを変更する
static UB m1PulseChangePosition; // pulsePositionCtがこの値のときに、pulseOutM1LATxを変更する

static void ( *m0PulseOutChangeFunc)(void) = NULL_PTR;
static void ( *m1PulseOutChangeFunc)(void) = NULL_PTR;

static UB m0PulseOutStep; // 0:P1出力、1:P2出力、2:LOW出力
static UB m1PulseOutStep; //

static UB m0PulseOutStepWork; // 上記のワーク
static UB m1PulseOutStepWork; //

static UB finishPositionCtSet[ MOTOR_NUM ]; // 駆動周波数に応じたmxPulsePositionCtの最後の値

static UB polarityBit; // 次に、または今出力する（している）モータ極性 0:次はCW1、1:次はCW2

//++++++++++++++++++++++++++Touch Read++++++++++++++++++++++++++++++++++++++
//..触読対応
static UB excitatingEnableBit; // Vrs検出したときに励磁させるかさせないか		0:励磁なし、1:励磁あり
static UH excitatingTime; // 励磁時間カウント
static UB excitatingOffMaskTime; // 励磁OFF後のマスク時間カウント
static UB detectSpkTurnBit; // 検出端子の切り替えフラグ 0:O4　1:O3

static UB spkPositionCt; // SPKパルスインターバル終了までのカウント0〜3
static UB spkChangePosition; // spkPositionCtがこの値の時出力変更準備する
static void ( *spkOutChangeFunc)(void) = NULL_PTR; //次に出力する関数へのポインタ

static UB spkOutTrisRequestFlug; // spk TRISxの変化必要な時の出力要求フラグ
static UB spkReqVrsCheckBit; // spk Vrsチェック要求フラグ

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//------------------------------------------------------------------------------
//..駆動制御
static UB driveType[ MOTOR_NUM ];
static UB driveDirectionBit; // 回転方向0:逆転、1:正転
static UB addShakePulseBit; // 揺動パルス追加　0:揺動パルス無し、1:揺動パルス有り
static UB reqAfterMotorStopFuncBit; // モータ駆動終了時に実行する処理
static UB reqStopMotorBit; // モータ駆動停止要求

static UB motorActiveBit; // モータ動作中ビット

static void ( * cbAfterMotorStopFunc[ MOTOR_NUM ])(UB motor_no);

static H motorStepCounterSet[ MOTOR_NUM ];
static H motorM0StepCounter;
static H motorM1StepCounter;

static UB correctDriveSteps[ MOTOR_NUM ]; // 補正駆動ステップ数
static UB correctDriveDirectionBit; // 補正駆動回転方向
static UB correctDriveFinishPositionCtSet[ MOTOR_NUM ]; // 駆動周波数に応じたmxPulsePositionCtの最後の値
//------------------------------------------------------------------------------
//..揺動パルス制御
static UB OutRequestFlug; // PulseChangePositionで変化必要な時の出力要求フラグ
static UB OutTrisRequestFlug; // TRISxの変化必要な時の出力要求フラグ

static UB vrsCountValue[ MOTOR_NUM ]; // Vcomp越えのVrs本数カウント値 IFでカウント

static UB reqComparatorOnBit; // コンパレータON要求フラグ
static UB reqComparatorOffBit; // コンパレータOFF要求フラグ
static UB reqVrsCheckBit; // Vrsチェック要求フラグ
//------------------------------------------------------------------------------
//..針位置検出
UB handPositionActiveBit; // 0:針位置無効→位置検出が未了、1:針位置有効→位置検出が終了
static UB reqAfterDetectLoadFuncBit; // 駆動中に負荷発生したので針位置確認

static UB reqSearchBasePosBit; // 基準位置検出要求
UB reqCorrectBasePosBit; // 針位置補正要求
UB reqCorrectFinePoBit; // 微調整位置に基づいて行う位置補正要求
static UB reqCheckLoadPosBit; // 負荷位置確認要求

static H positionSearchCt[ MOTOR_NUM ]; // 検出動作カウント

UB actCorrectDriveBit; // 補正駆動実行中フラグ

static H currentPosition[ MOTOR_NUM ]; // 現在位置
static H targetPosition[ MOTOR_NUM ]; // 目標位置

static UB detectLoadPositionIndex[ MOTOR_NUM ];

static H detectLoadPosition[ MOTOR_NUM ][ MAX_LOAD_POSITION_ARRAY_LENGTH ]; // 82
static UB detectLoadPositionType[ MOTOR_NUM ][ MAX_LOAD_POSITION_ARRAY_LENGTH ]; // 42 206byte
static H *detectLoadPositionPtr[ MOTOR_NUM ];
static UB *detectLoadPositionTypePtr[ MOTOR_NUM ];

static H detectBasePosition[ MOTOR_NUM ]; // 見つけた基準位置（今の針位置カウンタ上、取り付けオフセット込）

static UB loadPositionIndex[ MOTOR_NUM ];
static H loadPosition[ MOTOR_NUM ][ MAX_LOAD_POSITION_ARRAY_LENGTH ]; // 82
static UB loadPositionType[ MOTOR_NUM ][ MAX_LOAD_POSITION_ARRAY_LENGTH ]; // 42

static UB detectFinePosCt[ MOTOR_NUM ]; // 微調整位置を見つけた回数(微調整が8箇所、粗調整が1箇所で成功)
static UB detectRoughPosCt[ MOTOR_NUM ]; // 粗調整位置を見つけた回数(微調整が8箇所、粗調整が1箇所で成功)
static UB detectLoadType[ MOTOR_NUM ]; // 今回見つけた負荷のタイプ
static UB detectLoadCont[ MOTOR_NUM ]; // 一つの判定の連続回数
static UB positionSearchStat[ MOTOR_NUM ]; // 位置検出状態

static H searchTotalSteps[ MOTOR_NUM ]; // 検出パルスを出力した回数

static H detectRoughTotalMove[ MOTOR_NUM ]; // 粗調整と判定したトータル移動量（粗調整位置の移動量＋次の半端な値が微調整間隔か確認する）
static const H offsetBasePosition[ MOTOR_NUM ] = {
    OFFSET_M0_BASE_POSITION,
    OFFSET_M1_BASE_POSITION
}; // 基準位置オフセット

static void ( * cbAfterDetectLoadFunc[ MOTOR_NUM ])(UB motor_no);

static UB doubleInterval;
static H missingDetectFinePosition;
//------------------------------------------------------------------------------
//.-------関数--------------
//.InitPulsePositionTable

/*******************************************************************************
 @brief     PulsePositionTable作成
 @note
 @author
 @date
 ********************************************************************************/
void InitPulsePositionTable(UB motor_no)
{
    if (MOTOR_M0 == motor_no) {
        TblM0PulseChangePositionLast = PULSE_CHANGE_OFF;
    } else {
        TblM1PulseChangePositionLast = PULSE_CHANGE_OFF;
    }

#if 0

    UB *ptr;
    UB set_counter;
    UB i;

    set_counter = 0;

    if (MOTOR_M0 == motor_no) {
        ptr = &TblM0PulseChangePosition[ 0 ];
    } else {
        ptr = &TblM1PulseChangePosition[ 0 ];
    }


    set_counter += POS_P0_START;
    *ptr = set_counter;
    ptr++;

    set_counter += WIDTH_PULSE_P0;
    *ptr = set_counter;
    ptr++;

    set_counter += WIDTH_SPK_START_WAIT;
    *ptr = set_counter;
    ptr++;

    for (i = 0; i < NUMBER_OF_REPEAT_SPK; i++) {
        set_counter += WIDTH_SPK_ON;
        *ptr = set_counter;
        ptr++;

        set_counter += WIDTH_SPK_OFF;
        *ptr = set_counter;
        ptr++;
    }

    set_counter += WIDTH_PULSE_P1;
    *ptr = set_counter;
    ptr++;

    set_counter += WIDTH_PULSE_P2;
    *ptr = set_counter;
    ptr++;

    set_counter += WIDTH_PULSE_OFF_DEF;
    *ptr = set_counter;
    ptr++;
#endif
}
//------------------------------------------------------------------------------
//.TMR2_Interrupt
//..TMR2_Interrupt
/***************************************************************************//**
 @brief     122us毎に割り込みで実行される
 @note
 @author
 @date
*******************************************************************************/

void TMR2_Interrupt(void)
{
#ifdef DEBUG_TIMER2
    LATC |= b0000_0001;
#endif // DEBUG_RC4_TIMER2

    if (OutTrisRequestFlug == ON) {
        OutTrisRequestFlug = OFF;

        /**-- 入出力設定 --**/
        /**-- 今のTRISxをdataTRISxに保存 --**/
        dataTRISA = TRISA;
        dataTRISB = TRISB;
        dataTRISC = TRISC;

        /**-- dataTRISxのモータ端子の箇所を0にする --**/
        dataTRISA &= (UB) (~(BIT_TRISA_M0_ALL | BIT_TRISA_M1_ALL));
        dataTRISB &= (UB) (~(BIT_TRISB_M0_ALL | BIT_TRISB_M1_ALL));
        dataTRISC &= (UB) (~(BIT_TRISC_M0_ALL | BIT_TRISC_M1_ALL));

        /**-- dataTRISxの必要なモータ端子の箇所を1にする --**/
        dataTRISA |= (UB) (dataTRISAWork_M0 | dataTRISAWork_M1);
        dataTRISB |= (UB) (dataTRISBWork_M0 | dataTRISBWork_M1);
        dataTRISC |= (UB) (dataTRISCWork_M0 | dataTRISCWork_M1);

        /**-- データセット --**/
        TRISA = dataTRISA;
        TRISB = dataTRISB;
        TRISC = dataTRISC;

    }
    if (OutRequestFlug == ON) {
        OutRequestFlug = OFF;

        /**-- パルス出力 --**/
        /**-- 今のLATxをpulseOutLATxに保存 --**/
        pulseOutLATA = LATA;
        pulseOutLATB = LATB;
        pulseOutLATC = LATC;

        /**-- pulseOutLATxのモータ端子の箇所を0にする --**/
        pulseOutLATA &= (UB) (~(BIT_PORTA_M0_ALL | BIT_PORTA_M1_ALL));
        pulseOutLATB &= (UB) (~(BIT_PORTB_M0_ALL | BIT_PORTB_M1_ALL));
        pulseOutLATC &= (UB) (~(BIT_PORTC_M0_ALL | BIT_PORTC_M1_ALL));

        /**-- pulseOutLATxの必要なモータ端子の箇所を1にする --**/
        pulseOutLATA |= (UB) (pulseOutLATAWork_M0 | pulseOutLATAWork_M1);
        pulseOutLATB |= (UB) (pulseOutLATBWork_M0 | pulseOutLATBWork_M1);
        pulseOutLATC |= (UB) (pulseOutLATCWork_M0 | pulseOutLATCWork_M1);

        /**-- 出力 --**/
        LATA = pulseOutLATA;
        LATB = pulseOutLATB;
        LATC = pulseOutLATC;
    }

#ifdef DEBUG_TIMER2
    LATC &= ~b0000_0001;
#endif // DEBUG_RC4_TIMER2

    /**-- Vrsカウント 要求チェック --*/
    if (reqVrsCheckBit & BIT_MOTOR_M0) {
        reqVrsCheckBit &= ~BIT_MOTOR_M0;
        VrsCheck(MOTOR_M0); //M0 Vrs本数カウント
    }
    if (reqVrsCheckBit & BIT_MOTOR_M1) {
        reqVrsCheckBit &= ~BIT_MOTOR_M1;
        VrsCheck(MOTOR_M1); //M1 Vrs本数カウント
    }
    /**-- コンパレータON　要求チェック --**/
    if (reqComparatorOnBit & BIT_MOTOR_M0) {
        reqComparatorOnBit &= ~BIT_MOTOR_M0;
        ComparatorOn(MOTOR_M0); //M0用コンパレータON
    }
    if (reqComparatorOnBit & BIT_MOTOR_M1) {
        reqComparatorOnBit &= ~BIT_MOTOR_M1;
        ComparatorOn(MOTOR_M1); //M1用コンパレータON
    }
    /**-- コンパレータOFF 要求チェック --**/
    if (reqComparatorOffBit & BIT_MOTOR_M0) {
        reqComparatorOffBit &= ~BIT_MOTOR_M0;
        ComparatorOff(MOTOR_M0); //M0用コンパレータOFF
    }
    if (reqComparatorOffBit & BIT_MOTOR_M1) {
        reqComparatorOffBit &= ~BIT_MOTOR_M1;
        ComparatorOff(MOTOR_M1); //M1用コンパレータOFF
    }

    if (motorActiveBit & BIT_MOTOR_M0) {

#ifdef DEBUG_TIMER2
        LATC |= b0000_0001;
#endif // DEBUG_RC4_TIMER2

        m0PulsePositionCt++;
        if (m0PulsePositionCt == m0PulseChangePosition) {

            /**-- 変更するタイミング --**/
            m0PulseOutStep++;

            if (m0PulseOutStep >= ID_PULSE_OUT_STEP_MAX) {
                /**-- 1cycle終了 --**/
                if (0 == (actCorrectDriveBit & BIT_MOTOR_M0)) {
                    if ((reqSearchBasePosBit | reqCorrectBasePosBit | reqCheckLoadPosBit) & BIT_MOTOR_M0) {
                        /**-- 基準位置検出中 or 針位置補正駆動中 or 位置確認移動中--**/
                        positionSearchCt[ MOTOR_M0 ]++;

                        if ((0 == vrsCountValue[ MOTOR_M0 ])
                                || (positionSearchCt[ MOTOR_M0 ] >= TblMaxSteps[ MOTOR_M0 ])) {
                            motorActiveBit &= (UB) (~BIT_MOTOR_M0);
                            reqAfterDetectLoadFuncBit |= BIT_MOTOR_M0;
#ifdef DEBUG_VRS0
                            LATA |= b0000_0100;
#endif
                        }
                    }
                }

                if (motorM0StepCounter) {
                    /**-- 任意ステップ数駆動中 --**/
                    motorM0StepCounter--;
                    if (0 >= motorM0StepCounter) {
                        actCorrectDriveBit &= (UB) (~BIT_MOTOR_M0);
                        motorActiveBit &= (UB) (~BIT_MOTOR_M0);
                        reqAfterMotorStopFuncBit |= BIT_MOTOR_M0;
                        motorM0StepCounter = 0;
                    }
                }

                if (reqStopMotorBit & BIT_MOTOR_M0) {
                    reqStopMotorBit &= (UB) (~BIT_MOTOR_M0);
                    motorActiveBit &= (UB) (~BIT_MOTOR_M0);
                    reqAfterMotorStopFuncBit |= BIT_MOTOR_M0;
                }

                m0PulsePositionCt = m0PulsePositionCtWork; // 振り子パルス有り無しで変わる
                m0PulseOutStep = m0PulseOutStepWork; // 振り子パルス有り無しで変わる
                vrsCountValue[ MOTOR_M0 ] = 0; //Vrs本数カウンタクリア

            } else {
                /**-- 1cycle以内なら --**/
                if (m0PulseOutChangeFunc) { // NULL_POINTER確認
                    m0PulseOutChangeFunc(); // 値を変更する各関数の実行
                }
            }

            if (m0PulseOutStep < (ID_PULSE_OUT_STEP_MAX - 1)) {
                m0PulseChangePosition = TblM0PulseChangePosition[ m0PulseOutStep ];
            } else {
                m0PulseChangePosition = TblM0PulseChangePositionLast;
            }
        }

#ifdef DEBUG_TIMER2
        LATC &= ~b0000_0001;
#endif // DEBUG_RC4_TIMER2
    }

    if (motorActiveBit & BIT_MOTOR_M1) {

#ifdef DEBUG_TIMER2
        LATC |= b0000_0001;
#endif // DEBUG_RC4_TIMER2

        m1PulsePositionCt++;
        if (m1PulsePositionCt == m1PulseChangePosition) {

            /**-- 変更するタイミング --**/
            m1PulseOutStep++;

            if (m1PulseOutStep >= ID_PULSE_OUT_STEP_MAX) {
                /**-- 1cycle終了 --**/
                if (0 == (actCorrectDriveBit & BIT_MOTOR_M1)) {
                    if ((reqSearchBasePosBit | reqCorrectBasePosBit | reqCheckLoadPosBit) & BIT_MOTOR_M1) {
                        /**-- 基準位置検出中 or 針位置補正駆動中 or 位置確認移動中--**/
                        positionSearchCt[ MOTOR_M1 ]++;

                        if ((0 == vrsCountValue[ MOTOR_M1 ])
                                || (positionSearchCt[ MOTOR_M1 ] >= TblMaxSteps[ MOTOR_M1 ])) {
                            motorActiveBit &= (UB) (~BIT_MOTOR_M1);
                            reqAfterDetectLoadFuncBit |= BIT_MOTOR_M1;
#ifdef DEBUG_VRS0
                            LATA |= b0000_0100;
#endif
                        }
                    }
                }

                if (motorM1StepCounter) {
                    /**-- 任意ステップ数駆動中 --**/
                    motorM1StepCounter--;
                    if (0 >= motorM1StepCounter) {
                        actCorrectDriveBit &= (UB) (~BIT_MOTOR_M1);
                        motorActiveBit &= (UB) (~BIT_MOTOR_M1);
                        reqAfterMotorStopFuncBit |= BIT_MOTOR_M1;
                        motorM1StepCounter = 0;
                    }
                }

                if (reqStopMotorBit & BIT_MOTOR_M1) {
                    reqStopMotorBit &= (UB) (~BIT_MOTOR_M1);
                    motorActiveBit &= (UB) (~BIT_MOTOR_M1);
                    reqAfterMotorStopFuncBit |= BIT_MOTOR_M1;
                }

                m1PulsePositionCt = m1PulsePositionCtWork; // 振り子パルス有り無しで変わる
                m1PulseOutStep = m1PulseOutStepWork; // 振り子パルス有り無しで変わる
                vrsCountValue[ MOTOR_M1 ] = 0; //Vrs本数カウンタクリア

            } else {
                /**-- 1cycle以内なら --**/
                if (m1PulseOutChangeFunc) { // NULL_POINTER確認
                    m1PulseOutChangeFunc(); // 値を変更する各関数の実行
                }
            }

            if (m1PulseOutStep < (ID_PULSE_OUT_STEP_MAX - 1)) {
                m1PulseChangePosition = TblM1PulseChangePosition[ m1PulseOutStep ];
            } else {
                m1PulseChangePosition = TblM1PulseChangePositionLast;
            }
        }

#ifdef DEBUG_TIMER2
        LATC &= ~b0000_0001;
#endif // DEBUG_RC4_TIMER2
    }

#ifdef DEBUG_VRS0
    LATA &= ~b0000_0100;
#endif

    if (0 == motorActiveBit) {
        TMR2_StopTimer();
    }
}
//++++++++++++++++++++++++++Touch Read++++++++++++++++++++++++++++++++++++++
//..TMR2_Interrupt_SPK
/***************************************************************************//**
 @brief     SPK出力とVRS検出したら励磁する専用処理
 @note		122us毎に割り込みで実行される
 @author
 @date
*******************************************************************************/

void TMR2_Interrupt_SPK(void)
{
    if (spkOutTrisRequestFlug == ON) {//TRISXの出力要請の有無確認
        spkOutTrisRequestFlug = OFF;

        /**-- 入出力設定 --**/
        /**-- 今のTRISxをdataTRISxに保存 --**/
        dataTRISA = TRISA;
        dataTRISB = TRISB;
        dataTRISC = TRISC;

        /**-- dataTRISxのモータ端子の箇所を0にする --**/
        //検出に関する端子　O3,O4,VDのみ
        dataTRISA &= (UB) (~(BIT_TRISA_M0_ALL | BIT_TRISA_M1_ALL));
        dataTRISB &= (UB) (~(BIT_TRISB_M0_ALL | BIT_TRISB_M1_ALL));
        dataTRISC &= (UB) (~(BIT_TRISC_M0_ALL | BIT_TRISC_M1_ALL));

        /**-- dataTRISxの必要なモータ端子の箇所を1にする --**/
        dataTRISA |= (UB) (dataTRISAWork_M0 | dataTRISAWork_M1);
        dataTRISB |= (UB) (dataTRISBWork_M0 | dataTRISBWork_M1);
        dataTRISC |= (UB) (dataTRISCWork_M0 | dataTRISCWork_M1);

        /**-- データセット --**/
        TRISA = dataTRISA;
        TRISB = dataTRISB;
        TRISC = dataTRISC;

    }

#ifdef TOUCH_TEST

#else    
    /**-- 励磁OFF時のマスク時間チェック--**/
    if (excitatingOffMaskTime > 0) {//励磁OFF後のマスクタイム時間セットされているなら

        excitatingOffMaskTime--;
        PIR2bits.C1IF = 0; //マスク期間のVrsを無効に
        PIR2bits.C2IF = 0;


    }

    /**-- Vrsチェック要求確認 --**/
    if ((spkReqVrsCheckBit & BIT_MOTOR_M0) || (spkReqVrsCheckBit & BIT_MOTOR_M1)) {
        spkReqVrsCheckBit &= (UB) (~(BIT_MOTOR_M0 | BIT_MOTOR_M1));

        if (excitatingOffMaskTime == 0) {//マスク時間終了しているなら

            SpkVrsCheck(); //M0,M1 Vrsチェック
        }

    }

    /**-- 励磁ON時間チェック --**/
    if (excitatingTime > 0) {//励磁ON時間セットされているなら

        excitatingTime--;

        if (excitatingTime == 0) {

            ExcitatingOutOff(); //励磁OFF関数　LATAX,TRISX設定 excitatingOffMaskTimeセット

        }
    }
#endif

    /**-- SPKパルスポジションチェック --**/
    spkPositionCt++;

    if (spkPositionCt == 4) {

        spkPositionCt = 0;
    }

    if (spkPositionCt == spkChangePosition) {//TRISX OPEN/SHORT切り替え関数内で定義

        /**-- 変更するタイミング --**/
        if (spkOutChangeFunc) { // NULL_POINTER確認
            spkOutChangeFunc(); // 出力値を変更する各関数の実行　この中で、次の変化ポイントセット
        }
    }

}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//------------------------------------------------------------------------------
//.WatchMotorDriveFinish

/*******************************************************************************
 @brief     終了チェック
 @note
 @author
 @date
 ********************************************************************************/
void WatchMotorDriveFinish(void)
{
    void ( *func)(UB motor_no);

    if (reqAfterDetectLoadFuncBit & BIT_MOTOR_M0) {

        reqAfterDetectLoadFuncBit &= (UB) (~BIT_MOTOR_M0);

        if (cbAfterDetectLoadFunc[ MOTOR_M0 ]) {
            func = cbAfterDetectLoadFunc[ MOTOR_M0 ];
            cbAfterDetectLoadFunc[ MOTOR_M0 ] = NULL_PTR;
            func(MOTOR_M0);
        }
    }

    if (reqAfterDetectLoadFuncBit & BIT_MOTOR_M1) {

        reqAfterDetectLoadFuncBit &= (UB) (~BIT_MOTOR_M1);

        if (cbAfterDetectLoadFunc[ MOTOR_M1 ]) {
            func = cbAfterDetectLoadFunc[ MOTOR_M1 ];
            cbAfterDetectLoadFunc[ MOTOR_M1 ] = NULL_PTR;
            func(MOTOR_M1);
        }
    }

    if (reqAfterMotorStopFuncBit & BIT_MOTOR_M0) {

        reqAfterMotorStopFuncBit &= (UB) (~BIT_MOTOR_M0);

        if (cbAfterMotorStopFunc[ MOTOR_M0 ]) {
            func = cbAfterMotorStopFunc[ MOTOR_M0 ];
            cbAfterMotorStopFunc[ MOTOR_M0 ] = NULL_PTR;
            func(MOTOR_M0);
        }
    }

    if (reqAfterMotorStopFuncBit & BIT_MOTOR_M1) {

        reqAfterMotorStopFuncBit &= (UB) (~BIT_MOTOR_M1);

        if (cbAfterMotorStopFunc[ MOTOR_M1 ]) {
            func = cbAfterMotorStopFunc[ MOTOR_M1 ];
            cbAfterMotorStopFunc[ MOTOR_M1 ] = NULL_PTR;
            func(MOTOR_M1);
        }
    }
}
//------------------------------------------------------------------------------
//.PulseOut
/***************************************************************************//**
 @brief     パルス出力
 @note      TRISx       データ方向
            LATx        出力ラッチ
            PORTx       入力データ
 @author
 @date
*******************************************************************************/
//..StartPulseOutM0

void StartPulseOutM0(void)
{
    INTERRUPT_PeripheralInterruptDisable();

    if ((reqSearchBasePosBit & BIT_MOTOR_M0) || (motorStepCounterSet[ MOTOR_M0 ])) {

        TblM0PulseChangePositionLast = finishPositionCtSet[ MOTOR_M0 ];

        if (driveDirectionBit & BIT_MOTOR_M0) {
            /**-- 正転 --**/
            if (addShakePulseBit & BIT_MOTOR_M0) {
                /**-- 揺動パルス有り --**/
                if (polarityBit & BIT_MOTOR_M0) {
                    /**-- 極性1 --**/
                    m0PulseOutChangeFunc = SetPulseOutLATxWork_M0_SHAKE_POL1_P0;
                } else {
                    /**-- 極性0 --**/
                    m0PulseOutChangeFunc = SetPulseOutLATxWork_M0_SHAKE_POL0_P0;
                }

                m0PulsePositionCtWork = POS_SHAKE_INIT_DATA;
                m0PulseOutStepWork = OUTSTEP_SHAKE_INIT_DATA;

            } else {
                /**-- 揺動パルス無し --**/
                if (polarityBit & BIT_MOTOR_M0) {
                    /**-- 極性1 --**/
                    m0PulseOutChangeFunc = SetPulseOutLATxWork_M0_FWD_POL1_P1;
                } else {
                    /**-- 極性0 --**/
                    m0PulseOutChangeFunc = SetPulseOutLATxWork_M0_FWD_POL0_P1;
                }

                m0PulsePositionCtWork = POS_P1P2_INIT_DATA;
                m0PulseOutStepWork = OUTSTEP_P1P2_INIT_DATA;

            }
        } else {
            /**-- 逆転 --**/
            if (polarityBit & BIT_MOTOR_M0) {
                /**-- 極性1 --**/
                m0PulseOutChangeFunc = SetPulseOutLATxWork_M0_REV_POL1_P1;
            } else {
                /**-- 極性0 --**/
                m0PulseOutChangeFunc = SetPulseOutLATxWork_M0_REV_POL0_P1;
            }

            m0PulsePositionCtWork = POS_P1P2_INIT_DATA;
            m0PulseOutStepWork = OUTSTEP_P1P2_INIT_DATA;

        }

        motorActiveBit |= BIT_MOTOR_M0;
        m0PulsePositionCt = m0PulsePositionCtWork;
        m0PulseOutStep = m0PulseOutStepWork;
        m0PulseChangePosition = TblM0PulseChangePosition[ m0PulseOutStep ];

        motorM0StepCounter = motorStepCounterSet[ MOTOR_M0 ];
        motorStepCounterSet[ MOTOR_M0 ] = 0;

        TMR2_StartTimer();

    } else {

        actCorrectDriveBit &= (UB) (~BIT_MOTOR_M0);

    }

    INTERRUPT_PeripheralInterruptEnable();
}
//------------------------------------------------------------------------------
//..StartPulseOutM1

void StartPulseOutM1(void)
{
    INTERRUPT_PeripheralInterruptDisable();

    if ((reqSearchBasePosBit & BIT_MOTOR_M1) || (motorStepCounterSet[ MOTOR_M1 ])) {

        TblM1PulseChangePositionLast = finishPositionCtSet[ MOTOR_M1 ];

        if (driveDirectionBit & BIT_MOTOR_M1) {
            /**-- 正転 --**/
            if (addShakePulseBit & BIT_MOTOR_M1) {
                /**-- 揺動パルス有り --**/
                if (polarityBit & BIT_MOTOR_M1) {
                    /**-- 極性1 --**/
                    m1PulseOutChangeFunc = SetPulseOutLATxWork_M1_SHAKE_POL1_P0;
                } else {
                    /**-- 極性0 --**/
                    m1PulseOutChangeFunc = SetPulseOutLATxWork_M1_SHAKE_POL0_P0;
                }

                m1PulsePositionCtWork = POS_SHAKE_INIT_DATA;
                m1PulseOutStepWork = OUTSTEP_SHAKE_INIT_DATA;



            } else {
                /**-- 揺動パルス無し --**/
                if (polarityBit & BIT_MOTOR_M1) {
                    /**-- 極性1 --**/
                    m1PulseOutChangeFunc = SetPulseOutLATxWork_M1_FWD_POL1_P1;
                } else {
                    /**-- 極性0 --**/
                    m1PulseOutChangeFunc = SetPulseOutLATxWork_M1_FWD_POL0_P1;
                }

                m1PulsePositionCtWork = POS_P1P2_INIT_DATA;
                m1PulseOutStepWork = OUTSTEP_P1P2_INIT_DATA;

            }
        } else {
            /**-- 逆転 --**/
            if (polarityBit & BIT_MOTOR_M1) {
                /**-- 極性1 --**/
                m1PulseOutChangeFunc = SetPulseOutLATxWork_M1_REV_POL1_P1;
            } else {
                /**-- 極性0 --**/
                m1PulseOutChangeFunc = SetPulseOutLATxWork_M1_REV_POL0_P1;
            }

            m1PulsePositionCtWork = POS_P1P2_INIT_DATA;
            m1PulseOutStepWork = OUTSTEP_P1P2_INIT_DATA;

        }

        motorActiveBit |= BIT_MOTOR_M1;
        m1PulsePositionCt = m1PulsePositionCtWork;
        m1PulseOutStep = m1PulseOutStepWork;
        m1PulseChangePosition = TblM1PulseChangePosition[ m1PulseOutStep ];

        motorM1StepCounter = motorStepCounterSet[ MOTOR_M1 ];
        motorStepCounterSet[ MOTOR_M1 ] = 0;

        TMR2_StartTimer();

    } else {

        actCorrectDriveBit &= (UB) (~BIT_MOTOR_M1);

    }

    INTERRUPT_PeripheralInterruptEnable();
}
//++++++++++++++++++++++++++Touch Read++++++++++++++++++++++++++++++++++++++
//.Start-StopSPK
//..SetMotorStartSPK
/***************************************************************************//**
 @brief		SPK出力開始
 @note     ショート状態でスタート、コンパレータON、TurnはO4（カラ打ち）
 @author
 @date
*******************************************************************************/

void SetMotorStartSPK(void)
{
    /**-- 割り込み関数をSPK出力に変更する --**/
    TMR2_SetInterruptHandler(TMR2_Interrupt_SPK);
    // clear the TMR2 interrupt flag
    PIR1bits.TMR2IF = 0;

    detectSpkTurnBit = 0x00; //検出端子をO4側へ初期設定(ただし、これはカラ打ち)

    /* まずは、検出ショートにする*/
    dataTRISAWork_M0 = BIT_TRISA_M0_SHT;
    dataTRISBWork_M0 = BIT_TRISB_M0_SHT;
    dataTRISCWork_M0 = BIT_TRISC_M0_SHT;

    dataTRISAWork_M1 = BIT_TRISA_M1_SHT;
    dataTRISBWork_M1 = BIT_TRISB_M1_SHT;
    dataTRISCWork_M1 = BIT_TRISC_M1_SHT;

    spkOutTrisRequestFlug = ON;     //TRISXの出力要請

    /*コンパレータON ONのみで端子選択はしない*/
    SpkComparatorOn();

    spkPositionCt = 0; //ポジションカウンタを初期値へ
    spkChangePosition = CHANGE_POSITION_OPEN; //OPEN位置からスタート、ただしショートしている

    spkOutChangeFunc = SetDataTRISxWork_SPK_SHORT; //最初はSHORT関数から

#ifdef TOUCH_TEST
    ExcitatingOutOn();      //トルク測定テストのため、常に励磁ONとする
#endif
    /**-- Timer2割り込みスタート --**/
    TMR2_StartTimer();
}
//++++++++++++++++++++++++++Touch Read++++++++++++++++++++++++++++++++++++++
//..SetMotorStopSpk
/***************************************************************************//**
 @brief		SPK出力停止
 @note     励磁時間カウンタ　マスクカウンタクリア、コンパレータOFF
 @author
 @date
*******************************************************************************/

void SetMotorStopSPK(void)
{
    /**-- タイマー2ストップ --**/
    TMR2_StopTimer();
    // clear the TMR2 interrupt flag
    PIR1bits.TMR2IF = 0;

    SpkComparatorOff();
    excitatingTime = 0; //励磁時間カウンタクリア
    excitatingOffMaskTime = 0;

    SetExcitatingOutOffData(); //励磁パルスが出ていた場合強制OFF

    /**-- 割り込み関数を従来のモータパルス出力に戻す --**/
    TMR2_SetInterruptHandler(TMR2_Interrupt);
}
//++++++++++++++++++++++++++Touch Read++++++++++++++++++++++++++++++++++++++
//..SetMotorEnableExcitating
/***************************************************************************//**
 @brief		励磁有効設定
 @note
 @author
 @date
*******************************************************************************/

void SetMotorEnableExcitating(UB motor_no)
{
    motorBitOperation = (UB) (0x01 << motor_no);

    excitatingEnableBit |= (UB) (motorBitOperation);
}
//++++++++++++++++++++++++++Touch Read++++++++++++++++++++++++++++++++++++++
//..SetMotorDisableExcitating
/***************************************************************************//**
 @brief		励磁無効設定
 @note
 @author
 @date
*******************************************************************************/

void SetMotorDisableExcitating(UB motor_no)
{
    motorBitOperation = (UB) (0x01 << motor_no);

    excitatingEnableBit &= (UB) (~motorBitOperation);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//.駆動設定
//..SetDriveType

/*******************************************************************************
 @brief		モータ駆動設定
 @note

            drive_type

            DRIVE_SIMPLE					任意方向
            DRIVE_SEARCH_BASE_POSITION		正転のみ
            DRIVE_CHECK_POSITION			正転のみ
            DRIVE_CORRECT_BASE_POSITION		正転のみ
            DRIVE_ACT_CORRECT_POSITION		正転のみ	所定ステップ数、所定方向
 @author
 @date
 ********************************************************************************/
void SetDriveType(UB motor_no, UB drive_type)
{

    motorBitOperation = (UB) (0x01 << motor_no);

    driveType[ motor_no ] = drive_type;

    switch (drive_type) {

    case DRIVE_SIMPLE:
        addShakePulseBit &= (UB) (~motorBitOperation);
        reqSearchBasePosBit &= (UB) (~motorBitOperation);

        reqCheckLoadPosBit &= (UB) (~motorBitOperation);
        reqCorrectBasePosBit &= (UB) (~motorBitOperation);
        reqCorrectFinePoBit &= (UB) (~motorBitOperation);
        actCorrectDriveBit &= (UB) (~motorBitOperation);
        break;

    case DRIVE_SEARCH_BASE_POSITION:
    case DRIVE_SEARCH_1008_POSITION:
        addShakePulseBit |= (UB) (motorBitOperation);
        reqSearchBasePosBit |= (UB) (motorBitOperation);

        reqCheckLoadPosBit &= (UB) (~motorBitOperation);
        reqCorrectBasePosBit &= (UB) (~motorBitOperation);
        reqCorrectFinePoBit &= (UB) (~motorBitOperation);
        actCorrectDriveBit &= (UB) (~motorBitOperation);
        break;

    case DRIVE_CHECK_POSITION:
        addShakePulseBit |= (UB) (motorBitOperation);
        reqSearchBasePosBit &= (UB) (~motorBitOperation);

        reqCheckLoadPosBit |= (UB) (motorBitOperation);
        //reqCorrectBasePosBit&= ( UB )( ~motorBitOperation );
        //reqCorrectFinePoBit &= ( UB )( ~motorBitOperation );
        //actCorrectDriveBit  &= ( UB )( ~motorBitOperation );
        break;

    case DRIVE_CORRECT_BASE_POSITION:
        addShakePulseBit |= (UB) (motorBitOperation);
        reqSearchBasePosBit &= (UB) (~motorBitOperation);

        //reqCheckLoadPosBit  &= ( UB )( ~motorBitOperation );
        reqCorrectBasePosBit |= (UB) (motorBitOperation);
        //reqCorrectFinePoBit &= ( UB )( ~motorBitOperation );
        //actCorrectDriveBit  &= ( UB )( ~motorBitOperation );
        break;

    case DRIVE_ACT_CORRECT_POSITION:
        addShakePulseBit &= (UB) (~motorBitOperation);
        reqSearchBasePosBit &= (UB) (~motorBitOperation);

        //reqCheckLoadPosBit  &= ( UB )( ~motorBitOperation );
        //reqCorrectBasePosBit&= ( UB )( ~motorBitOperation );
        //reqCorrectFinePoBit &= ( UB )( ~motorBitOperation );
        actCorrectDriveBit |= (UB) (motorBitOperation);

    default:
        break;
    };

}
//------------------------------------------------------------------------------
//..SetSearchBasePosition

void SetSearchBasePosition(UB motor_no, UB sw)
{
    motorBitOperation = (UB) (0x01 << motor_no);

    if (sw) {

        reqSearchBasePosBit |= (UB) (motorBitOperation);

    } else {

        reqSearchBasePosBit &= (UB) (~motorBitOperation);

    }
}
//------------------------------------------------------------------------------
//..SetCheckLoadPosition

void SetCheckLoadPosition(UB motor_no, UB sw)
{
    motorBitOperation = (UB) (0x01 << motor_no);

    if (sw && (handPositionActiveBit & motorBitOperation)) {

        reqCheckLoadPosBit |= (UB) (motorBitOperation);

    } else {

        reqCheckLoadPosBit &= (UB) (~motorBitOperation);

    }
}
//------------------------------------------------------------------------------
//..SetAfterMotorStopFunc
/***************************************************************************//**
 @brief     駆動停止後の処理の登録
 @note
 @author
 @date
*******************************************************************************/

void SetAfterMotorStopFunc(UB motor_no, void ( * func)(UB motor_no))
{
    cbAfterMotorStopFunc[ motor_no ] = func;
}
//------------------------------------------------------------------------------
//..SetAfterDetectLoadFunc
/***************************************************************************//**
 @brief     負荷検出後の処理の登録
 @note
 @author
 @date
*******************************************************************************/

void SetAfterDetectLoadFunc(UB motor_no, void ( * func)(UB motor_no))
{
    cbAfterDetectLoadFunc[ motor_no ] = func;
}
//------------------------------------------------------------------------------
//..GetAfterDetectLoadFunc

void *GetAfterDetectLoadFunc(UB motor_no)
{
    return cbAfterDetectLoadFunc[ motor_no ];
}
//------------------------------------------------------------------------------
//..SetMotorDirection
/***************************************************************************//**
 @brief     回転方向設定
 @note
 @author
 @date
*******************************************************************************/

void SetMotorDirection(UB motor_no, UB dir)
{
    motorBitOperation = (UB) (0x01 << motor_no);

    if (FOWARD == dir) {

        driveDirectionBit |= motorBitOperation;

    } else {

        driveDirectionBit &= (UB) (~motorBitOperation);

    }

}
//------------------------------------------------------------------------------
//..SetMotorStepCounter
/***************************************************************************//**
 @brief     駆動パルス数設定
 @note
 @author
 @date
*******************************************************************************/

void SetMotorStepCounter(UB motor_no, H steps)
{
    motorStepCounterSet[ motor_no ] = steps;

    PriorUpdateCorrentPosition(motor_no);
}
//------------------------------------------------------------------------------
//..CalcMotorFwdStepCounter

/*******************************************************************************
 @brief     現在値と目標値にもどついて駆動ステップ数を計算する
 @note		正転でのステップ数にする
 @author
 @date
 ********************************************************************************/
void CalcMotorFwdStepCounter(UB motor_no)
{
    H steps;

    /**-- 正転での駆動パルス数算出 --**/
    SetMotorDirection(motor_no, FOWARD);
    steps = targetPosition[ motor_no ] - currentPosition[ motor_no ];

    while (steps < 0) {
        steps += TblMaxSteps[ motor_no ];
    }

    motorStepCounterSet[ motor_no ] = steps;

    PriorUpdateCorrentPosition(motor_no);
}
//------------------------------------------------------------------------------
//..CalcMotorStepCounter
/***************************************************************************//**
 @brief		回転方向＆駆動パルス数セット
 @note		残パルス数を加味する
 @author
 @date
*******************************************************************************/

void CalcMotorStepCounter(UB motor_no)
{
    H steps;
    UB motor_bit = (UB) (0x01 << motor_no);

    /**-- 残パルスで現在位置更新 --**/
    if (MOTOR_M0 == motor_no) {
        if (driveDirectionBit & motor_bit) {
            currentPosition[ motor_no ] -= motorM0StepCounter;
        } else {
            currentPosition[ motor_no ] += motorM0StepCounter;
        }
        motorM0StepCounter = 0;
    } else {
        if (driveDirectionBit & motor_bit) {
            currentPosition[ motor_no ] -= motorM1StepCounter;
        } else {
            currentPosition[ motor_no ] += motorM1StepCounter;
        }
        motorM1StepCounter = 0;
    }

    CorrectPosition(&currentPosition[ motor_no ], TblMaxSteps[ motor_no ]);

    /**-- 正転での駆動パルス数算出 --**/
    SetMotorDirection(motor_no, FOWARD);
    steps = targetPosition[ motor_no ] - currentPosition[ motor_no ];

    while (steps < 0) {
        steps += TblMaxSteps[ motor_no ];
    }

    if ((steps * 2) > TblMaxSteps[ motor_no ]) {
        SetMotorDirection(motor_no, REVERSE);
        steps = TblMaxSteps[ motor_no ] - steps;
    }

    motorStepCounterSet[ motor_no ] = steps;

    PriorUpdateCorrentPosition(motor_no);
}
//------------------------------------------------------------------------------
//..SetMotorAddShakePulse
/***************************************************************************//**
 @brief     揺動パルス設定
 @note      正転駆動のときのみ付加される
 @author
 @date
*******************************************************************************/

void SetMotorAddShakePulse(UB motor_no, UB sw)
{
    motorBitOperation = (UB) (0x01 << motor_no);

    if (sw) {

        addShakePulseBit |= motorBitOperation;

    } else {

        addShakePulseBit &= (UB) (~motorBitOperation);

    }
}
//------------------------------------------------------------------------------
//..SetMotorFrequency
/***************************************************************************//**
 @brief     モータ駆動周波数設定
 @note
 @param     motor_no    モータ種別      MOTOR_M0／MOTOR_M1
 @param     type        制御タイプ      FREQ_SET_SELF_CONTROL_NORMAL
                                        FREQ_SET_EXTERNAL_CONTROL_NORMAL
                                        FREQ_SET_SELF_CONTROL_SHAKE
                                        FREQ_SET_EXTERNAL_CONTROL_SHAKE

 @param     freq        自己制御時の駆動周波数
                        SET_FREQ_XXHZから選択すること
 @author
 @date
*******************************************************************************/

void SetMotorFrequency(UB motor_no, UB type, UB set_freq)
{
    if (FREQ_SET_SELF_CONTROL_NORMAL == type) {

        /**-- 駆動周波数を自己で決めるとき --**/
        finishPositionCtSet[ motor_no ] = POS_P1P2_INIT_DATA + set_freq;

    } else if (FREQ_SET_SELF_CONTROL_SHAKE == type) {

        if (MOTOR_M0 == motor_no) {
            if (TblM0PulseChangePosition[ PULSE_CHANGE_ARRAY_LAST ] < set_freq) {
                finishPositionCtSet[ motor_no ] = set_freq;
            } else {
                finishPositionCtSet[ motor_no ] = POS_PULSE_INTERVAL_FINISH_DEF;
            }
        } else {
            if (TblM1PulseChangePosition[ PULSE_CHANGE_ARRAY_LAST ] < set_freq) {
                finishPositionCtSet[ motor_no ] = set_freq;
            } else {
                finishPositionCtSet[ motor_no ] = POS_PULSE_INTERVAL_FINISH_DEF;
            }
        }
    } else {

        /**-- 駆動周波数は外部が決めるとき（SPIコマンドきたら出力するのみ） --**/
        finishPositionCtSet[ motor_no ] = POS_PULSE_INTERVAL_FINISH_MIN;

    }
}
//------------------------------------------------------------------------------
//..SetMotorStart
/***************************************************************************//**
 @brief     モータ駆動スタート
 @note
 @author
 @date
*******************************************************************************/

void SetMotorStart(UB motor_no)
{
    if (motor_no == MOTOR_M0) {

        StartPulseOutM0();

    } else {

        StartPulseOutM1();

    }
}
//------------------------------------------------------------------------------
//..SetMotorPositionSearchStart
/***************************************************************************//**
 @brief     位置検出スタート
 @note
 @author
 @date
*******************************************************************************/

void SetMotorPositionSearchStart(UB motor_no)
{
    //	offsetBasePosition[ motor_no ] = offset_pos;

    searchTotalSteps[ motor_no ] = 0;

    currentPosition[ motor_no ] = 0;

    InitPositionSearch(motor_no);

    SetAfterDetectLoadFunc(motor_no, AfterDetectedLoadPosition);

    SetMotorStart(motor_no);
}
//------------------------------------------------------------------------------
//..SetMotorCorrectPositionStart
/***************************************************************************//**
 @brief     運針しながらの位置補正スタート
 @note		実際には、運針タイミングは外部コントロールになる
 @author
 @date
*******************************************************************************/

void SetMotorCorrectPositionStart(UB motor_no)
{
    motorBitOperation = (UB) (0x01 << motor_no);

    reqCorrectBasePosBit |= motorBitOperation;
    reqCorrectFinePoBit |= motorBitOperation;

    searchTotalSteps[ motor_no ] = 0;

    InitPositionSearch(motor_no);

    SetAfterDetectLoadFunc(motor_no, AfterDetectedLoadPosition);

    //    SetMotorStart( motor_no );
}
//------------------------------------------------------------------------------
//...InitPositionSearch

/*******************************************************************************
 @brief     位置検出初期化
 @note
 @author
 @date
 ********************************************************************************/
void InitPositionSearch(UB motor_no)
{
    UB i;

    doubleInterval = OFF;

    positionSearchCt[ motor_no ] = 0;

    detectFinePosCt[ motor_no ] = 0;
    detectRoughPosCt[ motor_no ] = 0;
    positionSearchStat[ motor_no ] = ID_SEARCH_FINE_POS_1ST;
    detectLoadType[ motor_no ] = ID_DETECT_LOAD_NULL;
    detectLoadCont[ motor_no ] = 0;
    detectRoughTotalMove[ motor_no ] = 0;

    detectLoadPositionIndex[ motor_no ] = 0;
    for (i = 0; i < MAX_LOAD_POSITION_ARRAY_LENGTH; i++) {
        detectLoadPosition[ motor_no ][ i ] = 0;
        //        detectLoadPositionDiff[ motor_no ][ i ] = 0;
        detectLoadPositionType[ motor_no ][ i ] = 0;
    }

    detectLoadPositionPtr[ motor_no ] = &detectLoadPosition[ motor_no ][ 0 ];
    //    detectLoadPositionDiffPtr[ motor_no ] = &detectLoadPositionDiff[ motor_no ][ 0 ];
    detectLoadPositionTypePtr[ motor_no ] = &detectLoadPositionType[ motor_no ][ 0 ];
}
//------------------------------------------------------------------------------
//..CorrectPosition

/*******************************************************************************
 @brief     位置を0〜最大値以内に補正する
 @note
 @author
 @date
 ********************************************************************************/
void CorrectPosition(H *pos, H max)
{
    if (max == 0) {
        return;
    }

    while (*pos >= max) {
        *pos -= max;
    }

    while (*pos < 0) {
        *pos += max;
    }
}
//------------------------------------------------------------------------------
//..PriorUpdateCorrentPosition

/*******************************************************************************
 @brief		事前に針位置を更新する（針位置が有効な場合のみ）
 @note		motorStepCounterSetに設定されている駆動数で
            事前に現在値を更新する
 @author
 @date
 ********************************************************************************/
void PriorUpdateCorrentPosition(UB motor_no)
{
    motorBitOperation = (UB) (0x01 << motor_no);

    if (handPositionActiveBit & motorBitOperation) {

        if (driveDirectionBit & motorBitOperation) {
            /**-- 正転設定 --**/
            currentPosition[ motor_no ] += motorStepCounterSet[ motor_no ];
        } else {
            /**-- 逆転設定 --**/
            currentPosition[ motor_no ] -= motorStepCounterSet[ motor_no ];
        }

        CorrectPosition(&currentPosition[ motor_no ], TblMaxSteps[ motor_no ]);
    }
}
//------------------------------------------------------------------------------
//..UpdateTargetPosition
/***************************************************************************//**
 @brief		目標位置更新
 @note
 @author
 @date
*******************************************************************************/

void UpdateTargetPosition(UB motor_no, H steps)
{

    targetPosition[ motor_no ] += steps;

    CorrectPosition(&targetPosition[ motor_no ], TblMaxSteps[ motor_no ]);

}
//------------------------------------------------------------------------------
//..SetMotorStop

/*******************************************************************************
 @brief		モータ駆動停止要求
 @note
 @author
 @date
 ********************************************************************************/
void SetMotorStop(UB motor_no)
{
    motorBitOperation = (UB) (0x01 << motor_no);

    reqStopMotorBit |= motorBitOperation;

}
//------------------------------------------------------------------------------
//.基準位置検出
//..AfterDetectedLoadPosition

/*******************************************************************************
 @brief     負荷位置検出したときの処理
 @note      移動量で判定
 @author
 @date
 ********************************************************************************/
void AfterDetectedLoadPosition(UB motor_no)
{
    H move_steps;
    UB res = ID_SEARCH_RESULT_NULL;

    SetMotorBitMotorNo(motor_no);

    /**-- エラー用にトータル移動量積算 --**/
    if (reqSearchBasePosBit & motorBitOperation) {
        searchTotalSteps[ motorNoOperation ] += positionSearchCt[ motorNoOperation ];
    }
    /**-- 今回の移動量保存 --**/
    move_steps = positionSearchCt[ motorNoOperation ];

    /**-- 現在針位置更新 --**/
    if (reqSearchBasePosBit & motorBitOperation) {
        currentPosition[ motorNoOperation ] += positionSearchCt[ motorNoOperation ];
    } else {
        /**-- 位置補正動作中は事前に針位置更新されている --**/
    }

    if (searchTotalSteps[ motorNoOperation ] > TblMaxSearchSteps[ motorNoOperation ]) {
        /**-- 制限以上動かしても見つからなかった→終了 --**/
        res = ID_SEARCH_RESULT_ERR;
    } else {

        if (move_steps > THRESHOLD_FINE_DIFF_UPPER) {
            /**-- 47 → xx--**/
            /**-- 移動量が微調整位置上限を超過 → 初めからやり直し --**/
            if (((THRESHOLD_FINE_DIFF_UPPER * 2) >= move_steps)
                    && ((move_steps >= THRESHOLD_FINE_DIFF_LOWER * 2))) {
                /**-- 43 → 47 の2倍の値は一個抜けと判定 --**/

                if (ID_SEARCH_FINE_POS_1ST == positionSearchStat[ motorNoOperation ]) {
                    /**-- 初回の微調整位置サーチ --**/
                    positionSearchStat[ motorNoOperation ] = ID_SEARCH_FINE_ROUGH_POS;
                }

                doubleInterval = ON;
                detectLoadType[ motorNoOperation ] = ID_DETECT_LOAD_FINE;
                detectFinePosCt[ motorNoOperation ] += 2;
                detectLoadCont[ motorNoOperation ] = 2;

                res = ID_SEARCH_RESULT_NEW_POS;

            } else if ((ID_DETECT_LOAD_ROUGH == detectLoadType[ motorNoOperation ])
                    && ((THRESHOLD_FINE_DIFF_UPPER * 2) >= (detectRoughTotalMove[ motorNoOperation ] + move_steps))
                    && ((detectRoughTotalMove[ motorNoOperation ] + move_steps) >= (THRESHOLD_FINE_DIFF_LOWER * 2))) {

                doubleInterval = ON;
                detectLoadType[ motorNoOperation ] = ID_DETECT_LOAD_FINE;
                detectFinePosCt[ motorNoOperation ] += 2;
                detectLoadCont[ motorNoOperation ] = 2;

                res = ID_SEARCH_RESULT_NEW_POS;

            } else {
                /**-- 43 → 47の1個抜け以外の値はエラーとする --**/
                res = ID_SEARCH_RESULT_RETRY;
            }
        } else if ((THRESHOLD_FINE_DIFF_UPPER >= move_steps) && (move_steps >= THRESHOLD_FINE_DIFF_LOWER)) {
            /**-- 43 → 47 --**/
            if (ID_SEARCH_FINE_POS_1ST == positionSearchStat[ motorNoOperation ]) {
                /**-- 初回の微調整位置サーチ --**/
                positionSearchStat[ motorNoOperation ] = ID_SEARCH_FINE_ROUGH_POS;
            }

            detectLoadType[ motorNoOperation ] = ID_DETECT_LOAD_FINE;
            detectFinePosCt[ motorNoOperation ]++;
            detectLoadCont[ motorNoOperation ] = 1;

            res = ID_SEARCH_RESULT_NEW_POS;

        } else if (THRESHOLD_FINE_DIFF_LOWER > move_steps) {
            /**-- xx → 43 --**/
            if (ID_SEARCH_FINE_POS_1ST == positionSearchStat[ motorNoOperation ]) {
                /**-- 初回の負荷検出中に粗調整位置と判定 --**/

                /**-- 何もしない --**/

            } else if (ID_DETECT_LOAD_FINE == detectLoadType[ motorNoOperation ]) {
                /**-- 微調整位置検出後 --**/

                if (move_steps > MAX_INTERVAL_FINE_CONT) {
                    /**-- 新規の粗調整位置と判定 --**/
                    detectLoadType[ motorNoOperation ] = ID_DETECT_LOAD_ROUGH;
                    detectRoughPosCt[ motorNoOperation ]++;
                    detectLoadCont[ motorNoOperation ] = 1;
                    detectRoughTotalMove[ motorNoOperation ] = move_steps;
                    res = ID_SEARCH_RESULT_NEW_POS;
                } else {
                    /**-- 微調整位置が連続している→連続数確認 --**/
                    detectLoadCont[ motorNoOperation ]++;
                    if (detectLoadCont[ motorNoOperation ] > MAX_FINE_DETECTION_NUMBER) {
                        /**-- 微調整位置の負荷歯が3歯→検出失敗 --**/
                        res = ID_SEARCH_RESULT_RETRY;
                    } else {
                        res = ID_SEARCH_RESULT_CONT;
                    }
                }

            } else if (ID_DETECT_LOAD_ROUGH == detectLoadType[ motorNoOperation ]) {
                /**-- 粗調整位置検出後 --**/
                detectRoughTotalMove[ motorNoOperation ] += move_steps;

                if (move_steps > MAX_INTERVAL_ROUGH_CONT) {

                    /**-- 微調整位置か判定 --**/
                    if ((THRESHOLD_FINE_DIFF_UPPER >= detectRoughTotalMove[ motorNoOperation ]) && (detectRoughTotalMove[ motorNoOperation ] >= THRESHOLD_FINE_DIFF_LOWER)) {
                        /**-- 微調整位置 --**/
                        positionSearchStat[ motorNoOperation ] = ID_SEARCH_FINE_ROUGH_POS;
                        detectLoadType[ motorNoOperation ] = ID_DETECT_LOAD_FINE;
                        detectFinePosCt[ motorNoOperation ]++;
                        detectLoadCont[ motorNoOperation ] = 1;
                        res = ID_SEARCH_RESULT_NEW_POS;

                    } else {
                        /**-- 新規の粗調整位置と判定 --**/
                        detectLoadType[ motorNoOperation ] = ID_DETECT_LOAD_ROUGH;
                        detectRoughPosCt[ motorNoOperation ]++;
                        detectLoadCont[ motorNoOperation ] = 1;
                        detectRoughTotalMove[ motorNoOperation ] = move_steps;
                        res = ID_SEARCH_RESULT_NEW_POS;
                    }

                } else {
                    /**-- 粗調整位置の連続数確認 --**/
                    detectLoadCont[ motorNoOperation ]++;
                    if (detectLoadCont[ motorNoOperation ] > MAX_ROUGH_DETECTION_NUMBER) {
                        /**-- 粗調整位置の負荷歯が5歯→検出失敗 --**/
                        res = ID_SEARCH_RESULT_RETRY;
                    } else {
                        res = ID_SEARCH_RESULT_CONT;
                    }
                }

            } else {
                /**-- ここに来ることはない --**/
                res = ID_SEARCH_RESULT_RETRY;
            }
        }
    }


    if ((detectFinePosCt[ motorNoOperation ] == NUMBER_OF_FINE_POSITION)
            && (detectRoughPosCt[ motorNoOperation ] == NUMBER_OF_ROUGH_POSITION)) {
        /**-- 位置検出成功 --**/
        res = ID_SEARCH_RESULT_SUCCESS;

    } else if ((detectFinePosCt[ motorNoOperation ] == (NUMBER_OF_FINE_POSITION + 1))
            && (detectRoughPosCt[ motorNoOperation ] == NUMBER_OF_ROUGH_POSITION)) {
        /**-- 位置検出成功 --**/
        res = ID_SEARCH_RESULT_SUCCESS;

    } else if (detectFinePosCt[ motorNoOperation ] > (NUMBER_OF_FINE_POSITION + 1)) {
        /**-- 微調整位置が9箇所以上 → やり直し --**/
#ifdef DEBUG_CORRECT_POSITION
        LATA |= b0000_0100;
#endif
        res = ID_SEARCH_RESULT_RETRY;
    } else if (detectRoughPosCt[ motorNoOperation ] > NUMBER_OF_ROUGH_POSITION) {
        /**-- 粗調整位置が2箇所以上 → やり直し --**/
        res = ID_SEARCH_RESULT_RETRY;
    }

    if (ID_SEARCH_RESULT_ERR == res) {
        /**-- 検出失敗 --**/

    } else if (ID_SEARCH_RESULT_NULL == res) {
        /**-- 無視する --**/
        /**-- 初回の負荷検出中に粗調整位置→位置検出継続 --**/

        SetAfterDetectLoadFunc(motorNoOperation, AfterDetectedLoadPosition);
        if (reqSearchBasePosBit & motorBitOperation) {
            SetMotorStart(motorNoOperation);
        }
    } else if (ID_SEARCH_RESULT_RETRY == res) {

        if (motorBitOperation & BIT_MOTOR_M0) {
#ifdef DEBUG_CORRECT_POSITION
            LATA |= b0000_0100;
#endif
        }
        InitPositionSearch(motorNoOperation);
        SetAfterDetectLoadFunc(motorNoOperation, AfterDetectedLoadPosition);
        if (reqSearchBasePosBit & motorBitOperation) {
            SetMotorStart(motorNoOperation);
        } else {
            reqCorrectFinePoBit |= motorBitOperation;
        }
    } else {

        if (0 == (reqCorrectFinePoBit & motorBitOperation)) {
            /**-- ID_SEARCH_RESULT_NEW_POS ／ ID_SEARCH_RESULT_CONT --**/
            /**-- 負荷位置インデックス++ --**/

            if (ON == doubleInterval) {
                detectLoadPositionIndex[ motorNoOperation ]++;
                missingDetectFinePosition = currentPosition[ motorNoOperation ] - 45;
                CorrectPosition(&missingDetectFinePosition, TblMaxSteps[ motorNoOperation ]);

                *detectLoadPositionPtr[ motorNoOperation ] = missingDetectFinePosition;
                detectLoadPositionPtr[ motorNoOperation ]++;

                *detectLoadPositionTypePtr[ motorNoOperation ] = detectLoadType[ motorNoOperation ];
                detectLoadPositionTypePtr[ motorNoOperation ]++;
            }

            detectLoadPositionIndex[ motorNoOperation ]++;

            /**-- 「負荷位置検出」針配列に現在針位置を保存 --**/
            *detectLoadPositionPtr[ motorNoOperation ] = currentPosition[ motorNoOperation ];
            detectLoadPositionPtr[ motorNoOperation ]++;

            /**-- 「負荷検出TYPE」配列に今回のTYPEを保存 --**/
            *detectLoadPositionTypePtr[ motorNoOperation ] = detectLoadType[ motorNoOperation ];
            if (ID_SEARCH_RESULT_CONT == res) {
                *detectLoadPositionTypePtr[ motorNoOperation ] += MARK_DETECT_LOAD_TYPE_CONT;
            }

            detectLoadPositionTypePtr[ motorNoOperation ]++;

        }

        if (ID_SEARCH_RESULT_SUCCESS == res) {
            /**-- 基準位置決定 --**/
            DetermineBasePosition(motorNoOperation);

            /**-- 基準位置(オフセット込)を0として負荷位置を更新 --**/
            UpdataHandPosition(motorNoOperation);

            /**-- 負荷位置を保存 --**/
            SaveLoadPosition(motorNoOperation);

            /**-- 針位置管理有効化 --**/
            ActivateHandPosition(motorNoOperation);

            if (reqSearchBasePosBit & motorBitOperation) {
                /**-- 基準位置探しの場合は、基準位置へ移動させる --**/
                if (DRIVE_SEARCH_1008_POSITION == driveType[ motorNoOperation ]) {
                    /**-- 10時8分の位置へ移動 --**/
                    if (MOTOR_M0 == motorNoOperation) {
                        targetPosition[ MOTOR_M0 ] = 48;
                    } else {
                        targetPosition[ MOTOR_M1 ] = 304;
                    }
                } else {
                    targetPosition[ motorNoOperation ] = 0;
                }
                /**-- 移動完了後、コントローラに完了割り込み通知 --**/
                SetAfterMotorStopFunc(motorNoOperation, SPIControl_SetNotification);
                SetDriveType(motorNoOperation, DRIVE_SIMPLE);
            } else if (reqCorrectBasePosBit & motorBitOperation) {
                /**-- 基準位置補正の場合は、今の目標位置へ移動させる --**/
                /**-- 今の目標値のためtargetPosition変更しない --**/
                //SetDriveType( motorNoOperation, DRIVE_SIMPLE );
                reqCorrectBasePosBit &= (UB) (~motorBitOperation);
                SetDriveType(motorNoOperation, DRIVE_ACT_CORRECT_POSITION);
            }
            /**-- 基準位置（ターゲットポジション）へ移動開始 --**/

            MoveTargetPosition(motorNoOperation);
        } else {
            /**-- 位置検出継続 --**/
            SetAfterDetectLoadFunc(motorNoOperation, AfterDetectedLoadPosition);
            if (reqSearchBasePosBit & motorBitOperation) {

                /**-- 基準位置サーチ中 --**/
                /**-- 基準位置検出中 --**/
                SetMotorStart(motorNoOperation);

            } else if ((reqCorrectBasePosBit & motorBitOperation)
                    && (reqCorrectFinePoBit & motorBitOperation)
                    && (ID_DETECT_LOAD_FINE == detectLoadType[ motorNoOperation ])
                    && (ID_SEARCH_RESULT_NEW_POS == res)) {

                /**-- 位置補正駆動中 ∧ 微調整位置検出 ∧  連続する1個目の微調整位置 --**/

                UpdateCurrentPositionByFinePosition(motorNoOperation);

                //				detectLoadPositionPtr[ motorNoOperation ]--;
                //		        *detectLoadPositionPtr[ motorNoOperation ] = currentPosition[ motorNoOperation ];
                //				detectLoadPositionPtr[ motorNoOperation ]++;

                /**-- はじめからにする --**/
                InitPositionSearch(motorNoOperation);

                SetDriveType(motorNoOperation, DRIVE_ACT_CORRECT_POSITION);
                reqCorrectFinePoBit &= (UB) (~motorBitOperation);

                MoveTargetPosition(motorNoOperation);

            } else {
                /**-- 位置補正駆動中の上記以外 --**/

            }
        }
    }


    /**-- positionSearchCt初期化 --**/
    positionSearchCt[ motorNoOperation ] = 0;

    /**-- ダブルインターバルフラグOFF --**/
    doubleInterval = OFF;

#ifdef DEBUG_CORRECT_POSITION

    LATA &= (UB) (~b0000_0100);
#endif
}
//------------------------------------------------------------------------------
//...DetermineBasePosition

/*******************************************************************************
 @brief     基準位置決定
 @note
 @retval
 @author
 @date
 ********************************************************************************/
void DetermineBasePosition(UB motor_no)
{
    UB base_pos_index;
    H load_pos_type;

    base_pos_index = 0;

    /**-- 粗調整位置検索 --**/
    while (1) {

        load_pos_type = detectLoadPositionType[ motor_no ][ base_pos_index ];
        base_pos_index++;

        if (ID_DETECT_LOAD_ROUGH == load_pos_type) {
            /**-- 粗調整位置 --**/
            break;
        }
    }

    /**-- 粗調整後の微調整位置検索 --**/
    while (1) {

        load_pos_type = detectLoadPositionType[ motor_no ][ base_pos_index ];

        if (ID_DETECT_LOAD_FINE == load_pos_type) {
            /**-- 粗調整後の微調整位置 --**/
            break;
        }

        base_pos_index++;
        if (base_pos_index > detectLoadPositionIndex[ motor_no ]) {
            base_pos_index = 0;
        }
    }

    detectBasePosition[ motor_no ] = detectLoadPosition[ motor_no ][ base_pos_index ];
    detectBasePosition[ motor_no ] += offsetBasePosition[ motor_no ];

    CorrectPosition(&detectBasePosition[ motor_no ], TblMaxSteps[ motor_no ]);

}
//------------------------------------------------------------------------------
//...UpdataHandPosition

/*******************************************************************************
 @brief		見つけた基準位置に基づいて、基準位置が0となるように今の負荷位置を更新する
 @note		基準位置はdetectBasePositionに入っている

 @author
 @date
 ********************************************************************************/
void UpdataHandPosition(UB motor_no)
{
    UB i;

    currentPosition[ motor_no ] -= detectBasePosition[ motor_no ];
    CorrectPosition(&currentPosition[ motor_no ], TblMaxSteps[ motor_no ]);

    for (i = 0; i < detectLoadPositionIndex[ motor_no ]; i++) {

        detectLoadPosition[ motor_no ][ i ] -= detectBasePosition[ motor_no ];
        CorrectPosition(&detectLoadPosition[ motor_no ][ i ], TblMaxSteps[ motor_no ]);

    }
}
//------------------------------------------------------------------------------
//...UpdateCurrentPositionByFinePosition
/***************************************************************************//**
 @brief		見つけた微調整位置に基づいて、『 現在位置 』を更新する
			例えば、今までの負荷位置が
			48、49、93、94とあったとして、
			96の位置で微調整位置開始を見つけたとしたら、

			現在位置97を→93に補正する。

			→つまりLoadPositionから微調整位置を探して、一番近くかつ連番以外を探す
			差分が22以下の場所を探す

 @note
 @author
 @date
*******************************************************************************/

void UpdateCurrentPositionByFinePosition(UB motor_no)
{
    UB i;
    H revise_pos;
    H diff;

    for (i = 0; i < loadPositionIndex[ motor_no ]; i++) {

        if (ID_DETECT_LOAD_FINE == loadPositionType[ motor_no ][ i ]) {
            diff = currentPosition[ motor_no ] - loadPosition[ motor_no ][ i ];

            if (diff < 0) {
                diff = -diff;
            }

            if (diff <= 22) {
                currentPosition[ motor_no ] = loadPosition[ motor_no ][ i ];
                break;
            }
        }
    }
}
//------------------------------------------------------------------------------
//...SaveLoadPosition
/***************************************************************************//**
 @brief		detectLoadPositionとdetectLoadPositionIndexを
 			保存用に、loadPositionとloadPositionIndexに保存する
 @note
 @author
 @date
*******************************************************************************/

void SaveLoadPosition(UB motor_no)
{
    UB i;

    for (i = 0; i < MAX_LOAD_POSITION_ARRAY_LENGTH; i++) {
        loadPosition[ motor_no ][ i ] = detectLoadPosition[ motor_no ][ i ];
        loadPositionType[ motor_no ][ i ] = detectLoadPositionType[ motor_no ][ i ];
    }

    loadPositionIndex[ motor_no ] = detectLoadPositionIndex[ motor_no ];
}
//------------------------------------------------------------------------------
//...ActivateHandPosition

/*******************************************************************************
 @brief		針位置有効化
 @note		基準位置検出終了したら、針位置を有効化する
 @author
 @date
 ********************************************************************************/
void ActivateHandPosition(UB motor_no)
{
    motorBitOperation = (UB) (0x01 << motor_no);

    handPositionActiveBit |= motorBitOperation;
}
//------------------------------------------------------------------------------
//...DeactivateHandPosition

/*******************************************************************************
 @brief		針位置無効化
 @note		基準位置処理開始時に無効化する
 @author
 @date
 ********************************************************************************/
void DeactivateHandPosition(UB motor_no)
{
    motorBitOperation = (UB) (0x01 << motor_no);

    handPositionActiveBit &= (UB) (~motorBitOperation);
}
//------------------------------------------------------------------------------
//..MoveTargetPosition
/***************************************************************************//**
 @brief		targetPositionに移動
 @note
 @author
 @date
*******************************************************************************/

void MoveTargetPosition(UB motor_no)
{
    /**-- 駆動ステップ数計算 & 回転方向セット（近い方）--**/
    CalcMotorStepCounter(motor_no);

    /**-- 周波数設定 --**/
    SetMotorFrequency(motor_no, FREQ_SET_SELF_CONTROL_NORMAL, SET_FREQ_200HZ);

    /**-- 駆動スタート --**/
    SetMotorStart(motor_no);
}
//------------------------------------------------------------------------------
//...VerifyHandPosition_LoadPosition

/*******************************************************************************
 @brief		駆動中に負荷発生→負荷位置の検証と補正
 @note
 @attention	必ず正転中で有ること
 @author
 @date
 ********************************************************************************/
void VerifyHandPosition_LoadPosition(UB motor_no)
{
    UB i;
    UB misalignment_flag = ON;
    H remain_steps;
    H curt_pos;
    H load_pos;
    H load_pos_pl1;

    /**-- はじめにcurrentPositionを更新して現在位置にする --**/
    if (MOTOR_M0 == motor_no) {
        remain_steps = motorM0StepCounter;
    } else {
        remain_steps = motorM1StepCounter;
    }

    /**-- すでに事前に位置更新されているので現在値を補正する --**/
    currentPosition[ motor_no ] -= remain_steps;
    CorrectPosition(&currentPosition[ motor_no ], TblMaxSteps[ motor_no ]);

    curt_pos = currentPosition[ motor_no ];

    /**-- 基準位置サーチ時、現在の位置は負荷位置であったか確認 --**/
    /**-- 微調整の1発目の次の位置は負荷テーブルになくても許容する --**/
    //	for ( i = 0; i < loadPositionIndex[ motor_no ]; i++ ) {
    //
    //		load_pos = loadPosition[ motor_no ][ i ];
    //
    //		if ( curt_pos == load_pos ) {
    //			/**-- 過去のリストに今回の負荷位置がある --**/
    //			misalignment_flag = OFF;
    //			break;
    //		}
    //	}

    for (i = 0; i < loadPositionIndex[ motor_no ]; i++) {

        load_pos = loadPosition[ motor_no ][ i ];

        if (curt_pos == load_pos) {
            /**-- 過去のリストに今回の負荷位置がある --**/
            misalignment_flag = OFF;
            break;
        }

        if (ID_DETECT_LOAD_FINE == loadPositionType[ motor_no ][ i ]) {
            /**-- 微調整位置の次の位置とも比較する --**/
            load_pos++;
            if (load_pos >= TblMaxSteps[ motor_no ]) {
                load_pos = 0;
            }

            if (curt_pos == load_pos) {
                /**-- 過去のリストに今回の負荷位置がある --**/
                misalignment_flag = OFF;
                break;
            }
        }
    }

    if (misalignment_flag) {
        /**-- 新規の負荷位置 --**/
        /**-- 位置を間違っている可能性大→微調整位置を2回検出する処理を開始 --**/
        //		MoveTargetPosition( motor_no );
        SetMotorCorrectPositionStart(motor_no);
    } else {
        /**-- 既存の負荷位置 --**/
        /**-- 位置が正しい→補正付きの移動再開 --**/
        MoveTargetPosition(motor_no);
    }
}
//------------------------------------------------------------------------------
//.misc
//..GetIsMotorActive
/***************************************************************************//**
 @brief     モータ駆動状態読み出し
 @note
 @author
 @date
*******************************************************************************/

UB GetIsMotorActive(UB motor_no)
{
    UB res = FALSE;


    motorBitOperation = (UB) (0x01 << motor_no);

    if (motorActiveBit & motorBitOperation) {
        res = TRUE;
    }

    return res;
}
//------------------------------------------------------------------------------
//..SetMotorBitMotorNo

void SetMotorBitMotorNo(UB motor_no)
{
    motorNoOperation = motor_no;
    motorBitOperation = (UB) (0x01 << motor_no);
}
//------------------------------------------------------------------------------
//.コンパレータ設定
//..ComparatorOff

/*******************************************************************************
 *  コンパレータOFFと初期化
 *
 ******************************************************************************/
void ComparatorOff(UB motor_no)
{
    if (motor_no == MOTOR_M0) {
        CM1CON0 = CMP1_BASE; //CMP1 OFF、反転、 高速
        CM1CON1 = CMP1_VCOMP_BASE; //IFPOS,DAC出力
    } else {
        CM2CON0 = CMP2_BASE; //CMP2 OFF、反転、 高速
        CM2CON1 = CMP2_VCOMP_BASE; //IFPOS,DAC出力
    }
    PIR2bits.C1IF = 0;
    PIR2bits.C2IF = 0;
}
//------------------------------------------------------------------------------
//..ComparatorOn

/*******************************************************************************
 *  コンパレータON
 *
 ******************************************************************************/
void ComparatorOn(UB motor_no)
{
    if (motor_no == MOTOR_M0) {
        CM1CON0 = (CMP1_BASE | CMP1_ON); //CMP1 ON、反転、 高速
        if (polarityBit & BIT_MOTOR_M0) {
            /* 極性1 */
            CM1CON1 = (CMP1_VCOMP_BASE | M0_O3_CMP); //M0_O3側選択
        } else {
            /* 極性0 */
            CM1CON1 = (CMP1_VCOMP_BASE | M0_O4_CMP); //M0_O4側選択
        }
        PIR2bits.C1IF = 0; //要因クリア

    } else {

        CM2CON0 = (CMP2_BASE | CMP2_ON); //CMP2 ON、反転、 高速
        if (polarityBit & BIT_MOTOR_M1) {
            /* 極性1 */
            CM2CON1 = (CMP2_VCOMP_BASE | M1_O3_CMP); //M1_O3側選択
        } else {
            /* 極性0 */
            CM2CON1 = (CMP2_VCOMP_BASE | M1_O4_CMP); //M1_O4側選択
        }
        PIR2bits.C2IF = 0; //要因クリア

    }
}
//------------------------------------------------------------------------------
//.Vrs本数チェック
//..VrsCheck

/*******************************************************************************
 *  Vcomp越えのVrsェック　IF本数カウント
 *
 ******************************************************************************/
void VrsCheck(UB motor_no)
{
    if (motor_no == MOTOR_M0) {
        if (PIR2bits.C1IF) {
            vrsCountValue[ MOTOR_M0 ]++; //M0用Vrs本数カウント
            //          TRISC &= b1111_1110;
        }
        PIR2bits.C1IF = 0;
    } else {
        if (PIR2bits.C2IF) {
            vrsCountValue [ MOTOR_M1 ]++; //M1用Vrs本数カウント
        }
        PIR2bits.C2IF = 0;
    }
}
//++++++++++++++++++++++++++Touch Read++++++++++++++++++++++++++++++++++++++
//.触読用コンパレータ設定
//..SpkComparatorOff

/*******************************************************************************
 *  触読用コンパレータOFFと初期化
 *  Motor種別無し
 ******************************************************************************/
void SpkComparatorOff(void)
{
    CM1CON0 = CMP1_BASE; //CMP1 OFF、反転、 高速
    CM1CON1 = CMP1_VCOMP_BASE; //IFPOS,DAC出力

    CM2CON0 = CMP2_BASE; //CMP2 OFF、反転、 高速
    CM2CON1 = CMP2_VCOMP_BASE; //IFPOS,DAC出力

    PIR2bits.C1IF = 0;
    PIR2bits.C2IF = 0;
}
//++++++++++++++++++++++++++Touch Read++++++++++++++++++++++++++++++++++++++
//..SpkComparatorOn

/*******************************************************************************
 *  触読用コンパレータON
 *  Motor種別無し
 ******************************************************************************/
void SpkComparatorOn(void)
{
    CM1CON0 = (CMP1_BASE | CMP1_ON); //CMP1 ON、反転、 高速
    PIR2bits.C1IF = 0; //要因クリア

    CM2CON0 = (CMP2_BASE | CMP2_ON); //CMP2 ON、反転、 高速
    PIR2bits.C2IF = 0; //要因クリア


}
//++++++++++++++++++++++++++Touch Read++++++++++++++++++++++++++++++++++++++
//.触読Vrsチェック
//..SpkVrsCheck

/*******************************************************************************
 *  Vrsチェックし、検出で、励磁ONフラグ有効なら、励磁ON
 *
 ******************************************************************************/
void SpkVrsCheck(void)
{
    if ((PIR2bits.C1IF == 1) || (PIR2bits.C2IF == 1)) {

        PIR2bits.C1IF = 0;
        PIR2bits.C2IF = 0;

        if (excitatingEnableBit & (BIT_MOTOR_M0 | BIT_MOTOR_M1)) {//励磁ON設定なら

            ExcitatingOutOn();

        }

    }

}
//++++++++++++++++++++++++++Touch Read++++++++++++++++++++++++++++++++++++++
//.触読励磁OFF関数
//..ExcitatingOutOff

/*******************************************************************************
 *  励磁Off処理
 *　マスクカウンタセット インジケータLEDOFF
 ******************************************************************************/
void ExcitatingOutOff(void)
{

    SetExcitatingOutOffData();

    excitatingOffMaskTime = EXCITATING_OFF_MSK_TIME;

}
//++++++++++++++++++++++++++Touch Read++++++++++++++++++++++++++++++++++++++
//.触読励磁OFFdataセット関数
//..ExcitatingOutData

/*******************************************************************************
 *  励磁Off処理
 *　出力モード、励磁OFFパルス。LED消灯データセット出力
 ******************************************************************************/
void SetExcitatingOutOffData(void)
{

    /**-- TRISXデータ出力 --**/
    dataTRISAWork_M0 = BIT_TRISA_M0_DRV;
    dataTRISBWork_M0 = BIT_TRISB_M0_DRV;
    dataTRISCWork_M0 = BIT_TRISC_M0_DRV;

    dataTRISAWork_M1 = BIT_TRISA_M1_DRV;
    dataTRISBWork_M1 = BIT_TRISB_M1_DRV;
    dataTRISCWork_M1 = BIT_TRISC_M1_DRV;

    pulseOutLATAWork_M0 = 0; //M0_O1 OFF
    pulseOutLATBWork_M0 = 0;
    pulseOutLATCWork_M0 = 0;

    pulseOutLATAWork_M1 = 0;
    pulseOutLATBWork_M1 = 0; //M1_O1 OFF
    pulseOutLATCWork_M1 = 0;

    SetOutPutLATX(); //LATX 出力 ここでは、モータ端子のみの出力
    LATA &= ~BIT_PORTA_LED_ON; //ここで、LED OFFを出力

}
//++++++++++++++++++++++++++Touch Read++++++++++++++++++++++++++++++++++++++
//.触読励磁ON関数
//..ExcitatingOutOn

/*******************************************************************************
 *  励磁On処理
 *　励磁ON出力、励磁ON時間カウンタセット インジケータLED　ON
 ******************************************************************************/
void ExcitatingOutOn(void)
{
    /**-- TRISXデータ出力 --**/
    dataTRISAWork_M0 = BIT_TRISA_M0_DRV;
    dataTRISBWork_M0 = BIT_TRISB_M0_DRV;
    dataTRISCWork_M0 = BIT_TRISC_M0_DRV;

    dataTRISAWork_M1 = BIT_TRISA_M1_DRV;
    dataTRISBWork_M1 = BIT_TRISB_M1_DRV;
    dataTRISCWork_M1 = BIT_TRISC_M1_DRV;

    /**-- LATXデータ出力 -- OUT1にのみ出力**/
    pulseOutLATAWork_M0 = BIT_PORTA_M0_OUT1; //M0_O1 ON
    pulseOutLATBWork_M0 = BIT_PORTB_M0_OUT1;
    pulseOutLATCWork_M0 = BIT_PORTC_M0_OUT1;

    pulseOutLATAWork_M1 = BIT_PORTA_M1_OUT1;
    pulseOutLATBWork_M1 = BIT_PORTB_M1_OUT1; //M1_O1 ON
    pulseOutLATCWork_M1 = BIT_PORTC_M1_OUT1;

    SetOutPutLATX(); //LATX 出力 ここでは、モータ端子のみの出力
    LATA |= BIT_PORTA_LED_ON; //ここで、LED ONを出力

    excitatingTime = EXCITATING_TIME;       //励磁ON継続時間カウント値
}
//++++++++++++++++++++++++++Touch Read++++++++++++++++++++++++++++++++++++++
//.触読OPEN関数
/*******************************************************************************
 *  触読OPEN処理
 *　Turnフラグにより、O3/O4検出抵抗、コンパレータ端子選択
 *  Turnフラグ反転し、次の実施関数をSHORT関数にセット
 ******************************************************************************/
//------------------------------------------------------------------------------
//....SetDataTRISxWork_SPK_OPEN

void SetDataTRISxWork_SPK_OPEN(void)
{
    if (detectSpkTurnBit == 0) {// O4側検出
        dataTRISAWork_M0 = BIT_TRISA_M0_O4_OPN;
        dataTRISBWork_M0 = BIT_TRISB_M0_O4_OPN;
        dataTRISCWork_M0 = BIT_TRISC_M0_O4_OPN; //M0_O4側OPEN

        dataTRISAWork_M1 = BIT_TRISA_M1_O4_OPN;
        dataTRISBWork_M1 = BIT_TRISB_M1_O4_OPN;
        dataTRISCWork_M1 = BIT_TRISC_M1_O4_OPN; //M1_O4側OPEN

        CM1CON1 = (CMP1_VCOMP_BASE | M0_O4_CMP); //M0_O4側選択
        CM2CON1 = (CMP2_VCOMP_BASE | M1_O4_CMP); //M1_O4側選択

    } else {// O3側検出
        dataTRISAWork_M0 = BIT_TRISA_M0_O3_OPN;
        dataTRISBWork_M0 = BIT_TRISB_M0_O3_OPN;
        dataTRISCWork_M0 = BIT_TRISC_M0_O3_OPN; //M0_O3側OPEN

        dataTRISAWork_M1 = BIT_TRISA_M1_O3_OPN;
        dataTRISBWork_M1 = BIT_TRISB_M1_O3_OPN;
        dataTRISCWork_M1 = BIT_TRISC_M1_O3_OPN; //M1_O3側OPEN

        CM1CON1 = (CMP1_VCOMP_BASE | M0_O3_CMP); //M0_O3側選択
        CM2CON1 = (CMP2_VCOMP_BASE | M1_O3_CMP); //M1_O3側選択
    }

    spkOutChangeFunc = SetDataTRISxWork_SPK_SHORT; //次の実施関数をSHORT関数に
    spkChangePosition = CHANGE_POSITION_OPEN; //次のチェンジポジション値
    detectSpkTurnBit ^= 0x01; //検出する端子の入れ替え指示


#ifdef TOUCH_TEST
    if (excitatingEnableBit & (BIT_MOTOR_M0 | BIT_MOTOR_M1)) {
        spkOutTrisRequestFlug = ON;
    } else {
        spkOutTrisRequestFlug = OFF;
    }
#else
    spkOutTrisRequestFlug = ON;
#endif    

}
//++++++++++++++++++++++++++Touch Read++++++++++++++++++++++++++++++++++++++
//.触読SHORT関数
/*******************************************************************************
 *  触読SHORT処理
 *　TRISXをSHORT設定
 *  Vrsチェック要求
 *  次の実施関数をOPEN関数にセット
 ******************************************************************************/
//....SetDataTRISxWork_SPK_SHORT

void SetDataTRISxWork_SPK_SHORT(void)
{
    dataTRISAWork_M0 = BIT_TRISA_M0_SHT;
    dataTRISBWork_M0 = BIT_TRISB_M0_SHT;
    dataTRISCWork_M0 = BIT_TRISC_M0_SHT;

    dataTRISAWork_M1 = BIT_TRISA_M1_SHT;
    dataTRISBWork_M1 = BIT_TRISB_M1_SHT;
    dataTRISCWork_M1 = BIT_TRISC_M1_SHT;

    spkOutChangeFunc = SetDataTRISxWork_SPK_OPEN; //次の実施関数をOPEN関数に
    spkChangePosition = CHANGE_POSITION_SHORT; //次のチェンジポジション値

    spkReqVrsCheckBit |= (BIT_MOTOR_M0 | BIT_MOTOR_M1); //モータ種別無し Vrsチェック要求


    spkOutTrisRequestFlug = ON;

}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void SetOutPutLATX(void)
{

    /**-- 今のTRISxをdataTRISxに保存 --**/
    dataTRISA = TRISA;
    dataTRISB = TRISB;
    dataTRISC = TRISC;

    /**-- dataTRISxのモータ端子の箇所を0にする --**/
    //ここではSPKに関連するモータ端子のみO3,O4,VDを0にする
    dataTRISA &= (UB) (~(BIT_TRISA_M0_ALL | BIT_TRISA_M1_ALL));
    dataTRISB &= (UB) (~(BIT_TRISB_M0_ALL | BIT_TRISB_M1_ALL));
    dataTRISC &= (UB) (~(BIT_TRISC_M0_ALL | BIT_TRISC_M1_ALL));

    /**-- dataTRISxの必要なモータ端子の箇所を1にする --**/
    dataTRISA |= (UB) (dataTRISAWork_M0 | dataTRISAWork_M1);
    dataTRISB |= (UB) (dataTRISBWork_M0 | dataTRISBWork_M1);
    dataTRISC |= (UB) (dataTRISCWork_M0 | dataTRISCWork_M1);

    /**-- データセット --**/
    TRISA = dataTRISA;
    TRISB = dataTRISB;
    TRISC = dataTRISC;

    /**-- 今のLATxをpulseOutLATxに保存 --**/
    pulseOutLATA = LATA;
    pulseOutLATB = LATB;
    pulseOutLATC = LATC;

    /**-- pulseOutLATxのモータ端子の箇所を0にする --**/
    //ここはO1,O2,O3,O4のモータ端子のみ
    pulseOutLATA &= (UB) (~(BIT_PORTA_M0_ALL | BIT_PORTA_M1_ALL));
    pulseOutLATB &= (UB) (~(BIT_PORTB_M0_ALL | BIT_PORTB_M1_ALL));
    pulseOutLATC &= (UB) (~(BIT_PORTC_M0_ALL | BIT_PORTC_M1_ALL));

    /**-- pulseOutLATxの必要なモータ端子の箇所を1にする --**/
    pulseOutLATA |= (UB) (pulseOutLATAWork_M0 | pulseOutLATAWork_M1);
    pulseOutLATB |= (UB) (pulseOutLATBWork_M0 | pulseOutLATBWork_M1);
    pulseOutLATC |= (UB) (pulseOutLATCWork_M0 | pulseOutLATCWork_M1);

    /**-- 出力 --**/
    LATA = pulseOutLATA;
    LATB = pulseOutLATB;
    LATC = pulseOutLATC;

}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//.パルス設定
//..M0
//...FWD_POL0
//....SetPulseOutLATxWork_M0_FWD_POL0_P1

void SetPulseOutLATxWork_M0_FWD_POL0_P1(void)
{
    pulseOutLATAWork_M0 = BIT_PORTA_M0_FWD_POL0_P1;
    pulseOutLATBWork_M0 = BIT_PORTB_M0_FWD_POL0_P1;
    pulseOutLATCWork_M0 = BIT_PORTC_M0_FWD_POL0_P1;

    m0PulseOutChangeFunc = SetPulseOutLATxWork_M0_FWD_POL0_P2;
    OutRequestFlug = ON;
}
//------------------------------------------------------------------------------
//....SetPulseOutLATxWork_M0_FWD_POL0_P2

void SetPulseOutLATxWork_M0_FWD_POL0_P2(void)
{
    pulseOutLATAWork_M0 = BIT_PORTA_M0_FWD_POL0_P2;
    pulseOutLATBWork_M0 = BIT_PORTB_M0_FWD_POL0_P2;
    pulseOutLATCWork_M0 = BIT_PORTC_M0_FWD_POL0_P2;

    m0PulseOutChangeFunc = SetPulseOutLATxWork_M0_FWD_POL0_OFF;
    OutRequestFlug = ON;
}
//------------------------------------------------------------------------------
//....SetPulseOutLATxWork_M0_FWD_POL0_OFF

void SetPulseOutLATxWork_M0_FWD_POL0_OFF(void)
{
    pulseOutLATAWork_M0 = 0;
    pulseOutLATBWork_M0 = 0;
    pulseOutLATCWork_M0 = 0;

    polarityBit ^= BIT_MOTOR_M0;

    if (addShakePulseBit & BIT_MOTOR_M0) {
        m0PulseOutChangeFunc = SetPulseOutLATxWork_M0_SHAKE_POL1_P0;
    } else {
        m0PulseOutChangeFunc = SetPulseOutLATxWork_M0_FWD_POL1_P1;
    }

    OutRequestFlug = ON;

}
//------------------------------------------------------------------------------
//...FWD_POL1
//....SetPulseOutLATxWork_M0_FWD_POL1_P1

void SetPulseOutLATxWork_M0_FWD_POL1_P1(void)
{
    pulseOutLATAWork_M0 = BIT_PORTA_M0_FWD_POL1_P1;
    pulseOutLATBWork_M0 = BIT_PORTB_M0_FWD_POL1_P1;
    pulseOutLATCWork_M0 = BIT_PORTC_M0_FWD_POL1_P1;

    m0PulseOutChangeFunc = SetPulseOutLATxWork_M0_FWD_POL1_P2;
    OutRequestFlug = ON;
}
//------------------------------------------------------------------------------
//....SetPulseOutLATxWork_M0_FWD_POL1_P2

void SetPulseOutLATxWork_M0_FWD_POL1_P2(void)
{
    pulseOutLATAWork_M0 = BIT_PORTA_M0_FWD_POL1_P2;
    pulseOutLATBWork_M0 = BIT_PORTB_M0_FWD_POL1_P2;
    pulseOutLATCWork_M0 = BIT_PORTC_M0_FWD_POL1_P2;

    m0PulseOutChangeFunc = SetPulseOutLATxWork_M0_FWD_POL1_OFF;
    OutRequestFlug = ON;
}
//------------------------------------------------------------------------------
//....SetPulseOutLATxWork_M0_FWD_POL1_OFF

void SetPulseOutLATxWork_M0_FWD_POL1_OFF(void)
{
    pulseOutLATAWork_M0 = 0;
    pulseOutLATBWork_M0 = 0;
    pulseOutLATCWork_M0 = 0;

    polarityBit ^= BIT_MOTOR_M0;

    if (addShakePulseBit & BIT_MOTOR_M0) {
        m0PulseOutChangeFunc = SetPulseOutLATxWork_M0_SHAKE_POL0_P0;
    } else {
        m0PulseOutChangeFunc = SetPulseOutLATxWork_M0_FWD_POL0_P1;
    }

    OutRequestFlug = ON;

}
//------------------------------------------------------------------------------
//...REV_POL0
//....SetPulseOutLATxWork_M0_REV_POL0_P1

void SetPulseOutLATxWork_M0_REV_POL0_P1(void)
{
    pulseOutLATAWork_M0 = BIT_PORTA_M0_REV_POL0_P1;
    pulseOutLATBWork_M0 = BIT_PORTB_M0_REV_POL0_P1;
    pulseOutLATCWork_M0 = BIT_PORTC_M0_REV_POL0_P1;

    m0PulseOutChangeFunc = SetPulseOutLATxWork_M0_REV_POL0_P2;
    OutRequestFlug = ON;
}
//------------------------------------------------------------------------------
//....SetPulseOutLATxWork_M0_REV_POL0_P2

void SetPulseOutLATxWork_M0_REV_POL0_P2(void)
{
    pulseOutLATAWork_M0 = BIT_PORTA_M0_REV_POL0_P2;
    pulseOutLATBWork_M0 = BIT_PORTB_M0_REV_POL0_P2;
    pulseOutLATCWork_M0 = BIT_PORTC_M0_REV_POL0_P2;

    m0PulseOutChangeFunc = SetPulseOutLATxWork_M0_REV_POL0_OFF;
    OutRequestFlug = ON;
}
//------------------------------------------------------------------------------
//....SetPulseOutLATxWork_M0_REV_POL0_OFF

void SetPulseOutLATxWork_M0_REV_POL0_OFF(void)
{
    pulseOutLATAWork_M0 = 0;
    pulseOutLATBWork_M0 = 0;
    pulseOutLATCWork_M0 = 0;

    polarityBit ^= BIT_MOTOR_M0;

    m0PulseOutChangeFunc = SetPulseOutLATxWork_M0_REV_POL1_P1;
    OutRequestFlug = ON;
}
//------------------------------------------------------------------------------
//...REV_POL1
//....SetPulseOutLATxWork_M0_REV_POL1_P1

void SetPulseOutLATxWork_M0_REV_POL1_P1(void)
{
    pulseOutLATAWork_M0 = BIT_PORTA_M0_REV_POL1_P1;
    pulseOutLATBWork_M0 = BIT_PORTB_M0_REV_POL1_P1;
    pulseOutLATCWork_M0 = BIT_PORTC_M0_REV_POL1_P1;

    m0PulseOutChangeFunc = SetPulseOutLATxWork_M0_REV_POL1_P2;
    OutRequestFlug = ON;
}
//------------------------------------------------------------------------------
//....SetPulseOutLATxWork_M0_REV_POL1_P2

void SetPulseOutLATxWork_M0_REV_POL1_P2(void)
{
    pulseOutLATAWork_M0 = BIT_PORTA_M0_REV_POL1_P2;
    pulseOutLATBWork_M0 = BIT_PORTB_M0_REV_POL1_P2;
    pulseOutLATCWork_M0 = BIT_PORTC_M0_REV_POL1_P2;

    m0PulseOutChangeFunc = SetPulseOutLATxWork_M0_REV_POL1_OFF;
    OutRequestFlug = ON;
}
//------------------------------------------------------------------------------
//....SetPulseOutLATxWork_M0_REV_POL1_OFF

void SetPulseOutLATxWork_M0_REV_POL1_OFF(void)
{
    pulseOutLATAWork_M0 = 0;
    pulseOutLATBWork_M0 = 0;
    pulseOutLATCWork_M0 = 0;

    polarityBit ^= BIT_MOTOR_M0;

    m0PulseOutChangeFunc = SetPulseOutLATxWork_M0_REV_POL0_P1;
    OutRequestFlug = ON;
}
//------------------------------------------------------------------------------
//..M1
//...FWD_POL0
//....SetPulseOutLATxWork_M1_FWD_POL0_P1

void SetPulseOutLATxWork_M1_FWD_POL0_P1(void)
{
    pulseOutLATAWork_M1 = BIT_PORTA_M1_FWD_POL0_P1;
    pulseOutLATBWork_M1 = BIT_PORTB_M1_FWD_POL0_P1;
    pulseOutLATCWork_M1 = BIT_PORTC_M1_FWD_POL0_P1;

    m1PulseOutChangeFunc = SetPulseOutLATxWork_M1_FWD_POL0_P2;
    OutRequestFlug = ON;
}
//------------------------------------------------------------------------------
//....SetPulseOutLATxWork_M1_FWD_POL0_P2

void SetPulseOutLATxWork_M1_FWD_POL0_P2(void)
{
    pulseOutLATAWork_M1 = BIT_PORTA_M1_FWD_POL0_P2;
    pulseOutLATBWork_M1 = BIT_PORTB_M1_FWD_POL0_P2;
    pulseOutLATCWork_M1 = BIT_PORTC_M1_FWD_POL0_P2;

    m1PulseOutChangeFunc = SetPulseOutLATxWork_M1_FWD_POL0_OFF;
    OutRequestFlug = ON;
}
//------------------------------------------------------------------------------
//....SetPulseOutLATxWork_M1_FWD_POL0_OFF

void SetPulseOutLATxWork_M1_FWD_POL0_OFF(void)
{
    pulseOutLATAWork_M1 = 0;
    pulseOutLATBWork_M1 = 0;
    pulseOutLATCWork_M1 = 0;

    polarityBit ^= BIT_MOTOR_M1;

    if (addShakePulseBit & BIT_MOTOR_M1) {
        m1PulseOutChangeFunc = SetPulseOutLATxWork_M1_SHAKE_POL1_P0;
    } else {
        m1PulseOutChangeFunc = SetPulseOutLATxWork_M1_FWD_POL1_P1;
    }

    OutRequestFlug = ON;

}
//------------------------------------------------------------------------------
//...FWD_POL1
//....SetPulseOutLATxWork_M1_FWD_POL1_P1

void SetPulseOutLATxWork_M1_FWD_POL1_P1(void)
{
    pulseOutLATAWork_M1 = BIT_PORTA_M1_FWD_POL1_P1;
    pulseOutLATBWork_M1 = BIT_PORTB_M1_FWD_POL1_P1;
    pulseOutLATCWork_M1 = BIT_PORTC_M1_FWD_POL1_P1;

    m1PulseOutChangeFunc = SetPulseOutLATxWork_M1_FWD_POL1_P2;
    OutRequestFlug = ON;
}
//------------------------------------------------------------------------------
//....SetPulseOutLATxWork_M1_FWD_POL1_P2

void SetPulseOutLATxWork_M1_FWD_POL1_P2(void)
{
    pulseOutLATAWork_M1 = BIT_PORTA_M1_FWD_POL1_P2;
    pulseOutLATBWork_M1 = BIT_PORTB_M1_FWD_POL1_P2;
    pulseOutLATCWork_M1 = BIT_PORTC_M1_FWD_POL1_P2;

    m1PulseOutChangeFunc = SetPulseOutLATxWork_M1_FWD_POL1_OFF;
    OutRequestFlug = ON;
}
//------------------------------------------------------------------------------
//....SetPulseOutLATxWork_M1_FWD_POL1_OFF

void SetPulseOutLATxWork_M1_FWD_POL1_OFF(void)
{
    pulseOutLATAWork_M1 = 0;
    pulseOutLATBWork_M1 = 0;
    pulseOutLATCWork_M1 = 0;

    polarityBit ^= BIT_MOTOR_M1;

    if (addShakePulseBit & BIT_MOTOR_M1) {
        m1PulseOutChangeFunc = SetPulseOutLATxWork_M1_SHAKE_POL0_P0;
    } else {
        m1PulseOutChangeFunc = SetPulseOutLATxWork_M1_FWD_POL0_P1;
    }

    OutRequestFlug = ON;

}
//------------------------------------------------------------------------------
//...REV_POL0
//....SetPulseOutLATxWork_M1_REV_POL0_P1

void SetPulseOutLATxWork_M1_REV_POL0_P1(void)
{
    pulseOutLATAWork_M1 = BIT_PORTA_M1_REV_POL0_P1;
    pulseOutLATBWork_M1 = BIT_PORTB_M1_REV_POL0_P1;
    pulseOutLATCWork_M1 = BIT_PORTC_M1_REV_POL0_P1;

    m1PulseOutChangeFunc = SetPulseOutLATxWork_M1_REV_POL0_P2;
    OutRequestFlug = ON;
}
//------------------------------------------------------------------------------
//....SetPulseOutLATxWork_M1_REV_POL0_P2

void SetPulseOutLATxWork_M1_REV_POL0_P2(void)
{
    pulseOutLATAWork_M1 = BIT_PORTA_M1_REV_POL0_P2;
    pulseOutLATBWork_M1 = BIT_PORTB_M1_REV_POL0_P2;
    pulseOutLATCWork_M1 = BIT_PORTC_M1_REV_POL0_P2;

    m1PulseOutChangeFunc = SetPulseOutLATxWork_M1_REV_POL0_OFF;
    OutRequestFlug = ON;
}
//------------------------------------------------------------------------------
//....SetPulseOutLATxWork_M1_REV_POL0_OFF

void SetPulseOutLATxWork_M1_REV_POL0_OFF(void)
{
    pulseOutLATAWork_M1 = 0;
    pulseOutLATBWork_M1 = 0;
    pulseOutLATCWork_M1 = 0;

    polarityBit ^= BIT_MOTOR_M1;

    m1PulseOutChangeFunc = SetPulseOutLATxWork_M1_REV_POL1_P1;
    OutRequestFlug = ON;
}
//------------------------------------------------------------------------------
//...REV_POL1
//....SetPulseOutLATxWork_M1_REV_POL1_P1

void SetPulseOutLATxWork_M1_REV_POL1_P1(void)
{
    pulseOutLATAWork_M1 = BIT_PORTA_M1_REV_POL1_P1;
    pulseOutLATBWork_M1 = BIT_PORTB_M1_REV_POL1_P1;
    pulseOutLATCWork_M1 = BIT_PORTC_M1_REV_POL1_P1;

    m1PulseOutChangeFunc = SetPulseOutLATxWork_M1_REV_POL1_P2;
    OutRequestFlug = ON;
}
//------------------------------------------------------------------------------
//....SetPulseOutLATxWork_M1_REV_POL1_P2

void SetPulseOutLATxWork_M1_REV_POL1_P2(void)
{
    pulseOutLATAWork_M1 = BIT_PORTA_M1_REV_POL1_P2;
    pulseOutLATBWork_M1 = BIT_PORTB_M1_REV_POL1_P2;
    pulseOutLATCWork_M1 = BIT_PORTC_M1_REV_POL1_P2;

    m1PulseOutChangeFunc = SetPulseOutLATxWork_M1_REV_POL1_OFF;
    OutRequestFlug = ON;
}
//------------------------------------------------------------------------------
//....SetPulseOutLATxWork_M1_REV_POL1_OFF

void SetPulseOutLATxWork_M1_REV_POL1_OFF(void)
{
    pulseOutLATAWork_M1 = 0;
    pulseOutLATBWork_M1 = 0;
    pulseOutLATCWork_M1 = 0;

    polarityBit ^= BIT_MOTOR_M1;

    m1PulseOutChangeFunc = SetPulseOutLATxWork_M1_REV_POL0_P1;
    OutRequestFlug = ON;
}
//------------------------------------------------------------------------------
//.パルス設定（揺動パルス）
//..M0
//...SHAKE POL0
//....SetPulseOutLATxWork_M0_SHAKE_POL0_P0

void SetPulseOutLATxWork_M0_SHAKE_POL0_P0(void)
{
    pulseOutLATAWork_M0 = BIT_PORTA_M0_SHAKE_POL0_P0;
    pulseOutLATBWork_M0 = BIT_PORTB_M0_SHAKE_POL0_P0;
    pulseOutLATCWork_M0 = BIT_PORTC_M0_SHAKE_POL0_P0;

    m0PulseOutChangeFunc = SetPulseOutLATxWork_M0_SHAKE_POL0_OFF;
    OutRequestFlug = ON;
}
//------------------------------------------------------------------------------
//....SetPulseOutLATxWork_M0_SHAKE_POL0_OFF

void SetPulseOutLATxWork_M0_SHAKE_POL0_OFF(void)
{
    pulseOutLATAWork_M0 = 0;
    pulseOutLATBWork_M0 = 0;
    pulseOutLATCWork_M0 = 0;

    //次のタイミングから検出をするためにまずは、検出抵抗ONしショートしておく
    dataTRISAWork_M0 = BIT_TRISA_M0_SPK_POL0_SHT;
    dataTRISBWork_M0 = BIT_TRISB_M0_SPK_POL0_SHT;
    dataTRISCWork_M0 = BIT_TRISC_M0_SPK_POL0_SHT; //検出抵抗ONしO4側ショート

    m0PulseOutChangeFunc = SetDataTRISxWork_M0_SPK_POL0_OPEN;
    OutRequestFlug = ON;
    OutTrisRequestFlug = ON;
    reqComparatorOnBit |= BIT_MOTOR_M0; //コンパレータON要求
}
//------------------------------------------------------------------------------
//....SetDataTRISxWork_M0_SPK_POL0_OPEN

void SetDataTRISxWork_M0_SPK_POL0_OPEN(void)
{
    dataTRISAWork_M0 = BIT_TRISA_M0_SPK_POL0_OPN;
    dataTRISBWork_M0 = BIT_TRISB_M0_SPK_POL0_OPN;
    dataTRISCWork_M0 = BIT_TRISC_M0_SPK_POL0_OPN; //O4側OPEN

    m0PulseOutChangeFunc = SetDataTRISxWork_M0_SPK_POL0_SHORT;
    OutTrisRequestFlug = ON;
}
//------------------------------------------------------------------------------
//....SetDataTRISxWork_M0_SPK_POL0_SHORT

void SetDataTRISxWork_M0_SPK_POL0_SHORT(void)
{
    dataTRISAWork_M0 = BIT_TRISA_M0_SPK_POL0_SHT;
    dataTRISBWork_M0 = BIT_TRISB_M0_SPK_POL0_SHT;
    dataTRISCWork_M0 = BIT_TRISC_M0_SPK_POL0_SHT; //O4側ショート

    if (m0PulseOutStep == ID_PULSE_OUT_STEP_P1) {
        m0PulseOutChangeFunc = SetPulseOutLATxWork_M0_FWD_POL0_P1;
        reqComparatorOffBit |= BIT_MOTOR_M0; //コンパレータOFF要求
    } else {
        m0PulseOutChangeFunc = SetDataTRISxWork_M0_SPK_POL0_OPEN;
    }

    OutTrisRequestFlug = ON;
    reqVrsCheckBit |= BIT_MOTOR_M0; //Vrsチェック要求
}
//------------------------------------------------------------------------------
//...SHAKE POL1
//....SetPulseOutLATxWork_M0_SHAKE_POL1_P0

void SetPulseOutLATxWork_M0_SHAKE_POL1_P0(void)
{
    pulseOutLATAWork_M0 = BIT_PORTA_M0_SHAKE_POL1_P0;
    pulseOutLATBWork_M0 = BIT_PORTB_M0_SHAKE_POL1_P0;
    pulseOutLATCWork_M0 = BIT_PORTC_M0_SHAKE_POL1_P0;

    m0PulseOutChangeFunc = SetPulseOutLATxWork_M0_SHAKE_POL1_OFF;
    OutRequestFlug = ON;
}
//------------------------------------------------------------------------------
//....SetPulseOutLATxWork_M0_SHAKE_POL1_OFF

void SetPulseOutLATxWork_M0_SHAKE_POL1_OFF(void)
{
    pulseOutLATAWork_M0 = 0;
    pulseOutLATBWork_M0 = 0;
    pulseOutLATCWork_M0 = 0;

    //次のタイミングから検出をするためにまずは、検出抵抗ONしショートしておく
    dataTRISAWork_M0 = BIT_TRISA_M0_SPK_POL1_SHT; //O3側ショート
    dataTRISBWork_M0 = BIT_TRISB_M0_SPK_POL1_SHT;
    dataTRISCWork_M0 = BIT_TRISC_M0_SPK_POL1_SHT; //検出抵抗ON

    m0PulseOutChangeFunc = SetDataTRISxWork_M0_SPK_POL1_OPEN;
    OutRequestFlug = ON;
    OutTrisRequestFlug = ON;
    reqComparatorOnBit |= BIT_MOTOR_M0; //コンパレータON要求
}
//------------------------------------------------------------------------------
//....SetDataTRISxWork_M0_SPK_POL1_OPEN

void SetDataTRISxWork_M0_SPK_POL1_OPEN(void)
{
    dataTRISAWork_M0 = BIT_TRISA_M0_SPK_POL1_OPN; //O3側OPEN
    dataTRISBWork_M0 = BIT_TRISB_M0_SPK_POL1_OPN;
    dataTRISCWork_M0 = BIT_TRISC_M0_SPK_POL1_OPN;

    m0PulseOutChangeFunc = SetDataTRISxWork_M0_SPK_POL1_SHORT;
    OutTrisRequestFlug = ON;
}
//------------------------------------------------------------------------------
//....SetDataTRISxWork_M0_SPK_POL1_SHORT

void SetDataTRISxWork_M0_SPK_POL1_SHORT(void)
{
    dataTRISAWork_M0 = BIT_TRISA_M0_SPK_POL1_SHT; //O3側ショート
    dataTRISBWork_M0 = BIT_TRISB_M0_SPK_POL1_SHT;
    dataTRISCWork_M0 = BIT_TRISC_M0_SPK_POL1_SHT;

    if (m0PulseOutStep == ID_PULSE_OUT_STEP_P1) {
        m0PulseOutChangeFunc = SetPulseOutLATxWork_M0_FWD_POL1_P1;
        reqComparatorOffBit |= BIT_MOTOR_M0; //コンパレータOFF要求
    } else {
        m0PulseOutChangeFunc = SetDataTRISxWork_M0_SPK_POL1_OPEN;
    }

    OutTrisRequestFlug = ON;
    reqVrsCheckBit |= BIT_MOTOR_M0; //Vrsチェック要求
}
//------------------------------------------------------------------------------
//..M1
//...SHAKE POL0
//....SetPulseOutLATxWork_M1_SHAKE_POL0_P0

void SetPulseOutLATxWork_M1_SHAKE_POL0_P0(void)
{
    pulseOutLATAWork_M1 = BIT_PORTA_M1_SHAKE_POL0_P0;
    pulseOutLATBWork_M1 = BIT_PORTB_M1_SHAKE_POL0_P0;
    pulseOutLATCWork_M1 = BIT_PORTC_M1_SHAKE_POL0_P0;

    m1PulseOutChangeFunc = SetPulseOutLATxWork_M1_SHAKE_POL0_OFF;
    OutRequestFlug = ON;
}
//------------------------------------------------------------------------------
//....SetPulseOutLATxWork_M1_SHAKE_POL0_OFF

void SetPulseOutLATxWork_M1_SHAKE_POL0_OFF(void)
{
    pulseOutLATAWork_M1 = 0;
    pulseOutLATBWork_M1 = 0;
    pulseOutLATCWork_M1 = 0;

    //次のタイミングから検出をするためにまずは、検出抵抗ONしショートしておく
    dataTRISAWork_M1 = BIT_TRISA_M1_SPK_POL0_SHT;
    dataTRISBWork_M1 = BIT_TRISB_M1_SPK_POL0_SHT;
    dataTRISCWork_M1 = BIT_TRISC_M1_SPK_POL0_SHT; //検出抵抗ONしO4側ショート

    m1PulseOutChangeFunc = SetDataTRISxWork_M1_SPK_POL0_OPEN;
    OutRequestFlug = ON;
    OutTrisRequestFlug = ON;
    reqComparatorOnBit |= BIT_MOTOR_M1; //コンパレータON要求
}
//------------------------------------------------------------------------------
//....SetDataTRISxWork_M1_SPK_POL0_OPEN

void SetDataTRISxWork_M1_SPK_POL0_OPEN(void)
{
    dataTRISAWork_M1 = BIT_TRISA_M1_SPK_POL0_OPN;
    dataTRISBWork_M1 = BIT_TRISB_M1_SPK_POL0_OPN;
    dataTRISCWork_M1 = BIT_TRISC_M1_SPK_POL0_OPN; //O4側OPEN

    m1PulseOutChangeFunc = SetDataTRISxWork_M1_SPK_POL0_SHORT;
    OutTrisRequestFlug = ON;
}
//------------------------------------------------------------------------------
//....SetDataTRISxWork_M1_SPK_POL0_SHORT

void SetDataTRISxWork_M1_SPK_POL0_SHORT(void)
{
    dataTRISAWork_M1 = BIT_TRISA_M1_SPK_POL0_SHT;
    dataTRISBWork_M1 = BIT_TRISB_M1_SPK_POL0_SHT;
    dataTRISCWork_M1 = BIT_TRISC_M1_SPK_POL0_SHT; //O4側ショート

    if (m1PulseOutStep == ID_PULSE_OUT_STEP_P1) {
        m1PulseOutChangeFunc = SetPulseOutLATxWork_M1_FWD_POL0_P1;
        reqComparatorOffBit |= BIT_MOTOR_M1; //コンパレータOFF要求
    } else {
        m1PulseOutChangeFunc = SetDataTRISxWork_M1_SPK_POL0_OPEN;
    }

    OutTrisRequestFlug = ON;
    reqVrsCheckBit |= BIT_MOTOR_M1; //Vrsチェック要求
}
//------------------------------------------------------------------------------
//...SHAKE POL1
//....SetPulseOutLATxWork_M0_SHAKE_POL1_P0

void SetPulseOutLATxWork_M1_SHAKE_POL1_P0(void)
{
    pulseOutLATAWork_M1 = BIT_PORTA_M1_SHAKE_POL1_P0;
    pulseOutLATBWork_M1 = BIT_PORTB_M1_SHAKE_POL1_P0;
    pulseOutLATCWork_M1 = BIT_PORTC_M1_SHAKE_POL1_P0;

    m1PulseOutChangeFunc = SetPulseOutLATxWork_M1_SHAKE_POL1_OFF;
    OutRequestFlug = ON;
}
//------------------------------------------------------------------------------
//....SetPulseOutLATxWork_M1_SHAKE_POL1_OFF

void SetPulseOutLATxWork_M1_SHAKE_POL1_OFF(void)
{
    pulseOutLATAWork_M1 = 0;
    pulseOutLATBWork_M1 = 0;
    pulseOutLATCWork_M1 = 0;

    //次のタイミングから検出をするためにまずは、検出抵抗ONしショートしておく
    dataTRISAWork_M1 = BIT_TRISA_M1_SPK_POL1_SHT;
    dataTRISBWork_M1 = BIT_TRISB_M1_SPK_POL1_SHT;
    dataTRISCWork_M1 = BIT_TRISC_M1_SPK_POL1_SHT; //検出抵抗ONしO3側ショート

    m1PulseOutChangeFunc = SetDataTRISxWork_M1_SPK_POL1_OPEN;
    OutRequestFlug = ON;
    OutTrisRequestFlug = ON;
    reqComparatorOnBit |= BIT_MOTOR_M1; //コンパレータON要求
}
//------------------------------------------------------------------------------
//....SetDataTRISxWork_M1_SPK_POL1_OPEN

void SetDataTRISxWork_M1_SPK_POL1_OPEN(void)
{
    dataTRISAWork_M1 = BIT_TRISA_M1_SPK_POL1_OPN;
    dataTRISBWork_M1 = BIT_TRISB_M1_SPK_POL1_OPN;
    dataTRISCWork_M1 = BIT_TRISC_M1_SPK_POL1_OPN; //O3側OPEN

    m1PulseOutChangeFunc = SetDataTRISxWork_M1_SPK_POL1_SHORT;
    OutTrisRequestFlug = ON;
}
//------------------------------------------------------------------------------
//....SetDataTRISxWork_M1_SPK_POL1_SHORT

void SetDataTRISxWork_M1_SPK_POL1_SHORT(void)
{
    dataTRISAWork_M1 = BIT_TRISA_M1_SPK_POL1_SHT;
    dataTRISBWork_M1 = BIT_TRISB_M1_SPK_POL1_SHT;
    dataTRISCWork_M1 = BIT_TRISC_M1_SPK_POL1_SHT; //O3側ショート

    if (m1PulseOutStep == ID_PULSE_OUT_STEP_P1) {
        m1PulseOutChangeFunc = SetPulseOutLATxWork_M1_FWD_POL1_P1;
        reqComparatorOffBit |= BIT_MOTOR_M1; //コンパレータOFF要求
    } else {
        m1PulseOutChangeFunc = SetDataTRISxWork_M1_SPK_POL1_OPEN;
    }

    OutTrisRequestFlug = ON;
    reqVrsCheckBit |= BIT_MOTOR_M1; //Vrsチェック要求
}
//------------------------------------------------------------------------------

