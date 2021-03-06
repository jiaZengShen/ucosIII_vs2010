/*
*********************************************************************************************************
*                                                uC/OS-II
*                                          The Real-Time Kernel
*                                          Board Support Package
*
*                                 (c) Copyright 2010; Micrium; Weston, FL
*                                           All Rights Reserved
*
* Filename      : bsp.h
* Programmer(s) : FGK
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

void  BSP_Init(void);


/*
*********************************************************************************************************
*                                            LED SERVICES
*********************************************************************************************************
*/

void  LED_On    (CPU_INT08U  led);
void  LED_Off   (CPU_INT08U  led);
void  LED_Toggle(CPU_INT08U  led);

