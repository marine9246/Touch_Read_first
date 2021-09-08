//.spi_control.c
/***************************************************************************//**
 @brief     SPI$B@)8f=hM}(B
 @note      $B;29M!K(Bspi.c$B$NFbMF(B

            void SPI_Initialize(void);
                SPI$B%b%8%e!<%k=i4|2=(B
            -------------------------------------------------------
            bool SPI_Open(spi_modes_t spiUniqueConfiguration);
                SPI$B5!G=(B enable
            -------------------------------------------------------
            void SPI_Close(void);
                SPI$B5!G=(B Disable
            -------------------------------------------------------
            void SPI_ExchangeBlock(void *block, size_t blockSize);
                1Byte$B%G!<%?8r49(B
            -------------------------------------------------------
            void SPI_WriteBlock(void *block, size_t blockSize);
                $B=jDj%P%$%H?t%G!<%?8r49(B
            -------------------------------------------------------
            void SPI_ReadBlock(void *block, size_t blockSize);
                $B=jDj%P%$%H?tFI$_=P$7(B
            -------------------------------------------------------
            void SPI_WriteByte(uint8_t byte);
                $B=jDj%P%$%H?t=q$-9~$_(B
            -------------------------------------------------------

            $B@\B3(B
                                $B%b!<%?%I%i%$%P(B(slave)(PIC)      $B%3%s%H%m!<%i(B(master)(Q923)
            DREADY			    RC0                     $B"*(B      P53
            SPI_CLOCK           RB6                     $B"+(B      P56
            SPI_MOSI            RB4(SIN)                $B"+(B      P57
            SPI_MISO            RC7(SOUT)               $B"*(B      P55
            SS                  PA3                     $B"+(B      P47

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
//..$B%W%m%H%?%$%W@k8@(B
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

/**-- $B?(FIBP1~(B --**/
void SpiCmdFunc_StartDetectVrs( UB motor_no );
void SpiCmdFunc_StopDetectVrs( UB motor_no );
void SpiCmdFunc_EnableExcitating( UB motor_no );
void SpiCmdFunc_DisableExcitating( UB motor_no );
void SpiCmdFunc_Search1008Position( UB motor_no );
//------------------------------------------------------------------------------
//.$BDj?t(B
/*******************************************************************************
 @brief     $B%3%^%s%IDj?t(B
 @note
 @author
 @date
 ********************************************************************************/
enum ID_CMD_TYPE {
    ID_CMD_FWD_NULL = 0,
    ID_CMD_FWD_NORMAL = 1,                          // $BDL>o$N@5E>(B1$BH/6nF0(B
    ID_CMD_REV_NORMAL = 2,                          // $BDL>o$N5UE>(B1$BH/6nF0(B
    ID_CMD_FWD_WITH_SHAKE = 3,                      // $BMIF0%Q%k%9IU$-$N@5E>!"7k2L$O!V!!!!!W$KF~$k(B
    ID_CMD_SEARCH_LOAD_POSITION = 4,                // $BHyD4@00LCV$r@5E>$GC5$9(B
    ID_CMD_SEARCH_BASE_POSITION = 5,                // $B4p=`0LCV$r@5E>$GC5$9!JAFD4@08e$NHyD4@08!=P0LCV!K(B
    ID_CMD_FWD_WITH_CORRECT = 6,                    // $B4p=`0LCV%:%lJd@5IU$-$N1??K!"0LCV%:%l8!=P$7$?$i!"<+F0Jd@5(B
	ID_CMD_FWD_POSITION_CTL = 7,					// $B0LCV4IM}5!G=IU$-$N@5E>(B
	ID_CMD_REV_POSITION_CTL = 8,					// $B0LCV4IM}5!G=IU$-$N5UE>(B
	ID_CMD_START_DETECT_VRS = 9,					// VRS$B8!=P3+;O(B
	ID_CMD_STOP_DETECT_VRS = 10,					// VRS$B8!=PCf;_(B
	ID_CMD_ENABLE_EXCITATING = 11,					// VRS$B8!=P$7$?$i!"Ne<'%Q%k%9=PNO$9$k(B
	ID_CMD_DISABLE_EXCITATING = 12,					// VRS$B8!=P$7$F$b!"Ne<'%Q%k%9=PNO$7$J$$(B
	ID_CMD_SEARCH_1008_POSITION = 13,				// $B4p=`0LCV8!=P$7$F(B
    ID_CMD_TYPE_MAX
};
enum ID_CMD_MOTOR {
    ID_CMD_M0 = 0xB0,
    ID_CMD_M1 = 0xC0
};

