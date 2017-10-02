![](http://upload-images.jianshu.io/upload_images/3810775-972905b9ca235586.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

----------

## 1. 本次作业的考察要点：
作业地址：
[https://github.com/HustWolfzzb/STM32F429_Discovery_FreeRTOS_9](https://github.com/HustWolfzzb/STM32F429_Discovery_FreeRTOS_9)
Git/GCC/GDB/QEMU等工具的使用。
FreeRTOS多任务同步和通信机制的掌握。

----------------

## 2. 编程作业：

在github上，Fork例程项目（https://github.com/cbhust/STM32F429_Discovery_FreeRTOS_9.git） 到自己的个人账号。
clone项目到本地电脑的Ubuntu虚拟机中（虚拟机环境在第一次作业中已搭建）。
按照/Projects/Demo1/README.md中的提示编译Demo1例程并通过Qemu运行例程。
在Demo1的框架基础上完成自己的本次编程作业（具体要求见第3点）。
代码完成后提交到自己的github账号上，确保助教可以正常的clone并编译运行。
在作业博客上给出代码的github链接、代码说明以及运行结果展示。

-------------

## 3. 编程作业具体要求：

**创建三个任务：Sender_Task,Receiver_Task, Monitor_Task**

* Sender_Task的任务执行周期为2ms，Receiver_Task的任务执行周期为1000ms, Monitor_Task的任务执行周期为10000ms。
* Sender_Task在每个执行周期向Receiver_Task发送一个32位无符号整数，第一次发送1，然后依次发送2，3，4......，发送完10000后再从1开始发送。同时对发送的数据进行计算累加计算并保存当前累加结果。
* Receiver_Task对接收到的数据进行和Sender_Task同样的累加计算并保存当前累加结果。
* Monitor_Task在每个执行周期检查Sender_Task发送的每个数据是否都被Receiver_Task正确的接收和处理，请自行设计一种检查机制并实现。
* 可利用STM32F429I Discovery开发板的相关硬件（LED/LCD/串口）来输出相关状态信息。
* 使用FreeRTOS的任务间通信和同步API完成上述功能。


## 4. 任务说明（会用到的API）

*  **任务创建**
头文件：task.h    
portBASE_TYPE xTaskCreate (
pdTASK_CODE pvTaskCode, 指向任务的实现函数的指针。效果上仅仅是函数名
const portCHAR * const pcNane, 具有描述性的任务名。FreeRTOS 不会使用它。
unsigned portSHORT usStackDepth, 指定任务堆栈的大小
void *pvParameters, 指针用于作为一个参数传向创建的任务
unsigned portBASE_TYPE uxPriority, 任务运行时的优先级
xTaskHandle *pvCreatedTask 用于传递任务的句柄，可以引用从而对任务进行其他操作。
)

说明：
1. 这里的任务是指一个永远不会退出的C 函数，通常是一个死循环。
2. pcNane 其只是单纯地用于辅助调试。应用程序可以通过定义常量
config_MAX_TASK_NAME_LEN 来定义任务名的最大长度——包括’\0’结束符。如果传入的
字符串长度超过了这个最大值，字符串将会自动被截断
3. usStackDepth 这个值指定的是栈空间可以保存多少个字(word)，而不是多少个字节(byte)。栈空间
大小为usStackDepth*4(bytes)。
4. uxPriority 优先级的取值范围可以从最低优先级0 到最高优先级(configMAX_PRIORITIES–1)。
返回:
1. pdPASS 表明任务创建成功，准备运行。
2. errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY 由于内存堆空间不足，FreeRTOS 无法分配
足够的空间来保存任务结构数据和任务栈，因此无法创建任务。


*  **任务延时**
头文件：task.h
void vTaskDelay (
portTickType xTicksToDelay 时间数量，调用任务应该锁住的时间片周期
)

说明：
1. FreeRTOSConfig.h 中的INCLUDE_vTaskDelay=1，这个函数才能用。
2. 延时任务为已知时间片，任务被锁住剩余的实际时间由时间片速率决定。portTICK_RATE_MS 常量
以时间片速率来计算实际时间
3. vTaskDelay()指定一个任务希望的时间段，这个时间之后任务解锁。
4. vTaskDelay()不提供一个控制周期性任务频率的好方法，和其他任务和中断一样，在调用vTaskDelay()

*   **创建一个新的队列**
头文件：queue. H
xQueueHandle xQueueCreate (
unsigned portBASE_TYPE uxQueueLength, 队列中包含最大项目数量
unsigned portBASE_TYPE uxItemSize 队列中每个项目所需的字节数
);

说明：创建一个新的队列。为新的队列分配所需的存储内存，并返回一个队列处理。
注意：项目通过复制而不是引用排队，因此，所需的字节数，将复制给每个项目。队列中每个项目必须分配同样大小。
返回：如果队列成功创建，则返回一个新建队列的处理。如果不能创建队列，将返回0。

*  **传递项目到一个队列中的后面**
头文件：queue. H
portBASE_TYPE xQueueSendToBack (
xQueueHandle xQueue, 将项目传进的队列
const void * pvItemToQueue, 项目的指针【源数据】
portTickType xTicksToWait 等待的最大时间量
);

说明：这个与xQueueSend 是一样的，参照xQueueSend 的用法

*   **从队列接收一个项目**
头文件：queue. H
portBASE_TYPE xQueueReceive (
xQueueHandle xQueue, 发送项目的队列句柄
void *pvBuffer, 指向缓冲区的指针，将接收的项目被复制进去
portTickType xTicksToWait 任务中断并等待队列中可用空间的最大时间
);

说明：这个项目通过复制接收，因此缓冲器必须提供足够大的空间。这个函数一定不能在中断服务程序中使用当队列空时，肯定复制传递不成功，则等待xTicksToWait 个滴答周期后再复制，但如果xTicksToWait 设置为0，调用将立即返回。
返回：如果项目成功被队列接收为pdTRUE ，否则为 pdFALSE。


----------------------------

~~ ~~                           ~~~~~ ~~~~~~~
    #                     作业过程（多图预警）
~ ~~~ ~~ ~~                         ~~ ~~~~ ~

---------------------
## 1. QEMU安装
  本例程使用qemu-system-gnuarmeclipse，该qemu分支对stm32f4有更好的支持，主要面向Eclipse开发环境，本文档给出在Ubuntu 16.04命令行环境下单独使用的方式。
- 到网页 https://github.com/gnu-mcu-eclipse/qemu/releases/tag/gae-2.8.0-20161227 下载二进制文件 gnuarmeclipse-qemu-debian64-2.8.0-201612271623-dev.tgz到~/work目录

![](http://upload-images.jianshu.io/upload_images/3810775-08b393557ceaecad.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)



- 在工作目录解压缩文件，并把路径添加到$PATH变量中
```
#cd ~/work
#tar xvf gnuarmeclipse-qemu-debian64-2.8.0-201612271623-dev.tgz
#chmod -R -w ./qemu
export PATH=~/work/qemu/2.8.0-201612271623-dev/bin/:$PATH
```
![](http://upload-images.jianshu.io/upload_images/3810775-566177d621672d93.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)
- 测试qemu能否正常运行

```
#qemu-system-gnuarmeclipse --version
```
如正常则会显示版本信息为2.8.0。

![](http://upload-images.jianshu.io/upload_images/3810775-b8177d9e41a2fbbd.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)


## 2. 编译本例程

- 在Demo1目录下运行make，生成hello_rtos.elf文件

![](http://upload-images.jianshu.io/upload_images/3810775-f31be98f69dfd5ba.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

![](http://upload-images.jianshu.io/upload_images/3810775-bae44a5a55afc3b4.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)
![](http://upload-images.jianshu.io/upload_images/3810775-8b036eea0a399f64.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

> 说明：qemu-system-gnuarmeclipse当前版本不支持STM32F4的浮点，相应的，FreeRTOS使用的portable目录没有使用ARM_CM4F而是使用ARM_CM3。

## 3. QEMU仿真

在Demo1目录下提供了一个qemu.sh脚本文件，内容如下：
```
#!/bin/bash

qemu-system-gnuarmeclipse --verbose --verbose --board STM32F429I-Discovery --mcu STM32F429ZI -d unimp,guest_errors  --image hello_rtos.elf --semihosting-config enable
```
在Demo1目录下运行脚本文件：
```
#./qemu.sh
```
则qemu开始执行hello_rtos.elf文件，在弹出的GUI中看到程序执行的效果。


![](http://upload-images.jianshu.io/upload_images/3810775-ff23e38c029d318e.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

![](http://upload-images.jianshu.io/upload_images/3810775-327e0080b9ad6d01.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

## 4. GDB调试
 
 Ubuntu缺省安装中没有gdb-arm-none-eabi工具，需要先安装
 ```
 #sudo apt-get install gdb-arm-none-eabi
```
![](http://upload-images.jianshu.io/upload_images/3810775-d2da64368a324899.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

![](http://upload-images.jianshu.io/upload_images/3810775-5391731e530ec04e.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

在Demo1目录下运行qemu_gdb脚本文件，该文件中添加了--gdb tcp::1234 -S

![](http://upload-images.jianshu.io/upload_images/3810775-d76dcb60db43fae1.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

![](http://upload-images.jianshu.io/upload_images/3810775-df5cbabda7271e89.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

![](http://upload-images.jianshu.io/upload_images/3810775-7bfe985ecdab8867.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

qemu启动后等待来自gdb的调试指令，打开另外一个终端窗口，运行
```
#arm-none-eabi-gdb
```



![](http://upload-images.jianshu.io/upload_images/3810775-5da483bc2951b70e.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

在gdb界面内运行：
```
(gdb)target remote localhost:1234
(gdb)continue
```
可以看到qemu开始执行程序，GUI界面上可以看到程序执行效果。

![](http://upload-images.jianshu.io/upload_images/3810775-e41e4be5856ea9d5.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

![](http://upload-images.jianshu.io/upload_images/3810775-f75ad61e2fe91698.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)
![](http://upload-images.jianshu.io/upload_images/3810775-45892b7c2c1e288a.gif?imageMogr2/auto-orient/strip)

 ## 5. Trace（略）
 
本例程启动了FreeRTOS的trace功能。
- 用STM32 ST-LINK Utility把hello_rtos.hex烧写入STM32F429I Discovery开发板，程序运行时，用STM32 ST-LINK Utility读取0x20000000到0x20030000范围内的地址数据，并保存到一个二进制文件trace_data.bin中。
- 到https://percepio.com/tz/freertostrace/ 下载并安装Tracealyzer for FreeRTOS软件。
- 在Tracealyzer for FreeRTOS软件中打开trace_data.bin，可以看到详细的FreeRTOS的任务和事件信息。
 
##6.自主编程实现三个同步通信的函数：Sender_Task,Receiver_Task,Monitor_Task;
我采用了Mac编程，然后通过git的方式同步到ubuntu，不仅锻炼了团队写作的神器--“Git”的使用技巧，同时也优化了自己的编程环境

**说一下我的思路：
首先，采用全局变量来统领发送的数据和接收到的数据，通过在main函数外声明了一个发送数据的和，一个接收数据的和，一个队列用于传递由于周期不对等的发送和接受的数据。然后Sender_Task用于2ms发送一个数据，从1-10000循环，Receiver_Task 用于1000ms接收一波数据，大概是500个左右，所以队列的大小我定为510 ，虽然有不小的浪费，但是至少不会数据溢出。最后的Monitor_Task用于判定是否争取的发送和接受，所以这个时候就要用到两个全局变量，在10000ms的周期内判定一次是否两个数据和相等，如果发送的数据之和等于接受的数据之和，那么就毫无疑问的，发送没有问题。而且由于我规定，优先级上，Monitor>Receiver>Sender ，所以可以保证不会发生数据竞争导致的错漏。如果正确，那就亮绿灯，错误就是红灯，事实显示，一直是正确的！！** 

```
/**
  ******************************************************************************
  * @file    IO_Toggle/main.c 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    19-September-2011
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>© COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************  
  */ 

/* Includes ------------------------------------------------------------------*/
#include "stm32f429i_discovery.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "queue.h"

/** @addtogroup STM32F4_Discovery_Peripheral_Examples
  * @{
  */

/** @addtogroup IO_Toggle
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
GPIO_InitTypeDef  GPIO_InitStructure;

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
void Hardware_Init(void);
void Red_LED_On(void);
void Red_LED_Off(void);
void Green_LED_On(void);
void Green_LED_Off(void);
void ToggleLED1_Task(void*);
void ToggleLED2_Task(void*);
/**
  * @brief  Main program
  * @param  None
  * @retval None
  */


//张照博自己写的--START
int32_t Send_Sum=0;
int32_t Received_Sum=0;
   //建立队列 
xQueueHandle MyQueue; 
void Sender_Task(void *pvParameters)
{
    int32_t Send_Num = 1; 
    for( ;; )  
    {  
        vTaskDelay( 2 / portTICK_RATE_MS );
        if (Send_Num>10000)
        {
          Send_Num=1;
        }
        /* 向队列中填充内容 */  
        xQueueSendToBack( MyQueue, ( void* )&Send_Num, 0 );   
        *((int32_t*)pvParameters)+=Send_Sum;
        Send_Num++; 
     }
}

void Receiver_Task(void *pvParameters)
{
  int32_t  Received_Num = 0;  
    for( ;; )  
    {  
        /* 从队列中获取内容 */  
        if( xQueueReceive( MyQueue, &Received_Num, 1000 / portTICK_RATE_MS ) == pdTRUE)  
        {  
          
          *((int32_t*)pvParameters)+=Received_Num;
           Received_Num=*((int32_t*)pvParameters);
        }  
    }  
}

void Monitor_Task(void *pvParameters)
{
  vTaskDelay( 10000 / portTICK_RATE_MS );  
  if(Send_Sum-Received_Sum<5 && Send_Sum-Received_Sum>-5)
    {
      Green_LED_On();
      Red_LED_Off();
      Send_Sum=0;
      Received_Sum=0;
    } 
  else 
    {
      Green_LED_Off();
      Red_LED_On();
      Send_Sum=0;
      Received_Sum=0;
    }
}

//张照博自己写的--END

int main(void)
{
  /*!< At this stage the microcontroller clock setting is already configured, 
       this is done through SystemInit() function which is called from startup
       file (startup_stm32f4xx.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
        system_stm32f4xx.c file
     */
    Hardware_Init();
    // 初始化硬件平台  
    //prvSetupHardware();  
    //创建全局变量
 MyQueue = xQueueCreate( 510 , sizeof( int32_t ) ); 
    // 建立任务  
    xTaskCreate( Sender_Task, ( signed portCHAR * ) "Sender_Task", configMINIMAL_STACK_SIZE,(void*)&Send_Sum, tskIDLE_PRIORITY+3, NULL );  
    xTaskCreate( Receiver_Task, ( signed portCHAR * ) "Receiver_Task", configMINIMAL_STACK_SIZE,(void*)&Received_Sum, tskIDLE_PRIORITY+4, NULL );   
    xTaskCreate( Monitor_Task, ( signed portCHAR * ) "Monitor_Task", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+5, NULL ); 
  //启动OS  
    vTaskStartScheduler();  
    return 0; 
 //我的第一次调试，需要去掉后面的
       /* Init and start tracing*/
         vTraceEnable(TRC_INIT);
         vTraceEnable(TRC_START);

 //       /* Create tasks */
 //       xTaskCreate(
	// 	  ToggleLED1_Task,                 /* Function pointer */
	// 	  "Task_LED1",                          /* Task name - for debugging only*/
	// 	  configMINIMAL_STACK_SIZE,         /* Stack depth in words */
	// 	  (void*) NULL,                     /* Pointer to tasks arguments (parameter) */
	// 	  tskIDLE_PRIORITY + 3UL,           /* Task priority*/
	// 	  NULL                              /* Task handle */
 //       );

 //       xTaskCreate(
	// 	  ToggleLED2_Task,                 /* Function pointer */
	// 	  "Task_LED2",                           Task name - for debugging only
	// 	  configMINIMAL_STACK_SIZE,         /* Stack depth in words */
	// 	  (void*) NULL,                     /* Pointer to tasks arguments (parameter) */
	// 	  tskIDLE_PRIORITY + 2UL,           /* Task priority*/
	// 	  NULL                              /* Task handle */
 //       );

	// /* Start the scheduler. */
	// vTaskStartScheduler();

	// /* If all is well, the scheduler will now be running, and the following line
	// will never be reached.  If the following line does execute, then there was
	// insufficient FreeRTOS heap memory available for the idle and/or timer tasks
	// to be created.  See the memory management section on the FreeRTOS web site
	// for more details. */
	// for( ;; );
//我的第一次调试，去掉的位置结尾


}


/**
 * Hardware_Init: 
 */
void Hardware_Init(void)
{
        /* GPIOG Periph clock enable */
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);

        /* Configure PG13, PG14 in output pushpull mode */
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13| GPIO_Pin_14;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
        GPIO_Init(GPIOG, &GPIO_InitStructure);

}
/**
 * Red_LED_On: 
 */
void Red_LED_On(void)
{
//    GPIO_SetBits(GPIOG, GPIO_Pin_14);
    GPIOG->ODR |= 0x4000;
}

/**
 * Red_LED_Off: 
 */
void Red_LED_Off(void)
{
//    GPIO_ResetBits(GPIOG, GPIO_Pin_14);
  GPIOG->ODR &= 0xBFFF;
}

/**
 * Green_LED_On: 
 */
void Green_LED_On(void)
{
//    GPIO_SetBits(GPIOG, GPIO_Pin_13);
    GPIOG->ODR |= 0x2000;
}

/**
 * Green_LED_Off: 
 */
void Green_LED_Off(void)
{
//    GPIO_ResetBits(GPIOG, GPIO_Pin_13);
    GPIOG->ODR &= 0xDFFF;
}
/**
 * ToggleLED1_Task: Toggle LED1 via RTOS Timer
 */
void ToggleLED1_Task(void *pvParameters)
{
    int led = 0;  

    while (1) 
    {
        if(led == 0)
        {
            Red_LED_On();
            led = 1;
        } 
        else
        {
            Red_LED_Off();
            led = 0;
         }
        /*
        Delay for a period of time. vTaskDelay() places the task into
        the Blocked state until the period has expired.
        The delay period is spacified in 'ticks'. We can convert
        yhis in milisecond with the constant portTICK_RATE_MS.
        */
        vTaskDelay(1000 / portTICK_RATE_MS);
  }
}

/**
 * ToggleLED2_Task: Toggle LED2 via RTOS Timer
 */
void ToggleLED2_Task(void *pvParameters)

{
    int led = 0;  
    while (1) 
    {
        if(led == 0)
        {
            Green_LED_On();
            led = 1;
        } 
        else
        {
            Green_LED_Off();
            led = 0;
         }
        /*
        Delay for a period of time. vTaskDelay() places the task into
        the Blocked state until the period has expired.
        The delay period is spacified in 'ticks'. We can convert
        yhis in milisecond with the constant portTICK_RATE_MS.
        */
        vTaskDelay(2000 / portTICK_RATE_MS);
  }
}

void vApplicationTickHook( void )
{
}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------*/

void vApplicationMallocFailedHook( void )
{
	/* vApplicationMallocFailedHook() will only be called if
	configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h.  It is a hook
	function that will get called if a call to pvPortMalloc() fails.
	pvPortMalloc() is called internally by the kernel whenever a task, queue,
	timer or semaphore is created.  It is also called by various parts of the
	demo application.  If heap_1.c or heap_2.c are used, then the size of the
	heap available to pvPortMalloc() is defined by configTOTAL_HEAP_SIZE in
	FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
	to query the size of free heap space that remains (although it does not
	provide information on how the remaining heap might be fragmented). */
	taskDISABLE_INTERRUPTS();
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook( void )
{
	/* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
	to 1 in FreeRTOSConfig.h.  It will be called on each iteration of the idle
	task.  It is essential that code added to this hook function never attempts
	to block in any way (for example, call xQueueReceive() with a block time
	specified, or call vTaskDelay()).  If the application makes use of the
	vTaskDelete() API function (as this demo application does) then it is also
	important that vApplicationIdleHook() is permitted to return to its calling
	function, because it is the responsibility of the idle task to clean up
	memory allocated by the kernel to any task that has since been deleted. */
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
	( void ) pcTaskName;
	( void ) pxTask;

	/* Run time stack overflow checking is performed if
	configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	function is called if a stack overflow is detected. */
	taskDISABLE_INTERRUPTS();
	for( ;; );
}
/*-----------------------------------------------------------*/
#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */ 

/**
  * @}
  */ 

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
```
![](http://upload-images.jianshu.io/upload_images/3810775-3baef28dc2d0af68.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

![](http://upload-images.jianshu.io/upload_images/3810775-42443349fa8510d1.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)


![](http://upload-images.jianshu.io/upload_images/3810775-61b620eb6db71cb3.gif?imageMogr2/auto-orient/strip)
