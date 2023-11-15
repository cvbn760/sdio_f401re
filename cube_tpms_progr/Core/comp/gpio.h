#ifndef _TASKS_H_
#define _TASKS_H_

#include "app_cfg.h"

typedef enum {
	GPIO_M,
	I2C_M,
	NO_MODE
} SDA_SCL_MODE;

extern GPIO_PinState get_current_green_led();
extern void set_green_led(GPIO_PinState state);
extern GPIO_PinState get_current_button_state();
extern GPIO_PinState update_button_state(void);

extern BOOLEAN gpio_start(void);
extern void gpio_stop(void);

extern BOOLEAN switch_mode_sda_scl(SDA_SCL_MODE mode);
extern void set_sda(GPIO_PinState state);
extern void set_scl(GPIO_PinState state);
extern void set_power(GPIO_PinState state);

#endif /* _TASKS_H_ */
