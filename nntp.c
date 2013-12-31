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
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include "bonus.h"
#include "socks.h"

#pragma omp parallel

void InitNNTPGUI(){}

void InitNNTPText(char address[], int port, char path[]){
    char ip[TKB], comnd[(4*STKB)], ng[STKB], tmp[VKB];
    int cs = 0;
    SOCKET sock;

    InitSock();
    if (!(ip = ValidEnvelope2(address)){
        return;
    }
    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock==SOCKET_ERROR){
        return;
    }
    if (!ConnectTo(port, ip, sock)){
        fprintf(stderr, "Erro: Nao \0202 poss\0241vel conectar ao servidor!\r\n");
        return;
    }
    for(i=1; path[i]!='/' || path[i]!='\0'; i++){
        ng[i] = path[i];
    }
    sprintf(tmp, "GROUP %s", ng);
    snd(tmp, 1, sock);
    do{
        printf("Revox %s NNTP-> ", VERSION);
        fgets(comnd, 4096, stdin);
        sprintf(tmp, "%s", comnd);
        if(strstr(cmnd, "QUIT")!=NULL){
            cs = closesocket(sock);
        }
    }while(cs!=0);
    WSACleanup();

    return;
}
