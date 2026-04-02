#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <cJSON.h>
#include <curl/curl.h>
#include "json.h"
#include "tests.h"
#include <stdbool.h>
#include <string.h>


int main(int argc, char **argv) {
  int opt;
  int do_download = 0;
  int do_upload = 0;
  int auto_mode = 0;
  char *selected_host = NULL;
  bool has_valid_option = false;
  double download_speed = -1;
  double upload_speed = -1;

  opterr = 0;

  while ((opt = getopt(argc, argv, "duas:")) != -1) {
    switch (opt) {
      case 'd':
        do_download = 1;
        has_valid_option = true;
        break;
      case 'u':
        do_upload = 1;
        has_valid_option = true;
        break;
      case 'a':
        auto_mode = 1;
        has_valid_option = true;
        break;
      case 's':
        if (optarg == NULL) {
          fprintf(stderr, "Error: -s requires a host argument\n");
          return 1;
        }
        selected_host = optarg;
        has_valid_option = true;
        break;
      case '?':
        if (optopt == 's') {
          fprintf(stderr, "Error: -s requires a host\n");
        } else {
          fprintf(stderr, "Error: unknown option -%c\n", optopt);
        }
        return 1;
      default:
        return 1;
    }
  }
  if (!has_valid_option) {
    fprintf(stderr, "Usage: %s [-d] [-u] [-a] [-s host]\n", argv[0]);
    return 1;
  }
  cJSON* json = get_json("speedtest_server_list.json");
  if (!json) return 1;

  CURLcode result = curl_global_init(CURL_GLOBAL_ALL);
  if (result != CURLE_OK) return (int)result;

  CURL* dl = curl_easy_init();
  CURL* ul = curl_easy_init();
  CURL* ping = curl_easy_init();
  if (!dl || !ul || !ping) {
    printf("Error: failed to init curl\n");
    return 1;
  }

  Server server = {0};
  char *country = get_location_country(ping);
    if (!country) {
      printf("Failed to get location\n");
      return 1;
    }

    printf("Location: %s\n", country);
  if (auto_mode) {
    printf("Running automatic test...\n");

    server = find_best_server(json, country, dl);

    printf("Server: %s (%s, %s)\n",
        server.provider, server.city, server.country);

    do_download = 1;
    do_upload = 1;  }

  if (selected_host != NULL) {
    server.host = selected_host;
    printf("Using custom server: %s\n", selected_host);
  }

  if (!server.host) {
    printf("Error: No server selected\n");
    return 1;
  }

  if (do_download) {
    printf("Starting download test...\n");
    download_speed = download_test(dl, server.host);
  }

  if (do_upload) {
    printf("Starting upload test...\n");
    upload_speed = upload_test(ul, server.host);
  }

  printf("\n=== RESULTS ===\n");
  if (do_download){
    if(download_speed > -1.0f)
      printf("Download: %.6f Mbps\n", download_speed);
    else
      printf("Download: Unavailable\n");
  }
  if (do_upload){
    if(upload_speed > -1.0f)
      printf("Upload: %.6f Mbps\n", upload_speed);
    else
      printf("Upload: Unavailable\n");
  }
  printf("Server: %s\n", server.host);
  printf("User Location: %s", country);

  curl_easy_cleanup(ul);
  curl_easy_cleanup(dl);
  curl_easy_cleanup(ping);
  curl_global_cleanup();
  cJSON_Delete(json);

  return 0;
}
