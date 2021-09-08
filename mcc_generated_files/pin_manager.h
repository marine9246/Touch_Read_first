/**
  @Generated Pin Manager Header File

  @Company:
    Microchip Technology Inc.

  @File Name:
    pin_manager.h

  @Summary:
    This is the Pin Manager file generated using PIC10 / PIC12 / PIC16 / PIC18 MCUs

  @Description
    This header file provides APIs for driver for .
    Generation Information :
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.81.4
        Device            :  PIC16LF1509
        Driver Version    :  2.11
    The generated drivers are tested against the following:
        Compiler          :  XC8 2.20 and above
        MPLAB 	          :  MPLAB X 5.40	
*/

/*
    (c) 2018 Microchip Technology Inc. and its subsidiaries. 
    
    Subject to your compliance with these terms, you may use Microchip software and any 
    derivatives exclusively with Microchip products. It is your responsibility to comply with third party 
    license terms applicable to your use of third party software (including open source software) that 
    may accompany Microchip software.
    
    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY 
    IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS 
    FOR A PARTICULAR PURPOSE.
    
    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP 
    HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO 
    THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL 
    CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT 
    OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS 
    SOFTWARE.
*/

#ifndef PIN_MANAGER_H
#define PIN_MANAGER_H

/**
  Section: Included Files
*/

#include <xc.h>

#define INPUT   1
#define OUTPUT  0

#define HIGH    1
#define LOW     0

#define ANALOG      1
#define DIGITAL     0

#define PULL_UP_ENABLED      1
#define PULL_UP_DISABLED     0

// get/set M0_O1 aliases
#define M0_O1_TRIS                 TRISAbits.TRISA1
#define M0_O1_LAT                  LATAbits.LATA1
#define M0_O1_PORT                 PORTAbits.RA1
#define M0_O1_WPU                  WPUAbits.WPUA1
#define M0_O1_ANS                  ANSELAbits.ANSA1
#define M0_O1_SetHigh()            do { LATAbits.LATA1 = 1; } while(0)
#define M0_O1_SetLow()             do { LATAbits.LATA1 = 0; } while(0)
#define M0_O1_Toggle()             do { LATAbits.LATA1 = ~LATAbits.LATA1; } while(0)
#define M0_O1_GetValue()           PORTAbits.RA1
#define M0_O1_SetDigitalInput()    do { TRISAbits.TRISA1 = 1; } while(0)
#define M0_O1_SetDigitalOutput()   do { TRISAbits.TRISA1 = 0; } while(0)
#define M0_O1_SetPullup()          do { WPUAbits.WPUA1 = 1; } while(0)
#define M0_O1_ResetPullup()        do { WPUAbits.WPUA1 = 0; } while(0)
#define M0_O1_SetAnalogMode()      do { ANSELAbits.ANSA1 = 1; } while(0)
#define M0_O1_SetDigitalMode()     do { ANSELAbits.ANSA1 = 0; } while(0)

// get/set C1_OUT aliases
#define C1_OUT_TRIS                 TRISAbits.TRISA2
#define C1_OUT_LAT                  LATAbits.LATA2
#define C1_OUT_PORT                 PORTAbits.RA2
#define C1_OUT_WPU                  WPUAbits.WPUA2
#define C1_OUT_ANS                  ANSELAbits.ANSA2
#define C1_OUT_SetHigh()            do { LATAbits.LATA2 = 1; } while(0)
#define C1_OUT_SetLow()             do { LATAbits.LATA2 = 0; } while(0)
#define C1_OUT_Toggle()             do { LATAbits.LATA2 = ~LATAbits.LATA2; } while(0)
#define C1_OUT_GetValue()           PORTAbits.RA2
#define C1_OUT_SetDigitalInput()    do { TRISAbits.TRISA2 = 1; } while(0)
#define C1_OUT_SetDigitalOutput()   do { TRISAbits.TRISA2 = 0; } while(0)
#define C1_OUT_SetPullup()          do { WPUAbits.WPUA2 = 1; } while(0)
#define C1_OUT_ResetPullup()        do { WPUAbits.WPUA2 = 0; } while(0)
#define C1_OUT_SetAnalogMode()      do { ANSELAbits.ANSA2 = 1; } while(0)
#define C1_OUT_SetDigitalMode()     do { ANSELAbits.ANSA2 = 0; } while(0)

