#ifndef sd_card_h_
#define sd_card_h_

#define SEMA_LOG_FAIL 1
#define DLOG_OK 0

// 传感器数据结构体，要存数据
typedef struct
{
    uint8_t tem_hum[4];
    uint8_t CO2_valu[2];
    uint8_t PM_25[2];	
    uint8_t VOC_valu[2];
    uint8_t Li3DH_valu[6];
	
    uint8_t GPS_valu[16];
	
    uint8_t CO[3];
    uint8_t NO2[3];
    uint8_t SO2[3];
    uint8_t Ozone[3];
	
} STU_HIS;

extern STU_HIS sensor_data;


uint8_t  sd_card_init(void);
void task_write_sd_card_data(void);
void task_read_sd_card_data(void);

#endif