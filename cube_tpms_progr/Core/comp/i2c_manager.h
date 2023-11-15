#ifndef _I2C_MANAGER_H_
#define _I2C_MANAGER_H_

#include "app_cfg.h"

#define I2C_READ_TIMEOUT 5000 //(миллисекунд) Таймаут чтения
#define I2C_WRITE_TIMEOUT 5000 //(миллисекунд) Таймаут чтения


extern BOOLEAN i2c_init(void);
extern void scan_bus_and_print(void);

extern BOOLEAN i2c_send_data_to_device(uint16_t device_addr, uint8_t *data, uint16_t data_size);
extern BOOLEAN i2c_read_data_from_device(uint16_t device_addr, uint16_t reg_addr, uint8_t *buffer, uint16_t buffer_size);




#endif /* _I2C_MANAGER_H_ */
