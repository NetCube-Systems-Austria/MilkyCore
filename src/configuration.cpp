#include "configuration.h"

void MilkyConfigHelper::configurationMemoryInit() {
  Wire.setSDA(I2C1_SDA);
  Wire.setSCL(I2C1_SCL);
  Wire.begin();
     
  SystemLogger.writeLine(F("[ config ] I2C bus initialized"));
}

void MilkyConfigHelper::readConfigFromMemory() {
  vTaskSuspendAll();
  for ( int i = 0 ; i < EEPROMSIZE ; i++ ) {
    parameters.store[i] = eepromRead(i);
  }
  xTaskResumeAll();
  SystemLogger.writeLine(F("[ config ] Configuration read!"));
  verifyAndInitializeConfig();
}

void MilkyConfigHelper::writeConfigToMemory() {
  vTaskSuspendAll();
  for ( int i = 0 ; i < EEPROMSIZE ; i++ ) {
    eepromWrite(i, parameters.store[i]);
  }
  xTaskResumeAll();
  SystemLogger.writeLine(F("[ config ] Configuration written!"));
}

void MilkyConfigHelper::readMacAddressFromMemory() {
  vTaskSuspendAll();
  for ( int i = 0 ; i < 6 ; i++ ) {
    mac[i] = eepromRead(i + EEPROM_MAC_INDEX);
  }
  xTaskResumeAll();
  SystemLogger.writeLine(F("[ config ] MAC address read!"));
}

//returns false if config had to be intitialized
bool MilkyConfigHelper::verifyAndInitializeConfig() {
  if (memcmp(parameters.magicHeader, magic, 4) != 0) {
    initalizeConfig();
    writeConfigToMemory();
    return false;
  } else {
    return true;
  }
}

char *MilkyConfigHelper::getSystemName() {return parameters.systemName;}

void MilkyConfigHelper::initalizeConfig() {
  memcpy(parameters.magicHeader, magic, 4);
  parameters.configVersion = 0;
  sprintf(parameters.systemName, "Milky_%2x%2x%2x", mac[3], mac[4], mac[5]);
  parameters.rebootOnCrash = true;
  parameters.screenSaverTimeout = 120;
  
  parameters.enableDHCP = true;
  parameters.allowStaticFallback = false;
  memcpy(parameters.device, new byte[4] {192,168,0,100}, 4);
  memcpy(parameters.netmask, new byte[4] {255,255,255,0}, 4);
  memcpy(parameters.gateway, new byte[4] {192,168,0,1}, 4);
  memcpy(parameters.dns, new byte[4] {192,168,0,1}, 4);

  parameters.enableNTP = true;
  parameters.timezoneOffset = 1;
  parameters.enableDST = true;
  parameters.updateInterval = 15; //Every 15 mins
  
  SystemLogger.writeLine(F("[ config ] Configuration initialized!"));
}

uint8_t MilkyConfigHelper::eepromRead(uint8_t address) {
  uint8_t data;
  
  Wire.beginTransmission(EEPROMADDR); 
  Wire.write(address);
  Wire.endTransmission(); 
  
  Wire.requestFrom(EEPROMADDR, 1);
  if (Wire.available()) {
    data = Wire.read();
  }
  Wire.endTransmission();
  
  return data;
}

void MilkyConfigHelper::eepromWrite(uint8_t address, uint8_t data) {
  Wire.beginTransmission(EEPROMADDR); 
  Wire.write(address);
  Wire.write(data);
  delay(1);
  Wire.endTransmission();
}

String MilkyConfigHelper::macAddressToString(byte macAddress[]) {
  String s;
  for (byte i = 0; i < 6; ++i)
  {
    char buf[3];
    sprintf(buf, "%02X", macAddress[i]);
    s += buf;
    if (i < 5) s += ':';
  }
  return s;
}

MilkyConfigHelper SystemConfig;
