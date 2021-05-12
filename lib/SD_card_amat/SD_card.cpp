// #include 
#include <string>
#include "esp_err.h"
#include "esp_log.h"
#include "esp_vfs_fat.h"
#include "driver/sdmmc_host.h"
#include "driver/sdspi_host.h"
#include "sdmmc_cmd.h"
//#include "in_flash_data.h"
#include "sd_card.h"


//#include "ble_gatts.h"
#include "ff.h"

//#include "interface_rtc.h"

#include "freertos/task.h"
#include "freertos/FreeRTOS.h"

STU_HIS sensor_data;

static const char *TAG = "sd_card";


#define USE_SPI_MODE


// 定义 SPI 的io
#ifdef USE_SPI_MODE  

// #define PIN_NUM_MISO   19//2 
// #define PIN_NUM_MOSI   23// 15
// #define PIN_NUM_CLK    18//14
// #define PIN_NUM_CS     5//  13

// #define PIN_NUM_MISO   GPIO_NUM_19 
// #define PIN_NUM_MOSI   GPIO_NUM_23
// #define PIN_NUM_CLK    GPIO_NUM_18
// #define PIN_NUM_CS     GPIO_NUM_5
//这里的针脚信息是用来配置已过时的结构体的

#endif 

//declear semaphore for log and json file
QueueHandle_t semaphore_log = NULL;
QueueHandle_t semaphore_json = NULL;

static uint8_t is_init_sd_card_failed = 0; // 初始化sd 卡成


uint32_t sd_card_capacity_MB = 0; // 容量  单位为 ：MB

// sd_card_init 的初始化---------------
uint8_t  sd_card_init(void)
{

    ESP_LOGI(TAG, "Using SPI peripheral");

    sdmmc_host_t host = SDSPI_HOST_DEFAULT();
    sdspi_device_config_t device_config = SDSPI_DEVICE_CONFIG_DEFAULT();

    //根据具体卡的信息分配引脚
    /*device_config.host_id = 
    device_config.gpio_cs = 
    device_config.gpio_cd  = 
    device_config.gpio_wp   = 
    device_config.gpio_int = */

    esp_vfs_fat_sdmmc_mount_config_t mount_config = 
    {
        .format_if_mount_failed = false,
        .max_files = 5,
        .allocation_unit_size = 16 * 1024
    };

    // 初始化 SD卡
    sdmmc_card_t* card;
    esp_err_t ret = esp_vfs_fat_sdmmc_mount("/sdcard", &host, &device_config, &mount_config, &card);

   // 初始化不成功
    if (ret != ESP_OK) 
    {
       is_init_sd_card_failed = 1;
        if (ret == ESP_FAIL) 
        {
            ESP_LOGE(TAG, "Failed to mount filesystem. "
                "If you want the card to be formatted, set format_if_mount_failed = true.");
        } 
        else 
        {
            ESP_LOGE(TAG, "Failed to initialize the card (%s). "
                "Make sure SD card lines have pull-up resistors in place.", esp_err_to_name(ret));
        }
        return ret;
    }
    // Card has been initialized, print its properties  (特性)
    sdmmc_card_print_info(stdout, card);

     // 得到SD卡的最大容量-----转化为MB，后面检查使用
    sd_card_capacity_MB = ((uint64_t) card->csd.capacity) * card->csd.sector_size / (1024 * 1024);
    ESP_LOGE(TAG, "sd capatiy %d",sd_card_capacity_MB);

    semaphore_log = xSemaphoreCreateBinary();
    semaphore_json = xSemaphoreCreateBinary();

    return ret;

}

static void reset_sd_card(void)
{
	uint8_t i;
	 esp_vfs_fat_sdmmc_unmount();	   

	 for(i=0;i>3;i++)
	 {
        vTaskDelay(2000 / portTICK_PERIOD_MS);
		if(sd_card_init())
		{
			break;
		}
	 }
}

