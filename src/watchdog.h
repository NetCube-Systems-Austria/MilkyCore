#ifndef WATCHDOG_H
#define WATCHDOG_H

#include <IWatchdog.h>

#include "MilkyCore.h"

class MilkyWatchdogHelper
{
	public:
		void resetBoard();
		static void task(void *arg);

	  void earlyHardwareWatchdogInit();
	  void hardwareWatchdogInit();
	  void hardwareWatchdogTick();
	  
	  static void softwareWatchdogBark(void);
	  void earlySoftwareWatchdogInit();
	  void softwareWatchdogInit();
	  void softwareWatchdogTick();
	  void setSoftwareWatchdogEnabled(bool enabled);

	private:
		HardwareTimer *softwareWatchdog = new HardwareTimer(TIM2);
		bool softwareWatchdogEnabled = false;
};

extern MilkyWatchdogHelper WatchdogHelper;

#endif