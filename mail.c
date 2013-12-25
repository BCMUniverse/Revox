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
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <winsock2.h>
#include "bonus.h"
#include "socks.h"

typedef struct _mail{
    char host[STKB], ip[STKB], sender[STKB], recp[STKB], subj[STKB], body[STKB];
    SOCKET mSock;
}mail;

int SendMail(mail sml){
    char tmp[255];

    if (!ValidateEnvelope(sml.host, sml.recp, sml.sender, &sml.ip)){
        return 0;
    }
    if (sml.sender=="" || sml.recip=="" || sml.body=="" || sml.subject==""){
        return 0;
    }

    sml.mSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sml.mSock==SOCKET_ERROR){
        return 0;
    }

    if (!ConnectTo(25, sml.ip, sml.mSock)){
        fprintf(stderr, "Erro: Nao \0202 poss\0241vel conectar ao servidor!\n");
        return 0;
    }
    snd("HELO", 1);

    sprintf(tmp, "MAIL FROM: <%s>", sml.sender);
    snd(tmp, 1);
    sprintf(tmp, "RCPT TO:<%s>", sml.recp);
    snd(tmp, 0);

    snd("DATA", 1);
    sprintf(tmp, "From: %s", sml.sender);
    snd(tmp, 0);
    sprintf(tmp, "To: %s", sml.recp);
    snd(tmp, 0);
    sprintf(tmp, "Subject: %s", sml.subj);
    snd(tmp, 0);
    snd("", 0);
    snd(sml.body, 0);
    snd(".", 1);

    snd("NOOP", 1);
    snd("QUIT", 1);
    closesocket(sml.mSock);

    return 1;
}

void InitMailGUI(){}

void InitMailText(char *host, char *others){
    mail m1;
    if(InitSock()){
        SendMail(m1);
        WSACleanup();
    }
}
