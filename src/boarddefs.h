#ifndef BOARDDEFS_H
    #define BOARDDEFS_H

    #include "MilkyCore.h"

    #ifdef WMS_REV_B
        #define EXT_GPIO0 PA0
        #define EXT_GPIO1 PA1
        #define EXT_GPIO2 PA2
        #define EXT_GPIO3 PA3
        #define EXT_GPIO4 PA4
        #define EXT_GPIO5 PA5
        #define EXT_GPIO6 PA6
        #define EXT_GPIO7 PA7

        #define QBus_EN -1
        #define QBus_TX -1
        #define QBus_RX -1

        #define GPIO_A0 PA15
        #define GPIO_A1 PB3
        #define GPIO_A2 PB4
        #define GPIO_A3 PB5
        #define GPIO_B0 PA8
        #define GPIO_B1 PB10
        #define GPIO_B2 PB1
        #define GPIO_B3 PB2

        #define ETH_MOSI PB15
        #define ETH_MISO PB14
        #define ETH_SCK PB13
        #define ETH_CS PB12

        #define HEARTBEAT_LED PB8
        #define USER_LED -1
        #define ERROR_LED PB9

        #define I2C1_SDA PB7
        #define I2C1_SCL PB6
        #define EXPANDER_ADDR 0x20

        #define EEPROMADDR 0x50
        #define EEPROMSIZE 0x80
        #define EEPROM_MAC_INDEX 0xFA
    #elif WMS_REV_C
        #define EXT_GPIO0 PA0
        #define EXT_GPIO1 PA1
        #define EXT_GPIO2 PA2
        #define EXT_GPIO3 PA3
        #define EXT_GPIO4 PA4
        #define EXT_GPIO5 PA5
        #define EXT_GPIO6 PA6
        #define EXT_GPIO7 PA7

        #define QBus_EN PA8
        #define QBus_TX PA9
        #define QBus_RX PA10

        #define GPIO_A0 PB15
        #define GPIO_A1 PB14
        #define GPIO_A2 PB13
        #define GPIO_A3 PB12
        #define GPIO_B0 PB10
        #define GPIO_B1 PB2
        #define GPIO_B2 PB1
        #define GPIO_B3 PB0

        #define ETH_MOSI PB5
        #define ETH_MISO PB4
        #define ETH_SCK PB3
        #define ETH_CS PA15

        #define HEARTBEAT_LED PB8
        #define USER_LED PC13
        #define ERROR_LED PB9

        #define I2C1_SDA PB7
        #define I2C1_SCL PB6
        #define EXPANDER_ADDR 0x20

        #define EEPROMADDR 0x50
        #define EEPROMSIZE 0x80
        #define EEPROM_MAC_INDEX 0xFA
    #else
        #error "Board Type not defined. Please define 'WMS_REV_B' or 'WMS_REV_C' according to your board version."
    #endif
#endif