#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include "MilkyCore.h"

#define USER_PARAM_SIZE 16

class MilkyConfigHelper
{
  public:
    typedef union {
      struct {
        //Magic Header
        char magicHeader[4];
        uint8_t configVersion;
      
        //System Config
        char systemName[13];
        bool rebootOnCrash;
        int screenSaverTimeout;
      
        //Ethernet Config
        bool enableDHCP;
        bool allowStaticFallback;
        //Device IPs
        uint8_t device[4];
        uint8_t netmask[4];
        uint8_t gateway[4];
        uint8_t dns[4];

        //RTC / NTP Config
        bool enableNTP;
        int timezoneOffset;
        bool enableDST;
        int updateInterval;

        //User App Params
        uint8_t userParameters[USER_PARAM_SIZE];
      };
      byte store[EEPROMSIZE];
    } config_t ;

    byte mac[6];
    config_t parameters;

    void configurationMemoryInit();
    void readConfigFromMemory();
    void writeConfigToMemory();
    bool verifyAndInitializeConfig();
    void readMacAddressFromMemory();
    char *getSystemName();
    void initalizeConfig();
    void writeUserParameter(uint8_t index, uint8_t value);
    uint8_t readUserParameter(uint8_t index);
    String macAddressToString(byte macAddress[]);

  private:
    char magic[4] = {'W','M','S','2'};
    uint8_t eepromRead(uint8_t address);
    void eepromWrite(uint8_t address, uint8_t data);
};

extern MilkyConfigHelper SystemConfig;

#endif