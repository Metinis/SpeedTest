#include "json.h"
#include <stdio.h>
#include "download.h"

cJSON* get_json(const char* file) {
  FILE *fp = fopen("speedtest_server_list.json", "r");
  if(fp == NULL) {
    printf("Error: unable to open file \n");
    return NULL;
  }

  char buf[1048576]; //1mb
  int len = fread(buf, 1, sizeof(buf), fp);
  fclose(fp);

  cJSON *json = cJSON_Parse(buf);
  if(json == NULL) {
    const char* err = cJSON_GetErrorPtr();
    if(err != NULL) {
      printf("Error: %s\n", err);
    }
    cJSON_Delete(json);
    return NULL;
  }

  if(!cJSON_IsArray(json)){
    printf("Error: JSON not an array!\n");
    cJSON_Delete(json);
    return NULL;
  }
  return json;
}

void process_item(cJSON* item, CURLcode result, CURL* curl) {
    cJSON *country = cJSON_GetObjectItem(item, "country");
    cJSON *city = cJSON_GetObjectItem(item, "city");
    cJSON *provider = cJSON_GetObjectItem(item, "provider");
    cJSON *host = cJSON_GetObjectItem(item, "host");
    cJSON *id = cJSON_GetObjectItem(item, "id");

    if(country->valuestring != NULL) {
      printf("Country: %s\n", country->valuestring);
    }
    if(city->valuestring != NULL) {
      printf("City : %s\n", city->valuestring);
    }
    if(provider->valuestring != NULL) {
      printf("Provider: %s\n", provider->valuestring);
    }
    if (host->valuestring != NULL) {
      printf("Host: %s\n",host->valuestring);
      download_test(result, curl, host->valuestring);
    }     
    if(id->valuestring != NULL) {
      printf("ID: %s\n", id->valuestring);
    }

}
