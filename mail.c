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
#include <ctype.h>
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <winsock2.h>
#include "bonus.h"
#include "socks.h"
#include "htmlText.h"
#include "resource.h"

typedef struct _mail{
    char *host, *ip, *sender, *recp, *subj, *body;
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
char sendr[1025];

BOOL CALLBACK AboutDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam){
    switch(Message){
    case WM_INITDIALOG:
        return TRUE;
    case WM_COMMAND:
        switch(LOWORD(wParam)){
        case IDOK:
            GetDlgItemText(hwnd, IDCTEXT, (LPSTR)sendr, 1025);
            EndDialog(hwnd, IDOK);
            break;
        }
        break;
    default:
        return FALSE;
    }
    return TRUE;
}

int SendMail(mail sml){
    char tmp[255];
    int i;

    if (!ValidateEnvelope(sml.host, sml.recp, sml.sender, sml.ip)){
        return 0;
    }
    if (sml.sender=="" || sml.recp=="" || sml.body=="" || sml.subj==""){
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
    snd("HELO", 1, sml.mSock);

    sprintf(tmp, "MAIL FROM: <%s>", sml.sender);
    snd(tmp, 1, sml.mSock);
    for(i=0; i<sml.vezs; i++){
        sprintf(tmp, "RCPT TO:<%s>", sml.recp[i]);
        snd(tmp, 0, sml.mSock);
    }

    snd("DATA", 1, sml.mSock);
    sprintf(tmp, "From: %s", sml.sender);
    snd(tmp, 0, sml.mSock);
    sprintf(tmp, "To: %s", sml.recp);
    snd(tmp, 0, sml.mSock);
    sprintf(tmp, "Subject: %s", sml.subj);
    snd(tmp, 0, sml.mSock);
    snd("", 0, sml.mSock);
    snd(sml.body, 0, sml.mSock);
    snd(".", 1, sml.mSock);

    snd("NOOP", 1, sml.mSock);
    snd("QUIT", 1, sml.mSock);
    closesocket(sml.mSock);

    return 1;
}

char *InitMailGUI(char *host, char *others, int port, HINSTANCE hInst, HWND hwnd){
    mail m1;
    int i = 0, j = 0, k = 0;
    char aux[16];
    hmail hm;
    HeadMail headm;

    m1.recp = (char *)malloc(strlen(host)+1);
    for(i=0; host[i]!='\0' || host[i]!=';'; i++){
        if(host[i]=='\0'){
            break;
        }
        m1.recp[i] = host[i];
    }
    m1.vezs = j;
    RemvSubString(others, "?");
    for(i=0; i<4; i++){
        headm = i;
        strcpy(aux, strstr(others, mailc[i]));
        if(aux==NULL){
            for(j=0; j<8; j++){
                strcpy(aux, strstr(others, (char *)toupper((int)mailc[j])));
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
                    for(k=j; others[k]!='\0' || others[k]!=';' || others[k]!='&'; k++){
                        m1.recp[k] = others[k];
                    }
                    j = k;
                    break;
                case BCC:
                    for(k=j; others[k]!='\0' || others[k]!=';' || others[k]!='&'; k++){
                        m1.recp[k] = others[k];
                    }
                    j = k;
                    break;
                default:
                    fprintf(stderr, "Erro: Valor Inexistente!\r\n");
                }
            }
        }
    }
    DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG), hwnd, AboutDlgProc);
    strcpy(m1.sender, sendr);
    for(i=0; i<9; i++){
        strcpy(aux, strstr(host, email[i]));
        if(aux==NULL){
            strcpy(aux, strstr(host, strupr(email[i])));
        }
        else{
            hm = i;
            switch(hm){
            case BOL:
                strcpy(m1.host, "smtps.bol.com.br");
                m1.port = 587;
                break;
            case GMAIL:
                strcpy(m1.host, "smtp.gmail.com");
                m1.port = 465;
                break;
            case HOTMAIL:
                strcpy(m1.host, "smtp.live.com");
                m1.port = 587;
                break;
            case MSN:
                strcpy(m1.host, "smtp.live.com");
                m1.port = 587;
                break;
            case LIVE:
                strcpy(m1.host, "smtp.live.com");
                m1.port = 587;
                break;
            case OUTLOOK:
                strcpy(m1.host, "smtp-mail.outlook.com");
                m1.port = 25;
                break;
            case YAHOO:
                strcpy(m1.host, "smtp.mail.yahoo.com");
                m1.port = 465;
                break;
            case YMAIL:
                strcpy(m1.host, "smtp.mail.yahoo.com");
                m1.port = 465;
                break;
            case ROCKETMAIL:
                strcpy(m1.host, "smtp.mail.yahoo.com");
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
    return "Email enviado com sucesso!\r\n";
}

char *InitMailText(char *host, char *others, int port){
    mail m1;
    int i = 0, j = 0, k = 0;
    char aux[16];
    hmail hm;
    HeadMail headm;

    for(i=0; host[i]!='\0' || host[i]!=';'; i++){
        if(host[i]=='\0'){
            break;
        }
        m1.recp[i] = host[i];
    }
    m1.vezs = j;
    RemvSubString(others, "?");
    for(i=0; i<4; i++){
        headm = i;
        strcpy(aux, strstr(others, mailc[i]));
        if(aux==NULL){
            for(j=0; j<8; j++){
                strcpy(aux, strstr(others, (char *)toupper((int)mailc[j])));
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
                    for(k=j; others[k]!='\0' || others[k]!=';' || others[k]!='&'; k++){
                        m1.recp[k] = others[k];
                    }
                    j = k;
                    break;
                case BCC:
                    for(k=j; others[k]!='\0' || others[k]!=';' || others[k]!='&'; k++){
                        m1.recp[k] = others[k];
                    }
                    j = k;
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
                strcpy(m1.host, "smtps.bol.com.br");
                m1.port = 587;
                break;
            case GMAIL:
                strcpy(m1.host, "smtp.gmail.com");
                m1.port = 465;
                break;
            case HOTMAIL:
                strcpy(m1.host, "smtp.live.com");
                m1.port = 587;
                break;
            case MSN:
                strcpy(m1.host, "smtp.live.com");
                m1.port = 587;
                break;
            case LIVE:
                strcpy(m1.host, "smtp.live.com");
                m1.port = 587;
                break;
            case OUTLOOK:
                strcpy(m1.host, "smtp-mail.outlook.com");
                m1.port = 25;
                break;
            case YAHOO:
                strcpy(m1.host, "smtp.mail.yahoo.com");
                m1.port = 465;
                break;
            case YMAIL:
                strcpy(m1.host, "smtp.mail.yahoo.com");
                m1.port = 465;
                break;
            case ROCKETMAIL:
                strcpy(m1.host, "smtp.mail.yahoo.com");
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
    return "Email enviado com sucesso!\r\n";
}
