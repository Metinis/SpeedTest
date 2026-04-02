#pragma once
#include <curl/curl.h>
#include <sys/time.h>
#include "common.h"
#include <cJSON.h>

double download_test(CURL* curl, const char* url);
double upload_test(CURL* curl, const char* url);
char* get_location_country(CURL *curl);
Server find_first_working_server(cJSON* json);
double quick_latency(CURL *curl, const char *url);
Server find_best_server(cJSON* json, const char* country, CURL* curl);