#define     CMD_M0_FWD_NORMAL               ( ID_CMD_M0 + ID_CMD_FWD_NORMAL )                   	// $BDL>o$N@5E>(B1$BH/6nF0(B
#define     CMD_M0_REV_NORMAL               ( ID_CMD_M0 + ID_CMD_REV_NORMAL )                   	// $BDL>o$N5UE>(B1$BH/6nF0(B
#define     CMD_M0_FWD_WITH_SHAKE           ( ID_CMD_M0 + ID_CMD_FWD_WITH_SHAKE )               	// $BMIF0%Q%k%9IU$-$N@5E>!"7k2L$O!V!!!!!W$KF~$k(B
#define     CMD_M0_SEARCH_LOAD_POSITION     ( ID_CMD_M0 + ID_CMD_SEARCH_LOAD_POSITION )         	// $BHyD4@00LCV$r@5E>$GC5$9(B
#define     CMD_M0_SEARCH_BASE_POSITION     ( ID_CMD_M0 + ID_CMD_SEARCH_BASE_POSITION )         	// $B4p=`0LCV$r@5E>$GC5$9!JAFD4@08e$NHyD4@08!=P0LCV!K(B
#define     CMD_M0_FWD_WITH_CORRECT         ( ID_CMD_M0 + ID_CMD_FWD_WITH_CORRECT )             	// $B4p=`0LCV%:%lJd@5IU$-$N1??K!"0LCV%:%l8!=P$7$?$i!"<+F0Jd@5(B
#define     CMD_M0_FWD_POSITION_CTL         ( ID_CMD_M0 + ID_CMD_FWD_POSITION_CTL )             	//
#define     CMD_M0_REV_POSITION_CTL         ( ID_CMD_M0 + ID_CMD_REV_POSITION_CTL )             	//
#define     CMD_M0_START_DETECT_VRS			( ID_CMD_M0 + ID_CMD_START_DETECT_VRS )					//
#define     CMD_M0_STOP_DETECT_VRS			( ID_CMD_M0 + ID_CMD_STOP_DETECT_VRS  )					//
#define     CMD_M0_ENABLE_EXCITATING 		( ID_CMD_M0 + ID_CMD_ENABLE_EXCITATING )				//
#define     CMD_M0_DISABLE_EXCITATING 		( ID_CMD_M0 + ID_CMD_DISABLE_EXCITATING )				//
#define     CMD_M0_SEARCH_1008_POSITION		( ID_CMD_M0 + ID_CMD_SEARCH_1008_POSITION )				//

