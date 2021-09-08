//.filename MotorDrive.c
#include "mcc.h"
#include "MotorDrive.h"
#include "misc.h"
#include "data_types.h"
#include "spi_control.h"
/***************************************************************************//**
 @brief     TMR2$B3d$j9~$_$G%b!<%?$r6nF0$5$;$k(B
 @note      MOTOR$B%]!<%H3dEv(B

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
//..RC4$B=PNO@Z$jBX$((B
//#define		DEBUG_TIMER2            // TIMER2$B=hM};~4VB,DjMQ(B
//#define		DEBUG_VRS0			  	// $BIi2Y8!=P>l=j(B
//#define		DEBUG_CORRECT_POSITION	// $B0LCVJd@5%G%P%C%0(B

//#define		DEBUG_TEST_GEAR

#define     TOUCH_TEST          //$B?(FI%H%k%/B,DjMQ%G%P%C%0(BOPTION$B!!DL>o!'%3%a%s%H%"%&%H(B
/*******************************************************************************
$B$3$N(BFW$B$O!"%H%k%/B,DjMQ$K?(FI(BONSW$B$,F~NO$5$l$k$H!"Ne<'(BON$B$H$J$k!#(B
$B$=$N:]!"(BSPK$BM-$jL5$7$rNe<'M-$jL5$7(BSW$B$GA*Br$G$-$k(B
********************************************************************************/
//.$BDj?t(B
/**-- M0$B=PNO%]!<%H(B --**/
/**-- $B8!=PDq93$OL5A`:n(B --**/
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

/**-- M1$B=PNO%]!<%H(B --**/
/**-- $B8!=PDq93$OL5A`:n(B --**/
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

/*  $BF~=PNO%l%8%9%?(BTRISX$B!!(B*/
/**-- M0$BF~=PNO@Z$jBX$((B --**/
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
/* $B?(FIMQ(B******************************************************************/
#define     BIT_TRISA_M0_SHT    (BIT_TRISA_M0_O3_SHT | BIT_TRISA_M0_O4_SHT)
#define     BIT_TRISB_M0_SHT    (BIT_TRISB_M0_O3_SHT | BIT_TRISB_M0_O4_SHT)
#define     BIT_TRISC_M0_SHT    (BIT_TRISC_M0_O3_SHT | BIT_TRISC_M0_O4_SHT)

#define     BIT_PORTA_OUT1   (BIT_PORTA_M0_OUT1 | BIT_PORTA_M1_OUT1)
#define     BIT_PORTB_OUT1   (BIT_PORTB_M0_OUT1 | BIT_PORTB_M1_OUT1)
#define     BIT_PORTC_OUT1   (BIT_PORTC_M0_OUT1 | BIT_PORTC_M1_OUT1)
/**************************************************************************/
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

/*  $BF~=PNO%l%8%9%?(BTRISX$B!!(B*/
/**-- M1$BF~=PNO@Z$jBX$((B --**/
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
/* $B?(FIMQ(B******************************************************************/
#define     BIT_TRISA_M1_SHT    (BIT_TRISA_M1_O3_SHT | BIT_TRISA_M1_O4_SHT)
#define     BIT_TRISB_M1_SHT    (BIT_TRISB_M1_O3_SHT | BIT_TRISB_M1_O4_SHT)
#define     BIT_TRISC_M1_SHT    (BIT_TRISC_M1_O3_SHT | BIT_TRISC_M1_O4_SHT)

/* $B?(FIMQ(BTime$B@_DjCM(B*/
#define     EXCITATING_OFF_MSK_TIME  82      //$BNe<'(BOFF$B8e$N%^%9%/;~4V(B(=10msec)
#define     EXCITATING_TIME  8197        //$BNe<'(BON$B7QB3;~4V(B(=1sec)
/* $B?(FIMQ!!%$%s%8%1!<%?(BLED*/
#define     BIT_PORTA_LED_ON    b0000_0100

#define     CHANGE_POSITION_OPEN    1       //$BJQ2=%]%$%s%H!!(BOPEN
#define     CHANGE_POSITION_SHORT   0       //$BJQ2=%]%$%s%H!!(BSHORT

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**************************************************************************/
/* $B%3%s%Q%l!<%?@_DjCM(B */
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
//...M0_SHAKE $BMIF0%Q%k%9(B+SPK
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
//...SHAKE$B!!MIF0%Q%k%9(B+SPK
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
//..$B%Q%k%9=PNO@_DjCM(B
/*******************************************************************************
 @brief     Position$B%+%&%s%?%F!<%V%k!"(B
            mxPulsePositionCt$B$,$3$N%F!<%V%k$NCM$K0lCW$7$?$i!"(B
            $B4X?t%]%$%s%?<B9T$7%Q%k%9@Z$jBX$((B
 @note
 @author
 @date
 ********************************************************************************/
//...mxPulsePositionCt
#ifdef DEBUG_TEST_GEAR
//	#define     WIDTH_PULSE_P0              5           // P0$B%Q%k%9I}(B                        4:0.488ms / 5:0.610ms / 6:0.732ms
#define     WIDTH_PULSE_P0              4           // P0$B%Q%k%9I}(B                        4:0.488ms / 5:0.610ms / 6:0.732ms
#else
#define     WIDTH_PULSE_P0              5           // P0$B%Q%k%9I}(B(1$B9f5!(B)                 4:0.488ms / 5:0.610ms / 6:0.732ms
//	#define     WIDTH_PULSE_P0              5           // P0$B%Q%k%9I}(B(2$B9f5!(B)                 4:0.488ms / 5:0.610ms / 6:0.732ms
#endif

#define     WIDTH_SPK_START_WAIT            4           // P0$B$N$"$H$N(BSPK$B3+;OBT$A;~4V(B         4:0.488ms
#define     WIDTH_SPK_ON                    1           // SPK_ON$BI}(B                          1:0.122ms
#define     WIDTH_SPK_OFF                   3           // SPK_OFF$BI}(B                         3:0.366ms
#define     NUMBER_OF_REPEAT_SPK            10          // SPK$BK\?t(B                          10:4.880ms
#define     WIDTH_PULSE_P1                  6           // P1$B%Q%k%9I}(B                        6:0.732ms
#define     WIDTH_PULSE_P2                  16          // P2$B%Q%k%9I}(B                       16:1.952ms
#define     WIDTH_PULSE_OFF_MIN             7           // P2$B$N$"$H$N:GDc(BOFF$B3NJ];~4V(B         7:0.854ms ( 200Hz$B$h$j$bAa$$4V3V$N$_$r3NJ](B )
#define     WIDTH_PULSE_OFF_DEF             19          // P2$B$N$"$H$NI8=`(BOFF$B3NJ];~4V(B        19:2.318ms

#define     WIDTH_SHAKE_PULSE_ALL           ( POS_SHAKE_INIT_DATA + WIDTH_PULSE_P0 + WIDTH_SPK_START_WAIT + ( WIDTH_SPK_ON + WIDTH_SPK_OFF ) * NUMBER_OF_REPEAT_SPK )

#define     POS_P0_START                    1                           // $BI,$:(B1$B0J>e$N?t(B
#define     POS_P1_START                    ( 1 + WIDTH_SHAKE_PULSE_ALL )

#define     POS_SHAKE_INIT_DATA             ( POS_P0_START - 1 )        // POS_P0_START$B$N0l$DA0$K$9$k!J=i$a$N(B1$B%+%&%s%H$OH>C<$J;~4V$K$J$k$?$a!K(B
#define     POS_P1P2_INIT_DATA              ( POS_P1_START - 1 )        // POS_P1_START$B$N0l$DA0$K$9$k!J=i$a$N(B1$B%+%&%s%H$OH>C<$J;~4V$K$J$k$?$a!K(B


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
//..$B0LCV8!=P@_DjCM(B
//...$B8!=P>uBV(BID

enum ID_SEARCH_STAT {
    ID_SEARCH_FINE_POS_1ST = 0, // $B:G=i$NHyD4@00LCV%5!<%A(B
    ID_SEARCH_FINE_ROUGH_POS, // $BHyD4@0!&AFD4@0$N0LCV%5!<%A(B
    ID_SEARCH_MAX
};
//------------------------------------------------------------------------------
//...$B8!=P7k2L(BID

enum ID_SEARCH_RESULT {
    ID_SEARCH_RESULT_NULL = 0, // $B7k2L$J$7!JA0$NIi2Y$NB3$-$H$+!K(B
    ID_SEARCH_RESULT_NEW_POS, // $B?75,0LCV8!=P(B
    ID_SEARCH_RESULT_CONT, // $BA0$NB3$-(B
    ID_SEARCH_RESULT_RETRY, // $B0LCV8!=P<:GT"*$d$jD>$7(B
    ID_SEARCH_RESULT_SUCCESS, // $B0LCV8!=P@.8y(B
    ID_SEARCH_RESULT_ERR // $B0LCV8!=P<:GT"*%?%$%`%*!<%P!<(B
};
//------------------------------------------------------------------------------
//...$B8!=P$7$?Ii2Y$N<oJL(B
#define MARK_DETECT_LOAD_TYPE_CONT      10

enum ID_DETECT_LOAD_TYPE {
    ID_DETECT_LOAD_NULL = 0, // $B8!=P$J$7(B
    ID_DETECT_LOAD_FINE = 1, // $BHyD4@00LCV$r8!=P(B
    ID_DETECT_LOAD_ROUGH = 2, // $BAFD4@00LCV$r8!=P(B
    ID_DETECT_LOAD_FINE_CONT = (ID_DETECT_LOAD_FINE + MARK_DETECT_LOAD_TYPE_CONT), // $BHyD4@00LCVB3$-$r8!=P(B
    ID_DETECT_LOAD_ROUGH_CONT = (ID_DETECT_LOAD_ROUGH + MARK_DETECT_LOAD_TYPE_CONT), // $BAFD4@00LCVB3$-$r8!=P(B
    ID_DETECT_LOAD_MAX
};
//------------------------------------------------------------------------------
//...$B8!=P%;%C%F%#%s%0(B
#define     NUMBER_OF_FINE_POSITION     8           // 1$B<~$G(B8$B2U=jHyD4@00LCV$O8=$l$k(B
#define     NUMBER_OF_ROUGH_POSITION    1           // 1$B<~$G(B1$B2U=jAFD4@00LCV$O8=$l$k(B

#define     MAX_FINE_DETECTION_NUMBER   2           // 1$B$D$NHyD4@00LCV$OO"B3(B2$B2U=j$^$GIi2Y$,8=$l$F$b2D$H$9$k(B
#define     MAX_ROUGH_DETECTION_NUMBER  9           // 1$B$D$NAFD4@00LCV$OO"B3(B5$B2U=j$^$GIi2Y$,8=$l$F$b2D$H$9$k(B

#define     MAX_INTERVAL_FINE_CONT      2           // $BHyD4@00LCV$NO"B35vMF>r7o(B
#define     MAX_INTERVAL_ROUGH_CONT     3           // $BAFD4@00LCV$NO"B35vMF>r7o(B

#define     THRESHOLD_FINE_DIFF_UPPER   47          // $BHyD4@00LCV$O(B46step$B0J2<N%$l$F$$$k$H2>Dj$9$k(B
#define     THRESHOLD_FINE_DIFF_LOWER   43          // $BHyD4@00LCV$O(B44step$B0J>eN%$l$F$$$k$H2>Dj$9$k(B
// $BHyD4@00LCV$O(B44,45,46$B$r9MN8$7$F$*$/(B
// $B$D$^$jIi2Y0LCV$N:9$,(B44$BL$K~$N2U=j$OAFD40LCV$HH=Dj$9$k(B
#define     THRESHOLD_ROUGH_DIFF_UPPER  2           // $BAFD4@00LCV$O(B2step$B$h$jB?$/N%$l$F8!=P$5$l$J$$(B

#define     MAX_SEARCH_ROTATION         3           // $B8!=P$K$+$1$k:GBg2sE>?t(B

#define     MAX_LOAD_POSITION_ARRAY_LENGTH      ( ( ( NUMBER_OF_FINE_POSITION * MAX_FINE_DETECTION_NUMBER ) + ( NUMBER_OF_ROUGH_POSITION * MAX_ROUGH_DETECTION_NUMBER ) ) - 2 )         // $BIi2Y0LCV5vMF?t(B


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
//	#define     OFFSET_M0_BASE_POSITION       212         // $B4p=`0LCV(B
//	#define     OFFSET_M1_BASE_POSITION        6        // $B4p=`0LCV(B

#define     OFFSET_M0_BASE_POSITION        0         // $B4p=`0LCV(B
#define     OFFSET_M1_BASE_POSITION        0 	      // $B4p=`0LCV(B
#else
/*1$B9f5!(B*/
//	#define     OFFSET_M0_BASE_POSITION        293         // $B4p=`0LCV(B
//	#define     OFFSET_M1_BASE_POSITION        294        // $B4p=`0LCV(B

#define     OFFSET_M0_BASE_POSITION        -24      // $B4p=`0LCV(B
#define     OFFSET_M1_BASE_POSITION        -16        // $B4p=`0LCV(B

