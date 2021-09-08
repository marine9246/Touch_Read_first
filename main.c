#include "mcc_generated_files/mcc.h"
#include "MotorDrive.h"
#include "misc.h"
#include "spi_control.h"
#include "data_types.h"
/******************************************************************************/
/*
                         Main application
 */

//.main
/*******************************************************************************
 *  Main
 * $B%Z%j%U%'%i%k$N=i4|@_Dj$O!"<+F0@_Dj$7$F$$$k$,!"%^%K%e%"%k$GE,;~JQ99$9$k(B
 ******************************************************************************/
void main(void)
{
    // initialize the device
    SYSTEM_Initialize();

	TMR1_StopTimer();
//	TMR2_StopTimer();

    /*$B3d$j9~$_%O%s%I%i!<%;%C%H(B*/
    //TMR0_SetInterruptHandler(TMR0_Interrupt);
//    TMR1_SetInterruptHandler(TMR1_Interrupt);
    TMR2_SetInterruptHandler(TMR2_Interrupt);


    // When using interrupts, you need to set the Global and Peripheral Interrupt Enable bits
    // Use the following macros to:

    // Enable the Global Interrupts
    INTERRUPT_GlobalInterruptEnable();

    // Enable the Peripheral Interrupts
    INTERRUPT_PeripheralInterruptEnable();

    // Disable the Global Interrupts
    //INTERRUPT_GlobalInterruptDisable();

    // Disable the Peripheral Interrupts
    //INTERRUPT_PeripheralInterruptDisable();

	InitPulsePositionTable( MOTOR_M0 );
	InitPulsePositionTable( MOTOR_M1 );

	SetMotorDirection( MOTOR_M0, FOWARD );			// $B@5E>%;%C%H(B
	SetMotorStepCounter( MOTOR_M0, 80 );			// $B6nF0%9%F%C%W?t(B1$BH/(B
	SetMotorAddShakePulse( MOTOR_M0, OFF );			// $BMIF0%Q%k%9$J$7(B
	SetMotorFrequency( MOTOR_M0, FREQ_SET_SELF_CONTROL_NORMAL, SET_FREQ_200HZ );

	SetMotorDirection( MOTOR_M1, FOWARD );			// $B@5E>%;%C%H(B
	SetMotorStepCounter( MOTOR_M1, 120 );			// $B6nF0%9%F%C%W?t(B1$BH/(B
	SetMotorAddShakePulse( MOTOR_M1, OFF );			// $BMIF0%Q%k%9$J$7(B
	SetMotorFrequency( MOTOR_M1, FREQ_SET_SELF_CONTROL_NORMAL, SET_FREQ_200HZ );

	SetMotorStart( MOTOR_M0 );						// $B6nF03+;O(B
	SetMotorStart( MOTOR_M1 );						// $B6nF03+;O(B



    
    
    
	SPI_Open(SPI_DEFAULT);

    while (1) {

		SPIControl_RecieveComannd();

    }
}
//------------------------------------------------------------------------------
