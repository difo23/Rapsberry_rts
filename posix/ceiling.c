/*
 * Implement 3 periodic threads, with periods 50ms, 100ms, and 150ms.
 * The job bodies are functions named task1(), task2() and task3()
 */

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <stdint.h>
#include <wiringPiSPI.h>
#include <string.h>
#include <errno.h>
#include "periodic_tasks.h"
#include <wiringPi.h>


static pthread_mutex_t mymutex;
static int th_cnt;
int pwm;

/* periodic threads */

void *thread1(void *param)
{
  struct periodic_task *p_d;

  p_d = start_periodic_timer(0,250000);
  while (1) {
    wait_next_activation(p_d);
    task1();
  }
}

void *thread2(void *param)
{
  struct periodic_task *p_d;

  p_d = start_periodic_timer(0,450000);
  while (1) {
    wait_next_activation(p_d);
    task2();
  }
}

void *thread3(void *param)
{
  struct periodic_task *p_d;

  p_d = start_periodic_timer(0,300000);
  while (1) {
    wait_next_activation(p_d);
    task3();
  }
}

/* activity of tasks */

void task1(void)
{
    /*CUERPO DE LA TAREAS*/
    /*ADC*/
        int i,analog ;


  	if (wiringPiSPISetup (0, 1000000) < 0)//Definimos la conexion A 0.5 MHz
  	{
    		fprintf (stderr, "Unable to open SPI device 0: %s\n", strerror (errno)) ;
    		exit (1) ;

	}
            analog=analogRead(0);
            
			

	/*while(1){
		for(i=0;i<8;i++){
			analog=analogRead(i);
			printf("ADC%d:%d  ",i,analog);
			delay(100);
			}

		printf("\n");
		delay(250);
 	}*/
    
    pthread_mutex_lock(&mymutex);
    /*REGION CRITICA */
    pwm=analog;
    pthread_mutex_unlock(&mymutex);
}

void task2(void)
{
    /*CUERPO DE LA TAREAS*/

    pthread_mutex_lock(&mymutex);
    pinMode (1, PWM_OUTPUT) ;
    pwmWrite (1, pwm);
    delay (1) ;
    pthread_mutex_unlock(&mymutex);   
}

void task3(void)
{
    /*CUERPO DE LA TAREA */
	printf("Este es el Display \n");
	printf("ADC%d:%d  \n",0,pwm);
	printf("El Giro:%d \n",pwm);
}

/* -------------------- */
/* body of main program */
/* -------------------- */


/*ADC READ*/
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

int main(int argc, char *argv[])

{
	
	
  int err;
  void *returnvalue;
  pthread_t second_id, third_id, fourth_id;
  int pmin;
  pthread_attr_t attrs;
  struct sched_param sp;

  pthread_mutexattr_t mymutexattr;
  
  	printf("LOADING GPIO SPI\n");
	system("gpio load spi");

  //-------- init attributes of tasks 

  pthread_attr_init(&attrs);

  err = pthread_attr_setinheritsched(&attrs, PTHREAD_EXPLICIT_SCHED);
  if (err != 0) {
    perror("setineheritsched");
  }

  err = pthread_attr_setschedpolicy(&attrs, SCHED_FIFO);
  if (err != 0) {
    perror("setschedpolicy");
  }

  pmin = sched_get_priority_min(SCHED_FIFO);

  //-------- creation of mutex 

  pthread_mutexattr_init(&mymutexattr);

  if (pthread_mutexattr_setprotocol (&mymutexattr, PTHREAD_PRIO_PROTECT) != 0) {
    perror ("Error in mutex attribute setprotocol \n");
  }

  //-------- ceiling priority of mutex 

  if (pthread_mutexattr_setprioceiling (&mymutexattr, pmin + 5) != 0) {
    perror ("Error in mutex attribute setprotocol \n");
  }

  //-------- init mutex 
  if (pthread_mutex_init (&mymutex,&mymutexattr) != 0) {
    perror ("Error in mutex init");
  }

  pthread_mutexattr_destroy(&mymutexattr);

  //--------  creation and activation of the new thread 
  sp.sched_priority = pmin +2;
  err = pthread_attr_setschedparam(&attrs, &sp);
  if (err != 0) {
    perror("setschedparam");
  }
  err = pthread_create(&second_id, &attrs, thread1, (void *)NULL);
  if (err != 0) {
    fprintf(stderr, "Cannot create pthread 1");
  }


  sp.sched_priority = pmin + 3;
  err = pthread_attr_setschedparam(&attrs, &sp);
  if (err != 0) {
    perror("setschedparam");
  }
  err = pthread_create(&third_id, &attrs, thread2, (void *)NULL);
  if (err != 0) {
    fprintf(stderr, "Cannot create pthread 2");
  }


  sp.sched_priority = pmin + 4;
  err = pthread_attr_setschedparam(&attrs, &sp);
  if (err != 0) {
    perror("setschedparam");
  }
  err = pthread_create(&fourth_id, &attrs, thread3, (void *)NULL);
  if (err != 0) {
    fprintf(stderr, "Cannot create pthread 3");
  }

  //-------- We wait the end of the threads we just created. 
  pthread_join(second_id, &returnvalue);
  pthread_join(third_id, &returnvalue);
  pthread_join(fourth_id, &returnvalue);


  printf("main: returnvalue is %d\n", (int)returnvalue);

  return 0;
}

