#include "json.h"
#include <stdio.h>

cJSON* get_json(const char* file) {
  FILE *fp = fopen(file, "r");
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

Server parse_server(cJSON* item) {
    Server s = {0};

    cJSON *host = cJSON_GetObjectItem(item, "host");
    cJSON *country = cJSON_GetObjectItem(item, "country");
    cJSON *city = cJSON_GetObjectItem(item, "city");
    cJSON *provider = cJSON_GetObjectItem(item, "provider");

    if (cJSON_IsString(host)) s.host = host->valuestring;
    if (cJSON_IsString(country)) s.country = country->valuestring;
    if (cJSON_IsString(city)) s.city = city->valuestring;
    if (cJSON_IsString(provider)) s.provider = provider->valuestring;

    return s;
}