#define     CMD_M1_FWD_NORMAL               ( ID_CMD_M1 + ID_CMD_FWD_NORMAL )                   	// $BDL>o$N@5E>(B1$BH/6nF0(B
#define     CMD_M1_REV_NORMAL               ( ID_CMD_M1 + ID_CMD_REV_NORMAL )                   	// $BDL>o$N5UE>(B1$BH/6nF0(B
#define     CMD_M1_FWD_WITH_SHAKE           ( ID_CMD_M1 + ID_CMD_FWD_WITH_SHAKE )               	// $BMIF0%Q%k%9IU$-$N@5E>!"7k2L$O!V!!!!!W$KF~$k(B
#define     CMD_M1_SEARCH_LOAD_POSITION     ( ID_CMD_M1 + ID_CMD_SEARCH_LOAD_POSITION )         	// $BHyD4@00LCV$r@5E>$GC5$9(B
#define     CMD_M1_SEARCH_BASE_POSITION     ( ID_CMD_M1 + ID_CMD_SEARCH_BASE_POSITION )         	// $B4p=`0LCV$r@5E>$GC5$9!JAFD4@08e$NHyD4@08!=P0LCV!K(B
#define     CMD_M1_FWD_WITH_CORRECT         ( ID_CMD_M1 + ID_CMD_FWD_WITH_CORRECT )             	// $B4p=`0LCV%:%lJd@5IU$-$N1??K!"0LCV%:%l8!=P$7$?$i!"<+F0Jd@5(B
#define     CMD_M1_FWD_POSITION_CTL         ( ID_CMD_M1 + ID_CMD_FWD_POSITION_CTL )             	//
#define     CMD_M1_REV_POSITION_CTL         ( ID_CMD_M1 + ID_CMD_REV_POSITION_CTL )             	//
#define     CMD_M1_START_DETECT_VRS			( ID_CMD_M1 + ID_CMD_START_DETECT_VRS )					//
#define     CMD_M1_STOP_DETECT_VRS			( ID_CMD_M1 + ID_CMD_STOP_DETECT_VRS  )					//
#define     CMD_M1_ENABLE_EXCITATING 		( ID_CMD_M1 + ID_CMD_ENABLE_EXCITATING )				//
#define     CMD_M1_DISABLE_EXCITATING 		( ID_CMD_M1 + ID_CMD_DISABLE_EXCITATING )				//
#define     CMD_M1_SEARCH_1008_POSITION		( ID_CMD_M1 + ID_CMD_SEARCH_1008_POSITION )				//

#define		SPI_SLAVE_INT					LATC0
//------------------------------------------------------------------------------
//.$BJQ?t(B
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
//.$B4X?t(B
//.SPIControl_RecieveComannd
/***************************************************************************//**
 @brief     $B%3%^%s%I<u?.(B
 @note      $B%3%^%s%I<u$1IU$1$k$^$G!J(BSPI$B3d$j9~$_F~$k$^$G!K%Y%?BT$A(B
            $B%Y%?BT$ACf$G$b!"%b!<%?6nF0$,I,MW$J$H$-$O!"(B
            TMR2_Interrupt$B$G%b!<%?$O6nF0$9$k(B

            $B:GBg6nF0<~GH?t(B200Hz$B$G(B2$B%b!<%?$J$N$G!"(B
            5ms$B$N6h4V$G:GBg(B2$B2s$7$+%3%^%s%I$3$J$$(B
            $B!J%3%^%s%I%?%$%_%s%0$,>/$7Aa$+$C$?$H$7$F!"9g7W(B4$B%3%^%s%I!K(B
            $BF1$8%b!<%?$KBP$9$kL?Na$O(B1ms$B0J>e6u$1$k$3$H(B
            $B%G!<%?$O$f$C$/$j$5$P$$$FLdBj$J$$(B


 @author
 @date
*******************************************************************************/
void SPIControl_RecieveComannd( void )
{
    while ( 1 ) {
        /**-- 1byte$BFI$_=P$7(B --**/
        //      SPI_ReadBlock( &recieveCommand, 1 );            // recieveCommand$B$KJ]B8(B

        if ( PIR1bits.SSP1IF ) {
            PIR1bits.SSP1IF = 0;
            recieveCommand = SPI_ReadByte( );
            SPIControl_ExecuteCommand( ); // $B%3%^%s%I<B9T(B
        }

        WatchMotorDriveFinish( );
    }
}
//------------------------------------------------------------------------------
//.SPIControl_ExecuteCommand
/*******************************************************************************
 @brief     $B%3%^%s%I$NFbMF$K1~$8$F!"%b!<%?6nF0=hM}$r<B9T$9$k(B
 @note      $B%b!<%?6nF0Cf$N>l9g$O!"Dd;_8e$K<B9T$9$k!#(B
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
			/**-- $BJd@5IU$-6nF0$O%b!<%?6nF0Cf$G$b<B9T$9$k(B --**/
			    SpiCmmandFunction[ cmd_type ]( motor_select );
		} else {
			/**-- $BJd@5IU$-6nF00J30(B --**/
			if ( GetIsMotorActive( motor_select ) ) {
			    /**-- $B%b!<%?6nF0Cf$G$"$k"*2?$b$7$J$$(B --**/
			    SetAfterMotorStopFunc( motor_select, SpiCmmandFunction[ cmd_type ] );
			} else {
			    /**-- $B%b!<%?6nF0Cf$G$J$$"*$9$0$K%b!<%?6nF03+;O(B --**/
			    SpiCmmandFunction[ cmd_type ]( motor_select );
			}
		}
	}
}
//------------------------------------------------------------------------------
//..SpiCmdFunc_Test
/***************************************************************************//**
 @brief     $BF0:n%F%9%HMQ(B
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
 @brief     $B@5E>(B1$BH/6nF0(B(0xB1,0xC1)
 @note		$B%<%m%^%C%A=$@5$N%$%a!<%8!"?K0LCV%+%&%s%?99?7$7$J$$(B
 @author
 @date
 ********************************************************************************/
