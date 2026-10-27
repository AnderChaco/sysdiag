/* SPDX-License-Identifier: MIT */

#ifndef STYLE_H
#define STYLE_H

#include "checks.h"
#include "colors.h"

// Print [OK], [WARN], [FAIL] in color
void print_status_label(check_status st);

// Print a title for the section (ie.: "Memory")
void print_section_title(const char *title);

// Print a indent key-value pair
void print_key_val(const char *key, const char *value);

#endif

