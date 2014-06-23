/*
	Este arquivo faz parte do BCM Revox Engine;

	BCM Revox Engine é Software Livre; você pode redistribui-lo e/ou
	modificá-lo dentro dos termos da Licença Pública Geru GNU como
	publicada pela Fundação do Software Livre (FSF); na versão 3 da Licença.
	Este programa é distribuído na esperança que possa ser util,
	mas SEM NENHUMA GARANTIA; sem uma garantia implicita de ADEQUAÇÂO a
	qualquer MERCADO ou APLICAÇÃO EM PARTICULAR. Veja a Licença Pública Geral
	GNU para maiores detalhes.
	Você deve ter recebido uma cópia da Licença Pública Geral GNU junto com
	este programa, se não, escreva para a Fundação do Software Livre(FSF) Inc.,
	51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

	BCM Revox Engine v0.2
	BCM Revox Engine -> Ano: 2013, 2014|Tipo: WebEngine
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/time.h>
#include "bonus.h"
#include "http.h"
#include "manifest.h"
#include "socks.h"
#include "strs.h"

unsigned long mode = 1;
char server_reply[BUF32KB];

int recvtimeout(SOCKET s, int timeout){
    int tamanho, total = 0;
    struct timeval beg, now;
    double timediff;
    char temp[BUFKB];

    ioctlsocket(s, FIONBIO, &mode);
    gettimeofday(&beg, NULL);
    limpaVetor(temp);
    while(1){
        gettimeofday(&now, NULL);
        timediff = (now.tv_sec - beg.tv_sec)+1e-6*(now.tv_usec - beg.tv_usec);
        if(total>0 && timediff>timeout){
            break;
        }
        else if(timediff>timeout*2){
            break;
        }
        memset(temp, 0, BUFKB);
        if((tamanho = recv(s, temp, BUFKB, 0))<0){
            usleep(10000);
        }
        else{
            total += tamanho;
            gettimeofday(&beg, NULL);
            strcat(server_reply, temp);
        }
    }

    return total;
}

char *InitHTTP(char address[], int port, char caminho[], char cookie[]){
    struct sockaddr_in Server;
    SOCKET sock;
    char buffer[1024], buf2[512], ip[16], *aux = NULL, len[256];
    int recv_size, i, j;
    struct hostent *he;
    struct in_addr **addr_list;
    WSADATA wsa;

    //Inicialização
    //InitSock();
    if (WSAStartup(MAKEWORD(2,2),&wsa) != 0){
        fprintf(stderr, "Falhou. Codigo de Erro: %d\n", WSAGetLastError());
        return 0;
    }
    //Criar o socket IPv4
	if((sock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
		fprintf(stderr, "Nao pode criar o socket: %d\n", WSAGetLastError());
		return NULL;
	}
    printf("BCM Revox Engine v0.2 - Cliente HTTP\nAcessando %s...\n", address);

    //Estruturas do IPv4
    struct sockaddr_in {
        short            sin_family;   // e.g. AF_INET, AF_INET6
        unsigned short   sin_port;     // e.g. htons(3490)
        struct in_addr   sin_addr;     // see struct in_addr, below
        char             sin_zero[8];  // zero this if you want to
    };
    struct sockaddr {
        unsigned short    sa_family;    // address family, AF_xxx
        char              sa_data[14];  // 14 bytes of protocol address
    };
    struct hostent{
        char *h_name;         /* Official name of host.  */
        char **h_aliases;     /* Alias list.  */
        int h_addrtype;       /* Host address type.  */
        int h_length;         /* Length of address.  */
        char **h_addr_list;       /* List of addresses from name server.  */
    };

    if((he = gethostbyname(address)) == NULL){
        //gethostbyname failed
        printf("gethostbyname failed: %d" , WSAGetLastError());
        return NULL;
    }
    //Cast the h_addr_list to in_addr , since h_addr_list also has the ip address in long format only
    addr_list = (struct in_addr **) he->h_addr_list;

    for(i=0; addr_list[i]!=NULL; i++){
        //Return the first one;
        strcpy(ip, inet_ntoa(*addr_list[i]));
    }

    //Server
    Server.sin_addr.s_addr = inet_addr(ip);
    Server.sin_family = AF_INET;
    Server.sin_port = htons(port);

    //Connect to remote server
    if (connect(sock, (SOCKADDR *)&Server, sizeof(Server)) < 0){
        puts("Erro ao Conectar!");
        return "\0";
    }
    puts("Connectado.");
    //Send some data
    sprintf(buffer, "GET %s HTTP/1.1\r\n", caminho);
    sprintf(buf2, "Host: %s\r\n", address);
    strcat(buffer, buf2);
    strcat(buffer, "Connection: keep-alive\r\n");
    sprintf(buf2, "User-Agent: %s (Windows NT 6.1; %s)\r\n", RVXVERSION, RVXVERSIONFULL);
    strcat(buffer, buf2);
    strcat(buffer, "Accept-Language: pt-br,pt;q=0.7;en-us,en;q=0.5\r\n");
    strcat(buffer, "Accept-Charset: ISO-8859-1,UTF-8;q=0.7,*;q=0.7\r\n");
    strcat(buffer, "Cache-Control: no-cache\r\n");
    strcat(buffer, "Keep-Alive: 300\r\n"); //Ativar somente quando Connection for keeep-alive
    if(cookie != NULL){
        strcat(buffer, "Cookie: ");
        strcat(buffer, cookie);
        strcat(buffer, ";\r\n");
    }
    strcat(buffer, "\r\n");
    if(send(sock, buffer, strlen(buffer), 0)<0){
        puts("Falhou ao enviar");
        return NULL;
    }
    puts("Dados enviados");

    limpaVetor(server_reply);

    //Receive a reply from the server
    if((recv_size = recv(sock, server_reply, BUF32KB, 0))<INVALID_SOCKET){
        puts("recv falhou");
    }
    printf("Resposta recebida\nBytes recebidos: %d\n", recv_size);

    for(i=0; server_reply[i]!='\0';){
        if(aux!=NULL){
            limpaVetor(aux);
            aux = NULL;
        }
        aux = CopyManifst(server_reply, &i);
        if(SearchString(aux, "Content-Length:")>-1){
            for(i=i, j=0; server_reply[i]!='\r' && server_reply[i]!='\n'; i++, j++){
                len[j] = server_reply[i];
            }
            break;
        }
    }
    if(recv_size<(atoi(len)-256)){
        int total = recvtimeout(sock, 4);
        printf("\nPronto. Tamanho total do pacote: %d bytes\n", total);
    }

    char *conteudo = server_reply;

    closesocket(sock);
    WSACleanup();

    return conteudo;
}
