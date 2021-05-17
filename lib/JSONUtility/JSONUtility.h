#ifndef JSONUTILITY_H_
#define JSONUTILITY_H_

#define PARSE_JSONFILE_FAIL -1
static const char *TAG = "JSONUtility";

JsonObject read_jsonfile (std::string filename);
// Read jsonfile and return an JsonObject, throw fail as -1

#endif