#ifndef HTTP_H_INCLUDED
#define HTTP_H_INCLUDED

int recvtimeout(int s, int timeout);
char *InitHTTP(char *address, int port, char *caminho, char *cookie);

#endif // HTTP_H_INCLUDED