// get/set RA3 procedures
#define RA3_GetValue()              PORTAbits.RA3
#define RA3_SetDigitalInput()    do { TRISAbits.TRISA3 = 1; } while(0)
#define RA3_SetDigitalOutput()   do { TRISAbits.TRISA3 = 0; } while(0)
#define RA3_SetPullup()             do { WPUAbits.WPUA3 = 1; } while(0)
#define RA3_ResetPullup()           do { WPUAbits.WPUA3 = 0; } while(0)

// get/set M0_O3 aliases
#define M0_O3_TRIS                 TRISAbits.TRISA4
#define M0_O3_LAT                  LATAbits.LATA4
#define M0_O3_PORT                 PORTAbits.RA4
#define M0_O3_WPU                  WPUAbits.WPUA4
#define M0_O3_ANS                  ANSELAbits.ANSA4
#define M0_O3_SetHigh()            do { LATAbits.LATA4 = 1; } while(0)
#define M0_O3_SetLow()             do { LATAbits.LATA4 = 0; } while(0)
#define M0_O3_Toggle()             do { LATAbits.LATA4 = ~LATAbits.LATA4; } while(0)
#define M0_O3_GetValue()           PORTAbits.RA4
#define M0_O3_SetDigitalInput()    do { TRISAbits.TRISA4 = 1; } while(0)
#define M0_O3_SetDigitalOutput()   do { TRISAbits.TRISA4 = 0; } while(0)
#define M0_O3_SetPullup()          do { WPUAbits.WPUA4 = 1; } while(0)
#define M0_O3_ResetPullup()        do { WPUAbits.WPUA4 = 0; } while(0)
#define M0_O3_SetAnalogMode()      do { ANSELAbits.ANSA4 = 1; } while(0)
#define M0_O3_SetDigitalMode()     do { ANSELAbits.ANSA4 = 0; } while(0)

// get/set M0_O4 aliases
#define M0_O4_TRIS                 TRISAbits.TRISA5
#define M0_O4_LAT                  LATAbits.LATA5
#define M0_O4_PORT                 PORTAbits.RA5
#define M0_O4_WPU                  WPUAbits.WPUA5
#define M0_O4_SetHigh()            do { LATAbits.LATA5 = 1; } while(0)
#define M0_O4_SetLow()             do { LATAbits.LATA5 = 0; } while(0)
#define M0_O4_Toggle()             do { LATAbits.LATA5 = ~LATAbits.LATA5; } while(0)
#define M0_O4_GetValue()           PORTAbits.RA5
#define M0_O4_SetDigitalInput()    do { TRISAbits.TRISA5 = 1; } while(0)
#define M0_O4_SetDigitalOutput()   do { TRISAbits.TRISA5 = 0; } while(0)
#define M0_O4_SetPullup()          do { WPUAbits.WPUA5 = 1; } while(0)
#define M0_O4_ResetPullup()        do { WPUAbits.WPUA5 = 0; } while(0)

// get/set RB4 procedures
#define RB4_SetHigh()            do { LATBbits.LATB4 = 1; } while(0)
#define RB4_SetLow()             do { LATBbits.LATB4 = 0; } while(0)
#define RB4_Toggle()             do { LATBbits.LATB4 = ~LATBbits.LATB4; } while(0)
#define RB4_GetValue()              PORTBbits.RB4
#define RB4_SetDigitalInput()    do { TRISBbits.TRISB4 = 1; } while(0)
#define RB4_SetDigitalOutput()   do { TRISBbits.TRISB4 = 0; } while(0)
#define RB4_SetPullup()             do { WPUBbits.WPUB4 = 1; } while(0)
#define RB4_ResetPullup()           do { WPUBbits.WPUB4 = 0; } while(0)
#define RB4_SetAnalogMode()         do { ANSELBbits.ANSB4 = 1; } while(0)
#define RB4_SetDigitalMode()        do { ANSELBbits.ANSB4 = 0; } while(0)

