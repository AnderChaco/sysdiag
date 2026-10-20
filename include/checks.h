#ifndef CHECKS_H
#define CHECKS_H

#include <stddef.h>   // ← define size_t

typedef enum {
    CHECK_OK,
    CHECK_WARN,
    CHECK_FAIL
} check_status;

typedef struct {
    const char *name;
    check_status (*func)(char *msg, size_t msg_size);
} check_t;

// All CHECKS declarations here 
check_status check_memory(char *msg, size_t msg_size); 
check_status check_cpu(char *msg, size_t msg_size);
check_status check_disk(char *msg, size_t msg_size);
check_status check_network(char *msg, size_t msg_size);

extern check_t checks[];

void run_all_checks(void);

#endif

