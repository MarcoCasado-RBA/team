/*
 * communication.xc
 *
 *  Created on: 24-01-2018
 *  Author: marco casado para RBA
 * 
 * libreria modificada desde Demo_adxl355_pmdz adaptada y aumentada para su uso con RPI2
 * Copyright (c) 2015-2017 Analog Devices, Inc.

 */

/***************************** Include Files **********************************/
#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <inttypes.h>
#include "communication.h"
#include "ADXL355.h"

/************************* Functions Definitions ******************************/

/**
   @brief Inicia SPI a 8MHZ, nodo 0, MSB, CS_low
   @param  none
   @return none

**/
int SPI_Init(void)
{
    if (!bcm2835_init())
    {
        printf("bcm2835_init failed. Are you running as root??\n");
        return 1;
    }
    if (!bcm2835_spi_begin())
    {
        printf("bcm2835_spi_begin failed. Are you running as root??\n");
        return 1;
    }
    bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);      // MBS standar
    bcm2835_spi_setDataMode(BCM2835_SPI_MODE0);                   // ADLX en MODO 0
    bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_32);    //32 = 7.8125MHz on Rpi2 
    bcm2835_spi_chipSelect(BCM2835_SPI_CS0);                      // Utilizo Pin CS0 como chipEnable (CS1 queda disponible)
    bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS0, LOW);      // SPI activo en estado bajo
}

/**
   @brief Cierra comunicación SPI junto con la libreria bcm, debe ser reabierta si es utiliza I2C o GPIO
   @param  none
   @return none

**/
void SPI_End(void)
{
    bcm2835_spi_end();
    bcm2835_close();
}

/**
   @brief Metodo que escribe datos o comandos al ADLX355 via SPI halfDuplex

   @param ui8address    - Direccion de registro del ADLX355
   @param ui8Data       - valor 1° byte del registro
   @param ui8Data2      - valor 2° byte del registro si corresponde
   @param enMode        - numero de registros a escribir 
   @return none

**/
void SPI_Write(uint8_t ui8address, uint8_t ui8Data, uint8_t ui8Data2, enWriteData enMode)
{
    
    uint8_t mosi[3] = {0};
    mosi[0] = ((ui8address <<1)|ADXL355_WRITE);  
    if(enMode == SPI_WRITE_ONE_REG)
    {
        mosi[1] = ui8Data;
        bcm2835_spi_writenb(mosi,2);              
    }
    if(enMode == SPI_WRITE_TWO_REG)
    {
        mosi[1] = ui8Data;
        mosi[2] = ui8Data2;
        bcm2835_spi_writenb(mosi,3);                 
   }
}

/**
   @brief Lee 1,2 ó 3 registros en la direccion señalada via SPI para el ADLX355

   @param ui8address - Dirección del registro
   @param enRegs - cantidad de registros a leer
   @return resultado de la lectura en variable de 32bits

**/
uint32_t SPI_Read(uint8_t ui8address, enRegsNum enRegs)
{
	uint8_t mosi[4] = {0, 0xAA, 0xAA, 0xAA};
	uint8_t miso[4] = {0};
    uint32_t ui32Result;
	
    mosi[0] = ((ui8address <<1)|ADXL355_READ);
    if (enRegs == SPI_READ_ONE_REG) {
		bcm2835_spi_transfernb(mosi,miso,2);
		ui32Result = miso[1];
    }
    if (enRegs == SPI_READ_TWO_REG) {                       // Only used for Temp & X,Y,Z offset and threshold registers
        bcm2835_spi_transfernb(mosi,miso,3);
        ui32Result = ((miso[1] << 8) | miso[2]);      		// Set read result
    }
    if (enRegs == SPI_READ_THREE_REG) {                     // Only used for X,Y,Z axis data registers
        bcm2835_spi_transfernb(mosi,miso,4);
        ui32Result = ((miso[1] << 16) | (miso[2] << 8) | (miso[3]));
    }
    if(enRegs == SPI_READ_XYZ_FIFO){
                /*
                for(int i =0; i<4;i++){
                    uart_tx.write((cont>>(i*8)) & 0xff);
                }*/
    }
    return ui32Result;
}