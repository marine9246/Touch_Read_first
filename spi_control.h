/*
 * File:   spi_control.h
 * Author: 110202117
 *
 * Created on 2020/09/01, 14:28
 */

#ifndef SPI_CONTROL_H
#define	SPI_CONTROL_H

#ifdef	__cplusplus
extern "C" {
#endif

//
extern void SPIControl_RecieveComannd( void );
extern void SpiCmdFunc_FwdPulseNormal( UB motor_no );

extern void SPIControl_SetNotification( UB motor_no );

#ifdef	__cplusplus
}
#endif

#endif	/* SPI_CONTROL_H */

