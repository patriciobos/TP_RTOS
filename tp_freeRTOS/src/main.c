/*
FreeRTOS V6.1.1 - Copyright (C) 2011 Real Time Engineers Ltd.

This file is part of the FreeRTOS distribution.

FreeRTOS is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License (version 2) as published by the
Free Software Foundation AND MODIFIED BY the FreeRTOS exception.
 ***NOTE*** The exception to the GPL is included to allow you to distribute
a combined work that includes FreeRTOS without being obliged to provide the
source code for proprietary components outside of the FreeRTOS kernel.
FreeRTOS is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
more details. You should have received a copy of the GNU General Public
License and the FreeRTOS license exception along with FreeRTOS; if not it
can be viewed here: http://www.freertos.org/a00114.html and also obtained
by writing to Richard Barry, contact details for whom are available on the
FreeRTOS WEB site.

1 tab == 4 spaces!

http://www.FreeRTOS.org - Documentation, latest information, license and
contact details.

http://www.SafeRTOS.com - A version that is certified for use in safety
critical systems.

http://www.OpenRTOS.com - Commercial support, development, porting,
licensing and training services.
 */

/* FreeRTOS.org includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"

/* Demo includes. */
#include "basic_io.h"

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#include "lpc17xx_uart.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_rtc.h"
#include "lpc_types.h"
#endif

#include <string.h>
#include <cr_section_macros.h>
#include <NXP/crp.h>
#include <math.h>

/* The tasks to be created. */
//static void vHandlerTask( void *pvParameters );
static void vPeriodicTask1(void);
static void vAsincronicTask2(void);
static void vAsincronicTask3(void);

void vSetupRtc(void);
void vSetupIntExt(void);
void vSetupUart3(void);
void UART3_Enviar(unsigned char* buffer, uint32_t size);



/*-----------------------------------------------------------*/

/* Declare a variable of type xSemaphoreHandle. This is used to reference the
semaphore that is used to synchronize a task with an interrupt. */
xSemaphoreHandle xCountingSemaphore;

/* Declare a variable of type xSemaphoreHandle. This is used to reference the
mutex type semaphore that is used to ensure mutual exclusive access to UART3. */
xSemaphoreHandle xMutex;

/* Declare a variable of type xQueueHandle. This is used to store the queue
that is accessed by task 3. */
xQueueHandle xQueue;
/*-----------------------------------------------------------*/

const char welcome[]={"LPC1769 encendido...UART3 configurada\r\n"};
const char TextForPeriodicTask1[]={"Tarea 1 - Enviando informacion por la UART3\n\r"};
const char TextForTask2[]={"Tarea 2 - Caracter recibido: "};
const char TextForTask3[]={"Tarea 3 - "};
unsigned char datoString[]={"xx:xx:xx"};

unsigned char RxBuffer;
//unsigned char *pcRxBuffer;

static RTC_TIME_Type localTime;


