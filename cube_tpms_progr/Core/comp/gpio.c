#include "gpio.h"
#include "stm32f4xx_hal.h"

static BOOLEAN is_init = FALSE;
static SDA_SCL_MODE current_mode = NO_MODE;

// Зеленый индикатор
static GPIO_InitTypeDef GPIO_InitStruct_green_led;
#define LED_GREEN GPIO_PIN_5
#define LED_GREEN_PORT GPIOA

// Голубая кнопка
static GPIO_InitTypeDef GPIO_InitStruct_blue_button;
#define BLUE_BUTTON GPIO_PIN_13
#define BLUE_BUTTON_PORT GPIOC

// I2C1_SCL
static GPIO_InitTypeDef GPIO_InitStruct_i2c_sda;
#define SDA GPIO_PIN_8
#define SDA_PORT GPIOB

// I2C1 SCL
static GPIO_InitTypeDef GPIO_InitStruct_i2c_scl;
#define SCL GPIO_PIN_9
#define SCL_PORT GPIOB

// 3.3V OUT (PA12 GPIO12)
static GPIO_InitTypeDef GPIO_InitStruct_power;
#define POWER GPIO_PIN_12
#define POWER_PORT GPIOA


static BOOLEAN init_io(void);
static void gpio_watcher(void);


extern BOOLEAN gpio_start(void){
   if(is_init) return TRUE;
   is_init = init_io();
   gpio_watcher();
   return TRUE;
}


extern BOOLEAN switch_mode_sda_scl(SDA_SCL_MODE mode){
	switch(mode){
	case GPIO_M:
		printf("SDA/SCL has GPIO_MODE\n");
		GPIO_InitStruct_i2c_scl.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct_i2c_sda.Mode = GPIO_MODE_OUTPUT_PP;
		current_mode = GPIO_M;
		break;
	case I2C_M:
		printf("SDA/SCL has I2C_MODE\n");
	    GPIO_InitStruct_i2c_sda.Alternate = GPIO_AF4_I2C1;
		GPIO_InitStruct_i2c_scl.Alternate = GPIO_AF4_I2C1;
		GPIO_InitStruct_i2c_scl.Mode = GPIO_MODE_AF_OD;
		GPIO_InitStruct_i2c_sda.Mode = GPIO_MODE_AF_OD;
		current_mode = I2C_M;
        break;
	default:
		printf("Unknown state for SDA/SCL pins\n");
		return FALSE;
	}

	__HAL_RCC_GPIOB_CLK_ENABLE();

	GPIO_InitStruct_i2c_scl.Pin = SCL;
    GPIO_InitStruct_i2c_sda.Pin = SDA;

	GPIO_InitStruct_i2c_scl.Pull = GPIO_PULLUP;
	GPIO_InitStruct_i2c_sda.Pull = GPIO_PULLUP;

	GPIO_InitStruct_i2c_scl.Speed = GPIO_SPEED_HIGH;
	GPIO_InitStruct_i2c_sda.Speed = GPIO_SPEED_HIGH;

	HAL_GPIO_Init(SDA_PORT, &GPIO_InitStruct_i2c_sda);
	HAL_GPIO_Init(SCL_PORT, &GPIO_InitStruct_i2c_scl);
	return TRUE;
}

static void gpio_watcher(void){
	while(is_init){

	}
}

extern void gpio_stop(void){
	is_init = FALSE;
}

static BOOLEAN init_io(void){
	 // Инит кнопки
	 __HAL_RCC_GPIOC_CLK_ENABLE();
	 GPIO_InitStruct_green_led.Pin = BLUE_BUTTON;
	 GPIO_InitStruct_green_led.Mode = GPIO_MODE_INPUT; // вход
	 GPIO_InitStruct_green_led.Pull = GPIO_PULLDOWN;
	 GPIO_InitStruct_green_led.Speed = GPIO_SPEED_HIGH;
	 HAL_GPIO_Init(BLUE_BUTTON_PORT, &GPIO_InitStruct_green_led);

	 // Инит индикатора
	 __HAL_RCC_GPIOA_CLK_ENABLE();
	 GPIO_InitStruct_blue_button.Pin = LED_GREEN;
	 GPIO_InitStruct_blue_button.Mode = GPIO_MODE_OUTPUT_PP; // выход
	 GPIO_InitStruct_blue_button.Pull = GPIO_NOPULL;
	 GPIO_InitStruct_blue_button.Speed = GPIO_SPEED_HIGH;
	 HAL_GPIO_Init(LED_GREEN_PORT, &GPIO_InitStruct_blue_button);


	 // Инит питания
	 GPIO_InitStruct_power.Pin = POWER;
	 GPIO_InitStruct_power.Mode = GPIO_MODE_OUTPUT_PP; // выход
	 GPIO_InitStruct_power.Pull = GPIO_PULLDOWN;
	 GPIO_InitStruct_power.Speed = GPIO_SPEED_HIGH;
	 HAL_GPIO_Init(POWER_PORT, &GPIO_InitStruct_power);

	 return TRUE;
}

extern void set_power(GPIO_PinState state){
     HAL_GPIO_WritePin(POWER_PORT, POWER, state);
}

extern void set_sda(GPIO_PinState state){
     if(current_mode != GPIO_M) return;
	 HAL_GPIO_WritePin(SDA_PORT, SDA, state);
}

extern void set_scl(GPIO_PinState state){
	 if(current_mode != GPIO_M) return;
	 HAL_GPIO_WritePin(SCL_PORT, SCL, state);
}

extern GPIO_PinState get_current_green_led(){
	 return HAL_GPIO_ReadPin(LED_GREEN_PORT, LED_GREEN);
}

extern void set_green_led(GPIO_PinState state){
	 HAL_GPIO_WritePin(LED_GREEN_PORT, LED_GREEN, state);
}

extern GPIO_PinState get_current_button_state(void){
	return HAL_GPIO_ReadPin(BLUE_BUTTON_PORT, BLUE_BUTTON);
}
