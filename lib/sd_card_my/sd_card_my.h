#ifndef SD_CARD_MY_H
#define SD_CARD_MY_H

#include <cstdio>

#define SEMA_LOG_FAIL -1
#define DLOG_OK 0
#define SEMA_JSON_FAIL -1
#define JSON_OK 0
#define SD_CARD_READ_FAILED -1

static const char *TAG = "sd_card_my";
#define MOUNT_POINT "/sdcard"

#define dlog_comm_path "/sdcard/datalog.txt"

uint32_t  sd_card_init(void);

int sd_card_dlog_comm(std::string text);
// write communication log into sd card and return result:0 if ok -1 if not

std::string sd_card_json_tostring(std::string jsonpath);
// get json file as a string, throw fail as -1

#endif