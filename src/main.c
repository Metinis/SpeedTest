#include <stdio.h>
#include <cJSON.h>
#include <curl/curl.h>
#include <sys/time.h>
#include "json.h"


int main() {
  printf("Hello World!\n");
  cJSON* json = get_json("speedtest_server_list.json");
  if(json == NULL){
    return 1;
  }
    /* init libcurl */
  CURLcode result = curl_global_init(CURL_GLOBAL_ALL);
  if(result != CURLE_OK)
    return (int)result;
  CURL* curl;
  curl = curl_easy_init();
  if(!curl) {
    printf("Error: failed to init curl\n");
    return 1;
  }

  cJSON *item = NULL;
  cJSON_ArrayForEach(item, json) {process_item(item, result, curl);};

  curl_easy_cleanup(curl);
  curl_global_cleanup();
  cJSON_Delete(json);
  return 0;
}
