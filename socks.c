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
	BCM Revox Engine -> Ano: 2013|Tipo: WebEngine
*/
/*
    Baseado no código do site: http://www.news2news.com/vfp/?example=385&ver=vcpp.
    Acessado em 25 de Dezembro de 2013
*/
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <sys/types.h>

#pragma omp parallel

int InitSock(){
    WSADATA wsa;
    printf("Inicializando Revox...\r\n");
    if (WSAStartup(MAKEWORD(2,2),&wsa) != 0){
        printf("Falhou. Codigo de Erro: %d\n",WSAGetLastError());
        return 0;
    }
    return 1;
}

char *GetIP(char *address){
    in_addr ia;
    hostent* hbn;
    unsigned int addr;

    if(isalpha(address[0])){
        hbn = gethostbyname(address);
    }
    else{
        addr = inet_addr(address);
        hbn = gethostbyaddr((char *)&addr, 4, AF_INET);
    }
    if(hbn==0){
        fprintf(stderr, "Erro: Falha ao procurar o IP: %d\n", WSAGetLastError());
        return "";
    }
    ia.S_un.S_addr =* (DWORD *)hbn->h_addr_list[0];

    return inet_ntoa(ia);
}

int ValidateEnvelope(char *h, char *r, char *s, char *ip){
    if(h="" || r="" || s=""){
        fprintf(stderr, "Erro: Endere\0207o de email, host ou remente/destinat\0240rio inv\0240lido!\r\n");
        return 0;
    }
    *ip = GetIP(h);
    if(*ip==""){
        fprintf(stderr, "Erro: Nao pode resolver o nome do host %s para endere\0207o IP!\r\n", h);
        return 0;
    }

    return 1;
}
char *ValidEnvelope2(char *h){
    char ip[TKB];
    if(h=""){
        fprintf(stderr, "Erro: Endere\0207o inv\0240lido!\r\n");
        return NULL;
    }
    ip = GetIP(h);
    if(ip==""){
        fprintf(stderr, "Erro: Nao pode resolver o nome do host %s para endere\0207o IP!\r\n", h);
        return NULL;
    }
    return ip;
}

int ConnectTo(int port, char *ip, SOCKET sockt){
    const u_short SMTP_PORT = port;
    sockaddr_in sa;

    sa.sin_family = AF_INET;
    sa.sin_addr.S_un.S_addr = inet_addr(ip);
    sa.sin_port = htons(SMTP_PORT);

    return (connect(sockt, (SOCKADDR *)&sa, sizeof(sa))==0);
}

int snd(char *txt, int resp, SOCKET sockt){
    char tx[4096];

    sprintf(tx, "%s\r\n", txt);
    if(send(sockt, tx, strlen(tx), 0)==SOCKET_ERROR){
        fprintf(stderr, "Erro: %s%s\n", tx, " - falha ao enviar!");
        return 0;
    }
    if(!resp){
        return 1;
    }
    while(1){
        char rcv[1024], outp[255] = "";
        HANDLE hEvent = WSACreateEvent();

        WSAEventSelect(sockt, hEvent, FD_READ);

        DWORD nWait=WSAWaitForMultipleEvents(1, &hEvent, 0, 1000, 0);

        WSACloseEvent(hEvent);

        if (nWait!=0){
            return 0;
        }
        recv(sockt, rcv, sizeof(rcv), 0);
        strncpy(outp, rcv, strcspn(rcv, "\n")+1);
        printf("%s\n", outp);
    }
    return 1;
}