/*-----------------------------------------------------------*/
int main( void )
{
	/* Before semaphores are used they must be explicitly created.
	 * xCountingSemaphore is created to have a maximum count value of 1,
	 * and an initial count value of 0.
	 * xMutex is to protect access to UART3 resource */

	xCountingSemaphore = xSemaphoreCreateCounting( 1, 0 );
	xMutex = xSemaphoreCreateMutex();

	xQueue = xQueueCreate(1,8*sizeof(char));

	/* Check the semaphores and the queue were created successfully. */
	if( xCountingSemaphore != NULL && xMutex != NULL && xQueue != NULL)
	{
		/* Enable and configure the RTC module*/
		vSetupRtc();

		/* Enable and configure External Interruption */
		vSetupIntExt();

		/* Enable and configure UART3 */
		vSetupUart3();

		/* Create the 'handler' task. This is the task that will be synchronized
		with the interrupt. The handler task is created with a high priority to
		ensure it runs immediately after the interrupt exits. In this case a
		priority of 3 is chosen. */
		//xTaskCreate( vHandlerTask, "Handler", 240, NULL, 3, NULL );

		/* Tarea 1 - Genera periodicamente cada 100ms un mensaje constante por la
		 * UART3. Tiene la prioridad mas baja 1 para que sea expropieda por las otras
		 * Tareas cuando salgan del estado bloqueado. */
		xTaskCreate( vPeriodicTask1, "Tarea1", 240, NULL, 1, NULL );

		/* Tarea 2 - Sale del estado bloqueado cuando se recibe un caracter por la UART3.
		 * Envia por la UART un mensaje constante junto con el caracter recibido. */
		xTaskCreate( vAsincronicTask2, "Tarea2", 240, NULL, 2, NULL );

		/* Tarea 3 - Sale del estado bloqueado cuando hay informacion disponible en la cola.
		 * Envia por la UART un mensaje constante con el dato de la cola. */
		xTaskCreate( vAsincronicTask3, "Tarea3", 240, NULL, 3, NULL );

		/* Start the scheduler so the created tasks start executing. */
		vTaskStartScheduler();
	}

	/* If all is well we will never reach here as the scheduler will now be
running the tasks. If we do reach here then it is likely that there was
insufficient heap memory available for a resource to be created. */
	for( ;; );
	return 0;
}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------*/

static void vPeriodicTask1( )
{
	portTickType xLastWakeTime;

	/* The xLastWakeTime variable needs to be initialized with the current tick
count. Note that this is the only time we access this variable. From this
point on xLastWakeTime is managed automatically by the vTaskDelayUntil()
API function. */
	xLastWakeTime = xTaskGetTickCount();

	/* As per most tasks, this task is implemented in an infinite loop. */
	for( ;; )
	{
		xSemaphoreTake(xMutex,portMAX_DELAY);

		/* Envia por la UART3 el mensaje constante. */
		UART3_Enviar(TextForPeriodicTask1,strlen(TextForPeriodicTask1));

		xSemaphoreGive(xMutex);
		/* We want this task to execute exactly every 100 milliseconds. */
		vTaskDelayUntil( &xLastWakeTime, ( 100 / portTICK_RATE_MS ) );
	}

}

/*-----------------------------------------------------------*/
static void vAsincronicTask2()
{
	unsigned char * buffer;

	/* Inicializacion del puntero al buffer de recepcion */
	buffer = &RxBuffer;

	/* As per most tasks, this task is implemented within an infinite loop. */
	for( ;; )
	{
		/* Use the semaphore to wait for the event. The semaphore was created
		 * before the scheduler was started so before this task ran for the first
		 * time. The task blocks indefinitely meaning this function call will only
		 * return once the semaphore has been successfully obtained - so there is no
		 * need to check the returned value. */
		xSemaphoreTake( xCountingSemaphore, portMAX_DELAY );
		xSemaphoreTake(xMutex,portMAX_DELAY);

		/* Envia por la UART3 el mensaje constante. */
		UART3_Enviar(TextForTask2,strlen(TextForTask2));


		UART3_Enviar(buffer,1);
		UART_SendByte(LPC_UART3, '\n'); UART_SendByte(LPC_UART3, '\r');

		xSemaphoreGive(xMutex);
	}
}

/*-----------------------------------------------------------*/