/*2$B9f5!(B*/
//	#define     OFFSET_M0_BASE_POSITION        12         // $B4p=`0LCV(B
//	#define     OFFSET_M1_BASE_POSITION        35        // $B4p=`0LCV(B

#endif
//------------------------------------------------------------------------------
//.$B%W%m%H%?%$%W@k8@(B
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
//.$B%F!<%V%k%G!<%?(B
#if 0
static UB TblM0PulseChangePosition[ PULSE_CHANGE_ARRAY_LENGTH ];
static UB TblM1PulseChangePosition[ PULSE_CHANGE_ARRAY_LENGTH ];
#endif
static const UB TblM0PulseChangePosition[ PULSE_CHANGE_ARRAY_LENGTH ] = {
    PULSE_CHANGE_START_P0, // P0$B3+;O%?%$%_%s%0(B		0
    PULSE_CHANGE_STOP_P0, // P0$B=*N;%?%$%_%s%0(B     1
    PULSE_CHANGE_START_WAITE, // SPK1$B3+;O%?%$%_%s%0(B   2
    PULSE_CHANGE_START_SPK_ON_1, // SPK1$B=*N;%?%$%_%s%0(B   3
    PULSE_CHANGE_START_SPK_OFF_1, // SPK2$B3+;O%?%$%_%s%0(B   4
    PULSE_CHANGE_START_SPK_ON_2, // SPK2$B=*N;%?%$%_%s%0(B   5
    PULSE_CHANGE_START_SPK_OFF_2, // SPK3$B3+;O%?%$%_%s%0(B   6
    PULSE_CHANGE_START_SPK_ON_3, // SPK3$B=*N;%?%$%_%s%0(B   7
    PULSE_CHANGE_START_SPK_OFF_3, // SPK4$B3+;O%?%$%_%s%0(B   8
    PULSE_CHANGE_START_SPK_ON_4, // SPK4$B=*N;%?%$%_%s%0(B   9
    PULSE_CHANGE_START_SPK_OFF_4, // SPK5$B3+;O%?%$%_%s%0(B   10
    PULSE_CHANGE_START_SPK_ON_5, // SPK5$B=*N;%?%$%_%s%0(B   11
    PULSE_CHANGE_START_SPK_OFF_5, // SPK6$B3+;O%?%$%_%s%0(B   12
    PULSE_CHANGE_START_SPK_ON_6, // SPK6$B=*N;%?%$%_%s%0(B   13
    PULSE_CHANGE_START_SPK_OFF_6, // SPK7$B3+;O%?%$%_%s%0(B   14
    PULSE_CHANGE_START_SPK_ON_7, // SPK7$B=*N;%?%$%_%s%0(B   15
    PULSE_CHANGE_START_SPK_OFF_7, // SPK8$B3+;O%?%$%_%s%0(B   16
    PULSE_CHANGE_START_SPK_ON_8, // SPK8$B=*N;%?%$%_%s%0(B   17
    PULSE_CHANGE_START_SPK_OFF_8, // SPK9$B3+;O%?%$%_%s%0(B   18
    PULSE_CHANGE_START_SPK_ON_9, // SPK9$B=*N;%?%$%_%s%0(B   19
    PULSE_CHANGE_START_SPK_OFF_9, // SPK10$B3+;O%?%$%_%s%0(B  20
    PULSE_CHANGE_START_SPK_ON_10, // SPK10$B=*N;%?%$%_%s%0(B  21
    PULSE_CHANGE_START_SPK_OFF_10, // P1$B3+;O%?%$%_%s%0(B     22
    PULSE_CHANGE_START_P1, // P2$B3+;O%?%$%_%s%0(B     23
    PULSE_CHANGE_START_P2 // OFF$B3+;O%?%$%_%s%0(B    24
};
//------------------------------------------------------------------------------
static const UB TblM1PulseChangePosition[ PULSE_CHANGE_ARRAY_LENGTH ] = {
    PULSE_CHANGE_START_P0, // P0$B3+;O%?%$%_%s%0(B		0
    PULSE_CHANGE_STOP_P0, // P0$B=*N;%?%$%_%s%0(B     1
    PULSE_CHANGE_START_WAITE, // SPK1$B3+;O%?%$%_%s%0(B   2
    PULSE_CHANGE_START_SPK_ON_1, // SPK1$B=*N;%?%$%_%s%0(B   3
    PULSE_CHANGE_START_SPK_OFF_1, // SPK2$B3+;O%?%$%_%s%0(B   4
    PULSE_CHANGE_START_SPK_ON_2, // SPK2$B=*N;%?%$%_%s%0(B   5
    PULSE_CHANGE_START_SPK_OFF_2, // SPK3$B3+;O%?%$%_%s%0(B   6
    PULSE_CHANGE_START_SPK_ON_3, // SPK3$B=*N;%?%$%_%s%0(B   7
    PULSE_CHANGE_START_SPK_OFF_3, // SPK4$B3+;O%?%$%_%s%0(B   8
    PULSE_CHANGE_START_SPK_ON_4, // SPK4$B=*N;%?%$%_%s%0(B   9
    PULSE_CHANGE_START_SPK_OFF_4, // SPK5$B3+;O%?%$%_%s%0(B   10
    PULSE_CHANGE_START_SPK_ON_5, // SPK5$B=*N;%?%$%_%s%0(B   11
    PULSE_CHANGE_START_SPK_OFF_5, // SPK6$B3+;O%?%$%_%s%0(B   12
    PULSE_CHANGE_START_SPK_ON_6, // SPK6$B=*N;%?%$%_%s%0(B   13
    PULSE_CHANGE_START_SPK_OFF_6, // SPK7$B3+;O%?%$%_%s%0(B   14
    PULSE_CHANGE_START_SPK_ON_7, // SPK7$B=*N;%?%$%_%s%0(B   15
    PULSE_CHANGE_START_SPK_OFF_7, // SPK8$B3+;O%?%$%_%s%0(B   16
    PULSE_CHANGE_START_SPK_ON_8, // SPK8$B=*N;%?%$%_%s%0(B   17
    PULSE_CHANGE_START_SPK_OFF_8, // SPK9$B3+;O%?%$%_%s%0(B   18
    PULSE_CHANGE_START_SPK_ON_9, // SPK9$B=*N;%?%$%_%s%0(B   19
    PULSE_CHANGE_START_SPK_OFF_9, // SPK10$B3+;O%?%$%_%s%0(B  20
    PULSE_CHANGE_START_SPK_ON_10, // SPK10$B=*N;%?%$%_%s%0(B  21
    PULSE_CHANGE_START_SPK_OFF_10, // P1$B3+;O%?%$%_%s%0(B     22
    PULSE_CHANGE_START_P1, // P2$B3+;O%?%$%_%s%0(B     23
    PULSE_CHANGE_START_P2 // OFF$B3+;O%?%$%_%s%0(B    24
};
//------------------------------------------------------------------------------
//.$BJQ?t(B
//.$B$=$NB>(B
UB motorBitOperation;
UB motorNoOperation;
//..$B6nF0%Q%k%9@8@.(B
static UB TblM0PulseChangePositionLast;
static UB TblM1PulseChangePositionLast;

static UB pulseOutLATA; // $B%b!<%?C<;R$+$i(Bpulse$B=PNO$9$k$H$-$N%]!<%H%G!<%?(B
static UB pulseOutLATB;
static UB pulseOutLATC;

static UB pulseOutLATAWork_M0; // M0$B%b!<%?C<;R$+$i(Bpulse$B=PNO$9$k$H$-$N%]!<%H%G!<%?(B
static UB pulseOutLATBWork_M0;
static UB pulseOutLATCWork_M0;

static UB pulseOutLATAWork_M1; // M1$B%b!<%?C<;R$+$i(Bpulse$B=PNO$9$k$H$-$N%]!<%H%G!<%?(B
static UB pulseOutLATBWork_M1;
static UB pulseOutLATCWork_M1;

static UB dataTRISA; // $B%]!<%H$NF~=PNO@_Dj%G!<%?!J:G=*CM!K(B
static UB dataTRISB;
static UB dataTRISC;

static UB dataTRISAWork_M0; // $B%]!<%H$NF~=PNO%G!<%?(BWork
static UB dataTRISBWork_M0;
static UB dataTRISCWork_M0;

static UB dataTRISAWork_M1; // $B%]!<%H$NF~=PNO%G!<%?(BWork
static UB dataTRISBWork_M1;
static UB dataTRISCWork_M1;

static UB m0PulsePositionCt; // $B%Q%k%9%$%s%?!<%P%k=*N;$^$G$N%+%&%s%H(B0$B!A(B255
static UB m1PulsePositionCt; // $B%Q%k%9%$%s%?!<%P%k=*N;$^$G$N%+%&%s%H(B0$B!A(B255

static UB m0PulsePositionCtWork; // $B>e5-$N%o!<%/(B
static UB m1PulsePositionCtWork; //

static UB m0PulseChangePosition; // pulsePositionCt$B$,$3$NCM$N$H$-$K!"(BpulseOutM0LATx$B$rJQ99$9$k(B
static UB m1PulseChangePosition; // pulsePositionCt$B$,$3$NCM$N$H$-$K!"(BpulseOutM1LATx$B$rJQ99$9$k(B

static void ( *m0PulseOutChangeFunc)(void) = NULL_PTR;
static void ( *m1PulseOutChangeFunc)(void) = NULL_PTR;

static UB m0PulseOutStep; // 0:P1$B=PNO!"(B1:P2$B=PNO!"(B2:LOW$B=PNO(B
static UB m1PulseOutStep; //

static UB m0PulseOutStepWork; // $B>e5-$N%o!<%/(B
static UB m1PulseOutStepWork; //

static UB finishPositionCtSet[ MOTOR_NUM ]; // $B6nF0<~GH?t$K1~$8$?(BmxPulsePositionCt$B$N:G8e$NCM(B

static UB polarityBit; // $B<!$K!"$^$?$O:#=PNO$9$k!J$7$F$$$k!K%b!<%?6K@-(B 0:$B<!$O(BCW1$B!"(B1:$B<!$O(BCW2

//++++++++++++++++++++++++++Touch Read++++++++++++++++++++++++++++++++++++++
//..$B?(FIBP1~(B
static UB excitatingEnableBit; // Vrs$B8!=P$7$?$H$-$KNe<'$5$;$k$+$5$;$J$$$+(B		0:$BNe<'$J$7!"(B1:$BNe<'$"$j(B
static UH excitatingTime; // $BNe<';~4V%+%&%s%H(B
static UB excitatingOffMaskTime; // $BNe<'(BOFF$B8e$N%^%9%/;~4V%+%&%s%H(B
static UB detectSpkTurnBit; // $B8!=PC<;R$N@Z$jBX$(%U%i%0(B 0:O4$B!!(B1:O3

static UB spkPositionCt; // SPK$B%Q%k%9%$%s%?!<%P%k=*N;$^$G$N%+%&%s%H(B0$B!A(B3
static UB spkChangePosition; // spkPositionCt$B$,$3$NCM$N;~=PNOJQ99=`Hw$9$k(B
static void ( *spkOutChangeFunc)(void) = NULL_PTR; //$B<!$K=PNO$9$k4X?t$X$N%]%$%s%?(B

static UB spkOutTrisRequestFlug; // spk TRISx$B$NJQ2=I,MW$J;~$N=PNOMW5a%U%i%0(B
static UB spkReqVrsCheckBit; // spk Vrs$B%A%'%C%/MW5a%U%i%0(B

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//------------------------------------------------------------------------------
//..$B6nF0@)8f(B
static UB driveType[ MOTOR_NUM ];
static UB driveDirectionBit; // $B2sE>J}8~(B0:$B5UE>!"(B1:$B@5E>(B
static UB addShakePulseBit; // $BMIF0%Q%k%9DI2C!!(B0:$BMIF0%Q%k%9L5$7!"(B1:$BMIF0%Q%k%9M-$j(B
static UB reqAfterMotorStopFuncBit; // $B%b!<%?6nF0=*N;;~$K<B9T$9$k=hM}(B
static UB reqStopMotorBit; // $B%b!<%?6nF0Dd;_MW5a(B

static UB motorActiveBit; // $B%b!<%?F0:nCf%S%C%H(B

static void ( * cbAfterMotorStopFunc[ MOTOR_NUM ])(UB motor_no);

static H motorStepCounterSet[ MOTOR_NUM ];
static H motorM0StepCounter;
static H motorM1StepCounter;

static UB correctDriveSteps[ MOTOR_NUM ]; // $BJd@56nF0%9%F%C%W?t(B
static UB correctDriveDirectionBit; // $BJd@56nF02sE>J}8~(B
static UB correctDriveFinishPositionCtSet[ MOTOR_NUM ]; // $B6nF0<~GH?t$K1~$8$?(BmxPulsePositionCt$B$N:G8e$NCM(B
//------------------------------------------------------------------------------
//..$BMIF0%Q%k%9@)8f(B
static UB OutRequestFlug; // PulseChangePosition$B$GJQ2=I,MW$J;~$N=PNOMW5a%U%i%0(B
static UB OutTrisRequestFlug; // TRISx$B$NJQ2=I,MW$J;~$N=PNOMW5a%U%i%0(B

