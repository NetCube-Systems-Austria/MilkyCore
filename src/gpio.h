#ifndef GPIO_H
#define GPIO_H

#include "MilkyCore.h"

#define PINTYPE_DIGITALIN   0
#define PINTYPE_DIGITALOUT  32
#define PINTYPE_ANALOGIN    64
#define PINTYPE_ANALOGOUT   96

#define I1  PINTYPE_DIGITALIN + 0
#define I2  PINTYPE_DIGITALIN + 1
#define I3  PINTYPE_DIGITALIN + 2
#define I4  PINTYPE_DIGITALIN + 3
#define I5  PINTYPE_DIGITALIN + 4
#define I6  PINTYPE_DIGITALIN + 5
#define I7  PINTYPE_DIGITALIN + 6
#define I8  PINTYPE_DIGITALIN + 7
#define I9  PINTYPE_DIGITALIN + 8
#define I10 PINTYPE_DIGITALIN + 9
#define I11 PINTYPE_DIGITALIN + 10
#define I12 PINTYPE_DIGITALIN + 11
#define I13 PINTYPE_DIGITALIN + 12
#define I14 PINTYPE_DIGITALIN + 13
#define I15 PINTYPE_DIGITALIN + 14
#define I16 PINTYPE_DIGITALIN + 15
#define I17 PINTYPE_DIGITALIN + 16
#define I18 PINTYPE_DIGITALIN + 17
#define I19 PINTYPE_DIGITALIN + 18
#define I20 PINTYPE_DIGITALIN + 19
#define I21 PINTYPE_DIGITALIN + 20
#define I22 PINTYPE_DIGITALIN + 21
#define I23 PINTYPE_DIGITALIN + 22
#define I24 PINTYPE_DIGITALIN + 23
#define I25 PINTYPE_DIGITALIN + 24
#define I26 PINTYPE_DIGITALIN + 25
#define I27 PINTYPE_DIGITALIN + 26
#define I28 PINTYPE_DIGITALIN + 27
#define I29 PINTYPE_DIGITALIN + 28
#define I30 PINTYPE_DIGITALIN + 29
#define I31 PINTYPE_DIGITALIN + 30
#define I32 PINTYPE_DIGITALIN + 31
  
#define O1  PINTYPE_DIGITALOUT + 0
#define O2  PINTYPE_DIGITALOUT + 1
#define O3  PINTYPE_DIGITALOUT + 2
#define O4  PINTYPE_DIGITALOUT + 3
#define O5  PINTYPE_DIGITALOUT + 4
#define O6  PINTYPE_DIGITALOUT + 5
#define O7  PINTYPE_DIGITALOUT + 6
#define O8  PINTYPE_DIGITALOUT + 7
#define O9  PINTYPE_DIGITALOUT + 8
#define O10 PINTYPE_DIGITALOUT + 9
#define O11 PINTYPE_DIGITALOUT + 10
#define O12 PINTYPE_DIGITALOUT + 11
#define O13 PINTYPE_DIGITALOUT + 12
#define O14 PINTYPE_DIGITALOUT + 13
#define O15 PINTYPE_DIGITALOUT + 14
#define O16 PINTYPE_DIGITALOUT + 15
#define O17 PINTYPE_DIGITALOUT + 16
#define O18 PINTYPE_DIGITALOUT + 17
#define O19 PINTYPE_DIGITALOUT + 18
#define O20 PINTYPE_DIGITALOUT + 19
#define O21 PINTYPE_DIGITALOUT + 20
#define O22 PINTYPE_DIGITALOUT + 21
#define O23 PINTYPE_DIGITALOUT + 22
#define O24 PINTYPE_DIGITALOUT + 23
#define O25 PINTYPE_DIGITALOUT + 24
#define O26 PINTYPE_DIGITALOUT + 25
#define O27 PINTYPE_DIGITALOUT + 26
#define O28 PINTYPE_DIGITALOUT + 27
#define O29 PINTYPE_DIGITALOUT + 28
#define O30 PINTYPE_DIGITALOUT + 29
#define O31 PINTYPE_DIGITALOUT + 30
#define O32 PINTYPE_DIGITALOUT + 31
  
#define Q1  PINTYPE_DIGITALOUT + 0
#define Q2  PINTYPE_DIGITALOUT + 1
#define Q3  PINTYPE_DIGITALOUT + 2
#define Q4  PINTYPE_DIGITALOUT + 3
#define Q5  PINTYPE_DIGITALOUT + 4
#define Q6  PINTYPE_DIGITALOUT + 5
#define Q7  PINTYPE_DIGITALOUT + 6
#define Q8  PINTYPE_DIGITALOUT + 7
#define Q9  PINTYPE_DIGITALOUT + 8
#define Q10 PINTYPE_DIGITALOUT + 9
#define Q11 PINTYPE_DIGITALOUT + 10
#define Q12 PINTYPE_DIGITALOUT + 11
#define Q13 PINTYPE_DIGITALOUT + 12
#define Q14 PINTYPE_DIGITALOUT + 13
#define Q15 PINTYPE_DIGITALOUT + 14
#define Q16 PINTYPE_DIGITALOUT + 15
#define Q17 PINTYPE_DIGITALOUT + 16
#define Q18 PINTYPE_DIGITALOUT + 17
#define Q19 PINTYPE_DIGITALOUT + 18
#define Q20 PINTYPE_DIGITALOUT + 19
#define Q21 PINTYPE_DIGITALOUT + 20
#define Q22 PINTYPE_DIGITALOUT + 21
#define Q23 PINTYPE_DIGITALOUT + 22
#define Q24 PINTYPE_DIGITALOUT + 23
#define Q25 PINTYPE_DIGITALOUT + 24
#define Q26 PINTYPE_DIGITALOUT + 25
#define Q27 PINTYPE_DIGITALOUT + 26
#define Q28 PINTYPE_DIGITALOUT + 27
#define Q29 PINTYPE_DIGITALOUT + 28
#define Q30 PINTYPE_DIGITALOUT + 29
#define Q31 PINTYPE_DIGITALOUT + 30
#define Q32 PINTYPE_DIGITALOUT + 31
  
