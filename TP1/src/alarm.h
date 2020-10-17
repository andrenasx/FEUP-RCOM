#pragma once
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include "ll.h"

void alarmHandler(int signal);
void setAlarm();
void unsetAlarm();