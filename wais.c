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
	BCM Revox Engine -> Ano: 2014|Tipo: WebEngine
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include "bonus.h"
#include "socks.h"

char *InitWAIS(char address[], int port, char path[], int mode, HINSTANCE hInst, HWND hwnd){
    char result[BUF32KB], ip[TKB];
    SOCKET sock;

    InitSock();
    strcpy(ip, ValidEnvelope2(address));
    if (!ip){
        return "\0";
    }
    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock==SOCKET_ERROR){
        return "\0";
    }
    if (!ConnectTo(port, ip, sock)){
        fprintf(stderr, "Erro: Nao \0202 poss\0241vel conectar ao servidor!\r\n");
        return "\0";
    }
    WSACleanup();

    return result;
}
