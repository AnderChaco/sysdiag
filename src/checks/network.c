/* SPDX-License-Identifier: MIT */

#include <netinet/in.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <ifaddrs.h>
#include <arpa/inet.h>
#include "checks.h"

//Read simple file (such as /sys/class/net/eth0/operstate)
static int read_file(const char *path, char *buf, size_t size) {
    FILE *f = fopen(path, "r");
    
    if(!f) 
        return 0;
    if(!fgets(buf, size, f)) {
        fclose(f);
        return 0;
    }

    fclose(f);
    buf[strcspn(buf, "\n")] = '\0';
    
    return 1;
}

//Obtain IP from /proc/net/fib_trie (simple method)
//
static int get_ip_address(const char *iface, char *ip, size_t size) {
    FILE *f = fopen("/proc/net/fib_trie", "r");
    if(!f) return 0;

    char line[256];
    int found_iface = 0;

    while (fgets(line, sizeof(line), f)) {
        if (strstr(line, iface)) {
            found_iface = 1;
        }
        if (found_iface && strstr(line, "32 host")) {
            if (fgets(line, sizeof(size), f)) {
                char *p = strstr(line, "local");
                if (p) {
                    p += 6;
                    strncpy(ip, p, size);
                    ip[strcspn(ip, "\n")] = '\0';
                    fclose(f);
                    return 1;
                }
            }
        }
    }
    fclose(f);
    return 0;
}

//Obtain IP using getifaddrs()
//
static void get_ipv4(const char *iface, char *ip, size_t size) {
    struct ifaddrs *ifaddr, *ifa;

    if (getifaddrs(&ifaddr) == -1) {
        snprintf(ip, size, "Unknown");
        return;
    }

    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (!ifa->ifa_addr) continue;

        if (ifa->ifa_addr->sa_family == AF_INET && strcmp(ifa->ifa_name, iface) == 0) {
            struct sockaddr_in *sa = (struct sockaddr_in *)ifa->ifa_addr;
            inet_ntop(AF_INET, &(sa->sin_addr), ip, size);
            freeifaddrs(ifaddr);
            return;
        }
    }

    freeifaddrs(ifaddr);
    snprintf(ip, size, "Unknown");
}

check_status check_network(char *msg, size_t msg_size) {
    DIR *d = opendir("/sys/class/net");
    if(!d) {
        snprintf(msg, msg_size, "Could not read /sys/class/net\n");
        return CHECK_FAIL;
    }

    struct dirent *de;
    char iface[64] = {0};
    char path[512];
    char buf[128];
    
    //Search for active interface
    //
    while ((de = readdir(d)) != NULL) {
        if (strcmp(de->d_name, "lo") == 0) continue;  //ignore loopback...
                                                      
        snprintf(path, sizeof(path), "/sys/class/net/%s/operstate", de->d_name);
        
        if (!read_file(path, buf, sizeof(buf))) continue;

        if (strcmp(buf, "up") == 0) {
            strncpy(iface, de->d_name, sizeof(iface));
            break;
        }
    }
    closedir(d);

    if (iface[0] == '\0') {
        snprintf(msg, msg_size, "No active network interface found\n");
        return CHECK_FAIL;
    }

    //get the network speed
    //
    snprintf(path, sizeof(path), "/sys/class/net/%s/speed", iface);
    char speed[64] = "N/A (Wireless Interface)";
    
    if (read_file(path, buf, sizeof(buf))) {
        snprintf(speed, sizeof(speed), "%s Mb/s", buf);
    }

    //get the ip address...
    //
    char ip[64] = "Unknown";
    get_ipv4(iface, ip, sizeof(ip));

    //get traffic stats
    //
    unsigned long long rx = 0, tx = 0;
    FILE *f = fopen("/proc/net/dev", "r");

    if (f) {
        char line[256];
        while (fgets(line, sizeof(line), f)) {
            if (strstr(line, iface)) {
                sscanf(line, "%*[^:]: %llu %*u %*u %*u %*u %*u %*u %*u %llu", &rx, &tx);
                break;
            }
        }
        fclose(f);
    }

    // build message
    //
    snprintf(msg, msg_size, 
            "Interface: %s\n"
            "IP Address: %s\n"
            "Speed: %s\n"
            "RX Bytes: %llu\n"
            "TX Bytes: %llu\n",
            iface,
            ip,
            speed,
            rx,
            tx
    );

    return CHECK_OK;
}

