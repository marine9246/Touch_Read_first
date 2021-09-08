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
 * ペリフェラルの初期設定は、自動設定しているが、マニュアルで適時変更する
 ******************************************************************************/
void main(void)
{
    // initialize the device
    SYSTEM_Initialize();

	TMR1_StopTimer();
//	TMR2_StopTimer();

    /*割り込みハンドラーセット*/
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

	SetMotorDirection( MOTOR_M0, FOWARD );			// 正転セット
	SetMotorStepCounter( MOTOR_M0, 80 );			// 駆動ステップ数1発
	SetMotorAddShakePulse( MOTOR_M0, OFF );			// 揺動パルスなし
	SetMotorFrequency( MOTOR_M0, FREQ_SET_SELF_CONTROL_NORMAL, SET_FREQ_200HZ );

	SetMotorDirection( MOTOR_M1, FOWARD );			// 正転セット
	SetMotorStepCounter( MOTOR_M1, 120 );			// 駆動ステップ数1発
	SetMotorAddShakePulse( MOTOR_M1, OFF );			// 揺動パルスなし
	SetMotorFrequency( MOTOR_M1, FREQ_SET_SELF_CONTROL_NORMAL, SET_FREQ_200HZ );

	SetMotorStart( MOTOR_M0 );						// 駆動開始
	SetMotorStart( MOTOR_M1 );						// 駆動開始



    
    
    
	SPI_Open(SPI_DEFAULT);

    while (1) {

		SPIControl_RecieveComannd();

    }
}
//------------------------------------------------------------------------------
