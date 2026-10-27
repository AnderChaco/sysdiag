/* SPDX-License-Identifier: MIT */

#include <stdio.h>
#include "style.h"

void print_status_label(check_status st) {
    const char *color;
    const char *label;

    switch (st) {
        case CHECK_OK:
            color = COLOR_GREEN;
            label = "✓ OK";
            break;
        case CHECK_WARN:
            color = COLOR_YELLOW;
            label = "⚠ WARN";
            break;
        default:
            color = COLOR_RED;
            label = "✗ FAIL";
            break;
    }

    printf("[%s%s%s]", color, label, COLOR_RESET);
}

void print_section_title(const char *title) {
    printf("%s%s %s %s\n", COLOR_LIGHT_GREY, COLOR_BOLD, title, COLOR_RESET);
    printf("--------------------------------------\n");
}

void print_key_val(const char *key, const char *value) {
    if(key == NULL || key[0] == '\0') {
        // No key, no double indent. Only a character space
        printf(" %s\n", value);
        return;
    }
    printf("%-16s %s\n", key, value);
}