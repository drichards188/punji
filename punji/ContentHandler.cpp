//
// Created by drich on 6/3/17.
//

#include "ContentHandler.h"
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
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/instance.hpp>
#include <pcap.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <sys/socket.h>
#include "IpDef.h"
#include <sstream>
#include <chrono>
#include <iomanip>
#include "SocketIO.h"
#include <iostream>
#include <fstream>

using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::open_document;
using namespace std;

int handleConnection(int connection, string clientAddress) {
    string fileLocation = "netTraffic.txt";
    string geoipFile = "netLocation.txt";
    if (clientAddress != "127.0.0.1") {
        ipLocation(clientAddress.c_str(), geoipFile);
    }
//    logLocation(geoipFile, firstLocation);

    printf("connection received, thread spun\n");

    const std::string message = "220 WATCH YOUR FOOTING\n";
    write(connection, message.c_str(), message.size());

    while (true) {
        string line = readLine(connection);
        printf(line.c_str());
        if (checkVerb(line, "QUIT")) {
            logEvent(fileLocation, line);
            writeLine(connection, "221 OK CLOSING CONNECTION");
            logEvent(fileLocation, "221 OK CLOSING CONNECTION");
            close(connection);
            return 0;
        } else if (checkVerb(line, "EHLO")) {
            logEvent(fileLocation, line);
            writeLine(connection, "EHLO DANGERZONE.LANA");
            logEvent(fileLocation, "EHLO DANGERZONE.LANA");
            line.clear();
        } else if (checkVerb(line, "HELO")) {
            logEvent(fileLocation, line);
            writeLine(connection, "211 SYSTEM SUPPORTS EHLO");
            logEvent(fileLocation, "211 SYSTEM SUPPORTS EHLO");
            line.clear();
        } else if (checkVerb(line, "MAIL")) {
            logEvent(fileLocation, line);
            writeLine(connection, "250 OK");
            logEvent(fileLocation, "250 OK");
            line.clear();
        } else if (checkVerb(line, "RCPT")) {
            logEvent(fileLocation, line);
            writeLine(connection, "250 OK");
            logEvent(fileLocation, "250 OK");
            line.clear();
        }  else if (checkVerb(line, "DATA")) {
            logEvent(fileLocation, line);
            writeLine(connection, "353 START MAIL INPUT; END WITH <CRLF>.<CRLF>");
            logEvent(fileLocation, "250 OK");
            line.clear();
        } else if (checkVerb(line, "RSET")) {
            logEvent(fileLocation, line);
            writeLine(connection, "250 OK");
            logEvent(fileLocation, "250 OK");
            line.clear();
        } else if (checkVerb(line, "TURN")) {
            logEvent(fileLocation, line);
            writeLine(connection, "250 OK");
            logEvent(fileLocation, "250 OK");
            line.clear();
        } else if (checkVerb(line, "VRFY")) {
            logEvent(fileLocation, line);
            writeLine(connection, "250 OK");
            logEvent(fileLocation, "250 OK");
            line.clear();
        } else if (checkVerb(line, "HELP")) {
            logEvent(fileLocation, line);
            writeLine(connection, "250 OK");
            logEvent(fileLocation, "250 OK");
            line.clear();
        } else if (checkVerb(line, "NIGHTLOCK")) {
            logEvent(fileLocation, line);
            writeLine(connection, "KABOOM");
            logEvent(fileLocation, "KABOOM");
            return 1;
        }
    }
}

void writeLine(int connfd, string message) {
    message = message + "\r\n";
    write(connfd, message.c_str(), message.size());
}

void logEvent(string filePath, string message) {
    ofstream netMonitor;
    netMonitor.open(filePath, ios_base::app);
    netMonitor << message << endl;
}

void logLocation(string filePath, string location) {
    ofstream netMonitor;
    netMonitor.open(filePath, ios_base::app);
    netMonitor << location << endl;
}

static const char *_mk_NA(const char *p) {
    return p ? p : "N/A";
}

bool checkVerb(const string& buffer, const string& verb) {
    string temp = buffer;
    transform(temp.begin(), temp.end(), temp.begin(), ::toupper);

    return (temp.find(verb) == 0);

}

string readLine(int fd) {
    if (fd == -1) {
        return "read error1";
    }
    constexpr static size_t bufSize = 1024000;
    char buf[bufSize] = {0};
    char* buffer = buf;

    size_t totRead = 0; // Total bytes read so far
    ssize_t numRead = 0;                    // # of bytes fetched by last read()
    char ch = 0;

    for (; ;) {
        numRead = read(fd, &ch, 1);

        if (numRead == -1) {
            if (errno == EINTR) {         // Interrupted --> restart read()
                continue;
            } else {
                return "read error1";              // Some other error
            }

        } else if (numRead == 0) {      // EOF
            if (totRead == 0) {          // No bytes read; return 0
                return "read error1";
            } else {                        // Some bytes read; add '\0'
                break;
            }

        } else {                        // 'numRead' must be 1 if we get here
            if (totRead < bufSize - 1) {      // Discard > (n - 1) bytes
                totRead++;
                *buffer++ = ch;
            }

            if (ch == '\n') {
                break;
            }
        }
    }

    *buffer = '\0';
    return buf;
}

void ipLocation(const char *ipAddr, string filePath) {
            GeoIP *gi;
            GeoIPRecord *gir;
            char host[50] = {0};
            const char *time_zone = NULL;
            char **ret;

            gi = GeoIP_open("GeoLiteCity.dat", GEOIP_INDEX_CACHE);
            if (gi == NULL) {
                fprintf(stderr, "Error opening database\n");
                exit(1);
            }

            gir = GeoIP_record_by_name(gi, (const char *) ipAddr);

            if (gir != NULL) {
                ret = GeoIP_range_by_ip(gi, (const char *) ipAddr);
                time_zone =
                        GeoIP_time_zone_by_country_and_region(gir->country_code,
                                                              gir->region);

                ofstream netMonitor;
                netMonitor.open(filePath, ios_base::app);
                char scratch[4096] = {0};
                sprintf(scratch, "%s\t%s\t%s\t%s\t%s\t%s\t%f\t%f\t%d\t%d\t%s\t%s\t%s\n", host,
                       _mk_NA(gir->country_code), _mk_NA(gir->region),
                       _mk_NA(GeoIP_region_name_by_code
                                      (gir->country_code,
                                       gir->region)), _mk_NA(gir->city),
                       _mk_NA(gir->postal_code), gir->latitude, gir->longitude,
                       gir->metro_code, gir->area_code, _mk_NA(time_zone), ret[0],
                       ret[1]);
                netMonitor << scratch;
                GeoIP_range_by_ip_delete(ret);
                GeoIPRecord_delete(gir);
            }
            GeoIP_delete(gi);
}