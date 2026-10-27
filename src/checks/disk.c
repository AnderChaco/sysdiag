/* SPDX-License-Identifier: MIT */

#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <sys/statvfs.h>
#include "checks.h"

static void format_size(char *buf, size_t size, unsigned long long bytes) {
    double kib = bytes / 1024.0;
    double mib = kib / 1024.0;
    double gib = mib / 1024.0;

    snprintf(buf, size, "%.2fGiB | %.2fMiB | %.0fKiB", gib, mib, kib);
}

check_status check_disk(char *msg, size_t msg_size) {
    struct statvfs fs;

    if (statvfs("/", &fs) != 0) {
        snprintf(msg, msg_size, "Could not read filesystem stats.\n");
        return CHECK_FAIL;
    }

    unsigned long long total = (unsigned long long)fs.f_blocks * fs.f_frsize;
    unsigned long long free = (unsigned long long)fs.f_bfree * fs.f_frsize;
    unsigned long long avail = (unsigned long long)fs.f_bavail * fs.f_frsize;
    unsigned long long used = total - free;

    double used_pct = (total > 0) ? (used * 100 / total) : 0.0;

    char total_str[64];
    char used_str[64];
    char free_str[64];
    
    format_size(total_str, sizeof(total_str), total);
    format_size(used_str, sizeof(used_str), used);
    format_size(free_str, sizeof(free_str), avail);

    //Build the message (consider refactor to an external function)
    //
    snprintf(msg, msg_size,
        "Total: %s\n"
        "Used: %s\n"
        "Available: %s\n"
        "Usage: %.1f%%\n",
        total_str,
        used_str,
        free_str,
        used_pct
    );

    //Status according % used...
    if(used_pct > 95.0)
        return CHECK_FAIL;
    if(used_pct > 85.0)
        return CHECK_WARN;

    return CHECK_OK;
}