static UB vrsCountValue[ MOTOR_NUM ]; // Vcomp$B1[$($N(BVrs$BK\?t%+%&%s%HCM(B IF$B$G%+%&%s%H(B

static UB reqComparatorOnBit; // $B%3%s%Q%l!<%?(BON$BMW5a%U%i%0(B
static UB reqComparatorOffBit; // $B%3%s%Q%l!<%?(BOFF$BMW5a%U%i%0(B
static UB reqVrsCheckBit; // Vrs$B%A%'%C%/MW5a%U%i%0(B
//------------------------------------------------------------------------------
//..$B?K0LCV8!=P(B
UB handPositionActiveBit; // 0:$B?K0LCVL58z"*0LCV8!=P$,L$N;!"(B1:$B?K0LCVM-8z"*0LCV8!=P$,=*N;(B
static UB reqAfterDetectLoadFuncBit; // $B6nF0Cf$KIi2YH/@8$7$?$N$G?K0LCV3NG'(B

static UB reqSearchBasePosBit; // $B4p=`0LCV8!=PMW5a(B
UB reqCorrectBasePosBit; // $B?K0LCVJd@5MW5a(B
UB reqCorrectFinePoBit; // $BHyD4@00LCV$K4p$E$$$F9T$&0LCVJd@5MW5a(B
static UB reqCheckLoadPosBit; // $BIi2Y0LCV3NG'MW5a(B

static H positionSearchCt[ MOTOR_NUM ]; // $B8!=PF0:n%+%&%s%H(B

UB actCorrectDriveBit; // $BJd@56nF0<B9TCf%U%i%0(B

static H currentPosition[ MOTOR_NUM ]; // $B8=:_0LCV(B
static H targetPosition[ MOTOR_NUM ]; // $BL\I80LCV(B

static UB detectLoadPositionIndex[ MOTOR_NUM ];

static H detectLoadPosition[ MOTOR_NUM ][ MAX_LOAD_POSITION_ARRAY_LENGTH ]; // 82
static UB detectLoadPositionType[ MOTOR_NUM ][ MAX_LOAD_POSITION_ARRAY_LENGTH ]; // 42 206byte
static H *detectLoadPositionPtr[ MOTOR_NUM ];
static UB *detectLoadPositionTypePtr[ MOTOR_NUM ];

static H detectBasePosition[ MOTOR_NUM ]; // $B8+$D$1$?4p=`0LCV!J:#$N?K0LCV%+%&%s%?>e!"<h$jIU$1%*%U%;%C%H9~!K(B

static UB loadPositionIndex[ MOTOR_NUM ];
static H loadPosition[ MOTOR_NUM ][ MAX_LOAD_POSITION_ARRAY_LENGTH ]; // 82
static UB loadPositionType[ MOTOR_NUM ][ MAX_LOAD_POSITION_ARRAY_LENGTH ]; // 42

static UB detectFinePosCt[ MOTOR_NUM ]; // $BHyD4@00LCV$r8+$D$1$?2s?t(B($BHyD4@0$,(B8$B2U=j!"AFD4@0$,(B1$B2U=j$G@.8y(B)
static UB detectRoughPosCt[ MOTOR_NUM ]; // $BAFD4@00LCV$r8+$D$1$?2s?t(B($BHyD4@0$,(B8$B2U=j!"AFD4@0$,(B1$B2U=j$G@.8y(B)
static UB detectLoadType[ MOTOR_NUM ]; // $B:#2s8+$D$1$?Ii2Y$N%?%$%W(B
static UB detectLoadCont[ MOTOR_NUM ]; // $B0l$D$NH=Dj$NO"B32s?t(B
static UB positionSearchStat[ MOTOR_NUM ]; // $B0LCV8!=P>uBV(B

static H searchTotalSteps[ MOTOR_NUM ]; // $B8!=P%Q%k%9$r=PNO$7$?2s?t(B

static H detectRoughTotalMove[ MOTOR_NUM ]; // $BAFD4@0$HH=Dj$7$?%H!<%?%k0\F0NL!JAFD4@00LCV$N0\F0NL!\<!$NH>C<$JCM$,HyD4@04V3V$+3NG'$9$k!K(B
static const H offsetBasePosition[ MOTOR_NUM ] = {
    OFFSET_M0_BASE_POSITION,
    OFFSET_M1_BASE_POSITION
}; // $B4p=`0LCV%*%U%;%C%H(B

static void ( * cbAfterDetectLoadFunc[ MOTOR_NUM ])(UB motor_no);

static UB doubleInterval;
static H missingDetectFinePosition;
//------------------------------------------------------------------------------
//.-------$B4X?t(B--------------
//.InitPulsePositionTable

/*******************************************************************************
 @brief     PulsePositionTable$B:n@.(B
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
 @brief     122us$BKh$K3d$j9~$_$G<B9T$5$l$k(B
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

        /**-- $BF~=PNO@_Dj(B --**/
        /**-- $B:#$N(BTRISx$B$r(BdataTRISx$B$KJ]B8(B --**/
        dataTRISA = TRISA;
        dataTRISB = TRISB;
        dataTRISC = TRISC;

        /**-- dataTRISx$B$N%b!<%?C<;R$N2U=j$r(B0$B$K$9$k(B --**/
        dataTRISA &= (UB) (~(BIT_TRISA_M0_ALL | BIT_TRISA_M1_ALL));
        dataTRISB &= (UB) (~(BIT_TRISB_M0_ALL | BIT_TRISB_M1_ALL));
        dataTRISC &= (UB) (~(BIT_TRISC_M0_ALL | BIT_TRISC_M1_ALL));

        /**-- dataTRISx$B$NI,MW$J%b!<%?C<;R$N2U=j$r(B1$B$K$9$k(B --**/
        dataTRISA |= (UB) (dataTRISAWork_M0 | dataTRISAWork_M1);
        dataTRISB |= (UB) (dataTRISBWork_M0 | dataTRISBWork_M1);
        dataTRISC |= (UB) (dataTRISCWork_M0 | dataTRISCWork_M1);

        /**-- $B%G!<%?%;%C%H(B --**/
        TRISA = dataTRISA;
        TRISB = dataTRISB;
        TRISC = dataTRISC;

    }
    if (OutRequestFlug == ON) {
        OutRequestFlug = OFF;

        /**-- $B%Q%k%9=PNO(B --**/
        /**-- $B:#$N(BLATx$B$r(BpulseOutLATx$B$KJ]B8(B --**/
        pulseOutLATA = LATA;
        pulseOutLATB = LATB;
        pulseOutLATC = LATC;

        /**-- pulseOutLATx$B$N%b!<%?C<;R$N2U=j$r(B0$B$K$9$k(B --**/
        pulseOutLATA &= (UB) (~(BIT_PORTA_M0_ALL | BIT_PORTA_M1_ALL));
        pulseOutLATB &= (UB) (~(BIT_PORTB_M0_ALL | BIT_PORTB_M1_ALL));
        pulseOutLATC &= (UB) (~(BIT_PORTC_M0_ALL | BIT_PORTC_M1_ALL));

        /**-- pulseOutLATx$B$NI,MW$J%b!<%?C<;R$N2U=j$r(B1$B$K$9$k(B --**/
        pulseOutLATA |= (UB) (pulseOutLATAWork_M0 | pulseOutLATAWork_M1);
        pulseOutLATB |= (UB) (pulseOutLATBWork_M0 | pulseOutLATBWork_M1);
        pulseOutLATC |= (UB) (pulseOutLATCWork_M0 | pulseOutLATCWork_M1);

        /**-- $B=PNO(B --**/
        LATA = pulseOutLATA;
        LATB = pulseOutLATB;
        LATC = pulseOutLATC;
    }

#ifdef DEBUG_TIMER2
    LATC &= ~b0000_0001;
