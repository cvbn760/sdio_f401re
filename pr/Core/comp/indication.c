#include "indication.h"
#include "gpio.h"


static IND_TASK_COMMAND current_task = 0;
static UINT8 state = 0;
static void reset_task(void);
static void firmware_update(void);


extern void start_indication(void *argument){
	while(1){
        switch(current_task){
             case 1:
            	 firmware_update();
            	 break;
             default:
            	 break;
        }
	}
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

extern void set_indication_task(UINT8 cmd){
	reset_task();
	current_task = cmd;
}

static void reset_task(void){
	state = 0;
	set_green_led(GPIO_PIN_RESET);
}
