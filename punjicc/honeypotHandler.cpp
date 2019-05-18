//
// Created by drich on 6/3/17.
//
#include <iostream>
#include <fstream>
#include <proxygen/httpserver/ResponseBuilder.h>
#include <folly/dynamic.h>
#include <folly/json.h>
#include <regex>
#include <GeoIP.h>
#include <GeoIPCity.h>
#include <cstdint>
#include <iostream>
#include <vector>
#include <netinet/in.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <curl/curl.h>
#include <folly/dynamic.h>

#include "honeypotHandler.h"
#include "SocketIO.h"
#include "Log.h"

using folly::dynamic;

extern bool STOREDATA;

int handleConnection(int connection, string clientAddress) {
    curl_global_init(CURL_GLOBAL_ALL);
    Log::info("handleConnection", "connection received, thread spun");
    string geoipFile = "/var/log/punji/netLocation.log";
    string contentpath = "/home/ubuntu/punji/emailContent.vir";

    bool isData = false;
    if (clientAddress != "127.0.0.1") {
        Log::info("handleConnection", "calling ipLocation with "+clientAddress);
    }

    const std::string message = "Watson!!!";
    writeLine(connection, message);

    while (true) {

        std::string data;
        std::string line;
        std::string dataType;
        std::string content;
        readLine(connection, line);
        Log::debug("TEST", line);

        cout << line << endl;

        if (line != "") {
            try {
                dynamic jsonObj = folly::parseJson(line);
                auto pos = jsonObj.find("dataType");
                if (pos != jsonObj.items().end()) {
                    dataType = jsonObj["dataType"].getString();
                }

                pos = jsonObj.find("content");
                if (pos != jsonObj.items().end()) {
                    content = jsonObj["content"].getString();
                } else {
                    return false;
                }
            } catch (const std::exception& e) {
                Log::warn("readline", "caught std::exception: "+string(e.what()));
            }

            storeContent(dataType, content);
        }
        Log::info("TEST", line);
            line.clear();
        return 0;
    }
}

static const char *_mk_NA(const char *p) {
    return p ? p : "N/A";
}

void storeContent(string type, string content) {
    CURL *curlHandle = curl_easy_init();

    if (curlHandle) {
        curl_easy_setopt(curlHandle, CURLOPT_URL, "http://localhost:9200/punji/log");

        struct curl_slist *list = nullptr;
        list = curl_slist_append(list, "Content-Type: application/json");
        list = curl_slist_append(list, "Authorization: Basic ZWxhc3RpYzpGcmVjazFlcw==");
        curl_easy_setopt(curlHandle, CURLOPT_HTTPHEADER, list);

        time_t timev = time(nullptr);
        type = "location";

        vector<string> geoipSplit = splitOnString(content, "\t");

        dynamic curlObject = dynamic::object;

        curlObject["epoch"] = timev;
//        curlObject["dataType"] = type;
//        curlObject["ip"] = geoipSplit[0];
//        curlObject["country"] = geoipSplit[1];
//        curlObject["city"] = geoipSplit[4];
        curlObject["location"] = geoipSplit[6]+","+geoipSplit[7];
//        curlObject["lon"] = geoipSplit[7];

        string jsonString = folly::toJson(curlObject);
        Log::debug("DataStore curl message", jsonString);
        curl_easy_setopt(curlHandle, CURLOPT_POSTFIELDS, jsonString.c_str());

        CURLcode res = curl_easy_perform(curlHandle);

        if (res != CURLE_OK) {
            Log::debug("DataStore", "curl_easy_perform() failed: " + string(curl_easy_strerror(res)));
        }

        curl_easy_cleanup(curlHandle);
    }
}

vector<string> splitOnString(const string& buffer, const string& delimiter) {

    string::size_type startPos = 0;
    string::size_type stopPos = 0;
    vector<string> tokens;
    while ((stopPos = buffer.find(delimiter, startPos)) != string::npos) {
        if (stopPos != startPos) {
            tokens.push_back(buffer.substr(startPos, stopPos - startPos));
        }
        startPos = buffer.find_first_not_of(delimiter, stopPos);
    }

    if (startPos < buffer.length()) {
        tokens.push_back(buffer.substr(startPos));
    }

    return tokens;

}