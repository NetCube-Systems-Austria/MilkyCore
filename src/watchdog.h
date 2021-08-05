#ifndef WATCHDOG_H
#define WATCHDOG_H

#include <IWatchdog.h>

#include "MilkyCore.h"

class MilkyWatchdogHelper
{
	public:
		void resetBoard();
		static void task(void *arg);

		#ifdef ENABLE_HARDWARE_WATCHDOG
		  void earlyHardwareWatchdogInit();
		  void hardwareWatchdogInit();
		  void hardwareWatchdogTick();
		#endif

		#ifdef ENABLE_SOFTWARE_WATCHDOG
		  static void softwareWatchdogBark(void);
		  void earlySoftwareWatchdogInit();
		  void softwareWatchdogInit();
		  void softwareWatchdogTick();
		  void setSoftwareWatchdogEnabled(bool enabled);
		#endif

	private:
		HardwareTimer *softwareWatchdog = new HardwareTimer(TIM2);
		bool softwareWatchdogEnabled = false;
};

extern MilkyWatchdogHelper WatchdogHelper;

#endif