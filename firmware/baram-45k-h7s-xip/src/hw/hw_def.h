#ifndef HW_DEF_H_
#define HW_DEF_H_



#include "bsp.h"


#define _DEF_FIRMWATRE_VERSION    "V240825R1"
#define _DEF_BOARD_NAME           "BARAM-45K-H7S-XIP"


#define _USE_HW_QSPI
#define _USE_HW_FLASH


#define _USE_HW_LED
#define      HW_LED_MAX_CH          1

#define _USE_HW_UART
#define      HW_UART_MAX_CH         1
#define      HW_UART_CH_SWD         _DEF_UART1

#define _USE_HW_LOG
#define      HW_LOG_CH              HW_UART_CH_SWD
#define      HW_LOG_BOOT_BUF_MAX    2048
#define      HW_LOG_LIST_BUF_MAX    4096

#define _USE_HW_RTC
#define      HW_RTC_BOOT_MODE       RTC_BKP_DR3
#define      HW_RTC_RESET_BITS      RTC_BKP_DR4

#define _USE_HW_RESET
#define      HW_RESET_BOOT          1


#define FLASH_SIZE_TAG              0x400
#define FLASH_SIZE_VEC              0x400
#define FLASH_SIZE_VER              0x400
#define FLASH_SIZE_BOOT             (128*1024)
#define FLASH_SIZE_FIRM             (512*1024)

#define FLASH_ADDR_BOOT             0x90000000
#define FLASH_ADDR_FIRM             0x90080000
#define FLASH_ADDR_UPDATE           0x90100000

#define SRAM_ADDR_BOOT              0x24020000
#define SRAM_ADDR_FIRM              0x24020000

#endif
