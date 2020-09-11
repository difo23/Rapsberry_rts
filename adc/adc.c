#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>
#include <wiringPiSPI.h>

int analogRead(int pin){
	int ADC=-1;
	if((pin>=0)&&(pin<=7))
	{
		int ce = 0;
  		unsigned char ByteSPI[7];

  		// Cargamos los datos
  		ByteSPI[0] = 0b01;//El ultimo bit es el start
  		ByteSPI[1]=(0x80)|(pin<<4);//4 primeros bits configuracion ver tabla
  		ByteSPI[2]=0;//Byte donde nos escriben los 8 ultimos bits.
  		wiringPiSPIDataRW (ce, ByteSPI, 3);//Enviamos la trama
  		usleep(20);//Esperamos 20 microsegundos

   		ADC=((ByteSPI[1]&0x03)<<8)|ByteSPI[2];//Tratamos los datos
	}
	return (ADC);
}

int main (void) {
  	int i,analog ;
  	printf ("Tutorial ADC Raspberry www.electroensaimada.com\n") ;
	printf("LOADING GPIO SPI\n");
	system("gpio load spi");

  	if (wiringPiSPISetup (0, 1000000) < 0)//Definimos la conexion A 0.5 MHz
  	{
    		fprintf (stderr, "Unable to open SPI device 0: %s\n", strerror (errno)) ;
    		exit (1) ;

	}

	while(1){
		for(i=0;i<8;i++){
			analog=analogRead(i);
			printf("ADC%d:%d  ",i,analog);
			delay(100);
			}

		printf("\n");
		delay(250);
 	}
  return (0) ;
}
