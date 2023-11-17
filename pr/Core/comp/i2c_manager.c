#include "i2c_manager.h"

static I2C_HandleTypeDef hi2c1;

extern BOOLEAN i2c_init(void){
	  hi2c1.Instance = I2C1;
	  hi2c1.Init.ClockSpeed = 100000;
	  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
	  hi2c1.Init.OwnAddress1 = 0x0;
	  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	  hi2c1.Init.OwnAddress2 = 0;
	  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;

	  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
	  {
	    return FALSE;
	  }
	  switch_mode_sda_scl(I2C_M);
      return TRUE;
}

void print_hex(char *caption, unsigned char *m, int length) {
    int size = sizeof(caption) + 30 + length * 3 + 1;
    char buf[size];
    unsigned int pos = snprintf(buf, size, "%s [%i]: ", caption, length);
    for (int i = 0; i < length; i++) pos += snprintf(buf + pos, 4, "%2.2X ", m[i]);
    printf("%s\n", buf);
}

extern BOOLEAN i2c_read_data_from_device(uint16_t device_addr, uint16_t reg_addr, uint8_t *buffer, uint16_t buffer_size){
	 uint8_t reg[2] = {reg_addr >> 8, reg_addr};

	 // Подготовка к чтению
	 if(HAL_I2C_Master_Transmit(&hi2c1, (uint16_t) device_addr << 1, reg, 2, HAL_MAX_DELAY) != HAL_OK){
	    	printf("Error prepare to read register %02X %02X\n", reg[0], reg[1]);
	    	return FALSE;
	 }
     print_hex("ack data: ", reg, 2);


	 // Чтение
  	 uint8_t res = HAL_I2C_Master_Receive(&hi2c1, (uint16_t) device_addr << 1, &buffer[0], buffer_size, HAL_MAX_DELAY);
	 if(res != HAL_OK){
      	 printf("Error read data\n");
    	 return FALSE;
     }
     print_hex("read data: ", buffer, buffer_size);
     return TRUE;
}

extern BOOLEAN i2c_send_data_to_device(uint16_t device_addr, uint8_t *data, uint16_t data_size){
    if(HAL_I2C_Master_Transmit(&hi2c1, (uint16_t) device_addr << 1, &data[0], data_size, HAL_MAX_DELAY) != HAL_OK){
    	printf("Error during data transfer\n");
    	return FALSE;
    }
    print_hex("ack data: ", data, data_size);
	return TRUE;
}


extern BOOLEAN has_device(UINT16 addr){
	UINT8 state = 0;
	state = HAL_I2C_IsDeviceReady(&hi2c1, addr << 1, 30, 5000);
	if(state == HAL_OK){
	    printf("Has device 0x%02X\n", addr);
	} else {
		printf("Device was not found\n");
	}
    return TRUE;
}


extern void scan_bus_and_print(void){
	UINT8 state = 0;
	printf("Start scanning slaves I2C:\n");
	for(int  i = 0; i < 128; i++){
		state = HAL_I2C_IsDeviceReady(&hi2c1, (uint16_t)(i << 1), 30, 5000);
		if(state == HAL_OK){
			printf(" 0x%02X", i);
		} else {
			printf("  .  ");
		}

		if(i % 12 == 0){
			printf("\n");
		}
	}
	printf("\nStop scanning.\n");
}