void SpiCmdFunc_FwdPulseNormal( UB motor_no )
{
	SetDriveType( motor_no, DRIVE_SIMPLE );

    SetMotorDirection( motor_no, FOWARD );          // $B@5E>%;%C%H(B

    SetMotorStepCounter( motor_no, 1 );             // $B6nF0%9%F%C%W?t(B1$BH/(B

    SetMotorFrequency( motor_no, FREQ_SET_EXTERNAL_CONTROL, NA );

    SetMotorStart( motor_no );                      // $B6nF03+;O(B
}
//------------------------------------------------------------------------------
//..SpiCmdFunc_RevPulseNormal(2)
/*******************************************************************************
 @brief     $B5UE>(B1$BH/6nF0(B(0xB2,0xC2)
 @note		$B%<%m%^%C%A=$@5$N%$%a!<%8!"?K0LCV%+%&%s%?99?7$7$J$$(B
 @author
 @date
 ********************************************************************************/
void SpiCmdFunc_RevPulseNormal( UB motor_no )
{
	SetDriveType( motor_no, DRIVE_SIMPLE );

    SetMotorDirection( motor_no, REVERSE ); 		// $B5UE>%;%C%H(B

    SetMotorStepCounter( motor_no, 1 ); 			// $B6nF0%9%F%C%W?t(B1$BH/(B

    SetMotorFrequency( motor_no, FREQ_SET_EXTERNAL_CONTROL, NA );

    SetMotorStart( motor_no ); 						// $B6nF03+;O(B
}
//------------------------------------------------------------------------------
//..$BL$:n@.(BSpiCmdFunc_FwdPulseWithShake(3)
/*******************************************************************************
 @brief     $BMIF0%Q%k%9IU$-@5E>(B1$BH/6nF0(B(0xB3,0xC3)
 @note		$B%<%m%^%C%A=$@5$N%$%a!<%8!"?K0LCV%+%&%s%?99?7$7$J$$(B
 @author
 @date
 ********************************************************************************/
void SpiCmdFunc_FwdPulseWithShake( UB motor_no )
{

}
//------------------------------------------------------------------------------
//..$BL$:n@.(BSpiCmdFunc_SearchLoadPosition(4)
/*******************************************************************************
 @brief     $BHyD4@00LCV8!=P(B(0xB4,0xC4)
 @note      $BIi2Y$r8!=P$7$?$i;_$^$k!#?K0LCV%+%&%s%?99?7$7$J$$(B
 @author
 @date
 ********************************************************************************/
void SpiCmdFunc_SearchLoadPosition( UB motor_no )
{

}
//------------------------------------------------------------------------------
//..SpiCmdFunc_SearchBasePosition(5)
/*******************************************************************************
 @brief     $B4p=`0LCV8!=P(B(0xB5,0xC5)
 @note      $B4p=`0LCV$O!"AFD4@08e$NHyD4@00LCV$H$9$k(B
 @author
 @date
 ********************************************************************************/
