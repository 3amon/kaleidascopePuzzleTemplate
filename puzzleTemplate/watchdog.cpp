//
// Created by Lasciel on 8/13/2016.
//

#include <Arduino.h>
#include "config.h"

unsigned long watchdog_timer = 0;
bool watchdog_timer_active = false;

bool WatchdogTimerExpired()
{
    return watchdog_timer_active && (millis() - watchdog_timer > WATCHDOG_TIMEOUT);
}

void StartWatchdogTimer()
{
    watchdog_timer_active = true;
    watchdog_timer = millis();
}

void ResetWatchdogTimer()
{
    watchdog_timer = millis();
}

void StopWatchdogTimer()
{
    watchdog_timer_active = false;
}

#include "watchdog.h"