int sd_card_dlog(std::string text){
    if(xSemaphoreTake(semaphore_log, 2000 / portTICK_PERIOD_MS)==pdTRUE){
        
        
        return DLOG_OK;
    }else{
        ESP_LOGE(TAG,"Unable to get semaphore_log, data logging failed.");
        return SEMA_LOG_FAIL;
    }
}

// 比较两个数的大小
static uint16_t chg_2_int16(int16_t a,int16_t b)
{

    return a>b ? a-b:b-a;
}

// **检查还剩余的文件大小**
void check_file_size(void)
{
    struct  stat_t st;
    long sd_file_size =0;

    stat("/sdcard/sensor.txt",&st);
    sd_file_size = st.st_size;

    ESP_LOGE(TAG, "sd_file_size=%ld ------,sd_card_capacity= %d",sd_file_size,sd_card_capacity_MB);
     // 已经使用的sd卡容量超过，就删除
    if(st.st_size>=((sd_card_capacity_MB-2)*1024*1024))
    {
        ESP_LOGE(TAG, "remove sd file");
        remove("/sdcard/sensor.txt");
    }
}

// 
static uint8_t sd_save_data[sizeof(sensor_data)] ;
void get_sensor_data(void)
{
    sd_save_data[len++]=sensor_data.tem_hum[0];
    sd_save_data[len++]=sensor_data.tem_hum[1];
    sd_save_data[len++]=sensor_data.tem_hum[2];
    sd_save_data[len++]=sensor_data.tem_hum[3];

   // c02	
    sd_save_data[len++]=sensor_data.CO2_valu[0];
    sd_save_data[len++]=sensor_data.CO2_valu[1] ;

   // PM2.5
    sd_save_data[len++]=sensor_data.PM_25[0];
    sd_save_data[len++]=sensor_data.PM_25[1];

    sd_save_data[len++]=sensor_data.PM_25[0];
    sd_save_data[len++]=sensor_data.PM_25[1];
	
    sd_save_data[len++]=sensor_data.Li3DH_valu[0];
    sd_save_data[len++]=sensor_data.Li3DH_valu[1];
    sd_save_data[len++]=sensor_data.Li3DH_valu[2];

}


static uint8_t sd_card_reading =0; //  正在读的标志
//write sd card data-----------------------------------
void task_write_sd_card_data(void)
{

    int16_t timemin;
    static int16_t timemin_b=0;

    timemin=sensor_data.UTCTime;
    
    // step 1:  write time interval，写入的时间间隔，
   // 这边是定时写入
    if(chg_2_int16(timemin,timemin_b)>=user_data.save_sensor_minutes)
    {
        timemin_b=timemin;
    }
    else
    {
        return;
    }
    
   // step 2: 检测是否真正在读取，或初始化不成功就不进行写入
   if((sd_card_reading==1)||(is_init_sd_card_failed==1)) return;

   // step 3:  ׁ得到传感器的数据-----
   get_sensor_data();
  
    // step 4: 检查SD 卡的剩余的大小
    check_file_size();

    // step 5:打开sd 卡
    FILE* pfile = fopen("/sdcard/sensor.txt", "ab+"); // 
    if (pfile == NULL)   // 
    {
        ESP_LOGE(TAG, "Failed to open file for writing");
        reset_sd_card();
    }

    //进行写入数据
    fwrite(sd_save_data,sizeof(sensor_data),1,pfile);

    // 关闭sd卡
    fclose(pfile);
    ESP_LOGI(TAG, "sd write sensor data time_interv_s=%d",user_data.save_sensor_minutes);
 
}

 uint8_t  read_buffer[sizeof(sensor_data)];// 
void task_read_sd_card_data(void)
{
    uint32_t read_len =0;

    sd_card_reading =1; 
    memset(read_buffer,0,sizeof(read_buffer));

    FILE* pfile = fopen("/sdcard/sensor.txt", "rb");
    if (pfile == NULL) 
    {
		ESP_LOGE(TAG, "Failed to open file for writing");
		reset_sd_card();
    }

   while(!feof(pfile))
   {
        read_len = fread(read_buffer, 1, sizeof(sensor_data) , pfile);
   }
   sd_card_reading =0;   
  fclose(pfile);
}