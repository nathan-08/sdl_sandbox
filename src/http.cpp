#include "http.hpp"
#include <iostream>
#include <curl/curl.h>
#include <string>
#include "json.h"
#include <sstream>
#include "env.hpp"

static int ctof(int c) {
  return (9 * c / 5) + 32;
}

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
  ((std::string*)userp)->append((char*)contents, size * nmemb);
  return size * nmemb;
}

std::string HTTP::get_weather()
{
  CURL *curl;
  CURLcode res;
  std::string readBuffer;

  curl = curl_easy_init();
  if(curl) {
    curl_easy_setopt(curl, CURLOPT_URL, ENV::weather_api);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
    res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    Json::Reader reader;
    Json::Value root;
    if(!reader.parse(readBuffer.c_str(), root)) {
      std::cout << reader.getFormattedErrorMessages() << std::endl;
    }

    std::ostringstream oss;
    oss << root["weather"][0]["main"].asString() << " "
        << ctof( root["main"]["temp"].asInt() )
        << " in " << root["name"].asString() << std::endl;

    std::string output = oss.str();

    return output;
  }
  return "failed";
}