#endif // DEBUG_RC4_TIMER2

    /**-- Vrs$B%+%&%s%H(B $BMW5a%A%'%C%/(B --*/
    if (reqVrsCheckBit & BIT_MOTOR_M0) {
        reqVrsCheckBit &= ~BIT_MOTOR_M0;
        VrsCheck(MOTOR_M0); //M0 Vrs$BK\?t%+%&%s%H(B
    }
    if (reqVrsCheckBit & BIT_MOTOR_M1) {
        reqVrsCheckBit &= ~BIT_MOTOR_M1;
        VrsCheck(MOTOR_M1); //M1 Vrs$BK\?t%+%&%s%H(B
    }
    /**-- $B%3%s%Q%l!<%?(BON$B!!MW5a%A%'%C%/(B --**/
    if (reqComparatorOnBit & BIT_MOTOR_M0) {
        reqComparatorOnBit &= ~BIT_MOTOR_M0;
        ComparatorOn(MOTOR_M0); //M0$BMQ%3%s%Q%l!<%?(BON
    }
    if (reqComparatorOnBit & BIT_MOTOR_M1) {
        reqComparatorOnBit &= ~BIT_MOTOR_M1;
        ComparatorOn(MOTOR_M1); //M1$BMQ%3%s%Q%l!<%?(BON
    }
    /**-- $B%3%s%Q%l!<%?(BOFF $BMW5a%A%'%C%/(B --**/
    if (reqComparatorOffBit & BIT_MOTOR_M0) {
        reqComparatorOffBit &= ~BIT_MOTOR_M0;
        ComparatorOff(MOTOR_M0); //M0$BMQ%3%s%Q%l!<%?(BOFF
    }
    if (reqComparatorOffBit & BIT_MOTOR_M1) {
        reqComparatorOffBit &= ~BIT_MOTOR_M1;
        ComparatorOff(MOTOR_M1); //M1$BMQ%3%s%Q%l!<%?(BOFF
    }

    if (motorActiveBit & BIT_MOTOR_M0) {

#ifdef DEBUG_TIMER2
        LATC |= b0000_0001;
#endif // DEBUG_RC4_TIMER2

        m0PulsePositionCt++;
        if (m0PulsePositionCt == m0PulseChangePosition) {

            /**-- $BJQ99$9$k%?%$%_%s%0(B --**/
            m0PulseOutStep++;

            if (m0PulseOutStep >= ID_PULSE_OUT_STEP_MAX) {
                /**-- 1cycle$B=*N;(B --**/
                if (0 == (actCorrectDriveBit & BIT_MOTOR_M0)) {
                    if ((reqSearchBasePosBit | reqCorrectBasePosBit | reqCheckLoadPosBit) & BIT_MOTOR_M0) {
                        /**-- $B4p=`0LCV8!=PCf(B or $B?K0LCVJd@56nF0Cf(B or $B0LCV3NG'0\F0Cf(B--**/
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
                    /**-- $BG$0U%9%F%C%W?t6nF0Cf(B --**/
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

                m0PulsePositionCt = m0PulsePositionCtWork; // $B?6$j;R%Q%k%9M-$jL5$7$GJQ$o$k(B
                m0PulseOutStep = m0PulseOutStepWork; // $B?6$j;R%Q%k%9M-$jL5$7$GJQ$o$k(B
                vrsCountValue[ MOTOR_M0 ] = 0; //Vrs$BK\?t%+%&%s%?%/%j%"(B

            } else {
                /**-- 1cycle$B0JFb$J$i(B --**/
                if (m0PulseOutChangeFunc) { // NULL_POINTER$B3NG'(B
                    m0PulseOutChangeFunc(); // $BCM$rJQ99$9$k3F4X?t$N<B9T(B
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

            /**-- $BJQ99$9$k%?%$%_%s%0(B --**/
            m1PulseOutStep++;

            if (m1PulseOutStep >= ID_PULSE_OUT_STEP_MAX) {
                /**-- 1cycle$B=*N;(B --**/
                if (0 == (actCorrectDriveBit & BIT_MOTOR_M1)) {
                    if ((reqSearchBasePosBit | reqCorrectBasePosBit | reqCheckLoadPosBit) & BIT_MOTOR_M1) {
                        /**-- $B4p=`0LCV8!=PCf(B or $B?K0LCVJd@56nF0Cf(B or $B0LCV3NG'0\F0Cf(B--**/
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
                    /**-- $BG$0U%9%F%C%W?t6nF0Cf(B --**/
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

                m1PulsePositionCt = m1PulsePositionCtWork; // $B?6$j;R%Q%k%9M-$jL5$7$GJQ$o$k(B
                m1PulseOutStep = m1PulseOutStepWork; // $B?6$j;R%Q%k%9M-$jL5$7$GJQ$o$k(B
                vrsCountValue[ MOTOR_M1 ] = 0; //Vrs$BK\?t%+%&%s%?%/%j%"(B

            } else {
                /**-- 1cycle$B0JFb$J$i(B --**/
                if (m1PulseOutChangeFunc) { // NULL_POINTER$B3NG'(B
                    m1PulseOutChangeFunc(); // $BCM$rJQ99$9$k3F4X?t$N<B9T(B
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
 @brief     SPK$B=PNO$H(BVRS$B8!=P$7$?$iNe<'$9$k@lMQ=hM}(B
 @note		122us$BKh$K3d$j9~$_$G<B9T$5$l$k(B
 @author
 @date
*******************************************************************************/

void TMR2_Interrupt_SPK(void)
{
    if (spkOutTrisRequestFlug == ON) {//TRISX$B$N=PNOMW@A$NM-L53NG'(B
        spkOutTrisRequestFlug = OFF;

        /**-- $BF~=PNO@_Dj(B --**/
        /**-- $B:#$N(BTRISx$B$r(BdataTRISx$B$KJ]B8(B --**/
        dataTRISA = TRISA;
        dataTRISB = TRISB;
        dataTRISC = TRISC;

        /**-- dataTRISx$B$N%b!<%?C<;R$N2U=j$r(B0$B$K$9$k(B --**/
        //$B8!=P$K4X$9$kC<;R!!(BO3,O4,VD$B$N$_(B
        dataTRISA &= (UB) (~(BIT_TRISA_M0_ALL | BIT_TRISA_M1_ALL));
        dataTRISB &= (UB) (~(BIT_TRISB_M0_ALL | BIT_TRISB_M1_ALL));
        dataTRISC &= (UB) (~(BIT_TRISC_M0_ALL | BIT_TRISC_M1_ALL));

        /**-- dataTRISx$B$NI,MW$J%b!<%?C<;R$N2U=j$r(B1$B$K$9$k(B --**/
        dataTRISA |= (UB) (dataTRISAWork_M0 | dataTRISAWork_M1);
        dataTRISB |= (UB) (dataTRISBWork_M0 | dataTRISBWork_M1);
        dataTRISC |= (UB) (dataTRISCWork_M0 | dataTRISCWork_M1);

        /**-- $B%G!<%?%;%C%H(B --**/
        TRISA = dataTRISA;
        TRISB = dataTRISB;
        TRISC = dataTRISC;

    }

#ifdef TOUCH_TEST

#else    
    /**-- $BNe<'(BOFF$B;~$N%^%9%/;~4V%A%'%C%/(B--**/
    if (excitatingOffMaskTime > 0) {//$BNe<'(BOFF$B8e$N%^%9%/%?%$%`;~4V%;%C%H$5$l$F$$$k$J$i(B

        excitatingOffMaskTime--;
        PIR2bits.C1IF = 0; //$B%^%9%/4|4V$N(BVrs$B$rL58z$K(B
        PIR2bits.C2IF = 0;


    }

    /**-- Vrs$B%A%'%C%/MW5a3NG'(B --**/
    if ((spkReqVrsCheckBit & BIT_MOTOR_M0) || (spkReqVrsCheckBit & BIT_MOTOR_M1)) {
        spkReqVrsCheckBit &= (UB) (~(BIT_MOTOR_M0 | BIT_MOTOR_M1));

        if (excitatingOffMaskTime == 0) {//$B%^%9%/;~4V=*N;$7$F$$$k$J$i(B

            SpkVrsCheck(); //M0,M1 Vrs$B%A%'%C%/(B
        }

    }

    /**-- $BNe<'(BON$B;~4V%A%'%C%/(B --**/
    if (excitatingTime > 0) {//$BNe<'(BON$B;~4V%;%C%H$5$l$F$$$k$J$i(B

        excitatingTime--;

        if (excitatingTime == 0) {

            ExcitatingOutOff(); //$BNe<'(BOFF$B4X?t!!(BLATAX,TRISX$B@_Dj(B excitatingOffMaskTime$B%;%C%H(B

        }
    }
#endif

    /**-- SPK$B%Q%k%9%]%8%7%g%s%A%'%C%/(B --**/
    spkPositionCt++;

    if (spkPositionCt == 4) {

        spkPositionCt = 0;
    }

    if (spkPositionCt == spkChangePosition) {//TRISX OPEN/SHORT$B@Z$jBX$(4X?tFb$GDj5A(B

        /**-- $BJQ99$9$k%?%$%_%s%0(B --**/
        if (spkOutChangeFunc) { // NULL_POINTER$B3NG'(B
            spkOutChangeFunc(); // $B=PNOCM$rJQ99$9$k3F4X?t$N<B9T!!$3$NCf$G!"<!$NJQ2=%]%$%s%H%;%C%H(B
        }
    }

}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//------------------------------------------------------------------------------
//.WatchMotorDriveFinish

/*******************************************************************************
 @brief     $B=*N;%A%'%C%/(B
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
 @brief     $B%Q%k%9=PNO(B
 @note      TRISx       $B%G!<%?J}8~(B
            LATx        $B=PNO%i%C%A(B
            PORTx       $BF~NO%G!<%?(B
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
            /**-- $B@5E>(B --**/
            if (addShakePulseBit & BIT_MOTOR_M0) {
                /**-- $BMIF0%Q%k%9M-$j(B --**/
                if (polarityBit & BIT_MOTOR_M0) {
                    /**-- $B6K@-(B1 --**/
                    m0PulseOutChangeFunc = SetPulseOutLATxWork_M0_SHAKE_POL1_P0;
                } else {
                    /**-- $B6K@-(B0 --**/
                    m0PulseOutChangeFunc = SetPulseOutLATxWork_M0_SHAKE_POL0_P0;
                }

                m0PulsePositionCtWork = POS_SHAKE_INIT_DATA;
                m0PulseOutStepWork = OUTSTEP_SHAKE_INIT_DATA;

            } else {
                /**-- $BMIF0%Q%k%9L5$7(B --**/
                if (polarityBit & BIT_MOTOR_M0) {
                    /**-- $B6K@-(B1 --**/
                    m0PulseOutChangeFunc = SetPulseOutLATxWork_M0_FWD_POL1_P1;
                } else {
                    /**-- $B6K@-(B0 --**/
                    m0PulseOutChangeFunc = SetPulseOutLATxWork_M0_FWD_POL0_P1;
                }

                m0PulsePositionCtWork = POS_P1P2_INIT_DATA;
                m0PulseOutStepWork = OUTSTEP_P1P2_INIT_DATA;

            }
        } else {
            /**-- $B5UE>(B --**/
            if (polarityBit & BIT_MOTOR_M0) {
                /**-- $B6K@-(B1 --**/
                m0PulseOutChangeFunc = SetPulseOutLATxWork_M0_REV_POL1_P1;
            } else {
                /**-- $B6K@-(B0 --**/
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
            /**-- $B@5E>(B --**/
            if (addShakePulseBit & BIT_MOTOR_M1) {
                /**-- $BMIF0%Q%k%9M-$j(B --**/
                if (polarityBit & BIT_MOTOR_M1) {
                    /**-- $B6K@-(B1 --**/
                    m1PulseOutChangeFunc = SetPulseOutLATxWork_M1_SHAKE_POL1_P0;
                } else {
                    /**-- $B6K@-(B0 --**/
                    m1PulseOutChangeFunc = SetPulseOutLATxWork_M1_SHAKE_POL0_P0;
                }

                m1PulsePositionCtWork = POS_SHAKE_INIT_DATA;
                m1PulseOutStepWork = OUTSTEP_SHAKE_INIT_DATA;



            } else {
                /**-- $BMIF0%Q%k%9L5$7(B --**/
                if (polarityBit & BIT_MOTOR_M1) {
                    /**-- $B6K@-(B1 --**/
                    m1PulseOutChangeFunc = SetPulseOutLATxWork_M1_FWD_POL1_P1;
                } else {
                    /**-- $B6K@-(B0 --**/
                    m1PulseOutChangeFunc = SetPulseOutLATxWork_M1_FWD_POL0_P1;
                }

                m1PulsePositionCtWork = POS_P1P2_INIT_DATA;
                m1PulseOutStepWork = OUTSTEP_P1P2_INIT_DATA;

            }
        } else {
            /**-- $B5UE>(B --**/
            if (polarityBit & BIT_MOTOR_M1) {
                /**-- $B6K@-(B1 --**/
                m1PulseOutChangeFunc = SetPulseOutLATxWork_M1_REV_POL1_P1;
            } else {
                /**-- $B6K@-(B0 --**/
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
 @brief		SPK$B=PNO3+;O(B
 @note     $B%7%g!<%H>uBV$G%9%?!<%H!"%3%s%Q%l!<%?(BON$B!"(BTurn$B$O(BO4$B!J%+%iBG$A!K(B
 @author
 @date
*******************************************************************************/

void SetMotorStartSPK(void)
{
    /**-- $B3d$j9~$_4X?t$r(BSPK$B=PNO$KJQ99$9$k(B --**/
    TMR2_SetInterruptHandler(TMR2_Interrupt_SPK);
    // clear the TMR2 interrupt flag
    PIR1bits.TMR2IF = 0;

    detectSpkTurnBit = 0x00; //$B8!=PC<;R$r(BO4$BB&$X=i4|@_Dj(B($B$?$@$7!"$3$l$O%+%iBG$A(B)

    /* $B$^$:$O!"8!=P%7%g!<%H$K$9$k(B*/
    dataTRISAWork_M0 = BIT_TRISA_M0_SHT;
    dataTRISBWork_M0 = BIT_TRISB_M0_SHT;
    dataTRISCWork_M0 = BIT_TRISC_M0_SHT;

    dataTRISAWork_M1 = BIT_TRISA_M1_SHT;
    dataTRISBWork_M1 = BIT_TRISB_M1_SHT;
    dataTRISCWork_M1 = BIT_TRISC_M1_SHT;

    spkOutTrisRequestFlug = ON;     //TRISX$B$N=PNOMW@A(B

    /*$B%3%s%Q%l!<%?(BON ON$B$N$_$GC<;RA*Br$O$7$J$$(B*/
    SpkComparatorOn();

    spkPositionCt = 0; //$B%]%8%7%g%s%+%&%s%?$r=i4|CM$X(B
    spkChangePosition = CHANGE_POSITION_OPEN; //OPEN$B0LCV$+$i%9%?!<%H!"$?$@$7%7%g!<%H$7$F$$$k(B

    spkOutChangeFunc = SetDataTRISxWork_SPK_SHORT; //$B:G=i$O(BSHORT$B4X?t$+$i(B

#ifdef TOUCH_TEST
    ExcitatingOutOn();      //$B%H%k%/B,Dj%F%9%H$N$?$a!">o$KNe<'(BON$B$H$9$k(B
#endif
    /**-- Timer2$B3d$j9~$_%9%?!<%H(B --**/
    TMR2_StartTimer();
}
//++++++++++++++++++++++++++Touch Read++++++++++++++++++++++++++++++++++++++
//..SetMotorStopSpk
/***************************************************************************//**
 @brief		SPK$B=PNODd;_(B
 @note     $BNe<';~4V%+%&%s%?!!%^%9%/%+%&%s%?%/%j%"!"%3%s%Q%l!<%?(BOFF
 @author
 @date
*******************************************************************************/

void SetMotorStopSPK(void)
{
    /**-- $B%?%$%^!<(B2$B%9%H%C%W(B --**/
    TMR2_StopTimer();
    // clear the TMR2 interrupt flag
    PIR1bits.TMR2IF = 0;

    SpkComparatorOff();
    excitatingTime = 0; //$BNe<';~4V%+%&%s%?%/%j%"(B
    excitatingOffMaskTime = 0;

    SetExcitatingOutOffData(); //$BNe<'%Q%k%9$,=P$F$$$?>l9g6/@)(BOFF

    /**-- $B3d$j9~$_4X?t$r=>Mh$N%b!<%?%Q%k%9=PNO$KLa$9(B --**/
    TMR2_SetInterruptHandler(TMR2_Interrupt);
}
//++++++++++++++++++++++++++Touch Read++++++++++++++++++++++++++++++++++++++
//..SetMotorEnableExcitating
/***************************************************************************//**
 @brief		$BNe<'M-8z@_Dj(B
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
 @brief		$BNe<'L58z@_Dj(B
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
//.$B6nF0@_Dj(B
//..SetDriveType

/*******************************************************************************
 @brief		$B%b!<%?6nF0@_Dj(B
 @note

            drive_type

            DRIVE_SIMPLE					$BG$0UJ}8~(B
            DRIVE_SEARCH_BASE_POSITION		$B@5E>$N$_(B
            DRIVE_CHECK_POSITION			$B@5E>$N$_(B
            DRIVE_CORRECT_BASE_POSITION		$B@5E>$N$_(B
            DRIVE_ACT_CORRECT_POSITION		$B@5E>$N$_(B	$B=jDj%9%F%C%W?t!"=jDjJ}8~(B
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
 @brief     $B6nF0Dd;_8e$N=hM}$NEPO?(B
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
 @brief     $BIi2Y8!=P8e$N=hM}$NEPO?(B
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
 @brief     $B2sE>J}8~@_Dj(B
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
 @brief     $B6nF0%Q%k%9?t@_Dj(B
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
 @brief     $B8=:_CM$HL\I8CM$K$b$I$D$$$F6nF0%9%F%C%W?t$r7W;;$9$k(B
 @note		$B@5E>$G$N%9%F%C%W?t$K$9$k(B
 @author
 @date
 ********************************************************************************/
void CalcMotorFwdStepCounter(UB motor_no)
{
    H steps;

    /**-- $B@5E>$G$N6nF0%Q%k%9?t;;=P(B --**/
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
 @brief		$B2sE>J}8~!u6nF0%Q%k%9?t%;%C%H(B
 @note		$B;D%Q%k%9?t$r2CL#$9$k(B
 @author
 @date
*******************************************************************************/

void CalcMotorStepCounter(UB motor_no)
{
    H steps;
    UB motor_bit = (UB) (0x01 << motor_no);

    /**-- $B;D%Q%k%9$G8=:_0LCV99?7(B --**/
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

    /**-- $B@5E>$G$N6nF0%Q%k%9?t;;=P(B --**/
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
 @brief     $BMIF0%Q%k%9@_Dj(B
 @note      $B@5E>6nF0$N$H$-$N$_IU2C$5$l$k(B
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
 @brief     $B%b!<%?6nF0<~GH?t@_Dj(B
 @note
 @param     motor_no    $B%b!<%?<oJL(B      MOTOR_M0$B!?(BMOTOR_M1
 @param     type        $B@)8f%?%$%W(B      FREQ_SET_SELF_CONTROL_NORMAL
                                        FREQ_SET_EXTERNAL_CONTROL_NORMAL
                                        FREQ_SET_SELF_CONTROL_SHAKE
                                        FREQ_SET_EXTERNAL_CONTROL_SHAKE

 @param     freq        $B<+8J@)8f;~$N6nF0<~GH?t(B
                        SET_FREQ_XXHZ$B$+$iA*Br$9$k$3$H(B
 @author
 @date
*******************************************************************************/

void SetMotorFrequency(UB motor_no, UB type, UB set_freq)
{
    if (FREQ_SET_SELF_CONTROL_NORMAL == type) {

        /**-- $B6nF0<~GH?t$r<+8J$G7h$a$k$H$-(B --**/
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

        /**-- $B6nF0<~GH?t$O30It$,7h$a$k$H$-!J(BSPI$B%3%^%s%I$-$?$i=PNO$9$k$N$_!K(B --**/
        finishPositionCtSet[ motor_no ] = POS_PULSE_INTERVAL_FINISH_MIN;

    }
}
//------------------------------------------------------------------------------
//..SetMotorStart
/***************************************************************************//**
 @brief     $B%b!<%?6nF0%9%?!<%H(B
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
 @brief     $B0LCV8!=P%9%?!<%H(B
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
 @brief     $B1??K$7$J$,$i$N0LCVJd@5%9%?!<%H(B
 @note		$B<B:]$K$O!"1??K%?%$%_%s%0$O30It%3%s%H%m!<%k$K$J$k(B
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
 @brief     $B0LCV8!=P=i4|2=(B
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
 @brief     $B0LCV$r(B0$B!A:GBgCM0JFb$KJd@5$9$k(B
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
 @brief		$B;vA0$K?K0LCV$r99?7$9$k!J?K0LCV$,M-8z$J>l9g$N$_!K(B
 @note		motorStepCounterSet$B$K@_Dj$5$l$F$$$k6nF0?t$G(B
            $B;vA0$K8=:_CM$r99?7$9$k(B
 @author
 @date
 ********************************************************************************/
void PriorUpdateCorrentPosition(UB motor_no)
{
    motorBitOperation = (UB) (0x01 << motor_no);

    if (handPositionActiveBit & motorBitOperation) {

        if (driveDirectionBit & motorBitOperation) {
            /**-- $B@5E>@_Dj(B --**/
            currentPosition[ motor_no ] += motorStepCounterSet[ motor_no ];
        } else {
            /**-- $B5UE>@_Dj(B --**/
            currentPosition[ motor_no ] -= motorStepCounterSet[ motor_no ];
        }

        CorrectPosition(&currentPosition[ motor_no ], TblMaxSteps[ motor_no ]);
    }
}
//------------------------------------------------------------------------------
//..UpdateTargetPosition
/***************************************************************************//**
 @brief		$BL\I80LCV99?7(B
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
 @brief		$B%b!<%?6nF0Dd;_MW5a(B
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
//.$B4p=`0LCV8!=P(B
//..AfterDetectedLoadPosition

/*******************************************************************************
 @brief     $BIi2Y0LCV8!=P$7$?$H$-$N=hM}(B
 @note      $B0\F0NL$GH=Dj(B
 @author
 @date
 ********************************************************************************/
void AfterDetectedLoadPosition(UB motor_no)
{
    H move_steps;
    UB res = ID_SEARCH_RESULT_NULL;

    SetMotorBitMotorNo(motor_no);

    /**-- $B%(%i!<MQ$K%H!<%?%k0\F0NL@Q;;(B --**/
    if (reqSearchBasePosBit & motorBitOperation) {
        searchTotalSteps[ motorNoOperation ] += positionSearchCt[ motorNoOperation ];
    }
    /**-- $B:#2s$N0\F0NLJ]B8(B --**/
    move_steps = positionSearchCt[ motorNoOperation ];

    /**-- $B8=:_?K0LCV99?7(B --**/
    if (reqSearchBasePosBit & motorBitOperation) {
        currentPosition[ motorNoOperation ] += positionSearchCt[ motorNoOperation ];
    } else {
        /**-- $B0LCVJd@5F0:nCf$O;vA0$K?K0LCV99?7$5$l$F$$$k(B --**/
    }

    if (searchTotalSteps[ motorNoOperation ] > TblMaxSearchSteps[ motorNoOperation ]) {
        /**-- $B@)8B0J>eF0$+$7$F$b8+$D$+$i$J$+$C$?"*=*N;(B --**/
        res = ID_SEARCH_RESULT_ERR;
    } else {

        if (move_steps > THRESHOLD_FINE_DIFF_UPPER) {
            /**-- 47 $B"*(B xx--**/
            /**-- $B0\F0NL$,HyD4@00LCV>e8B$rD62a(B $B"*(B $B=i$a$+$i$d$jD>$7(B --**/
            if (((THRESHOLD_FINE_DIFF_UPPER * 2) >= move_steps)
                    && ((move_steps >= THRESHOLD_FINE_DIFF_LOWER * 2))) {
                /**-- 43 $B"*(B 47 $B$N(B2$BG\$NCM$O0l8DH4$1$HH=Dj(B --**/

                if (ID_SEARCH_FINE_POS_1ST == positionSearchStat[ motorNoOperation ]) {
                    /**-- $B=i2s$NHyD4@00LCV%5!<%A(B --**/
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
                /**-- 43 $B"*(B 47$B$N(B1$B8DH4$10J30$NCM$O%(%i!<$H$9$k(B --**/
                res = ID_SEARCH_RESULT_RETRY;
            }
        } else if ((THRESHOLD_FINE_DIFF_UPPER >= move_steps) && (move_steps >= THRESHOLD_FINE_DIFF_LOWER)) {
            /**-- 43 $B"*(B 47 --**/
            if (ID_SEARCH_FINE_POS_1ST == positionSearchStat[ motorNoOperation ]) {
                /**-- $B=i2s$NHyD4@00LCV%5!<%A(B --**/
                positionSearchStat[ motorNoOperation ] = ID_SEARCH_FINE_ROUGH_POS;
            }

            detectLoadType[ motorNoOperation ] = ID_DETECT_LOAD_FINE;
            detectFinePosCt[ motorNoOperation ]++;
            detectLoadCont[ motorNoOperation ] = 1;

            res = ID_SEARCH_RESULT_NEW_POS;

        } else if (THRESHOLD_FINE_DIFF_LOWER > move_steps) {
            /**-- xx $B"*(B 43 --**/
            if (ID_SEARCH_FINE_POS_1ST == positionSearchStat[ motorNoOperation ]) {
                /**-- $B=i2s$NIi2Y8!=PCf$KAFD4@00LCV$HH=Dj(B --**/

                /**-- $B2?$b$7$J$$(B --**/

            } else if (ID_DETECT_LOAD_FINE == detectLoadType[ motorNoOperation ]) {
                /**-- $BHyD4@00LCV8!=P8e(B --**/

                if (move_steps > MAX_INTERVAL_FINE_CONT) {
                    /**-- $B?75,$NAFD4@00LCV$HH=Dj(B --**/
                    detectLoadType[ motorNoOperation ] = ID_DETECT_LOAD_ROUGH;
                    detectRoughPosCt[ motorNoOperation ]++;
                    detectLoadCont[ motorNoOperation ] = 1;
                    detectRoughTotalMove[ motorNoOperation ] = move_steps;
                    res = ID_SEARCH_RESULT_NEW_POS;
                } else {
                    /**-- $BHyD4@00LCV$,O"B3$7$F$$$k"*O"B3?t3NG'(B --**/
                    detectLoadCont[ motorNoOperation ]++;
                    if (detectLoadCont[ motorNoOperation ] > MAX_FINE_DETECTION_NUMBER) {
                        /**-- $BHyD4@00LCV$NIi2Y;u$,(B3$B;u"*8!=P<:GT(B --**/
                        res = ID_SEARCH_RESULT_RETRY;
                    } else {
                        res = ID_SEARCH_RESULT_CONT;
                    }
                }

            } else if (ID_DETECT_LOAD_ROUGH == detectLoadType[ motorNoOperation ]) {
                /**-- $BAFD4@00LCV8!=P8e(B --**/
                detectRoughTotalMove[ motorNoOperation ] += move_steps;

                if (move_steps > MAX_INTERVAL_ROUGH_CONT) {

                    /**-- $BHyD4@00LCV$+H=Dj(B --**/
                    if ((THRESHOLD_FINE_DIFF_UPPER >= detectRoughTotalMove[ motorNoOperation ]) && (detectRoughTotalMove[ motorNoOperation ] >= THRESHOLD_FINE_DIFF_LOWER)) {
                        /**-- $BHyD4@00LCV(B --**/
                        positionSearchStat[ motorNoOperation ] = ID_SEARCH_FINE_ROUGH_POS;
                        detectLoadType[ motorNoOperation ] = ID_DETECT_LOAD_FINE;
                        detectFinePosCt[ motorNoOperation ]++;
                        detectLoadCont[ motorNoOperation ] = 1;
                        res = ID_SEARCH_RESULT_NEW_POS;

                    } else {
                        /**-- $B?75,$NAFD4@00LCV$HH=Dj(B --**/
                        detectLoadType[ motorNoOperation ] = ID_DETECT_LOAD_ROUGH;
                        detectRoughPosCt[ motorNoOperation ]++;
                        detectLoadCont[ motorNoOperation ] = 1;
                        detectRoughTotalMove[ motorNoOperation ] = move_steps;
                        res = ID_SEARCH_RESULT_NEW_POS;
                    }

                } else {
                    /**-- $BAFD4@00LCV$NO"B3?t3NG'(B --**/
                    detectLoadCont[ motorNoOperation ]++;
                    if (detectLoadCont[ motorNoOperation ] > MAX_ROUGH_DETECTION_NUMBER) {
                        /**-- $BAFD4@00LCV$NIi2Y;u$,(B5$B;u"*8!=P<:GT(B --**/
                        res = ID_SEARCH_RESULT_RETRY;
                    } else {
                        res = ID_SEARCH_RESULT_CONT;
                    }
                }

            } else {
                /**-- $B$3$3$KMh$k$3$H$O$J$$(B --**/
                res = ID_SEARCH_RESULT_RETRY;
            }
        }
    }


    if ((detectFinePosCt[ motorNoOperation ] == NUMBER_OF_FINE_POSITION)
            && (detectRoughPosCt[ motorNoOperation ] == NUMBER_OF_ROUGH_POSITION)) {
        /**-- $B0LCV8!=P@.8y(B --**/
        res = ID_SEARCH_RESULT_SUCCESS;

    } else if ((detectFinePosCt[ motorNoOperation ] == (NUMBER_OF_FINE_POSITION + 1))
            && (detectRoughPosCt[ motorNoOperation ] == NUMBER_OF_ROUGH_POSITION)) {
        /**-- $B0LCV8!=P@.8y(B --**/
        res = ID_SEARCH_RESULT_SUCCESS;

    } else if (detectFinePosCt[ motorNoOperation ] > (NUMBER_OF_FINE_POSITION + 1)) {
        /**-- $BHyD4@00LCV$,(B9$B2U=j0J>e(B $B"*(B $B$d$jD>$7(B --**/
#ifdef DEBUG_CORRECT_POSITION
        LATA |= b0000_0100;
#endif
        res = ID_SEARCH_RESULT_RETRY;
    } else if (detectRoughPosCt[ motorNoOperation ] > NUMBER_OF_ROUGH_POSITION) {
        /**-- $BAFD4@00LCV$,(B2$B2U=j0J>e(B $B"*(B $B$d$jD>$7(B --**/
        res = ID_SEARCH_RESULT_RETRY;
    }

    if (ID_SEARCH_RESULT_ERR == res) {
        /**-- $B8!=P<:GT(B --**/

    } else if (ID_SEARCH_RESULT_NULL == res) {
        /**-- $BL5;k$9$k(B --**/
        /**-- $B=i2s$NIi2Y8!=PCf$KAFD4@00LCV"*0LCV8!=P7QB3(B --**/

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
            /**-- ID_SEARCH_RESULT_NEW_POS $B!?(B ID_SEARCH_RESULT_CONT --**/
            /**-- $BIi2Y0LCV%$%s%G%C%/%9(B++ --**/

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

            /**-- $B!VIi2Y0LCV8!=P!W?KG[Ns$K8=:_?K0LCV$rJ]B8(B --**/
            *detectLoadPositionPtr[ motorNoOperation ] = currentPosition[ motorNoOperation ];
            detectLoadPositionPtr[ motorNoOperation ]++;

            /**-- $B!VIi2Y8!=P(BTYPE$B!WG[Ns$K:#2s$N(BTYPE$B$rJ]B8(B --**/
            *detectLoadPositionTypePtr[ motorNoOperation ] = detectLoadType[ motorNoOperation ];
            if (ID_SEARCH_RESULT_CONT == res) {
                *detectLoadPositionTypePtr[ motorNoOperation ] += MARK_DETECT_LOAD_TYPE_CONT;
            }

            detectLoadPositionTypePtr[ motorNoOperation ]++;

        }

        if (ID_SEARCH_RESULT_SUCCESS == res) {
            /**-- $B4p=`0LCV7hDj(B --**/
            DetermineBasePosition(motorNoOperation);

            /**-- $B4p=`0LCV(B($B%*%U%;%C%H9~(B)$B$r(B0$B$H$7$FIi2Y0LCV$r99?7(B --**/
            UpdataHandPosition(motorNoOperation);

            /**-- $BIi2Y0LCV$rJ]B8(B --**/
            SaveLoadPosition(motorNoOperation);

            /**-- $B?K0LCV4IM}M-8z2=(B --**/
            ActivateHandPosition(motorNoOperation);

            if (reqSearchBasePosBit & motorBitOperation) {
                /**-- $B4p=`0LCVC5$7$N>l9g$O!"4p=`0LCV$X0\F0$5$;$k(B --**/
                if (DRIVE_SEARCH_1008_POSITION == driveType[ motorNoOperation ]) {
                    /**-- 10$B;~(B8$BJ,$N0LCV$X0\F0(B --**/
                    if (MOTOR_M0 == motorNoOperation) {
                        targetPosition[ MOTOR_M0 ] = 48;
                    } else {
                        targetPosition[ MOTOR_M1 ] = 304;
                    }
                } else {
                    targetPosition[ motorNoOperation ] = 0;
                }
                /**-- $B0\F040N;8e!"%3%s%H%m!<%i$K40N;3d$j9~$_DLCN(B --**/
                SetAfterMotorStopFunc(motorNoOperation, SPIControl_SetNotification);
                SetDriveType(motorNoOperation, DRIVE_SIMPLE);
            } else if (reqCorrectBasePosBit & motorBitOperation) {
                /**-- $B4p=`0LCVJd@5$N>l9g$O!":#$NL\I80LCV$X0\F0$5$;$k(B --**/
                /**-- $B:#$NL\I8CM$N$?$a(BtargetPosition$BJQ99$7$J$$(B --**/
                //SetDriveType( motorNoOperation, DRIVE_SIMPLE );
                reqCorrectBasePosBit &= (UB) (~motorBitOperation);
                SetDriveType(motorNoOperation, DRIVE_ACT_CORRECT_POSITION);
            }
            /**-- $B4p=`0LCV!J%?!<%2%C%H%]%8%7%g%s!K$X0\F03+;O(B --**/

            MoveTargetPosition(motorNoOperation);
        } else {
            /**-- $B0LCV8!=P7QB3(B --**/
            SetAfterDetectLoadFunc(motorNoOperation, AfterDetectedLoadPosition);
            if (reqSearchBasePosBit & motorBitOperation) {

                /**-- $B4p=`0LCV%5!<%ACf(B --**/
                /**-- $B4p=`0LCV8!=PCf(B --**/
                SetMotorStart(motorNoOperation);

            } else if ((reqCorrectBasePosBit & motorBitOperation)
                    && (reqCorrectFinePoBit & motorBitOperation)
                    && (ID_DETECT_LOAD_FINE == detectLoadType[ motorNoOperation ])
                    && (ID_SEARCH_RESULT_NEW_POS == res)) {

                /**-- $B0LCVJd@56nF0Cf(B $B"J(B $BHyD4@00LCV8!=P(B $B"J(B  $BO"B3$9$k(B1$B8DL\$NHyD4@00LCV(B --**/

                UpdateCurrentPositionByFinePosition(motorNoOperation);

                //				detectLoadPositionPtr[ motorNoOperation ]--;
                //		        *detectLoadPositionPtr[ motorNoOperation ] = currentPosition[ motorNoOperation ];
                //				detectLoadPositionPtr[ motorNoOperation ]++;

                /**-- $B$O$8$a$+$i$K$9$k(B --**/
                InitPositionSearch(motorNoOperation);

                SetDriveType(motorNoOperation, DRIVE_ACT_CORRECT_POSITION);
                reqCorrectFinePoBit &= (UB) (~motorBitOperation);

                MoveTargetPosition(motorNoOperation);

            } else {
                /**-- $B0LCVJd@56nF0Cf$N>e5-0J30(B --**/

            }
        }
    }


    /**-- positionSearchCt$B=i4|2=(B --**/
    positionSearchCt[ motorNoOperation ] = 0;

    /**-- $B%@%V%k%$%s%?!<%P%k%U%i%0(BOFF --**/
    doubleInterval = OFF;

#ifdef DEBUG_CORRECT_POSITION

    LATA &= (UB) (~b0000_0100);
#endif
}
//------------------------------------------------------------------------------
//...DetermineBasePosition

/*******************************************************************************
 @brief     $B4p=`0LCV7hDj(B
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

    /**-- $BAFD4@00LCV8!:w(B --**/
    while (1) {

        load_pos_type = detectLoadPositionType[ motor_no ][ base_pos_index ];
        base_pos_index++;

        if (ID_DETECT_LOAD_ROUGH == load_pos_type) {
            /**-- $BAFD4@00LCV(B --**/
            break;
        }
    }

    /**-- $BAFD4@08e$NHyD4@00LCV8!:w(B --**/
    while (1) {

        load_pos_type = detectLoadPositionType[ motor_no ][ base_pos_index ];

        if (ID_DETECT_LOAD_FINE == load_pos_type) {
            /**-- $BAFD4@08e$NHyD4@00LCV(B --**/
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
 @brief		$B8+$D$1$?4p=`0LCV$K4p$E$$$F!"4p=`0LCV$,(B0$B$H$J$k$h$&$K:#$NIi2Y0LCV$r99?7$9$k(B
 @note		$B4p=`0LCV$O(BdetectBasePosition$B$KF~$C$F$$$k(B

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
 @brief		$B8+$D$1$?HyD4@00LCV$K4p$E$$$F!"!X(B $B8=:_0LCV(B $B!Y$r99?7$9$k(B
			$BNc$($P!":#$^$G$NIi2Y0LCV$,(B
			48$B!"(B49$B!"(B93$B!"(B94$B$H$"$C$?$H$7$F!"(B
			96$B$N0LCV$GHyD4@00LCV3+;O$r8+$D$1$?$H$7$?$i!"(B

			$B8=:_0LCV(B97$B$r"*(B93$B$KJd@5$9$k!#(B

			$B"*$D$^$j(BLoadPosition$B$+$iHyD4@00LCV$rC5$7$F!"0lHV6a$/$+$DO"HV0J30$rC5$9(B
			$B:9J,$,(B22$B0J2<$N>l=j$rC5$9(B

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
 @brief		detectLoadPosition$B$H(BdetectLoadPositionIndex$B$r(B
 			$BJ]B8MQ$K!"(BloadPosition$B$H(BloadPositionIndex$B$KJ]B8$9$k(B
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
 @brief		$B?K0LCVM-8z2=(B
 @note		$B4p=`0LCV8!=P=*N;$7$?$i!"?K0LCV$rM-8z2=$9$k(B
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
 @brief		$B?K0LCVL58z2=(B
 @note		$B4p=`0LCV=hM}3+;O;~$KL58z2=$9$k(B
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
 @brief		targetPosition$B$K0\F0(B
 @note
 @author
 @date
*******************************************************************************/

void MoveTargetPosition(UB motor_no)
{
    /**-- $B6nF0%9%F%C%W?t7W;;(B & $B2sE>J}8~%;%C%H!J6a$$J}!K(B--**/
    CalcMotorStepCounter(motor_no);

    /**-- $B<~GH?t@_Dj(B --**/
    SetMotorFrequency(motor_no, FREQ_SET_SELF_CONTROL_NORMAL, SET_FREQ_200HZ);

    /**-- $B6nF0%9%?!<%H(B --**/
    SetMotorStart(motor_no);
}
//------------------------------------------------------------------------------
//...VerifyHandPosition_LoadPosition

/*******************************************************************************
 @brief		$B6nF0Cf$KIi2YH/@8"*Ii2Y0LCV$N8!>Z$HJd@5(B
 @note
 @attention	$BI,$:@5E>Cf$GM-$k$3$H(B
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

    /**-- $B$O$8$a$K(BcurrentPosition$B$r99?7$7$F8=:_0LCV$K$9$k(B --**/
    if (MOTOR_M0 == motor_no) {
        remain_steps = motorM0StepCounter;
    } else {
        remain_steps = motorM1StepCounter;
    }

    /**-- $B$9$G$K;vA0$K0LCV99?7$5$l$F$$$k$N$G8=:_CM$rJd@5$9$k(B --**/
    currentPosition[ motor_no ] -= remain_steps;
    CorrectPosition(&currentPosition[ motor_no ], TblMaxSteps[ motor_no ]);

    curt_pos = currentPosition[ motor_no ];

    /**-- $B4p=`0LCV%5!<%A;~!"8=:_$N0LCV$OIi2Y0LCV$G$"$C$?$+3NG'(B --**/
    /**-- $BHyD4@0$N(B1$BH/L\$N<!$N0LCV$OIi2Y%F!<%V%k$K$J$/$F$b5vMF$9$k(B --**/
    //	for ( i = 0; i < loadPositionIndex[ motor_no ]; i++ ) {
    //
    //		load_pos = loadPosition[ motor_no ][ i ];
    //
    //		if ( curt_pos == load_pos ) {
    //			/**-- $B2a5n$N%j%9%H$K:#2s$NIi2Y0LCV$,$"$k(B --**/
    //			misalignment_flag = OFF;
    //			break;
    //		}
    //	}

    for (i = 0; i < loadPositionIndex[ motor_no ]; i++) {

        load_pos = loadPosition[ motor_no ][ i ];

        if (curt_pos == load_pos) {
            /**-- $B2a5n$N%j%9%H$K:#2s$NIi2Y0LCV$,$"$k(B --**/
            misalignment_flag = OFF;
            break;
        }

        if (ID_DETECT_LOAD_FINE == loadPositionType[ motor_no ][ i ]) {
            /**-- $BHyD4@00LCV$N<!$N0LCV$H$bHf3S$9$k(B --**/
            load_pos++;
            if (load_pos >= TblMaxSteps[ motor_no ]) {
                load_pos = 0;
            }

            if (curt_pos == load_pos) {
                /**-- $B2a5n$N%j%9%H$K:#2s$NIi2Y0LCV$,$"$k(B --**/
                misalignment_flag = OFF;
                break;
            }
        }
    }

    if (misalignment_flag) {
        /**-- $B?75,$NIi2Y0LCV(B --**/
        /**-- $B0LCV$r4V0c$C$F$$$k2DG=@-Bg"*HyD4@00LCV$r(B2$B2s8!=P$9$k=hM}$r3+;O(B --**/
        //		MoveTargetPosition( motor_no );
        SetMotorCorrectPositionStart(motor_no);
    } else {
        /**-- $B4{B8$NIi2Y0LCV(B --**/
        /**-- $B0LCV$,@5$7$$"*Jd@5IU$-$N0\F0:F3+(B --**/
        MoveTargetPosition(motor_no);
    }
}
//------------------------------------------------------------------------------
//.misc
//..GetIsMotorActive
/***************************************************************************//**
 @brief     $B%b!<%?6nF0>uBVFI$_=P$7(B
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
//.$B%3%s%Q%l!<%?@_Dj(B
//..ComparatorOff

/*******************************************************************************
 *  $B%3%s%Q%l!<%?(BOFF$B$H=i4|2=(B
 *
 ******************************************************************************/
void ComparatorOff(UB motor_no)
{
    if (motor_no == MOTOR_M0) {
        CM1CON0 = CMP1_BASE; //CMP1 OFF$B!"H?E>!"(B $B9bB.(B
        CM1CON1 = CMP1_VCOMP_BASE; //IFPOS,DAC$B=PNO(B
    } else {
        CM2CON0 = CMP2_BASE; //CMP2 OFF$B!"H?E>!"(B $B9bB.(B
        CM2CON1 = CMP2_VCOMP_BASE; //IFPOS,DAC$B=PNO(B
    }
    PIR2bits.C1IF = 0;
    PIR2bits.C2IF = 0;
}
//------------------------------------------------------------------------------
//..ComparatorOn

/*******************************************************************************
 *  $B%3%s%Q%l!<%?(BON
 *
 ******************************************************************************/
void ComparatorOn(UB motor_no)
{
    if (motor_no == MOTOR_M0) {
        CM1CON0 = (CMP1_BASE | CMP1_ON); //CMP1 ON$B!"H?E>!"(B $B9bB.(B
        if (polarityBit & BIT_MOTOR_M0) {
            /* $B6K@-(B1 */
            CM1CON1 = (CMP1_VCOMP_BASE | M0_O3_CMP); //M0_O3$BB&A*Br(B
        } else {
            /* $B6K@-(B0 */
            CM1CON1 = (CMP1_VCOMP_BASE | M0_O4_CMP); //M0_O4$BB&A*Br(B
        }
        PIR2bits.C1IF = 0; //$BMW0x%/%j%"(B

    } else {

        CM2CON0 = (CMP2_BASE | CMP2_ON); //CMP2 ON$B!"H?E>!"(B $B9bB.(B
        if (polarityBit & BIT_MOTOR_M1) {
            /* $B6K@-(B1 */
            CM2CON1 = (CMP2_VCOMP_BASE | M1_O3_CMP); //M1_O3$BB&A*Br(B
        } else {
            /* $B6K@-(B0 */
            CM2CON1 = (CMP2_VCOMP_BASE | M1_O4_CMP); //M1_O4$BB&A*Br(B
        }
        PIR2bits.C2IF = 0; //$BMW0x%/%j%"(B

    }
}
//------------------------------------------------------------------------------
//.Vrs$BK\?t%A%'%C%/(B
//..VrsCheck

/*******************************************************************************
 *  Vcomp$B1[$($N(BVrs$B%'%C%/!!(BIF$BK\?t%+%&%s%H(B
 *
 ******************************************************************************/
void VrsCheck(UB motor_no)
{
    if (motor_no == MOTOR_M0) {
        if (PIR2bits.C1IF) {
            vrsCountValue[ MOTOR_M0 ]++; //M0$BMQ(BVrs$BK\?t%+%&%s%H(B
            //          TRISC &= b1111_1110;
        }
        PIR2bits.C1IF = 0;
    } else {
        if (PIR2bits.C2IF) {
            vrsCountValue [ MOTOR_M1 ]++; //M1$BMQ(BVrs$BK\?t%+%&%s%H(B
        }
        PIR2bits.C2IF = 0;
    }
}
//++++++++++++++++++++++++++Touch Read++++++++++++++++++++++++++++++++++++++
//.$B?(FIMQ%3%s%Q%l!<%?@_Dj(B
//..SpkComparatorOff

/*******************************************************************************
 *  $B?(FIMQ%3%s%Q%l!<%?(BOFF$B$H=i4|2=(B
 *  Motor$B<oJLL5$7(B
 ******************************************************************************/
void SpkComparatorOff(void)
{
    CM1CON0 = CMP1_BASE; //CMP1 OFF$B!"H?E>!"(B $B9bB.(B
    CM1CON1 = CMP1_VCOMP_BASE; //IFPOS,DAC$B=PNO(B

    CM2CON0 = CMP2_BASE; //CMP2 OFF$B!"H?E>!"(B $B9bB.(B
    CM2CON1 = CMP2_VCOMP_BASE; //IFPOS,DAC$B=PNO(B

    PIR2bits.C1IF = 0;
    PIR2bits.C2IF = 0;
}
//++++++++++++++++++++++++++Touch Read++++++++++++++++++++++++++++++++++++++
//..SpkComparatorOn

/*******************************************************************************
 *  $B?(FIMQ%3%s%Q%l!<%?(BON
 *  Motor$B<oJLL5$7(B
 ******************************************************************************/
void SpkComparatorOn(void)
{
    CM1CON0 = (CMP1_BASE | CMP1_ON); //CMP1 ON$B!"H?E>!"(B $B9bB.(B
    PIR2bits.C1IF = 0; //$BMW0x%/%j%"(B

    CM2CON0 = (CMP2_BASE | CMP2_ON); //CMP2 ON$B!"H?E>!"(B $B9bB.(B
    PIR2bits.C2IF = 0; //$BMW0x%/%j%"(B


}
//++++++++++++++++++++++++++Touch Read++++++++++++++++++++++++++++++++++++++
//.$B?(FI(BVrs$B%A%'%C%/(B
//..SpkVrsCheck

/*******************************************************************************
 *  Vrs$B%A%'%C%/$7!"8!=P$G!"Ne<'(BON$B%U%i%0M-8z$J$i!"Ne<'(BON
 *
 ******************************************************************************/
void SpkVrsCheck(void)
{
    if ((PIR2bits.C1IF == 1) || (PIR2bits.C2IF == 1)) {

        PIR2bits.C1IF = 0;
        PIR2bits.C2IF = 0;

        if (excitatingEnableBit & (BIT_MOTOR_M0 | BIT_MOTOR_M1)) {//$BNe<'(BON$B@_Dj$J$i(B

            ExcitatingOutOn();

        }

    }

}
//++++++++++++++++++++++++++Touch Read++++++++++++++++++++++++++++++++++++++
//.$B?(FINe<'(BOFF$B4X?t(B
//..ExcitatingOutOff

/*******************************************************************************
 *  $BNe<'(BOff$B=hM}(B
 *$B!!%^%9%/%+%&%s%?%;%C%H(B $B%$%s%8%1!<%?(BLEDOFF
 ******************************************************************************/
void ExcitatingOutOff(void)
{

    SetExcitatingOutOffData();

    excitatingOffMaskTime = EXCITATING_OFF_MSK_TIME;

}
//++++++++++++++++++++++++++Touch Read++++++++++++++++++++++++++++++++++++++
//.$B?(FINe<'(BOFFdata$B%;%C%H4X?t(B
//..ExcitatingOutData

/*******************************************************************************
 *  $BNe<'(BOff$B=hM}(B
 *$B!!=PNO%b!<%I!"Ne<'(BOFF$B%Q%k%9!#(BLED$B>CEt%G!<%?%;%C%H=PNO(B
 ******************************************************************************/
void SetExcitatingOutOffData(void)
{

    /**-- TRISX$B%G!<%?=PNO(B --**/
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

    SetOutPutLATX(); //LATX $B=PNO(B $B$3$3$G$O!"%b!<%?C<;R$N$_$N=PNO(B
    LATA &= ~BIT_PORTA_LED_ON; //$B$3$3$G!"(BLED OFF$B$r=PNO(B

}
//++++++++++++++++++++++++++Touch Read++++++++++++++++++++++++++++++++++++++
//.$B?(FINe<'(BON$B4X?t(B
//..ExcitatingOutOn

/*******************************************************************************
 *  $BNe<'(BOn$B=hM}(B
 *$B!!Ne<'(BON$B=PNO!"Ne<'(BON$B;~4V%+%&%s%?%;%C%H(B $B%$%s%8%1!<%?(BLED$B!!(BON
 ******************************************************************************/
void ExcitatingOutOn(void)
{
    /**-- TRISX$B%G!<%?=PNO(B --**/
    dataTRISAWork_M0 = BIT_TRISA_M0_DRV;
    dataTRISBWork_M0 = BIT_TRISB_M0_DRV;
    dataTRISCWork_M0 = BIT_TRISC_M0_DRV;

    dataTRISAWork_M1 = BIT_TRISA_M1_DRV;
    dataTRISBWork_M1 = BIT_TRISB_M1_DRV;
    dataTRISCWork_M1 = BIT_TRISC_M1_DRV;

    /**-- LATX$B%G!<%?=PNO(B -- OUT1$B$K$N$_=PNO(B**/
    pulseOutLATAWork_M0 = BIT_PORTA_M0_OUT1; //M0_O1 ON
    pulseOutLATBWork_M0 = BIT_PORTB_M0_OUT1;
    pulseOutLATCWork_M0 = BIT_PORTC_M0_OUT1;

    pulseOutLATAWork_M1 = BIT_PORTA_M1_OUT1;
    pulseOutLATBWork_M1 = BIT_PORTB_M1_OUT1; //M1_O1 ON
    pulseOutLATCWork_M1 = BIT_PORTC_M1_OUT1;

    SetOutPutLATX(); //LATX $B=PNO(B $B$3$3$G$O!"%b!<%?C<;R$N$_$N=PNO(B
    LATA |= BIT_PORTA_LED_ON; //$B$3$3$G!"(BLED ON$B$r=PNO(B

    excitatingTime = EXCITATING_TIME;       //$BNe<'(BON$B7QB3;~4V%+%&%s%HCM(B
}
//++++++++++++++++++++++++++Touch Read++++++++++++++++++++++++++++++++++++++
//.$B?(FI(BOPEN$B4X?t(B
/*******************************************************************************
 *  $B?(FI(BOPEN$B=hM}(B
 *$B!!(BTurn$B%U%i%0$K$h$j!"(BO3/O4$B8!=PDq93!"%3%s%Q%l!<%?C<;RA*Br(B
 *  Turn$B%U%i%0H?E>$7!"<!$N<B;\4X?t$r(BSHORT$B4X?t$K%;%C%H(B
 ******************************************************************************/
//------------------------------------------------------------------------------
//....SetDataTRISxWork_SPK_OPEN

void SetDataTRISxWork_SPK_OPEN(void)
{
    if (detectSpkTurnBit == 0) {// O4$BB&8!=P(B
        dataTRISAWork_M0 = BIT_TRISA_M0_O4_OPN;
        dataTRISBWork_M0 = BIT_TRISB_M0_O4_OPN;
        dataTRISCWork_M0 = BIT_TRISC_M0_O4_OPN; //M0_O4$BB&(BOPEN

        dataTRISAWork_M1 = BIT_TRISA_M1_O4_OPN;
        dataTRISBWork_M1 = BIT_TRISB_M1_O4_OPN;
        dataTRISCWork_M1 = BIT_TRISC_M1_O4_OPN; //M1_O4$BB&(BOPEN

        CM1CON1 = (CMP1_VCOMP_BASE | M0_O4_CMP); //M0_O4$BB&A*Br(B
        CM2CON1 = (CMP2_VCOMP_BASE | M1_O4_CMP); //M1_O4$BB&A*Br(B

    } else {// O3$BB&8!=P(B
        dataTRISAWork_M0 = BIT_TRISA_M0_O3_OPN;
        dataTRISBWork_M0 = BIT_TRISB_M0_O3_OPN;
        dataTRISCWork_M0 = BIT_TRISC_M0_O3_OPN; //M0_O3$BB&(BOPEN

        dataTRISAWork_M1 = BIT_TRISA_M1_O3_OPN;
        dataTRISBWork_M1 = BIT_TRISB_M1_O3_OPN;
        dataTRISCWork_M1 = BIT_TRISC_M1_O3_OPN; //M1_O3$BB&(BOPEN

        CM1CON1 = (CMP1_VCOMP_BASE | M0_O3_CMP); //M0_O3$BB&A*Br(B
        CM2CON1 = (CMP2_VCOMP_BASE | M1_O3_CMP); //M1_O3$BB&A*Br(B
    }

    spkOutChangeFunc = SetDataTRISxWork_SPK_SHORT; //$B<!$N<B;\4X?t$r(BSHORT$B4X?t$K(B
    spkChangePosition = CHANGE_POSITION_OPEN; //$B<!$N%A%'%s%8%]%8%7%g%sCM(B
    detectSpkTurnBit ^= 0x01; //$B8!=P$9$kC<;R$NF~$lBX$(;X<((B


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
//.$B?(FI(BSHORT$B4X?t(B
/*******************************************************************************
 *  $B?(FI(BSHORT$B=hM}(B
 *$B!!(BTRISX$B$r(BSHORT$B@_Dj(B
 *  Vrs$B%A%'%C%/MW5a(B
 *  $B<!$N<B;\4X?t$r(BOPEN$B4X?t$K%;%C%H(B
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

    spkOutChangeFunc = SetDataTRISxWork_SPK_OPEN; //$B<!$N<B;\4X?t$r(BOPEN$B4X?t$K(B
    spkChangePosition = CHANGE_POSITION_SHORT; //$B<!$N%A%'%s%8%]%8%7%g%sCM(B

    spkReqVrsCheckBit |= (BIT_MOTOR_M0 | BIT_MOTOR_M1); //$B%b!<%?<oJLL5$7(B Vrs$B%A%'%C%/MW5a(B


    spkOutTrisRequestFlug = ON;

}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void SetOutPutLATX(void)
{

    /**-- $B:#$N(BTRISx$B$r(BdataTRISx$B$KJ]B8(B --**/
    dataTRISA = TRISA;
    dataTRISB = TRISB;
    dataTRISC = TRISC;

    /**-- dataTRISx$B$N%b!<%?C<;R$N2U=j$r(B0$B$K$9$k(B --**/
    //$B$3$3$G$O(BSPK$B$K4XO"$9$k%b!<%?C<;R$N$_(BO3,O4,VD$B$r(B0$B$K$9$k(B
    dataTRISA &= (UB) (~(BIT_TRISA_M0_ALL | BIT_TRISA_M1_ALL));
    dataTRISB &= (UB) (~(BIT_TRISB_M0_ALL | BIT_TRISB_M1_ALL));
    dataTRISC &= (UB) (~(BIT_TRISC_M0_ALL | BIT_TRISC_M1_ALL));

    /**-- dataTRISx$B$NI,MW$J%b!<%?C<;R$N2U=j$r(B1$B$K$9$k(B --**/
    dataTRISA |= (UB) (dataTRISAWork_M0 | dataTRISAWork_M1);
    dataTRISB |= (UB) (dataTRISBWork_M0 | dataTRISBWork_M1);
    dataTRISC |= (UB) (dataTRISCWork_M0 | dataTRISCWork_M1);

    /**-- $B%G!<%?%;%C%H(B --**/
    TRISA = dataTRISA;
    TRISB = dataTRISB;
    TRISC = dataTRISC;

    /**-- $B:#$N(BLATx$B$r(BpulseOutLATx$B$KJ]B8(B --**/
    pulseOutLATA = LATA;
    pulseOutLATB = LATB;
    pulseOutLATC = LATC;

    /**-- pulseOutLATx$B$N%b!<%?C<;R$N2U=j$r(B0$B$K$9$k(B --**/
    //$B$3$3$O(BO1,O2,O3,O4$B$N%b!<%?C<;R$N$_(B
    pulseOutLATA &= (UB) (~(BIT_PORTA_M0_ALL | BIT_PORTA_M1_ALL));
    pulseOutLATB &= (UB) (~(BIT_PORTB_M0_ALL | BIT_PORTB_M1_ALL));
    pulseOutLATC &= (UB) (~(BIT_PORTC_M0_ALL | BIT_PORTC_M1_ALL));

    /**-- pulseOutLATx$B$NI,MW$J%b!<%?C<;R$N2U=j$r(B1$B$K$9$k(B --**/
    pulseOutLATA |= (UB) (pulseOutLATAWork_M0 | pulseOutLATAWork_M1);
    pulseOutLATB |= (UB) (pulseOutLATBWork_M0 | pulseOutLATBWork_M1);
    pulseOutLATC |= (UB) (pulseOutLATCWork_M0 | pulseOutLATCWork_M1);

    /**-- $B=PNO(B --**/
    LATA = pulseOutLATA;
    LATB = pulseOutLATB;
    LATC = pulseOutLATC;

}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//.$B%Q%k%9@_Dj(B
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
//.$B%Q%k%9@_Dj!JMIF0%Q%k%9!K(B
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

    //$B<!$N%?%$%_%s%0$+$i8!=P$r$9$k$?$a$K$^$:$O!"8!=PDq93(BON$B$7%7%g!<%H$7$F$*$/(B
    dataTRISAWork_M0 = BIT_TRISA_M0_SPK_POL0_SHT;
    dataTRISBWork_M0 = BIT_TRISB_M0_SPK_POL0_SHT;
    dataTRISCWork_M0 = BIT_TRISC_M0_SPK_POL0_SHT; //$B8!=PDq93(BON$B$7(BO4$BB&%7%g!<%H(B

    m0PulseOutChangeFunc = SetDataTRISxWork_M0_SPK_POL0_OPEN;
    OutRequestFlug = ON;
    OutTrisRequestFlug = ON;
    reqComparatorOnBit |= BIT_MOTOR_M0; //$B%3%s%Q%l!<%?(BON$BMW5a(B
}
//------------------------------------------------------------------------------
//....SetDataTRISxWork_M0_SPK_POL0_OPEN

void SetDataTRISxWork_M0_SPK_POL0_OPEN(void)
{
    dataTRISAWork_M0 = BIT_TRISA_M0_SPK_POL0_OPN;
    dataTRISBWork_M0 = BIT_TRISB_M0_SPK_POL0_OPN;
    dataTRISCWork_M0 = BIT_TRISC_M0_SPK_POL0_OPN; //O4$BB&(BOPEN

    m0PulseOutChangeFunc = SetDataTRISxWork_M0_SPK_POL0_SHORT;
    OutTrisRequestFlug = ON;
}
//------------------------------------------------------------------------------
//....SetDataTRISxWork_M0_SPK_POL0_SHORT

void SetDataTRISxWork_M0_SPK_POL0_SHORT(void)
{
    dataTRISAWork_M0 = BIT_TRISA_M0_SPK_POL0_SHT;
    dataTRISBWork_M0 = BIT_TRISB_M0_SPK_POL0_SHT;
    dataTRISCWork_M0 = BIT_TRISC_M0_SPK_POL0_SHT; //O4$BB&%7%g!<%H(B

    if (m0PulseOutStep == ID_PULSE_OUT_STEP_P1) {
        m0PulseOutChangeFunc = SetPulseOutLATxWork_M0_FWD_POL0_P1;
        reqComparatorOffBit |= BIT_MOTOR_M0; //$B%3%s%Q%l!<%?(BOFF$BMW5a(B
    } else {
        m0PulseOutChangeFunc = SetDataTRISxWork_M0_SPK_POL0_OPEN;
    }

    OutTrisRequestFlug = ON;
    reqVrsCheckBit |= BIT_MOTOR_M0; //Vrs$B%A%'%C%/MW5a(B
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

    //$B<!$N%?%$%_%s%0$+$i8!=P$r$9$k$?$a$K$^$:$O!"8!=PDq93(BON$B$7%7%g!<%H$7$F$*$/(B
    dataTRISAWork_M0 = BIT_TRISA_M0_SPK_POL1_SHT; //O3$BB&%7%g!<%H(B
    dataTRISBWork_M0 = BIT_TRISB_M0_SPK_POL1_SHT;
    dataTRISCWork_M0 = BIT_TRISC_M0_SPK_POL1_SHT; //$B8!=PDq93(BON

    m0PulseOutChangeFunc = SetDataTRISxWork_M0_SPK_POL1_OPEN;
    OutRequestFlug = ON;
    OutTrisRequestFlug = ON;
    reqComparatorOnBit |= BIT_MOTOR_M0; //$B%3%s%Q%l!<%?(BON$BMW5a(B
}
//------------------------------------------------------------------------------
//....SetDataTRISxWork_M0_SPK_POL1_OPEN

void SetDataTRISxWork_M0_SPK_POL1_OPEN(void)
{
    dataTRISAWork_M0 = BIT_TRISA_M0_SPK_POL1_OPN; //O3$BB&(BOPEN
    dataTRISBWork_M0 = BIT_TRISB_M0_SPK_POL1_OPN;
    dataTRISCWork_M0 = BIT_TRISC_M0_SPK_POL1_OPN;

    m0PulseOutChangeFunc = SetDataTRISxWork_M0_SPK_POL1_SHORT;
    OutTrisRequestFlug = ON;
}
//------------------------------------------------------------------------------
//....SetDataTRISxWork_M0_SPK_POL1_SHORT

void SetDataTRISxWork_M0_SPK_POL1_SHORT(void)
{
    dataTRISAWork_M0 = BIT_TRISA_M0_SPK_POL1_SHT; //O3$BB&%7%g!<%H(B
    dataTRISBWork_M0 = BIT_TRISB_M0_SPK_POL1_SHT;
    dataTRISCWork_M0 = BIT_TRISC_M0_SPK_POL1_SHT;

    if (m0PulseOutStep == ID_PULSE_OUT_STEP_P1) {
        m0PulseOutChangeFunc = SetPulseOutLATxWork_M0_FWD_POL1_P1;
        reqComparatorOffBit |= BIT_MOTOR_M0; //$B%3%s%Q%l!<%?(BOFF$BMW5a(B
    } else {
        m0PulseOutChangeFunc = SetDataTRISxWork_M0_SPK_POL1_OPEN;
    }

    OutTrisRequestFlug = ON;
    reqVrsCheckBit |= BIT_MOTOR_M0; //Vrs$B%A%'%C%/MW5a(B
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

    //$B<!$N%?%$%_%s%0$+$i8!=P$r$9$k$?$a$K$^$:$O!"8!=PDq93(BON$B$7%7%g!<%H$7$F$*$/(B
    dataTRISAWork_M1 = BIT_TRISA_M1_SPK_POL0_SHT;
    dataTRISBWork_M1 = BIT_TRISB_M1_SPK_POL0_SHT;
    dataTRISCWork_M1 = BIT_TRISC_M1_SPK_POL0_SHT; //$B8!=PDq93(BON$B$7(BO4$BB&%7%g!<%H(B

    m1PulseOutChangeFunc = SetDataTRISxWork_M1_SPK_POL0_OPEN;
    OutRequestFlug = ON;
    OutTrisRequestFlug = ON;
    reqComparatorOnBit |= BIT_MOTOR_M1; //$B%3%s%Q%l!<%?(BON$BMW5a(B
}
//------------------------------------------------------------------------------
//....SetDataTRISxWork_M1_SPK_POL0_OPEN

void SetDataTRISxWork_M1_SPK_POL0_OPEN(void)
{
    dataTRISAWork_M1 = BIT_TRISA_M1_SPK_POL0_OPN;
    dataTRISBWork_M1 = BIT_TRISB_M1_SPK_POL0_OPN;
    dataTRISCWork_M1 = BIT_TRISC_M1_SPK_POL0_OPN; //O4$BB&(BOPEN

    m1PulseOutChangeFunc = SetDataTRISxWork_M1_SPK_POL0_SHORT;
    OutTrisRequestFlug = ON;
}
//------------------------------------------------------------------------------
//....SetDataTRISxWork_M1_SPK_POL0_SHORT

void SetDataTRISxWork_M1_SPK_POL0_SHORT(void)
{
    dataTRISAWork_M1 = BIT_TRISA_M1_SPK_POL0_SHT;
    dataTRISBWork_M1 = BIT_TRISB_M1_SPK_POL0_SHT;
    dataTRISCWork_M1 = BIT_TRISC_M1_SPK_POL0_SHT; //O4$BB&%7%g!<%H(B

    if (m1PulseOutStep == ID_PULSE_OUT_STEP_P1) {
        m1PulseOutChangeFunc = SetPulseOutLATxWork_M1_FWD_POL0_P1;
        reqComparatorOffBit |= BIT_MOTOR_M1; //$B%3%s%Q%l!<%?(BOFF$BMW5a(B
    } else {
        m1PulseOutChangeFunc = SetDataTRISxWork_M1_SPK_POL0_OPEN;
    }

    OutTrisRequestFlug = ON;
    reqVrsCheckBit |= BIT_MOTOR_M1; //Vrs$B%A%'%C%/MW5a(B
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

    //$B<!$N%?%$%_%s%0$+$i8!=P$r$9$k$?$a$K$^$:$O!"8!=PDq93(BON$B$7%7%g!<%H$7$F$*$/(B
    dataTRISAWork_M1 = BIT_TRISA_M1_SPK_POL1_SHT;
    dataTRISBWork_M1 = BIT_TRISB_M1_SPK_POL1_SHT;
    dataTRISCWork_M1 = BIT_TRISC_M1_SPK_POL1_SHT; //$B8!=PDq93(BON$B$7(BO3$BB&%7%g!<%H(B

    m1PulseOutChangeFunc = SetDataTRISxWork_M1_SPK_POL1_OPEN;
    OutRequestFlug = ON;
    OutTrisRequestFlug = ON;
    reqComparatorOnBit |= BIT_MOTOR_M1; //$B%3%s%Q%l!<%?(BON$BMW5a(B
}
//------------------------------------------------------------------------------
//....SetDataTRISxWork_M1_SPK_POL1_OPEN

void SetDataTRISxWork_M1_SPK_POL1_OPEN(void)
{
    dataTRISAWork_M1 = BIT_TRISA_M1_SPK_POL1_OPN;
    dataTRISBWork_M1 = BIT_TRISB_M1_SPK_POL1_OPN;
    dataTRISCWork_M1 = BIT_TRISC_M1_SPK_POL1_OPN; //O3$BB&(BOPEN

    m1PulseOutChangeFunc = SetDataTRISxWork_M1_SPK_POL1_SHORT;
    OutTrisRequestFlug = ON;
}
//------------------------------------------------------------------------------
//....SetDataTRISxWork_M1_SPK_POL1_SHORT

void SetDataTRISxWork_M1_SPK_POL1_SHORT(void)
{
    dataTRISAWork_M1 = BIT_TRISA_M1_SPK_POL1_SHT;
    dataTRISBWork_M1 = BIT_TRISB_M1_SPK_POL1_SHT;
    dataTRISCWork_M1 = BIT_TRISC_M1_SPK_POL1_SHT; //O3$BB&%7%g!<%H(B

    if (m1PulseOutStep == ID_PULSE_OUT_STEP_P1) {
        m1PulseOutChangeFunc = SetPulseOutLATxWork_M1_FWD_POL1_P1;
        reqComparatorOffBit |= BIT_MOTOR_M1; //$B%3%s%Q%l!<%?(BOFF$BMW5a(B
    } else {
        m1PulseOutChangeFunc = SetDataTRISxWork_M1_SPK_POL1_OPEN;
    }

    OutTrisRequestFlug = ON;
    reqVrsCheckBit |= BIT_MOTOR_M1; //Vrs$B%A%'%C%/MW5a(B
}
//------------------------------------------------------------------------------

