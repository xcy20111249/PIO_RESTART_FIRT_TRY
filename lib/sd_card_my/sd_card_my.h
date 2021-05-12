#ifndef SD_CARD_MY_H
#define SD_CARD_MY_H

#include <cstdio>

#define SEMA_LOG_FAIL -1
#define DLOG_OK 0
#define SEMA_JSON_FAIL -1
#define JSON_OK 0



#define dlog_comm_path "/sdcard/datalog.txt"

uint32_t  sd_card_init(void);
int sd_card_dlog_comm(std::string text);

#endif