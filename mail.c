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
    char host[STKB], ip[STKB], sender[STKB], recp[64][STKB], subj[STKB], body[STKB];
    int port, vezs;
    SOCKET mSock;
}mail;

typedef struct _hmail{
    BOL,
    GMAIL,
    HOTMAIL,
    MSN,
    LIVE,
    OUTLOOK,
    YAHOO,
    YMAIL,
    ROCKETMAIL
}hmail;

char email[][16] = {"@bol.com", "@gmail.com", "@hotmail.com", "@msn.com", "@live.com", "@outlook.com", "@yahoo.com", "@ymail.com", "@rocketmail.com"};

int SendMail(mail sml){
    char tmp[255];
    int i;

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

    if (!ConnectTo(sml.port, sml.ip, sml.mSock)){
        fprintf(stderr, "Erro: Nao \0202 poss\0241vel conectar ao servidor!\n");
        return 0;
    }
    snd("HELO", 1);

    sprintf(tmp, "MAIL FROM: <%s>", sml.sender);
    snd(tmp, 1);
    for(i=0; i<sml.vezs; i++){
        sprintf(tmp, "RCPT TO:<%s>", sml.recp[i]);
        snd(tmp, 0);
    }

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

void InitMailText(char *host, char *others, int port){
    mail m1;
    int i, j;
    char aux[16];
    hmail hm;

    printf("Digite o remetente: ");
    fgets(m1.sender, 256, stdin);
    for(i=0; i<9; i++){
        strcpy(aux, strstr(host, email[i]));
        if(aux==NULL){
            strcpy(aux, strstr(host, strupr(email[i])));
        }
        else{
            hm = i;
            switch(hm){
            case BOL:
                m1.host = "smtps.bol.com.br";
                m1.port = 587;
            case GMAIL:
                m1.host = "smtp.gmail.com";
                m1.port = 465;
            case HOTMAIL:
                m1.host = "smtp.live.com";
                m1.port = 587;
            case MSN:
                m1.host = "smtp.live.com";
                m1.port = 587;
            case LIVE:
                m1.host = "smtp.live.com";
                m1.port = 587;
            case OUTLOOK:
                m1.host = "smtp-mail.outlook.com";
                m1.port = 25;
            case YAHOO:
                m1.host = "smtp.mail.yahoo.com";
                m1.port = 465;
            case YMAIL:
                m1.host = "smtp.mail.yahoo.com";
                m1.port = 465;
            case ROCKETMAIL:
                m1.host = "smtp.mail.yahoo.com";
                m1.port = 465;
            default:
                fprintf(stderr, "Erro: Valor Inxistente!\r\n");
                m1.port = port;
            }
        }
    }

    if(InitSock()){
        SendMail(m1);
        WSACleanup();
    }
}
