#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int main (void)
{
	int bright ;

	printf ("Raspberry Pi wiringPi PWM test program\n") ;

	if (wiringPiSetup () == -1)
	exit (1) ;

	pinMode (1, PWM_OUTPUT) ;

	for (;;)
	{
		printf("GIRO 1\n");
		for (bright = 0 ; bright < 1024 ; ++bright)
		{
			pwmWrite (1, bright) ;
			delay (1) ;
		}

		printf("GIRO 2\n");
		for (bright = 1023 ; bright >= 0 ; --bright)
		{
			pwmWrite (1, bright) ;
			delay (1) ;
		}
	}

	return 0 ;
}
