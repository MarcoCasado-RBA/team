/*!
 *****************************************************************************
 * @file:    main.c
 * @brief:   Main source file
 * @version: $0.1$
 * @date:    $Date$
 * 
 * 
 * 
 * Modo de compilacion solo spi: gcc -o spi main.c -l bcm2835 communication.h communication.c ADXL355.c ADXL355.h bcm2835.h bcm2835.c
 * 
 * cuando uso gpio gcc -o spi main.c communication.h communication.c ADXL355.c ADXL355.h bcm2835.h bcm2835.c

git test

 * 
 *****************************************************************************/

/***************************** Include Files **********************************/
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include "bcm2835.h"
#include "ADXL355.h"
#include "communication.h"
#include <sys/time.h>
//#include <time.h>

/* Sample pragmas to cope with warnings. Please note the related line at
  the end of this function, used to pop the compiler diagnostics status. */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"


//**************************** Definicion de funciones ****************************//

/*
* Loop infinito, nunca retorna (dah!)
*/

int main(int argc, char *argv[])
{
  float volatile f32temp = 0.0f;
  float adxl355Scale;
  struct timeval tv;
 
  unsigned long a;
  char titulo;

  //printf("minutos a grabar: \n");
  //scanf("%lu",&a);
  
  // Inicio el SPI1 
  SPI_Init();
  
  //Rango se define en header comunication.h
  #if ADXL_RANGE == 2
    SPI_Write(RANGE, 0x81, 0x00, SPI_WRITE_ONE_REG);          /* Set sensor range within RANGE register */
    adxl355Scale = 256000.0f;
  #endif

  #if ADXL_RANGE == 4
    SPI_Write(RANGE, 0x82, 0x00, SPI_WRITE_ONE_REG);          /* Set sensor range within RANGE register */
    adxl355Scale = 128000.0f;
  #endif

  #if ADXL_RANGE == 8
    SPI_Write(RANGE, 0x83, 0x00, SPI_WRITE_ONE_REG);          /* Set sensor range within RANGE register */
    adxl355Scale = 64000.0f;
  #endif

  //se considera implementar parámetros para ingresar el valor pasabajos y retornar valor del registro
  ADXL355_ODR_LPF(); //limpia ODR y filtro
  
  /* El acelerometro comienza a medir */
  ADXL355_Start_Sensor();


  printf("\nNombre del archivo a grabar: \n");
  scanf("%s",&titulo);
  /*
  inicio el pin para dt_ready
  */
  //bcm2835_init(); 
  //bcm2835_gpio_fsel(RPI_V2_GPIO_P1_22,BCM2835_GPIO_FSEL_INPT);  //pin22 como entrada


  // Creo el archivo que guardará los datos con la fecha y horas actuales 
  //char filename[40];
  //struct tm *timenow;
  //time_t now = time(NULL);
  //timenow = gmtime(&now);
  //strftime(filename, sizeof(filename), "TEST_ADXL355_%Y-%m-%d_%H:%M:%S", timenow);
  //FILE *f = fopen(filename, "a");
  FILE *f = fopen(&titulo, "a");
  if (f == NULL)
  {
      printf("Error al abrir el archivo!\n");
      exit(1);
  }




  //Leo registros de ID para comprobar comunicacion (valores no cambian)
  uint32_t volatile ui32test = SPI_Read(DEVID_AD, SPI_READ_ONE_REG);       /* Read the ID register */
  uint32_t volatile ui32test2 = SPI_Read(DEVID_MST, SPI_READ_ONE_REG);     /* Read the Manufacturer register */
  uint32_t volatile ui32test3 = SPI_Read(PARTID, SPI_READ_ONE_REG);        /* Read the Part ID register */

  printf("\r\n The DEVID_AD  = 0x%2x", ui32test);                 /* Print the ID register */
  printf("\r\n The DEVID_MST = 0x%2x", ui32test2);                /* Print the Manufacturer register */
  printf("\r\n The PARTID    = 0x%2x", ui32test3);                /* Print the Part ID register */

  
  //por alguna razón nunca me imprime esto (revisar)
  ui32SensorT = SPI_Read(TEMP2, SPI_READ_TWO_REG);
  f32temp = ((((float)ui32SensorT - ADXL355_TEMP_BIAS)) / ADXL355_TEMP_SLOPE) + 25.0;
  printf("\r\n The temperature data is: %.2f [C]", f32temp);                                 /* Print the Temperature data */
  
  // Infinite loop
  uint32_t cont = 0;
  
  while (cont < 600000)
  {
      //if(bcm2835_gpio_lev(RPI_V2_GPIO_P1_22)) //polling mediante pin dataReady
      if(ADXL355_Data_Ready()== 0x01) //polling mediante dato SPI dataReady (pierde datos)
      {
        gettimeofday(&tv,NULL); //...marca el tiempo y ...
        ADXL355_Data_Scan();   //...leelo 
        /*
        fprintf(f,"\r\n X acceleration data [G]: %f [g]", (float)i32SensorX / adxl355Scale);        // Print the X-axis data 
        fprintf(f,"\r\n Y acceleration data [G]: %f [g]", (float)i32SensorY / adxl355Scale);        // Print the Y-axis data 
        fprintf(f,"\r\n Z acceleration data [G]: %f [g]", (float)i32SensorZ / adxl355Scale);        // Print the Z-axis data 
        */

        //guardo en el fichero bajo formato tiempo;accX;accY;aCCZ;\n
        fprintf(f,"%"PRIu64";",(((long long int) tv.tv_sec) * 1000000ll + (long long int) tv.tv_usec));     
        fprintf(f,"%"PRId32";", i32SensorX);                          // Print the X-axis data 
        fprintf(f,"%"PRId32";", i32SensorY);                          // Print the Y-axis data 
        fprintf(f,"%"PRId32";\n", i32SensorZ);                        // Print the Z-axis data 
        cont++;
      }
  }
  
  ADXL355_Stop_Sensor(); //Apago el acelerometro
  SPI_End();  //cierro SPI
  fclose(f);  //Cierro el txt
	f = NULL;
  printf("\n Datos listos \n");
}

#pragma GCC diagnostic pop

/* End Of File *///
