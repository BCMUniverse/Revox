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
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <winsock2.h>
#include "bonus.h"
#include "socks.h"
#include "htmlText.h"

typedef struct _mail{
    char host[STKB], ip[STKB], sender[STKB], recp[64][STKB], subj[STKB], body[STKB];
    int port, vezs;
    SOCKET mSock;
}mail;

typedef enum _hmail{
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

typedef enum _HeadMail{
    SUBJECT,
    BODY,
    CC,
    BCC
}HeadMail;

char email[][16] = {"@bol.com", "@gmail.com", "@hotmail.com", "@msn.com", "@live.com", "@outlook.com", "@yahoo.com", "@ymail.com", "@rocketmail.com"};
char mailc[][8] = {"subject", "body", "CC", "BCC"};

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
    int i, j, k, l;
    char aux[16];
    hmail hm;
    HeadMail headm;

    for(j=0; j<64; j++){
        for(i=0; host[i]!='\0' || host[i]!=';'; i++){
            m1.recp[j][i] = host[i];
        }
        if(host[i]=='\0'){
            break;
        }
    }
    m1.vezs = j;
    RemvSubString(others, "?");
    for(i=0; i<4; i++){
        headm = i;
        strcpy(aux, strstr(others, mailc[i]));
        if(aux==NULL){
            for(j=0; j<8; j++){
                strcpy(aux, strstr(others, toupper(mailc[j])));
                if(aux!=NULL){
                    break;
                }
            }
            if(aux==NULL){
                strcpy(aux, strstr(others, strupr(mailc[i])));
            }
        }
        else{
            RemvSubString(others, aux);
            for(j=1; others[j]!='&' || others[j]!='\0'; j++){
                switch(headm){
                case SUBJECT:
                    m1.subj[j] = others[j];
                    break;
                case BODY:
                    m1.body[j] = others[j];
                    break;
                case CC:
                    l = m1.vezs;
                    for(k=0; others[k]!='\0' || others[k]!=';' || others[k]!='&'; k++){
                        m1.recp[l][k] = others[k];
                        l++;
                    }
                    m1.vezs = l;
                    break;
                case BCC:
                    l = m1.vezs;
                    for(k=0; others[k]!='\0' || others[k]!=';' || others[k]!='&'; k++){
                        m1.recp[l][k] = others[k];
                        l++;
                    }
                    m1.vezs = l;
                    break;
                default:
                    fprintf(stderr, "Erro: Valor Inexistente!\r\n");
                }
            }
        }
    }
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
                break;
            case GMAIL:
                m1.host = "smtp.gmail.com";
                m1.port = 465;
                break;
            case HOTMAIL:
                m1.host = "smtp.live.com";
                m1.port = 587;
                break;
            case MSN:
                m1.host = "smtp.live.com";
                m1.port = 587;
                break;
            case LIVE:
                m1.host = "smtp.live.com";
                m1.port = 587;
                break;
            case OUTLOOK:
                m1.host = "smtp-mail.outlook.com";
                m1.port = 25;
                break;
            case YAHOO:
                m1.host = "smtp.mail.yahoo.com";
                m1.port = 465;
                break;
            case YMAIL:
                m1.host = "smtp.mail.yahoo.com";
                m1.port = 465;
                break;
            case ROCKETMAIL:
                m1.host = "smtp.mail.yahoo.com";
                m1.port = 465;
                break;
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
