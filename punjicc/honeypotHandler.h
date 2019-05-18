//
// Created by drich on 6/3/17.
//

#pragma once

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

void readLine(int fd, string& line);

void storeContent(string type, string content);

vector<string> splitOnString(const string& buffer, const string& delimiter);