#include "cmsis_os.h"
#include "gpio.h"
#include "i2c_manager.h"
#include "stm32f4xx_hal.h"
#include "fw_updater.h"
#include "indication.h"
#include "tasks_cmp.h"

// GPIO task
static osThreadId_t gpio_taskHandle;
static const osThreadAttr_t gpio_task_attributes = {.name = "gpio_task", .stack_size = 2048, .priority = (osPriority_t) osPriorityBelowNormal};

// I2C task
static osThreadId_t i2c_taskHandle;
static const osThreadAttr_t i2c_task_attributes = {.name = "i2c_task", .stack_size = 8192, .priority = (osPriority_t) osPriorityBelowNormal};

// Indication task
static osThreadId_t ind_taskHandle;
static const osThreadAttr_t ind_task_attributes = {.name = "ind_task", .stack_size = 2048, .priority = (osPriority_t) osPriorityBelowNormal};


static void init_gpio_task(void *argument);
static void init_i2c_task(void *argument);
static void init_ind_task(void *argument);


extern BOOLEAN init_tasks(void){
	  osKernelInitialize();

	  UINT8 fail_cnt = 0;
	  gpio_taskHandle = osThreadNew(init_gpio_task, NULL, &gpio_task_attributes);
	  if(gpio_taskHandle == NULL){
		 printf("Error init gpio task\n");
		 fail_cnt++;
	  }

	  i2c_taskHandle = osThreadNew(init_i2c_task, NULL, &i2c_task_attributes);
	  if(i2c_taskHandle == NULL){
		 printf("Error init i2c task\n");
		 fail_cnt++;
	  }

	  ind_taskHandle = osThreadNew(init_ind_task, NULL, &ind_task_attributes);
	  if(ind_taskHandle == NULL){
	 	 printf("Error init ind task\n");
	 	 fail_cnt++;
	  }

	  return (fail_cnt == 0) ? TRUE : FALSE;
}


static void init_gpio_task(void *argument){
	gpio_start();
}

static void init_ind_task(void *argument){
	start_indication(NULL);
}



static BOOLEAN is_start = FALSE;

static void init_i2c_task(void *argument){
	set_power(GPIO_PIN_SET);
	osDelay(1000);
	while(1){
		osDelay(100);
		if(get_current_button_state() == GPIO_PIN_RESET){
			if(!is_start){
				is_start = TRUE;
				set_indication_task(1);
				if(update_firmware() == TRUE){
					// Успешно
					set_indication_task(2);
				} else {
					// Провально
					set_indication_task(3);
				}
				is_start = FALSE;
			}
		}
	}
}





