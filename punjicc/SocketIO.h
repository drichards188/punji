#pragma once

using namespace std;

void readLine(int fd, string &line);

void writeLine(int connfd, string message);

bool checkVerb(const string& buffer, const string& verb);