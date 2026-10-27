/* SPDX-License-Identifier: MIT */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "checks.h"

check_status check_cpu(char *msg, size_t msg_size) {
    FILE *f;
    char line[256];

    char model[128] = "Unknown";
    int cores = 0;
    double mhz = 0.0;

    // Read CPU info
    f = fopen("/proc/cpuinfo", "r");
    if(!f) {
        snprintf(msg, msg_size, "Could not read /proc/cpuinfo\n");
        return CHECK_FAIL;
    }

    while (fgets(line, sizeof(line), f))
    {
        if (strncmp(line, "model name", 10) == 0) {
            char *p = strchr(line, ':');
            if(p) {
                p++;
                while (*p == ' ' || *p == '\t') p++;
                strncpy(model, p, sizeof(model));
                model[strcspn(model, "\n")]  = '\0';
            }
        }

        if(strncmp(line, "cpu cores", 9) == 0) {
            char *p = strchr(line, ':');
            if(p) cores = atoi(p + 1);
        }

        if(strncmp(line, "cpu MHz", 7) == 0) {
            char *p = strchr(line, ':');
            if(p) mhz = atof(p + 1);
        }
    }
    fclose(f);

    // Read load average (1m)
    double load_avg_1m = 0.0;

    f = fopen("/proc/loadavg", "r");
    if(f) { // Only show this if is available
        fscanf(f, "%lf", &load_avg_1m);
        fclose(f);
    }

    //Build the message
    snprintf(msg, msg_size, 
        "Model: %s\n"
        "Cores: %d\n"
        "Frequency: %.2fGHz\n"
        "Load (1m): %.2f\n"
        , model, cores, (mhz / 1000), load_avg_1m);

    // Status (according to load)
    if (cores > 0) {
        if (load_avg_1m > (cores * 1.2))
            return CHECK_FAIL;
        if(load_avg_1m > (cores * 0.9))
            return CHECK_WARN;
    }

    return CHECK_OK;
}

