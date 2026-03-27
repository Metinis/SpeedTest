#pragma once
#include <curl/curl.h>
#include <cJSON.h>

cJSON* get_json(const char* file);

void process_item(cJSON* item, CURLcode result, CURL* curl);


