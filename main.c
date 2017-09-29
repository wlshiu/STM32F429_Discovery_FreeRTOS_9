#include <queue.h>
#include <task.h>



void Sender_Task(void *pvParameters)
{
	int32_t Send_Num = 1;  
    for( ;; )  
    {  
        vTaskDelay( 2/portTICK_RATE_MS );  
        /* 向队列中填充内容 */  
        xQueueSend( MyQueue, ( void* )&Send_Num, 0 );  
        Send_Num++;  
}

void Receiver_Task(void *pvParameters)
{
	int32_t  Received_Num = 0;  
    for( ;; )  
    {  
        /* 从队列中获取内容 */  
        if( xQueueReceive( MyQueue, &Received_Num, 1000/portTICK_RATE_MS ) == pdPASS)  
        {  
            printf("Received_Num:%d\r\n",Received_Num);  
        }  
    }  
}

void Monitor_Task(void *pvParameters)
{

}

int main()
{
    /* 初始化硬件平台 */  
    prvSetupHardware();  
     
    /* 建立队列 */  
    MyQueue = xQueueCreate( 510 , sizeof( int32_t ) );  
    /* 建立任务 */  
    xTaskCreate( Sender_Task, ( signed portCHAR * ) "Sender_Task", configMINIMAL_STACK_SIZE,  
                            NULL, tskIDLE_PRIORITY+3, NULL );  
    xTaskCreate( Receiver_Task, ( signed portCHAR * ) "Receiver_Task", configMINIMAL_STACK_SIZE,  
                            NULL, tskIDLE_PRIORITY+4, NULL );  
    /* 启动OS */  

    vTaskStartScheduler();  
     
    return 0; 
}
