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
static char readBuff[64];

static uint8_t sensor_number = 2;
SD_HandleTypeDef hsd;
DMA_HandleTypeDef hdma_sdio_rx;
DMA_HandleTypeDef hdma_sdio_tx;

static void MX_DMA_Init(void);
static void MX_SDIO_SD_Init(void);

static BOOLEAN prep_firmware(void);
static BOOLEAN reset_to_factory(void);
static void print_err(FRESULT fr);

static BOOLEAN is_init = FALSE;
static BOOLEAN init_sd(void);
static BOOLEAN read_sd_and_firmware(void);

static BOOLEAN init_sd(void){
	if(is_init == FALSE){
	   MX_DMA_Init();
	   MX_SDIO_SD_Init();
	   MX_FATFS_Init();
	}
	is_init = TRUE;
}

static BOOLEAN read_sd_and_firmware(void){
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

	  while(f_read(&fil, readBuff, 64, &bytesRead) == FR_OK){
		  if(bytesRead == 0) goto down;
		  print_hex("READ: ", readBuff, bytesRead);
	  }
	  down:

	  // Закрытие файла
      f_close(&fil);
}

static BOOLEAN reset_to_factory(void){
     // 1) write to 0x36 ack data: 0xA2 0x01 0x76 0x32
//     uint8_t reset_cmd_1[4] = {0xA2, 0x01, 0x76, 0x32};
//     if(!i2c_send_data_to_device(SNP_ADDR, &reset_cmd_1, 4)) return FALSE;

     // 2) write to 0x36 ack data: 0xA2, 0x02, 0x46, 0x51
//     uint8_t reset_cmd_2[4] = {0xA2, 0x02, 0x46, 0x51};
//     if(!i2c_send_data_to_device(SNP_ADDR, reset_cmd_2, 4)) return FALSE;

     return TRUE;
}

extern BOOLEAN update_firmware(void){
	init_sd(); // Инит SD карты

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

    // Чтение файла с SD карты и прошивка датчика
    if(!read_sd_and_firmware()){
    	printf("Firmware error\n");
    	return FALSE;
    }







    // 4) write to 0x36 ack data: 0x40 0x00 0x00 0x00 0x00 0x08 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0x05 0x69
    char data_4[100] = {0x40, 0x00, sensor_number, 0x00, 0x00, 0x08, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x05, 0x69};
    i2c_send_data_to_device(0x36, data_4, 68);



    //write to 0x36 ack data: 0x40 0x00
    //read to 0x36 ack data: 0x00 0x00 0x00 0x08 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0x03 0x97
    //                        1     2    3   4    5    6    7    8    9    10   11   12   13   14   15   16   17   18   19   20   21   22   23   24   25   26   27   28   29   30   31   32   33   34   35   36   37   38   39   40   41   42   43   44   45   46   47   48   49   50   51   52   53   54   55   56   57   58   59   60   61   62   63   64   65   66
//    char buff[66];
//    memset(&buff[0], 0x00, 66);
//    i2c_read_data_from_device(0x36, 0x4000, buff, 66);

    printf("FW was finish\n");
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
	osDelay(1000);
 	set_power(GPIO_PIN_SET);   // VDD on

	// GPIO0(SCL) GPIO1(SDA) должны удерживаться в состояниий  GPIO1 = 1, GPIO0 = 0 не менее 256 мсек
	set_sda(GPIO_PIN_RESET);    // SDA on
	set_scl(GPIO_PIN_SET);  // SCL off
	osDelay(2000);

	// Перевод SDA/SCL пинов в режим I2C
	switch_mode_sda_scl(I2C_M);
	osDelay(1000);

	// Убедиться, что на шине появилось устройство с адресом 0x36
    scan_bus_and_print();
	return has_device(SNP_ADDR);
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