static void vAsincronicTask3(void)
{
	/* Declare the variable that will hold the values received from the queue. */

	portBASE_TYPE xStatus;
	char dato[8];


	/* This task is also defined within an infinite loop. */
	for( ;; )
	{
		/* As this task unblocks immediately that data is written to the queue this
		 * call should always find the queue empty. */
		if( uxQueueMessagesWaiting( xQueue ) != 0 )
		{
			xStatus = xQueueReceive( xQueue, dato, 0 );
			xSemaphoreTake(xMutex,portMAX_DELAY);

			UART3_Enviar("RTC inicializado en ",strlen("RTC inicializado en "));

			UART3_Enviar(dato,8);
			UART_SendByte(LPC_UART3, '\n');
			UART_SendByte(LPC_UART3, '\r');

			xSemaphoreGive(xMutex);
		}

		/* The first parameter is the queue from which data is to be received. The
		queue is created before the scheduler is started, and therefore before this
		task runs for the first time.

		The second parameter is the buffer into which the received data will be
		placed. In this case the buffer is simply the address of a variable that
		has the required size to hold the received data.

		the last parameter is the block time. the maximum amount of time that the
		task should remain in the Blocked state to wait for data to be available should
		the queue already be empty. */
		xStatus = xQueueReceive( xQueue, dato, portMAX_DELAY );

		if( xStatus == pdPASS )
		{
			/* Data was successfully received from the queue, print out the received
			value. */
			xSemaphoreTake(xMutex,portMAX_DELAY);

			UART3_Enviar(TextForTask3,strlen(TextForTask3));

			UART3_Enviar(dato,8);
			UART_SendByte(LPC_UART3, '\n');
			UART_SendByte(LPC_UART3, '\r');

			xSemaphoreGive(xMutex);
		}
	}
}

/*-----------------------------------------------------------*/

void vSetupRtc()
{
	/* RTC init module*/
	RTC_Init(LPC_RTC);

	/* Enable rtc (starts increase the tick counter and second counter register) */
	RTC_ResetClockTickCounter(LPC_RTC);
	RTC_Cmd(LPC_RTC, ENABLE);

	/* Set current time for RTC
	 * Current time is 00:00:00 AM, 2013-01-01
	 */
	RTC_SetTime(LPC_RTC, RTC_TIMETYPE_SECOND, 0);
	RTC_SetTime(LPC_RTC, RTC_TIMETYPE_MINUTE, 0);
	RTC_SetTime(LPC_RTC, RTC_TIMETYPE_HOUR, 0);
	RTC_SetTime(LPC_RTC, RTC_TIMETYPE_MONTH, 1);
	RTC_SetTime(LPC_RTC, RTC_TIMETYPE_YEAR, 2013);
	RTC_SetTime(LPC_RTC, RTC_TIMETYPE_DAYOFMONTH, 1);
	/* Get and print current time */
	RTC_GetFullTime(LPC_RTC, &localTime);

}

/*-----------------------------------------------------------*/

void vSetupIntExt()
{
	// Configure EINT0 to detect
	// set P2.10 as EINT0
	LPC_PINCON->PINSEL4 = 0x00100000;
	// Port 2.10 is rising edge .
	LPC_GPIOINT->IO2IntEnR = 0x400;
	// INT0 edge trigger
	LPC_SC->EXTMODE = 0x00000001;
	// INT0 is rising edge
	LPC_SC->EXTPOLAR = 0x00000001;
	NVIC_EnableIRQ(EINT0_IRQn);
}

/*-----------------------------------------------------------*/
void vSetupUart3()
{
	PINSEL_CFG_Type PinCfg;

	PinCfg.Funcnum = PINSEL_FUNC_2;
	PinCfg.OpenDrain = PINSEL_PINMODE_NORMAL;
	PinCfg.Pinmode = PINSEL_PINMODE_PULLUP;
	PinCfg.Pinnum = PINSEL_PIN_0;
	PinCfg.Portnum = PINSEL_PORT_0;

	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = PINSEL_PIN_1;
	PINSEL_ConfigPin(&PinCfg);


	UART_CFG_Type UARTConfigStruct;
	UARTConfigStruct.Baud_rate = 115200;
	UARTConfigStruct.Databits = UART_DATABIT_8;
	UARTConfigStruct.Parity = UART_PARITY_NONE;
	UARTConfigStruct.Stopbits = UART_STOPBIT_1;
	UART_Init(LPC_UART3, &UARTConfigStruct);
	UART_TxCmd(LPC_UART3, ENABLE);

	UART_IntConfig(LPC_UART3, UART_INTCFG_RBR, ENABLE);
	NVIC_EnableIRQ(UART3_IRQn);

	/* mensaje de startup. No tomo el semáforo xMutex
	 * porque todavía no arrancó el schedule */
	UART3_Enviar(welcome,strlen(welcome));

}