// get/set M1_O4 aliases
#define M1_O4_TRIS                 TRISBbits.TRISB5
#define M1_O4_LAT                  LATBbits.LATB5
#define M1_O4_PORT                 PORTBbits.RB5
#define M1_O4_WPU                  WPUBbits.WPUB5
#define M1_O4_ANS                  ANSELBbits.ANSB5
#define M1_O4_SetHigh()            do { LATBbits.LATB5 = 1; } while(0)
#define M1_O4_SetLow()             do { LATBbits.LATB5 = 0; } while(0)
#define M1_O4_Toggle()             do { LATBbits.LATB5 = ~LATBbits.LATB5; } while(0)
#define M1_O4_GetValue()           PORTBbits.RB5
#define M1_O4_SetDigitalInput()    do { TRISBbits.TRISB5 = 1; } while(0)
#define M1_O4_SetDigitalOutput()   do { TRISBbits.TRISB5 = 0; } while(0)
#define M1_O4_SetPullup()          do { WPUBbits.WPUB5 = 1; } while(0)
#define M1_O4_ResetPullup()        do { WPUBbits.WPUB5 = 0; } while(0)
#define M1_O4_SetAnalogMode()      do { ANSELBbits.ANSB5 = 1; } while(0)
#define M1_O4_SetDigitalMode()     do { ANSELBbits.ANSB5 = 0; } while(0)

// get/set RB6 procedures
#define RB6_SetHigh()            do { LATBbits.LATB6 = 1; } while(0)
#define RB6_SetLow()             do { LATBbits.LATB6 = 0; } while(0)
#define RB6_Toggle()             do { LATBbits.LATB6 = ~LATBbits.LATB6; } while(0)
#define RB6_GetValue()              PORTBbits.RB6
#define RB6_SetDigitalInput()    do { TRISBbits.TRISB6 = 1; } while(0)
#define RB6_SetDigitalOutput()   do { TRISBbits.TRISB6 = 0; } while(0)
#define RB6_SetPullup()             do { WPUBbits.WPUB6 = 1; } while(0)
#define RB6_ResetPullup()           do { WPUBbits.WPUB6 = 0; } while(0)

// get/set M1_O3 aliases
#define M1_O3_TRIS                 TRISBbits.TRISB7
#define M1_O3_LAT                  LATBbits.LATB7
#define M1_O3_PORT                 PORTBbits.RB7
#define M1_O3_WPU                  WPUBbits.WPUB7
#define M1_O3_SetHigh()            do { LATBbits.LATB7 = 1; } while(0)
#define M1_O3_SetLow()             do { LATBbits.LATB7 = 0; } while(0)
#define M1_O3_Toggle()             do { LATBbits.LATB7 = ~LATBbits.LATB7; } while(0)
#define M1_O3_GetValue()           PORTBbits.RB7
#define M1_O3_SetDigitalInput()    do { TRISBbits.TRISB7 = 1; } while(0)
#define M1_O3_SetDigitalOutput()   do { TRISBbits.TRISB7 = 0; } while(0)
#define M1_O3_SetPullup()          do { WPUBbits.WPUB7 = 1; } while(0)
#define M1_O3_ResetPullup()        do { WPUBbits.WPUB7 = 0; } while(0)

