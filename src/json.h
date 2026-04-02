#pragma once
#include <curl/curl.h>
#include <cJSON.h>
#include "common.h"

cJSON* get_json(const char* file);
Server parse_server(cJSON* item);
void process_item(cJSON* item, CURLcode result, CURL* curl);


