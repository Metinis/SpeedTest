#include "tests.h"
#include <string.h>

static size_t write_cb(void *ptr, size_t size, size_t nmemb, void *data)
{
  (void)ptr;
  (void)data;
  return size * nmemb;
}

size_t read_cb(char *buffer, size_t size, size_t nitems, void *userdata) {
    size_t max = size * nitems;

    memset(buffer, 'A', max);
    return max;
}



void download_test(CURLcode result, CURL* curl, const char* url) {
  curl_easy_setopt(curl, CURLOPT_URL, url);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_cb);
  curl_easy_setopt(curl, CURLOPT_TIMEOUT, 15L);
  curl_easy_setopt(curl, CURLOPT_USERAGENT,
      "Mozilla/5.0");

  result = curl_easy_perform(curl);

  curl_off_t val;
  result = curl_easy_getinfo(curl, CURLINFO_SPEED_DOWNLOAD_T, &val);

  if ((result == CURLE_OK) && (val > 0)) {
    double mbps = (double)val * 8.0 / 1000000.0;

    printf("Download speed: %.6f Mbps\n", mbps);
  }       
  else {
    printf("Curl error: %s\n", curl_easy_strerror(result));
  }

}

void upload_test(CURLcode result, CURL* curl, const char* url) {
  curl_easy_setopt(curl, CURLOPT_URL, url);
  curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
  curl_easy_setopt(curl, CURLOPT_READFUNCTION, read_cb);
  curl_easy_setopt(curl, CURLOPT_READDATA, NULL);
  curl_easy_setopt(curl, CURLOPT_TIMEOUT, 15L);
  curl_easy_setopt(curl, CURLOPT_USERAGENT,
      "Mozilla/5.0");


  result = curl_easy_perform(curl);

  curl_off_t val;
  result = curl_easy_getinfo(curl, CURLINFO_SPEED_UPLOAD_T, &val);

  if ((result == CURLE_OK) && (val > 0)) {
    double mbps = (double)val * 8.0 / 1000000.0;

    printf("Upload speed: %.6f Mbps\n", mbps);
  }       
  else {
    printf("Curl error: %s\n", curl_easy_strerror(result));
  }

}
