//
// Created by drich on 6/3/17.
//

#pragma once

#include <folly/Memory.h>
#include <proxygen/httpserver/RequestHandler.h>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/instance.hpp>
#include <pcap.h>
#include <netinet/if_ether.h>
#include <iostream>
#include <fstream>

using namespace std;

void got_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *packet);

void store_type(std::string packetData);

void manipulate_ipv4(const u_char * packet);

int handleConnection(int connection, string clientAddress);

void writeLine(int connfd, string message);

void logEvent(string filePath, string message);

void ipLocation(const char *ipAddr, string filePath);

void logLocation(string filePath, string location);

bool checkVerb(const string& buffer, const string& verb);

string pullVerb(int fd);

string readLine(int fd);