void SpiCmdFunc_SearchBasePosition( UB motor_no )
{
//	H recieve_offset;

	/**-- $B4p=`0LCV%*%U%;%C%H$r<u?.(B --**/
//	SPI_ReadBlock( ( UB* )&recieve_offset, 2 );

	DeactivateHandPosition( motor_no );

	/**-- $B6nF03+;O(B --**/
	SetDriveType( motor_no, DRIVE_SEARCH_BASE_POSITION );

    SetMotorDirection( motor_no, FOWARD );      // $B@5E>%;%C%H(B

    SetMotorStepCounter( motor_no, 0 );     	// $B6nF0%9%F%C%WL$@_Dj(B

    SetMotorFrequency( motor_no, FREQ_SET_SELF_CONTROL_SHAKE, SET_FREQ_60HZ ); //

    SetMotorPositionSearchStart( motor_no );         // $B6nF03+;O(B
}
//------------------------------------------------------------------------------
//..SpiCmdFunc_FwdPulseWithCorrect(6)
/*******************************************************************************
 @brief     $B0LCVJd@5IU$-$N@5E>(B1$BH/6nF0(B(0xB6,0xC6)
 @note      $B0LCV$,4|BTCM$H0c$C$?>l9g!"6/@)E*$K0LCVJd@5$r9T$&(B
			$B?K0LCV4IM}5!G=IU$-$N@5E>6nF0(B
			$BBP>]$N%b!<%?$,6nF0Cf$O0lC6Dd;_$5$;$F$+$i<B9T$5$;$k(B
			$B"*Dd;_8e=hM}$KEPO?$9$k(B
 @author
 @date
 ********************************************************************************/
void SpiCmdFunc_FwdPulseWithCorrect( UB motor_no )
{
	UB motor_bit;

	motor_bit = ( UB )( 0x01 << motor_no );

	if ( GetIsMotorActive( motor_no ) ) {
		/**-- $B%b!<%?6nF0Cf(B $B"*(B $B%b!<%?Dd;_MW5a(B & $BDd;_8e=hM}$rEPO?(B--**/
		SetMotorStop( motor_no );
		SetAfterMotorStopFunc( motor_no, SetFwdPulseWithCorrectProc );
	} else {
		/**-- $B%b!<%?6nF0Cf$G$O$J$$(B --**/
		SetFwdPulseWithCorrectProc( motor_no );
	}
}
//------------------------------------------------------------------------------
//...SetFwdPulseWithCorrectProc
/*******************************************************************************
 @brief		$B0LCVJd@5IU$-$N@5E>(B1$BH/6nF0L?NaK\BN(B
 @note		targetPosition$B$r(B+1$B$E!5$9$k(B

 			$BL\I80LCV$r8=:_0LCV$HHf3S$7$F!":9J,7W;;$7!"(B
 			$B@55U2sE>H=JL$7$F!"6nF0$5$;$k(B
 @author
 @date
********************************************************************************/
void SetFwdPulseWithCorrectProc( UB motor_no )
{
    UB motor_bit = ( UB )( 0x01 << motor_no );

    if ( 0 == ( actCorrectDriveBit & motor_bit ) ) {
		/**-- $BJd@56nF0Cf0J30(B --**/
    	SetDriveType( motor_no, DRIVE_CHECK_POSITION );
	    SetMotorFrequency( motor_no, FREQ_SET_EXTERNAL_CONTROL, NA );
    }

	UpdateTargetPosition( motor_no, 1 );			// $BL\I80LCV99?7(B
	CalcMotorStepCounter( motor_no );				// $BL\I80LCV$^$G$N6nF0%Q%k%9?t;;=P!u2sE>J}8~%;%C%H(B

	/**-- $BIi2Y0LCV$r8+$D$1$?$H$-$N=hM}$rDI2C(B --**/
	if ( ( reqCorrectBasePosBit | actCorrectDriveBit ) & motor_bit ) {
		/**-- $B0LCV:F8!=PCf(B --**/
		/**-- $BIi2YH/8+%3!<%k%P%C%/$OJQ99$7$J$$(B --**/
	} else {
		/**-- $BDL>oF0:nCf(B --**/
		SetAfterDetectLoadFunc( motor_no, VerifyHandPosition_LoadPosition );
	}

	/**-- $B6nF0%9%?!<%H(B --**/
    SetMotorStart( motor_no );                      // $B6nF03+;O(B
}
//------------------------------------------------------------------------------
//..SpiCmdFunc_FwdPulsePositionCtl(7)
/*******************************************************************************
 @brief     $B@5E>(B1$BH/6nF0(B(0xB7,0xC7)
 @note		$B@5E>(B1$BH/6nF0$7?K0LCV%+%&%s%?$b99?7$9$k(B
 @author
 @date
 ********************************************************************************/