// get/set IO_RC0 aliases
#define IO_RC0_TRIS                 TRISCbits.TRISC0
#define IO_RC0_LAT                  LATCbits.LATC0
#define IO_RC0_PORT                 PORTCbits.RC0
#define IO_RC0_ANS                  ANSELCbits.ANSC0
#define IO_RC0_SetHigh()            do { LATCbits.LATC0 = 1; } while(0)
#define IO_RC0_SetLow()             do { LATCbits.LATC0 = 0; } while(0)
#define IO_RC0_Toggle()             do { LATCbits.LATC0 = ~LATCbits.LATC0; } while(0)
#define IO_RC0_GetValue()           PORTCbits.RC0
#define IO_RC0_SetDigitalInput()    do { TRISCbits.TRISC0 = 1; } while(0)
#define IO_RC0_SetDigitalOutput()   do { TRISCbits.TRISC0 = 0; } while(0)
#define IO_RC0_SetAnalogMode()      do { ANSELCbits.ANSC0 = 1; } while(0)
#define IO_RC0_SetDigitalMode()     do { ANSELCbits.ANSC0 = 0; } while(0)

// get/set M0_O2 aliases
#define M0_O2_TRIS                 TRISCbits.TRISC1
#define M0_O2_LAT                  LATCbits.LATC1
#define M0_O2_PORT                 PORTCbits.RC1
#define M0_O2_ANS                  ANSELCbits.ANSC1
#define M0_O2_SetHigh()            do { LATCbits.LATC1 = 1; } while(0)
#define M0_O2_SetLow()             do { LATCbits.LATC1 = 0; } while(0)
#define M0_O2_Toggle()             do { LATCbits.LATC1 = ~LATCbits.LATC1; } while(0)
#define M0_O2_GetValue()           PORTCbits.RC1
#define M0_O2_SetDigitalInput()    do { TRISCbits.TRISC1 = 1; } while(0)
#define M0_O2_SetDigitalOutput()   do { TRISCbits.TRISC1 = 0; } while(0)
#define M0_O2_SetAnalogMode()      do { ANSELCbits.ANSC1 = 1; } while(0)
#define M0_O2_SetDigitalMode()     do { ANSELCbits.ANSC1 = 0; } while(0)

// get/set M1_O1 aliases
#define M1_O1_TRIS                 TRISCbits.TRISC2
#define M1_O1_LAT                  LATCbits.LATC2
#define M1_O1_PORT                 PORTCbits.RC2
#define M1_O1_ANS                  ANSELCbits.ANSC2
#define M1_O1_SetHigh()            do { LATCbits.LATC2 = 1; } while(0)
#define M1_O1_SetLow()             do { LATCbits.LATC2 = 0; } while(0)
#define M1_O1_Toggle()             do { LATCbits.LATC2 = ~LATCbits.LATC2; } while(0)
#define M1_O1_GetValue()           PORTCbits.RC2
#define M1_O1_SetDigitalInput()    do { TRISCbits.TRISC2 = 1; } while(0)
#define M1_O1_SetDigitalOutput()   do { TRISCbits.TRISC2 = 0; } while(0)
#define M1_O1_SetAnalogMode()      do { ANSELCbits.ANSC2 = 1; } while(0)
#define M1_O1_SetDigitalMode()     do { ANSELCbits.ANSC2 = 0; } while(0)

// get/set M1_O2 aliases
#define M1_O2_TRIS                 TRISCbits.TRISC3
#define M1_O2_LAT                  LATCbits.LATC3
#define M1_O2_PORT                 PORTCbits.RC3
#define M1_O2_ANS                  ANSELCbits.ANSC3
#define M1_O2_SetHigh()            do { LATCbits.LATC3 = 1; } while(0)
#define M1_O2_SetLow()             do { LATCbits.LATC3 = 0; } while(0)
#define M1_O2_Toggle()             do { LATCbits.LATC3 = ~LATCbits.LATC3; } while(0)
#define M1_O2_GetValue()           PORTCbits.RC3
#define M1_O2_SetDigitalInput()    do { TRISCbits.TRISC3 = 1; } while(0)
#define M1_O2_SetDigitalOutput()   do { TRISCbits.TRISC3 = 0; } while(0)
#define M1_O2_SetAnalogMode()      do { ANSELCbits.ANSC3 = 1; } while(0)
#define M1_O2_SetDigitalMode()     do { ANSELCbits.ANSC3 = 0; } while(0)

