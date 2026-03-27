#pragma once
#include <curl/curl.h>
#include <sys/time.h>

void download_test(CURLcode result, CURL* curl, const char* url);
void upload_test(CURLcode result, CURL* curl, const char* url);
