#pragma once

#include <winsock.h>
#pragma comment(lib, "ws2_32.lib")


struct hostent * WINAPI New_gethostbyname(const char * name);
int PASCAL New_connect (SOCKET s, const struct sockaddr *name, int namelen);



static struct hostent * (WINAPI * OLD_gethostbyname)(const char * name) = gethostbyname;
static int (WINAPI * OLD_connect)(SOCKET s, const struct sockaddr *name, int namelen) = connect;