#define AI1  PINTYPE_ANALOGIN + 0
#define AI2  PINTYPE_ANALOGIN + 1
#define AI3  PINTYPE_ANALOGIN + 2
#define AI4  PINTYPE_ANALOGIN + 3
#define AI5  PINTYPE_ANALOGIN + 4
#define AI6  PINTYPE_ANALOGIN + 5
#define AI7  PINTYPE_ANALOGIN + 6
#define AI8  PINTYPE_ANALOGIN + 7
#define AI9  PINTYPE_ANALOGIN + 8
#define AI10 PINTYPE_ANALOGIN + 9
#define AI11 PINTYPE_ANALOGIN + 10
#define AI12 PINTYPE_ANALOGIN + 11
#define AI13 PINTYPE_ANALOGIN + 12
#define AI14 PINTYPE_ANALOGIN + 13
#define AI15 PINTYPE_ANALOGIN + 14
#define AI16 PINTYPE_ANALOGIN + 15
#define AI17 PINTYPE_ANALOGIN + 16
#define AI18 PINTYPE_ANALOGIN + 17
#define AI19 PINTYPE_ANALOGIN + 18
#define AI20 PINTYPE_ANALOGIN + 19
#define AI21 PINTYPE_ANALOGIN + 20
#define AI22 PINTYPE_ANALOGIN + 21
#define AI23 PINTYPE_ANALOGIN + 22
#define AI24 PINTYPE_ANALOGIN + 23
#define AI25 PINTYPE_ANALOGIN + 24
#define AI26 PINTYPE_ANALOGIN + 25
#define AI27 PINTYPE_ANALOGIN + 26
#define AI28 PINTYPE_ANALOGIN + 27
#define AI29 PINTYPE_ANALOGIN + 28
#define AI30 PINTYPE_ANALOGIN + 29
#define AI31 PINTYPE_ANALOGIN + 30
#define AI32 PINTYPE_ANALOGIN + 31
  
#define AO1  PINTYPE_ANALOGOUT + 0
#define AO2  PINTYPE_ANALOGOUT + 1
#define AO3  PINTYPE_ANALOGOUT + 2
#define AO4  PINTYPE_ANALOGOUT + 3
#define AO5  PINTYPE_ANALOGOUT + 4
#define AO6  PINTYPE_ANALOGOUT + 5
#define AO7  PINTYPE_ANALOGOUT + 6
#define AO8  PINTYPE_ANALOGOUT + 7
#define AO9  PINTYPE_ANALOGOUT + 8
#define AO10 PINTYPE_ANALOGOUT + 9
#define AO11 PINTYPE_ANALOGOUT + 10
#define AO12 PINTYPE_ANALOGOUT + 11
#define AO13 PINTYPE_ANALOGOUT + 12
#define AO14 PINTYPE_ANALOGOUT + 13
#define AO15 PINTYPE_ANALOGOUT + 14
#define AO16 PINTYPE_ANALOGOUT + 15
#define AO17 PINTYPE_ANALOGOUT + 16
#define AO18 PINTYPE_ANALOGOUT + 17
#define AO19 PINTYPE_ANALOGOUT + 18
#define AO20 PINTYPE_ANALOGOUT + 19
#define AO21 PINTYPE_ANALOGOUT + 20
#define AO22 PINTYPE_ANALOGOUT + 21
#define AO23 PINTYPE_ANALOGOUT + 22
#define AO24 PINTYPE_ANALOGOUT + 23
#define AO25 PINTYPE_ANALOGOUT + 24
#define AO26 PINTYPE_ANALOGOUT + 25
#define AO27 PINTYPE_ANALOGOUT + 26
#define AO28 PINTYPE_ANALOGOUT + 27
#define AO29 PINTYPE_ANALOGOUT + 28
#define AO30 PINTYPE_ANALOGOUT + 29
#define AO31 PINTYPE_ANALOGOUT + 30
#define AO32 PINTYPE_ANALOGOUT + 31

class LocalIO {
  public:
    void init();

    void setDigital(uint8_t pin, uint8_t state);
    uint8_t getDigital(uint8_t pin);
    void setAnalog(uint8_t pin, uint16_t value);
    uint16_t getAnalog(uint8_t pin);

  private:
    void expanderSetPin(uint8_t pin, uint8_t state);
    uint8_t expanderGetPin(uint8_t pin);

    uint8_t expanderState = 0;
};

extern LocalIO GPIO;

#endif