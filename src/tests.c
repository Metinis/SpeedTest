#include "tests.h"
#include <string.h>
#include <cJSON.h>
#include "json.h"

size_t write_cb_speed(void *ptr, size_t size, size_t nmemb, void *data) {
  (void)ptr;
  (void)data;
  return size * nmemb;
}

size_t write_cb_country(void *ptr, size_t size, size_t nmemb, void *data) {
  size_t total = size * nmemb;
  strncat((char *)data, (char *)ptr, total);
  return total;
}
size_t read_cb(char *buffer, size_t size, size_t nitems, void *userdata) {
  size_t max = size * nitems;

  memset(buffer, 'A', max);
  return max;
}



double download_test(CURL* curl, const char* url) {
  printf("Running download test for: %s ..\n", url);
  CURLcode res;

  curl_easy_reset(curl);

  curl_easy_setopt(curl, CURLOPT_URL, url);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_cb_speed);
  curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0");
  curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
  curl_easy_setopt(curl, CURLOPT_TIMEOUT, 15L);
  curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
  curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
  curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

  res = curl_easy_perform(curl);

  if (res != CURLE_OK) {
    return -1;
  }

  double total_time = 0.0;
  curl_off_t total_bytes = 0;

  curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &total_time);
  curl_easy_getinfo(curl, CURLINFO_SIZE_DOWNLOAD_T, &total_bytes);

  if (total_time > 0.0) {
    double mbps = (total_bytes * 8.0) / (total_time * 1000000.0);
    return mbps;
  } else {
    return -1;
  }

}

double upload_test(CURL* curl, const char* url) {
  curl_easy_reset(curl);

  printf("Running upload test for: %s ..\n", url);

  static const size_t upload_size = 1024 * 1024; // 1MB

  curl_easy_setopt(curl, CURLOPT_URL, url);
  curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

  curl_easy_setopt(curl, CURLOPT_READFUNCTION, read_cb);
  curl_easy_setopt(curl, CURLOPT_READDATA, &upload_size);

  curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, (curl_off_t)upload_size);

  curl_easy_setopt(curl, CURLOPT_TIMEOUT, 20L);
  curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0");

  CURLcode res = curl_easy_perform(curl);

  if (res != CURLE_OK) {
    printf("Upload error: %s\n", curl_easy_strerror(res));
    return -1;
  }

  double total_time = 0.0;
  curl_off_t bytes_sent = 0;

  curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &total_time);
  curl_easy_getinfo(curl, CURLINFO_SIZE_UPLOAD_T, &bytes_sent);

  if (total_time <= 0.0)
    return -1;

  return (bytes_sent * 8.0) / (total_time * 1000000.0);
}
char* get_location_country(CURL *curl) {
  printf("Getting user location\n");
  static char buffer[4096] = {0};
  buffer[0] = '\0';

  curl_easy_reset(curl);

  curl_easy_setopt(curl, CURLOPT_URL, "http://ip-api.com/json");
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_cb_country);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, buffer);
  curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);
  curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0");

  CURLcode res = curl_easy_perform(curl);
  if (res != CURLE_OK) {
    printf("Location curl error: %s\n", curl_easy_strerror(res));
    return NULL;
  }

  if (strlen(buffer) == 0) {
    printf("Error: empty response\n");
    return NULL;
  }

  cJSON *json = cJSON_Parse(buffer);
  if (!json) {
    printf("JSON parse failed\n");
    return NULL;
  }

  cJSON *country = cJSON_GetObjectItem(json, "country");

  char *result = NULL;
  if (cJSON_IsString(country)) {
    result = strdup(country->valuestring);
  }
  printf("User Location: %s\n", result);

  cJSON_Delete(json);
  return result;
}
Server find_first_working_server(cJSON* json) {
  cJSON *item = NULL;

  cJSON_ArrayForEach(item, json) {
    Server s = parse_server(item);
    if (s.host != NULL) {
      return s;        }
  }

  Server empty = {0};
  return empty;
}
double quick_latency(CURL *curl, const char *url) {
  curl_easy_reset(curl);
  printf("Running latency test for: %s ..\n", url);
  double time = 0.0;

  curl_easy_reset(curl);
  curl_easy_setopt(curl, CURLOPT_URL, url);
  curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);
  curl_easy_setopt(curl, CURLOPT_TIMEOUT,3L);
  curl_easy_setopt(curl, CURLOPT_CONNECT_ONLY, 0L);

  if (curl_easy_perform(curl) != CURLE_OK)
    return -1;

  curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &time);

  return time;
}
Server find_best_server(cJSON* json, const char* country, CURL* curl) {
  cJSON *item = NULL;

  Server best_server;
  int found = 0;
  double best_time = INT_MAX;

  cJSON_ArrayForEach(item, json) {
    Server s = parse_server(item);

    if (!s.country || strcmp(s.country, country) != 0)
      continue;

    double time = quick_latency(curl, s.host);


    if (!found || (time < best_time && time > 0)) {
      best_time = time;
      best_server = s;
      found = 1;
    }
  }

  if (found)
    return best_server;

  return find_first_working_server(json);
}

