#include "alarm.h"

void alarmHandler(int signal) {
    if(signal != SIGALRM) {
        return;
    }

    printf("Alarm: %d\n", linklayer.numTransmissions + 1);

    linklayer.alarm = 1;
    linklayer.numTransmissions++;
    linklayer.stats.numTimeouts++;
}

// Set sigaction struct and linkLayer struct
void setAlarm() {
    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_handler = &alarmHandler;
    sa.sa_flags = 0;

    sigaction(SIGALRM, &sa, NULL);

    linklayer.alarm = 0;
}

void startAlarm() {
    linklayer.alarm = 0;
    alarm(linklayer.timeout); // start alarm
}

void stopAlarm() {
    linklayer.alarm = 0;
    alarm(0); // uninstall alarm
}


// Unset sigaction struct
void unsetAlarm() {
    struct sigaction sa;
    sa.sa_handler = NULL;

    sigaction(SIGALRM, &sa, NULL);

    linklayer.alarm = 0;
}