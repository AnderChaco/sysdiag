/* SPDX-License-Identifier: MIT */

#include <stdio.h>
#include <string.h>
#include "checks.h"

static void format_memory(char *buf, size_t size, long kib) {
    double mib = kib / 1024.0;
    double gib = mib / 1024.0;

    snprintf(buf, size,
        "%.2f GiB | %.2f MiB | %ld KiB",
        gib, mib, kib
    );
}

check_status check_memory(char *msg, size_t msg_size) {
    FILE *f = fopen("/proc/meminfo", "r");
    if (!f) {
        snprintf(msg, msg_size, "Could not read /proc/meminfo\n");
        return CHECK_FAIL;
    }

    char key[64];
    long value;
    long mem_total_kb = 0;
    long mem_available_kb = 0;

    while (fscanf(f, "%63s %ld kB", key, &value) == 2) {
        if (strcmp(key, "MemTotal:") == 0)
            mem_total_kb = value;
        else if (strcmp(key, "MemAvailable:") == 0)
            mem_available_kb = value;

        if (mem_total_kb && mem_available_kb)
            break;
    }
    fclose(f);

    char total_str[64];
    char avail_str[64];

    format_memory(total_str, sizeof(total_str), mem_total_kb);
    format_memory(avail_str, sizeof(avail_str), mem_available_kb);

    // Build the message
    snprintf(msg, msg_size,
        "Total Memory:     %s\n"
        "Available Memory: %s\n",
        total_str,
        avail_str
    );

    if (mem_available_kb < 200 * 1024)
        return CHECK_WARN;

    return CHECK_OK;
}
