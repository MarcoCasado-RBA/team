/*!
 *****************************************************************************
 * @file:    AD7798.h
 * @brief:   AD7798 Driver
 * @version: $Revision$
 * @date:    $Date$
 *-----------------------------------------------------------------------------
 *
 *****************************************************************************/
 #include <inttypes.h> 
 #include "bcm2835.h"
#ifndef _COMMUNICATION_H_
#define _COMMUNICATION_H_

/*******************************************************************************
**************************** Internal types ************************************
********************************************************************************/

/* Write data mode */
typedef enum {
   SPI_WRITE_ONE_REG = 1,         /* Write 1 ACC register */
   SPI_WRITE_TWO_REG,             /* Write 2 ACC register */

} enWriteData;

typedef enum {
   SPI_READ_ONE_REG = 1,            /* Read one ACC register */
   SPI_READ_TWO_REG,                /* Read two ACC registers */
   SPI_READ_THREE_REG,              /* Read X,Y,Z ACC registers */
   SPI_READ_XYZ_FIFO = 6

} enRegsNum;


/*******************************************************************************
**************************** Internal definitions ******************************
********************************************************************************/

/* Accelerometer write command */
#define ADXL355_WRITE         0x0

/* Accelerometer read command */
#define ADXL355_READ          0x1

/*******************************************************************************
**************************** Functions declarations *****************************
********************************************************************************/

/* SPI Functions */
int SPI_Init(void);
void SPI_End(void);
void SPI_Write(uint8_t ui8address, uint8_t ui8Data, uint8_t ui8Data2, enWriteData enMode);
uint32_t SPI_Read(uint8_t ui8address, enRegsNum enRegs);

#endif /* _COMMUNICATION_H_ */

