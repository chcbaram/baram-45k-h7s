#ifndef HW_DEF_H_
#define HW_DEF_H_



#include "bsp.h"


//-- config.h
//
#define KBD_NAME                    "BARAM-45K-H7S"
#define USB_VID                     0x0483
#define USB_PID                     0x5301

#define      MATRIX_ROWS            4
#define      MATRIX_COLS            12

#define QMK_KEYMAP_CONFIG_H         "def.h"


/* HID report IDs */
enum hid_report_ids {
    REPORT_ID_KEYBOARD = 1,
    REPORT_ID_MOUSE,
    REPORT_ID_SYSTEM,
    REPORT_ID_CONSUMER,
    REPORT_ID_PROGRAMMABLE_BUTTON,
    REPORT_ID_NKRO,
    REPORT_ID_JOYSTICK,
    REPORT_ID_DIGITIZER
};



//--

#define _DEF_FIRMWATRE_VERSION      "V241028R1"
#define _DEF_BOARD_NAME             "BARAM-45K-H7S-FW"


#define _USE_HW_CACHE
#define _USE_HW_MICROS
// #define _USE_HW_QSPI
#define _USE_HW_FLASH
#define _USE_HW_VCOM


#define _USE_HW_LED
#define      HW_LED_MAX_CH          1

#define _USE_HW_UART
#define      HW_UART_MAX_CH         2
#define      HW_UART_CH_SWD         _DEF_UART1
#define      HW_UART_CH_USB         _DEF_UART2
#define      HW_UART_CH_CLI         HW_UART_CH_SWD

#define _USE_HW_CLI
#define      HW_CLI_CMD_LIST_MAX    32
#define      HW_CLI_CMD_NAME_MAX    16
#define      HW_CLI_LINE_HIS_MAX    8
#define      HW_CLI_LINE_BUF_MAX    64

#define _USE_HW_CLI_GUI
#define      HW_CLI_GUI_WIDTH       80
#define      HW_CLI_GUI_HEIGHT      24

#define _USE_HW_LOG
#define      HW_LOG_CH              HW_UART_CH_SWD
#define      HW_LOG_BOOT_BUF_MAX    2048
#define      HW_LOG_LIST_BUF_MAX    4096

#define _USE_HW_I2C
#define      HW_I2C_MAX_CH          1

#define _USE_HW_EEPROM
#define         EEPROM_CHIP_ZD24C128

#define _USE_HW_RTC
#define      HW_RTC_BOOT_MODE       RTC_BKP_DR3
#define      HW_RTC_RESET_BITS      RTC_BKP_DR4

#define _USE_HW_RESET
#define      HW_RESET_BOOT          1

#define _USE_HW_KEYS
#define      HW_KEYS_PRESS_MAX      8

#define _USE_HW_WS2812
#define     HW_WS2812_MAX_CH        45

#define _USE_HW_USB
#define _USE_HW_CDC
#ifdef  _USE_HW_VCOM
#define      HW_USB_LOG             0
#define      HW_USB_CMP             1
#define      HW_USB_CDC             1
#define      HW_USB_MSC             0
#define      HW_USB_HID             1
#else
#define      HW_USB_LOG             0
#define      HW_USB_CMP             0
#define      HW_USB_CDC             0
#define      HW_USB_MSC             0
#define      HW_USB_HID             1
#endif


//-- CLI
//
#define _USE_CLI_HW_EEPROM          1
#define _USE_CLI_HW_I2C             1
#define _USE_CLI_HW_QSPI            1
#define _USE_CLI_HW_FLASH           1
#define _USE_CLI_HW_RTC             1
#define _USE_CLI_HW_RESET           1
#define _USE_CLI_HW_KEYS            1
#define _USE_CLI_HW_WS2812          1


#endif
