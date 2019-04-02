#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include "ADXL355.h"
#include "communication.h"
#include <inttypes.h>

/****************************** Global Data ***********************************/
 int32_t  i32SensorX;
 int32_t  i32SensorY;
 int32_t  i32SensorZ;
 int32_t  i32SensorT;

 uint32_t  ui32SensorX;
 uint32_t  ui32SensorY;
 uint32_t  ui32SensorZ;
 uint32_t  ui32SensorT;

/************************* Global scope functions *****************************/

/**
   @brief Lee bit data_ready del registro STATUS, bit se pone a 0 cuando la informacion X,Y,Z o FIFO es leida
   @return 1 si existe un nuevo dato
**/
int8_t ADXL355_Data_Ready(void)
 {
    uint8_t ui8temp;
    ui8temp = (uint8_t)SPI_Read(STATUS, SPI_READ_ONE_REG) & 0x01; //bit 0 data ready
    return ui8temp;
 }

/**
 * Pone al acelerometro en modo de prueba, no entrega informacion del MEM, 
 * solo valores fijos aproximados X=0.3, Y=0.3, Z =1.5
*/
void ADXL355_Self_Test(void)
 {
    uint8_t ui8temp;
    ui8temp = 0x03; //Documentar!! o comentar al menos
    SPI_Write(SELF_TEST, ui8temp, 0x00, SPI_WRITE_ONE_REG);
 }


/**
 * @brief Setea los primeros 4 bits del registro filter (no enmascara los 4 restantes!!)
 * este registro especifica los parametros del filtro pasa bajos y el ODR
   @return none
            Filter    Resultado
            0b0000 -> 4000 Hz and 1000 Hz  
            0b0001 -> 2000 Hz and 500 Hz 
            0b0010 -> 1000 Hz and 250 Hz 
            0b0011 -> 500 Hz and 125 Hz  
            0b0100 -> 250 Hz and 62.5 Hz 
            0b0101 -> 125 Hz and 31.25 Hz 
            0b0110 -> 62.5 Hz and 15.625 Hz 
            0b0111 -> 31.25 Hz and 7.813 Hz 
            0b1000 -> 15.625 Hz and 3.906 Hz 
            0b1001 -> 7.813 Hz and 1.953 Hz 
            0b1010 -> 3.906 Hz and 0.977 Hz 
 */
void ADXL355_ODR_LPF(void)
 {
    uint8_t ui8temp = 0x00;
    SPI_Write(FILTER, ui8temp, 0x00, SPI_WRITE_ONE_REG);
    ui8temp = (uint8_t)SPI_Read(FILTER, SPI_READ_ONE_REG);
    ui8temp = 0x02;
    SPI_Write(FILTER, ui8temp, 0x00, SPI_WRITE_ONE_REG);
 }

/**
 * @brief Inicia el acelerometro, si esta funcion no es llamada la salida cerá 0
   @return none
 */
void ADXL355_Start_Sensor(void)
{
   uint8_t ui8temp;
   ui8temp = (uint8_t)SPI_Read(POWER_CTL, SPI_READ_ONE_REG);       /* Read POWER_CTL register, before modifying it */
   ui8temp = ui8temp & 0xFE;                                            /* Set measurement bit in POWER_CTL register */
   SPI_Write(POWER_CTL, ui8temp, 0x00, SPI_WRITE_ONE_REG);         /* Write the new value to POWER_CTL register */
}


/**
   @brief Pone el acelerometro en modo standby.
   @return none
**/
void ADXL355_Stop_Sensor(void)
{
   uint8_t ui8temp;
   ui8temp = (uint8_t)SPI_Read(POWER_CTL, SPI_READ_ONE_REG);            //Read POWER_CTL register, before modifying it */
   ui8temp = ui8temp | 0x01;                                            //Clear measurement bit in POWER_CTL register */
   SPI_Write(POWER_CTL, ui8temp, 0x00, SPI_WRITE_ONE_REG);              // Write the new value to POWER_CTL register */
}

/**
   @brief Lee la informacion mas actual del accelerometro (no FIFO).
   @return none
**/
void ADXL355_Data_Scan(void)
{
      ui32SensorX = SPI_Read(XDATA3, SPI_READ_THREE_REG);
      ui32SensorY = SPI_Read(YDATA3, SPI_READ_THREE_REG);
      ui32SensorZ = SPI_Read(ZDATA3, SPI_READ_THREE_REG);

      i32SensorX = ADXL355_Acceleration_Data_Conversion(ui32SensorX);
      i32SensorY = ADXL355_Acceleration_Data_Conversion(ui32SensorY);
      i32SensorZ = ADXL355_Acceleration_Data_Conversion(ui32SensorZ);
}

/**
   @brief Convierte datos con complemento a 2 en los registros X,Y,Z en enteros con signo
   @param ui32SensorData - raw data from register
   @return int32_t - signed integer data
**/
int32_t ADXL355_Acceleration_Data_Conversion (uint32_t ui32SensorData)
{
   int32_t  i32Conversion = 0;

   ui32SensorData = (ui32SensorData  >> 4);
   if(ui32SensorData & (1 << 20 - 1)){i32Conversion = ui32SensorData - (1 << 20);}
   else{i32Conversion = ui32SensorData;}

   return i32Conversion;
}
