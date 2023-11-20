#include "fw_updater.h"
#include "gpio.h"
#include "i2c_manager.h"
#include "fatfs.h"
#include "string.h"
#include "stdio.h"

#define SNP_ADDR 0x36
#define LOCKBYTE_ONE_ADDR 0x42FF
#define LOCKBYTE_THREE_ADDR 0x407F
#define UNLOCK_AREA 0xFF

static FATFS FatFs;
static FIL fil;
static FRESULT fr;
static unsigned int bytesRead;
static char *readBuff;
static char *content;
static char opt_data[68] = {0x40, 0x00, 0x02, 0x00, 0x00, 0x09, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
static UINT16 crc16;

 SD_HandleTypeDef hsd;
 DMA_HandleTypeDef hdma_sdio_rx;
 DMA_HandleTypeDef hdma_sdio_tx;

static void MX_DMA_Init(void);
static void MX_SDIO_SD_Init(void);

static BOOLEAN prep_firmware(void);
static BOOLEAN reset_to_factory(void);
static void print_err(FRESULT fr);
static uint16_t f_addr = 0x0000;

static BOOLEAN is_init = FALSE;
static BOOLEAN init_sd(void);
static BOOLEAN read_sd_and_firmware(void);
static BOOLEAN send_opt(void);
static BOOLEAN check_firmware(void);
static BOOLEAN finish_firmware(void);

static BOOLEAN init_sd(void){
	if(is_init == FALSE){
	   readBuff = (char*) malloc(100);
	   content = (char*) malloc(100);
	   MX_DMA_Init();
	   MX_SDIO_SD_Init();
	   MX_FATFS_Init();
	}
	is_init = TRUE;
	return TRUE;
}

static BOOLEAN send_opt(void){
	// Считаем CRC16 для OPT data
    crc16 = crc16_augccitt_false(&opt_data[0], 66);
    opt_data[66] = crc16 >> 8;
    opt_data[67] = crc16;

    i2c_send_data_to_device(0x36, opt_data, 68);
    // Проверка OPT

    // 0x40 0x00 0x02 0x00 0x00 0x09 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xF0 0x5C
    //   1    2    3    4   5    6    7    8    9    10   11   12   13   14   15   16   17   18   19   20   21   22   23   24   25   26   27   28   29   30   31   32   33   34   35   36   37   38   39   40   41   42   43   44   45   46   47   48   49   50   51   52   53   54   55   56   57   58   59   60   61   62   63   64   65   66   67   68
    if(!i2c_send_data_to_device(0x36, opt_data, 68)){
    	printf("Can't write OPT data\n");
    	return FALSE;
    }

    // Считываем и проверяем что записано
    char buff[66];
    memset(&buff[0], 0x00, 66);
    if(!i2c_read_data_from_device(0x36, 0x4000, buff, 66)){
    	printf("Can't read OPT data\n");
    }

    char expect[64] = {0x00};
    char actual[64] = {0x00};
    memset(&expect[0], 0x00, 64);
    memset(&actual[0], 0x00, 64);

    memcpy(&expect[0], opt_data[2], 64);
    memcpy(&actual[0], &buff[0], 64);

    for(int i = 0; i < 64; i++){
    	if(actual[i] != opt_data[i + 2]){
    		printf("Mismatch bytes OPT data index = %d, expect = %02X, actual = %02X", i, opt_data[i + 2], actual[i]);
    		print_hex("exp:", &opt_data[2], 64);
    		print_hex("act:", actual, 64);
    		return FALSE;
    	}
    }
    return TRUE;

}


static BOOLEAN read_sd_and_firmware(void){
	  f_addr = 0x0000; // Сброс адреса регистров

	  // Подключение флешки
	  fr = f_mount(&FatFs, "", 1);
	  if(fr != FR_OK){
		  printf("SD card detection error %d\n", fr);
		  return FALSE;
	  }
	  printf("SD card detected\n");

      // Открытие файла
	  fr = f_open(&fil, "senasic_app_snp736.bin", FA_READ);
	  if(fr != FR_OK){
		  printf("Can't open file >senasic_app_snp736.bin<  %d\n", fr);
		  return FALSE;
	  }
	  printf("File >senasic_app_snp736.bin< was opened success\n");

	  memset(&readBuff[0], 0x00, 100);
	  memset(&content[0], 0x00, 100);
	  while(f_read(&fil, readBuff, 64, &bytesRead) == FR_OK){
	  if(bytesRead == 0) goto down; // Если ничего не прочитано

	     // Добавляем адрес регистра
	     content[0] = f_addr >> 8;
	     content[1] = f_addr;
	     memcpy(&content[2], &readBuff[0], bytesRead);

	     if(f_addr == 0x1FC0){
	        content[bytesRead + 1] = 0xEF;
		    content[bytesRead] = 0xCD;
		    content[bytesRead - 1] = 0xAB;
	     }

	     // Добавляем контрольную сумму
	     crc16 = crc16_augccitt_false(&content[0], bytesRead + 2);
	     content[bytesRead + 2] = crc16 >> 8;
	     content[bytesRead + 3] = crc16;

	     // Отправка
	     if(!i2c_send_data_to_device(0x36, content, bytesRead + 4)) {
		    print_hex("ERROR TO SEND: ", content, bytesRead + 4);
		    return FALSE;
	     }

	     f_addr += bytesRead;

	     memset(&readBuff[0], 0x00, 100);
	     memset(&content[0], 0x00, 100);
	  }
	  down:

	  // Закрытие файла
      f_close(&fil);
      return TRUE;
}

static BOOLEAN check_firmware(void){
	 f_addr = 0x0000; // Сброс адреса регистров

	 // Подключение флешки
     fr = f_mount(&FatFs, "", 1);
	 if(fr != FR_OK){
		printf("SD card detection error %d\n", fr);
		return FALSE;
     }
     printf("SD card detected\n");

	 // Открытие файла
     fr = f_open(&fil, "senasic_app_snp736.bin", FA_READ);
	 if(fr != FR_OK){
	     printf("Can't open file >senasic_app_snp736.bin<  %d\n", fr);
		 return FALSE;
	  }
	  printf("File >senasic_app_snp736.bin< was opened success\n");

	  memset(&readBuff[0], 0x00, 100);
	  memset(&content[0], 0x00, 100);
	  while(f_read(&fil, readBuff, 64, &bytesRead) == FR_OK){            // Читаем из файла 64 байта
		     if(f_addr == 0x1FC0){
		    	 print_hex("PRINT before: ", readBuff, bytesRead);
		    	 readBuff[bytesRead - 1] = 0xEF;
		    	 readBuff[bytesRead - 2] = 0xCD;
		    	 readBuff[bytesRead - 3] = 0xAB;
		    	 print_hex("PRINT after: ", readBuff, bytesRead);
		  	 }

			 if(!i2c_read_data_from_device(0x36, f_addr, content, 66)){  // Читаем из памяти 66 байт
				printf("error check\n");
			    return FALSE;
			 }

//			 print_hex("PRINT content: ", content, 66);

			 // Сравниваем
			 for(int i = 0; i < 64; i ++){
				 if(readBuff[i] != content[i]){
					 printf("expect/actual %02X %02X \n", readBuff[i], content[i]);
					 print_hex("SD:", readBuff, 64);
					 print_hex("SN:", content, 64);
					 return FALSE;
				 }
			 }

			 f_addr += 64;
			 if(f_addr == 0x4000) return TRUE;
	  }
	  return TRUE;
}

static BOOLEAN reset_to_factory(void){
     // 1) write to 0x36 ack data: 0xA2 0x01 0x76 0x32
     uint8_t reset_cmd_1[4] = {0xA2, 0x01, 0x76, 0x32};
     if(!i2c_send_data_to_device(SNP_ADDR, &reset_cmd_1, 4)) return FALSE;
     osDelay(100);
     // 2) write to 0x36 ack data: 0xA2, 0x02, 0x46, 0x51
     uint8_t reset_cmd_2[4] = {0xA2, 0x02, 0x46, 0x51};
     if(!i2c_send_data_to_device(SNP_ADDR, reset_cmd_2, 4)) return FALSE;
     return TRUE;
}

extern BOOLEAN update_firmware(void){
	// Подготовка к прошивке
    if(!prep_firmware()) {
    	printf("Can't switch SNP73X to programming mode\n");
    	return FALSE;
    }
    printf("Switch SNP73X to factory state was success\n");

    // Возврат заводской прошивки
    if(!reset_to_factory()) {
    	printf("Can't rollback SNP76X to factory state\n");
    	return FALSE;
    }
    printf("Reset to factory was success\n");
    osDelay(125);
    finish_firmware();
    // Инит SD карты
    init_sd();

    // Чтение файла с SD карты и прошивка датчика
    if(!read_sd_and_firmware()){
    	printf("Firmware error\n");
    	return FALSE;
    }

    // Прочитать все записанные данные
    if(!check_firmware()){
       printf("Error check firmware\n");
       return FALSE;
    }

    // Запись OPT
    if(!send_opt()){
       printf("Error write OPT data\n");
       return FALSE;
    }

    printf("FW was finish success\n");
    osDelay(500);
    finish_firmware();
    return TRUE;
}

/**
  * @brief SDIO Initialization Function
  * @param None
  * @retval None
  */static void MX_SDIO_SD_Init(void) {
    hsd.Instance = SDIO;
    hsd.Init.ClockEdge = SDIO_CLOCK_EDGE_RISING;
    hsd.Init.ClockBypass = SDIO_CLOCK_BYPASS_DISABLE;
    hsd.Init.ClockPowerSave = SDIO_CLOCK_POWER_SAVE_DISABLE;
    hsd.Init.BusWide = SDIO_BUS_WIDE_1B;
    hsd.Init.HardwareFlowControl = SDIO_HARDWARE_FLOW_CONTROL_DISABLE;
    hsd.Init.ClockDiv = 0;
    HAL_SD_Init(&hsd);
  }

static BOOLEAN prep_firmware(void){
	// Перевод SDA/SCL пинов в режим GPIO
		switch_mode_sda_scl(GPIO_M);

		set_sda(GPIO_PIN_RESET);   // SDA off
		set_scl(GPIO_PIN_RESET);   // SCL off
		set_power(GPIO_PIN_RESET); // VDD off
		osDelay(80);
	 	set_power(GPIO_PIN_SET);   // VDD on

		// GPIO0(SCL) GPIO1(SDA) должны удерживаться в состояниий  GPIO1 = 1, GPIO0 = 0 не менее 256 мсек
		set_sda(GPIO_PIN_RESET);    // SDA on
		set_scl(GPIO_PIN_SET);  // SCL off
		osDelay(500);

		// Перевод SDA/SCL пинов в режим I2C
		switch_mode_sda_scl(I2C_M);
		return TRUE;
}

static BOOLEAN finish_firmware(void){
	switch_mode_sda_scl(GPIO_M);
	set_sda(GPIO_PIN_RESET);
	set_scl(GPIO_PIN_RESET);
	osDelay(80);
	set_scl(GPIO_PIN_SET);
	osDelay(320);
	set_sda(GPIO_PIN_SET);
	switch_mode_sda_scl(I2C_M);
	return TRUE;
}

static void print_err(FRESULT fr){
	switch(fr){
	case FR_OK:				/* (0) Succeeded */
		printf("SD: FR_OK \n");
		break;
	case FR_DISK_ERR:			/* (1) A hard error occurred in the low level disk I/O layer */
		printf("SD: FR_DISK_ERR \n");
		break;
	case FR_INT_ERR:				/* (2) Assertion failed */
		printf("SD: FR_INT_ERR \n");
		break;
	case FR_NOT_READY:			/* (3) The physical drive cannot work */
		printf("SD: FR_NOT_READY \n");
		break;
	case FR_NO_FILE:				/* (4) Could not find the file */
		printf("SD: FR_NO_FILE \n");
		break;
	case FR_NO_PATH:				/* (5) Could not find the path */
		printf("SD: FR_NO_PATH \n");
		break;
	case FR_INVALID_NAME:		/* (6) The path name format is invalid */
		printf("SD: FR_INVALID_NAME \n");
		break;
	case FR_DENIED:				/* (7) Access denied due to prohibited access or directory full */
		printf("SD: FR_DENIED \n");
		break;
	case FR_EXIST:				/* (8) Access denied due to prohibited access */
		printf("SD: FR_EXIST \n");
		break;
	case FR_INVALID_OBJECT:		/* (9) The file/directory object is invalid */
		printf("SD: FR_INVALID_OBJECT \n");
		break;
	case FR_WRITE_PROTECTED:		/* (10) The physical drive is write protected */
		printf("SD: FR_WRITE_PROTECTED \n");
		break;
	case FR_INVALID_DRIVE:		/* (11) The logical drive number is invalid */
		printf("SD: FR_INVALID_DRIVE \n");
		break;
	case FR_NOT_ENABLED:			/* (12) The volume has no work area */
		printf("SD: FR_NOT_ENABLED \n");
		break;
	case FR_NO_FILESYSTEM:		/* (13) There is no valid FAT volume */
		printf("SD: FR_NO_FILESYSTEM \n");
		break;
	case FR_MKFS_ABORTED:		/* (14) The f_mkfs() aborted due to any problem */
		printf("SD: FR_MKFS_ABORTED \n");
		break;
	case FR_TIMEOUT:				/* (15) Could not get a grant to access the volume within defined period */
		printf("SD: FR_TIMEOUT \n");
		break;
	case FR_LOCKED:				/* (16) The operation is rejected according to the file sharing policy */
		printf("SD: FR_LOCKED \n");
		break;
	case FR_NOT_ENOUGH_CORE:		/* (17) LFN working buffer could not be allocated */
		printf("SD: FR_NOT_ENOUGH_CORE \n");
		break;
	case FR_TOO_MANY_OPEN_FILES:	/* (18) Number of open files > _FS_LOCK */
		printf("SD: FR_TOO_MANY_OPEN_FILES \n");
		break;
	case FR_INVALID_PARAMETER:	/* (19) Given parameter is invalid */
		printf("SD: FR_INVALID_PARAMETER \n");
		break;
	}
}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{
  /* DMA controller clock enable */
  __HAL_RCC_DMA2_CLK_ENABLE();
  /* DMA interrupt init */
  /* DMA2_Stream3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream3_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream3_IRQn);
  /* DMA2_Stream6_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream6_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream6_IRQn);

}

