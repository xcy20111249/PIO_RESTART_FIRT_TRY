#include "esp_log.h"

#include "JSON.h"
#include "sd_card_my.h"

#include "JSONUtility.h"

//declare path to json files
std::string distrib_info_path = "/sdcard/configuration/imeds-distrib.json";

JsonObject read_jsonfile (std::string filename){
    std::string json_cont = sd_card_json_tostring(filename);
    try {
        JsonObject json_info = JSON::parseObject(json_cont);
    }catch(...){
        ESP_LOGE(TAG, "Json file parse failed.");
        throw PARSE_JSONFILE_FAIL;
    }
}

