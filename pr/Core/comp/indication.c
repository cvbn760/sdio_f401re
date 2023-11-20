#include "indication.h"
#include "gpio.h"

static IND_TASK_COMMAND current_task = 0;
static UINT8 state = 0;
static void reset_task(void);
static void firmware_update(void);
static void firmware_success(void);
static void firmware_failure(void);


extern void start_indication(void *argument){
	while(1){
        switch(current_task){
             case 1:
            	 firmware_update();
            	 break;
             case 2:
            	 firmware_success();
            	 break;
             case 3:
            	 firmware_failure();
            	 break;
             default:
            	 break;
        }
	}
}

static void firmware_success(void){
	set_green_led(GPIO_PIN_SET);
	osDelay(2000);
	reset_task();
	printf("end\n");
}

static void firmware_update(void){
	if(state == 0){
		state = 1;
	    set_green_led(GPIO_PIN_RESET);
	} else {
	    state = 0;
	    set_green_led(GPIO_PIN_SET);
	}
	osDelay(50);
}

static void firmware_failure(void){
	if(state == 0){
		osDelay(1000);
		state = 1;
		set_green_led(GPIO_PIN_RESET);
	} else {
		osDelay(100);
		state = 0;
		set_green_led(GPIO_PIN_SET);
	}
}

extern void set_indication_task(UINT8 cmd){
	osDelay(100);
	reset_task();
	current_task = cmd;
}



static void reset_task(void){
	current_task = 0;
	state = 0;
	set_green_led(GPIO_PIN_RESET);
}