// get/set C2OUT aliases
#define C2OUT_TRIS                 TRISCbits.TRISC4
#define C2OUT_LAT                  LATCbits.LATC4
#define C2OUT_PORT                 PORTCbits.RC4
#define C2OUT_SetHigh()            do { LATCbits.LATC4 = 1; } while(0)
#define C2OUT_SetLow()             do { LATCbits.LATC4 = 0; } while(0)
#define C2OUT_Toggle()             do { LATCbits.LATC4 = ~LATCbits.LATC4; } while(0)
#define C2OUT_GetValue()           PORTCbits.RC4
#define C2OUT_SetDigitalInput()    do { TRISCbits.TRISC4 = 1; } while(0)
#define C2OUT_SetDigitalOutput()   do { TRISCbits.TRISC4 = 0; } while(0)

// get/set M0_VD aliases
#define M0_VD_TRIS                 TRISCbits.TRISC5
#define M0_VD_LAT                  LATCbits.LATC5
#define M0_VD_PORT                 PORTCbits.RC5
#define M0_VD_SetHigh()            do { LATCbits.LATC5 = 1; } while(0)
#define M0_VD_SetLow()             do { LATCbits.LATC5 = 0; } while(0)
#define M0_VD_Toggle()             do { LATCbits.LATC5 = ~LATCbits.LATC5; } while(0)
#define M0_VD_GetValue()           PORTCbits.RC5
#define M0_VD_SetDigitalInput()    do { TRISCbits.TRISC5 = 1; } while(0)
#define M0_VD_SetDigitalOutput()   do { TRISCbits.TRISC5 = 0; } while(0)

// get/set M1_VD aliases
#define M1_VD_TRIS                 TRISCbits.TRISC6
#define M1_VD_LAT                  LATCbits.LATC6
#define M1_VD_PORT                 PORTCbits.RC6
#define M1_VD_ANS                  ANSELCbits.ANSC6
#define M1_VD_SetHigh()            do { LATCbits.LATC6 = 1; } while(0)
#define M1_VD_SetLow()             do { LATCbits.LATC6 = 0; } while(0)
#define M1_VD_Toggle()             do { LATCbits.LATC6 = ~LATCbits.LATC6; } while(0)
#define M1_VD_GetValue()           PORTCbits.RC6
#define M1_VD_SetDigitalInput()    do { TRISCbits.TRISC6 = 1; } while(0)
#define M1_VD_SetDigitalOutput()   do { TRISCbits.TRISC6 = 0; } while(0)
#define M1_VD_SetAnalogMode()      do { ANSELCbits.ANSC6 = 1; } while(0)
#define M1_VD_SetDigitalMode()     do { ANSELCbits.ANSC6 = 0; } while(0)

// get/set RC7 procedures
#define RC7_SetHigh()            do { LATCbits.LATC7 = 1; } while(0)
#define RC7_SetLow()             do { LATCbits.LATC7 = 0; } while(0)
#define RC7_Toggle()             do { LATCbits.LATC7 = ~LATCbits.LATC7; } while(0)
#define RC7_GetValue()              PORTCbits.RC7
#define RC7_SetDigitalInput()    do { TRISCbits.TRISC7 = 1; } while(0)
#define RC7_SetDigitalOutput()   do { TRISCbits.TRISC7 = 0; } while(0)
#define RC7_SetAnalogMode()         do { ANSELCbits.ANSC7 = 1; } while(0)
#define RC7_SetDigitalMode()        do { ANSELCbits.ANSC7 = 0; } while(0)

/**
   @Param
    none
   @Returns
    none
   @Description
    GPIO and peripheral I/O initialization
   @Example
    PIN_MANAGER_Initialize();
 */
void PIN_MANAGER_Initialize (void);

/**
 * @Param
    none
 * @Returns
    none
 * @Description
    Interrupt on Change Handling routine
 * @Example
    PIN_MANAGER_IOC();
 */
void PIN_MANAGER_IOC(void);



#endif // PIN_MANAGER_H
/**
 End of File
*/