void SpiCmdFunc_FwdPulsePositionCtl( UB motor_no )
{
	SetDriveType( motor_no, DRIVE_SIMPLE );

	UpdateTargetPosition( motor_no, 1 );			// $BL\I80LCV99?7(B
	CalcMotorStepCounter( motor_no );				// $BL\I80LCV$^$G$N6nF0%Q%k%9?t;;=P!u2sE>J}8~%;%C%H(B
													// $B8=:_0LCV99?7(B

    SetMotorFrequency( motor_no, FREQ_SET_EXTERNAL_CONTROL, NA );

    SetMotorStart( motor_no );                      // $B6nF03+;O(B
}
//------------------------------------------------------------------------------
//..SpiCmdFunc_RevPulsePositionCtl(8)
/*******************************************************************************
 @brief     $B5UE>(B1$BH/6nF0(B(0xB8,0xC8)
 @note		$B5UE>(B1$BH/6nF0$7?K0LCV%+%&%s%?$b99?7$9$k(B
 @author
 @date
 ********************************************************************************/
void SpiCmdFunc_RevPulsePositionCtl( UB motor_no )
{
	SetDriveType( motor_no, DRIVE_SIMPLE );

	UpdateTargetPosition( motor_no, -1 );			// $BL\I80LCV99?7(B
	CalcMotorStepCounter( motor_no );				// $BL\I80LCV$^$G$N6nF0%Q%k%9?t;;=P!u2sE>J}8~%;%C%H(B
													// $B8=:_0LCV99?7(B

    SetMotorFrequency( motor_no, FREQ_SET_EXTERNAL_CONTROL, NA );

    SetMotorStart( motor_no ); 						// $B6nF03+;O(B
}
//------------------------------------------------------------------------------
//..SpiCmdFunc_StartDetectVrs(9)
/***************************************************************************//**
 @brief		VRS$B8!=P3+;O(B(0xB9,0xC9)
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
 @brief		VRS$B8!=PDd;_(B(0xBA,0xCA)
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
 @brief		VRS$B8!=P$7$?$H$-$KNe<'$9$k@_Dj(B(0xBB,0xCB)
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
 @brief		VRS$B8!=P$7$?$H$-$KNe<'$7$J$$@_Dj(B(0xBC,0xCC)
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
 @brief     $B4p=`0LCV8!=P8e(B10$B!'(B08$B$K0\F0$9$k(B
 @note
 @author
 @date
 ********************************************************************************/
void SpiCmdFunc_Search1008Position( UB motor_no )
{
//	H recieve_offset;

	/**-- $B4p=`0LCV%*%U%;%C%H$r<u?.(B --**/
//	SPI_ReadBlock( ( UB* )&recieve_offset, 2 );

	DeactivateHandPosition( motor_no );

	/**-- $B6nF03+;O(B --**/
	SetDriveType( motor_no, DRIVE_SEARCH_1008_POSITION );

    SetMotorDirection( motor_no, FOWARD );      // $B@5E>%;%C%H(B

    SetMotorStepCounter( motor_no, 0 );     	// $B6nF0%9%F%C%WL$@_Dj(B

    SetMotorFrequency( motor_no, FREQ_SET_SELF_CONTROL_SHAKE, SET_FREQ_60HZ ); //

    SetMotorPositionSearchStart( motor_no );         // $B6nF03+;O(B
}
//------------------------------------------------------------------------------
//.SPIControl_SetNotification
/*******************************************************************************
 @brief		$B40N;DLCN(B
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