/*-----------------------------------------------------------*/
void UART3_Enviar(unsigned char* buffer, uint32_t size)
{
	UART_Send(LPC_UART3, buffer, size, BLOCKING);

}


/*-----------------------------------------------------------*/

void EINT0_IRQHandler ()
{
	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
	portBASE_TYPE xStatus;


	uint32_t seg_int,min_int,hor_int;
	char decena,unidad;

	/* se limpia la interrupcion */
	LPC_SC->EXTINT = 0x00000001;

	RTC_GetFullTime(LPC_RTC, &localTime);

	seg_int=localTime.SEC;
	min_int=localTime.MIN;
	hor_int=localTime.HOUR;

	decena=(char)floor(hor_int/10)+48;
	unidad=(char)fmod(hor_int,10)+48;
	datoString[0]=decena;
	datoString[1]=unidad;

	decena=(char)floor(min_int/10)+48;
	unidad=(char)fmod(min_int,10)+48;
	datoString[3]=decena;
	datoString[4]=unidad;

	decena=(char)floor(seg_int/10)+48;
	unidad=(char)fmod(seg_int,10)+48;
	datoString[6]=decena;
	datoString[7]=unidad;

	xStatus = xQueueSendToBack(xQueue, datoString,0);

	if (xStatus != pdPASS) {
		/* the send operation could not be completed. xQueue full?
		 * must be an error cause the queue could never contain
		 * more than 1 element */
		for(;;){
		}
	}


	/* NOTE: The syntax for forcing a context switch within an ISR varies between
	 * FreeRTOS ports. The portEND_SWITCHING_ISR() macro is provided as part of
	 * the Cortex-M3 port layer for this purpose. taskYIELD() must never be called
	 * from an ISR! */
	portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
}

/*-----------------------------------------------------------*/

void UART3_IRQHandler(void)
{
	int i;

	// Ugly Delay
	for (i=0;i<100000;i++);

	RxBuffer = UART_ReceiveByte(LPC_UART3);
	unsigned char dummy= UART_ReceiveByte(LPC_UART3);

	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

	/* 'Give' the semaphore to unlock tarea2 */
	xSemaphoreGiveFromISR( xCountingSemaphore, &xHigherPriorityTaskWoken );

	/* Giving the semaphore may have unblocked a task - if it did and the
	unblocked task has a priority equal to or above the currently executing
	task then xHigherPriorityTaskWoken will have been set to pdTRUE and
	portEND_SWITCHING_ISR() will force a context switch to the newly unblocked
	higher priority task.

	Note, it's important to ensure peripherals clear the
	reason for the interrupt within the ISR. For UART3
	this is done simple by reading the IIR register.*/

	/* NOTE: The syntax for forcing a context switch within an ISR varies between
	FreeRTOS ports. The portEND_SWITCHING_ISR() macro is provided as part of
	the Cortex-M3 port layer for this purpose. taskYIELD() must never be called
	from an ISR! */
	portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
}


/*-----------------------------------------------------------*/

void vApplicationMallocFailedHook( void )
{
	/* This function will only be called if an API call to create a task, queue
	or semaphore fails because there is too little heap RAM remaining. */
	for( ;; );
}

/*-----------------------------------------------------------*/
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook( xTaskHandle *pxTask, signed char *pcTaskName )
{
	/* This function will only be called if a task overflows its stack. Note
	that stack overflow checking does slow down the context switch
	implementation. */
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook( void )
{
	/* This example does not use the idle hook to perform any processing. */
}
/*-----------------------------------------------------------*/

void vApplicationTickHook( void )
{
	/* This example does not use the tick hook to perform any processing. */
}

