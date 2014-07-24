/*
	Este arquivo faz parte do BCM Revox Engine;

	BCM Revox Engine é Software Livre; você pode redistribui-lo e/ou
	modificá-lo dentro dos termos da Licença Pública Geral GNU como
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
#include "bonus.h"
#include "ftp.h"
#include "http.h"
#include "mail.h"
#include "htmlText.h"
#include "manifest.h"
#include "nntp.h"
#include "prospero.h"
#include "revox.h"
#include "strs.h"
#include "typeparser.h"
#include "urlparser.h"
#include "usenet.h"
#include "wais.h"

char ports[][16] = {"http", "https", "ftp", "telnet", "gopher", "file", "mailto", "news", "nntp", "wais", "prospero"};
char bars[][4] = {":", "//", "/", "?"};

url UrlParser(char host[]){
    char aux[16], aux2[VKB];
    int i = 0;
    url addr;
    uports port1 = (uports)0;

    limpaVetor(addr.host);
    limpaVetor(addr.url_path);
    for(i=0; i<11; i++){
        strcpy(aux, CreateTag(ports[i], bars[0], bars[1]));
        strcpy(aux2, strstr(host, aux));
        if(aux2==NULL){
            strcpy(aux2, strstr(host, strupr(aux)));
        }
        if(aux2==NULL){
            strcpy(aux, CreateTag(ports[i], bars[0], "\0"));
            strcpy(aux2, strstr(host, aux));
            if(aux2==NULL){
                strcpy(aux2, strstr(host, strupr(aux)));
            }
        }
        if(aux2!=NULL){
            port1 = i;
            switch(port1){
            case HTTP:
                addr.port = 80;
                strcpy(addr.prtcol, ports[i]);
                RemvSubString(host, aux);
                break;
            case HTTPS:
                addr.port = 443;
                strcpy(addr.prtcol, ports[i]);
                RemvSubString(host, aux);
                break;
            case FTP:
                addr.port = 21;
                strcpy(addr.prtcol, ports[i]);
                RemvSubString(host, aux);
                break;
            case TELNET:
                addr.port = 23;
                strcpy(addr.prtcol, ports[i]);
                RemvSubString(host, aux);
                break;
            case GOPHER:
                addr.port = 70;
                strcpy(addr.prtcol, ports[i]);
                RemvSubString(host, aux);
                break;
            case FILES:
                addr.port = 80;
                strcpy(addr.prtcol, ports[i]);
                RemvSubString(host, aux);
                break;
            case MAILTO:
                addr.port = 25;
                strcpy(addr.prtcol, ports[i]);
                RemvSubString(host, aux);
                break;
            case NEWS:
                addr.port = 0;
                strcpy(addr.prtcol, ports[i]);
                RemvSubString(host, aux);
                break;
            case NNTP:
                addr.port = 119;
                strcpy(addr.prtcol, ports[i]);
                RemvSubString(host, aux);
                break;
            case WAIS:
                addr.port = 210;
                strcpy(addr.prtcol, ports[i]);
                RemvSubString(host, aux);
                break;
            case PROSPERO:
                addr.port = 1525;
                strcpy(addr.prtcol, ports[i]);
                RemvSubString(host, aux);
                break;
            default:
                fprintf(stderr, "Erro: protocolo não encontrado!\r\n");
                addr.port = 80;
                strcpy(addr.prtcol, ports[i]);
            }
            break;
        }
        else if(aux2==NULL){
            addr.port = 80;
            strcpy(addr.prtcol, ports[i]);
        }
    }
    for(i=0; i<strlen(host) && host[i]!=':' && host[i]!='/' && host[i]!='?'; i++){
        addr.host[i] = host[i];
    }
    RemvSubString(host, addr.host);
    if(strchr(host, ':')!=NULL){
        RemvSubString(host, bars[0]);
        limpaVetor(aux);
        for(i=0; host[i]!='/'; i++){
            aux[i] = host[i];
        }
        addr.port = atoi(aux);
    }
    else{
        if(strchr(host, '/')!=NULL){
            strcpy(addr.url_path, host);
        }
        else{
            if(strchr(host, '?')!=NULL){
                strcpy(addr.url_path, host);
            }
        }
    }
    if(strchr(host, '/')==NULL){
        strcpy(addr.url_path, "/");
    }

    return addr;
}

char *UrlConstructor(char Url[], char path[], int mode){
    int tam = (strlen(Url)+strlen(path));
    char result[tam], aux2[128] = {}, url2[strlen(Url)], path2[strlen(path)], path3[strlen(path)], urlpath[strlen(Url)];
    int i = 0, aux = 0, ultbarra = 0, ultpto = 0;
    url url1;

    // limpa os vetores
    memset(result, 0, sizeof(result));
    memset(url2, 0, sizeof(url2));
    memset(path2, 0, sizeof(path2));
    memset(path3, 0, sizeof(path3));
    memset(urlpath, 0, sizeof(urlpath));

    //procura se o caminho a ser incorporado a url é um endereço externo
    while(i<11){
        sprintf(aux2, "%s%s%s", ports[i++], bars[0], bars[1]);
        aux = SearchString(path, aux2);
        if(aux>-1){
            break;
        }
    }
    if(aux==-1){
        strcpy(url2, Url);
        strcpy(path3, path);
        url1 = UrlParser(url2);
        ultbarra = strrchr(url1.url_path, '/')-url1.url_path;
        switch(mode){
        case 0:
            sprintf(path2, "%s%s", url1.url_path, path3);
            break;
        case 1:
            ultpto = strrchr(url1.url_path, '.')-url1.url_path;
            if(ultpto>ultbarra){
                strncpy(urlpath, url1.url_path, ultbarra+1);
                RemvSubString(urlpath, "/");
                sprintf(path2, "%s%s", urlpath, path3);
            }
            else{
                sprintf(path2, "%s%s", url1.url_path, path3);
            }
            break;
        }
        sprintf(result, "%s%s%s%s%s%s", url1.prtcol, bars[0], bars[1], url1.host, bars[2], path2);
        return result;
    }
    else{
        return path;
    }
}

Type UrlConnect(char host[], int mode, HINSTANCE hInst, HWND hwnd){
    char msg[2*BUFKB], *cached = NULL, *aux = NULL;
    int i;
    uports up1 = HTTP;
    FILE *input;
    Type tp1;
    url url1 = UrlParser(host);
    if((cached = IsCached(host))!=NULL){
        strcpy(tp1.content, cached);
        strcpy(tp1.url, host);
        return tp1;
    }
    for(i=0; i<11; i++){
        if(strstr(url1.prtcol, ports[i])!=NULL){
            up1 = i;
            break;
        }
    }
    aux = CreateTag(bars[0], bars[1], url1.host);
    printf("Inicializado\r\n");
    switch(up1){
    case HTTP:
        tp1.content = InitHTTP(url1.host, url1.port, url1.url_path, NULL);
        tp1.url = CreateTag(url1.prtcol, aux, url1.url_path);
        break;
    case HTTPS:
        tp1.content = InitHTTP(url1.host, url1.port, url1.url_path, NULL);
        tp1.url = CreateTag(url1.prtcol, aux, url1.url_path);
        break;
    case FTP:
        InitFTP(url1.host);
        break;
    case TELNET:
        sprintf(msg, "telnet %s %d", url1.host, url1.port);
        system(msg);
        tp1.content = NULL;
        tp1.url = CreateTag(url1.prtcol, bars[0], url1.url_path);
        break;
    case GOPHER:
        tp1.content = NULL;
        break;
    case FILES:
        if(url1.host==NULL){
            RemvSubString(url1.url_path, "/");
            if((input = fopen(url1.url_path, "r"))==NULL){
                fprintf(stderr, "Erro: Arquivo Invalido!\n");
                tp1.content = NULL;
                tp1.url = url1.url_path;
                return tp1;
            }
            tp1.url = url1.url_path;
            fgets(tp1.content, BUF32KB, input);
            fclose(input);
        }
        else{
            tp1.content = InitHTTP(url1.host, url1.port, url1.url_path, NULL);
            tp1.url = CreateTag(url1.prtcol, CreateTag(bars[0], bars[1], url1.host), url1.url_path);
        }
        break;
    case MAILTO:
        switch(mode){
        case 0:
            tp1.content = InitMailGUI(url1.host, url1.url_path, url1.port, hInst, hwnd);
            break;
        case 1:
            tp1.content = InitMailText(url1.host, url1.url_path, url1.port);
            break;
        default:
            fprintf(stderr, "Erro: Valor inexistente!\r\n");
            tp1.content = NULL;
        }
        tp1.url = NULL;
        break;
    case NEWS:
        tp1.content = NULL;
        break;
    case NNTP:
        switch(mode){
        case 0:
            InitNNTPGUI(url1.host, url1.port, url1.url_path, hInst, hwnd);
            break;
        case 1:
            InitNNTPText(url1.host, url1.port, url1.url_path);
            break;
        default:
            fprintf(stderr, "Erro: Valor inexistente!\r\n");
        }
        tp1.content = NULL;
        break;
    case WAIS:
        tp1.content = InitWAIS(url1.host, url1.port, url1.url_path, mode, hInst, hwnd);
        tp1.url = CreateTag(url1.prtcol, bars[0], url1.host);
        break;
    case PROSPERO:
        tp1.content = InitProspero(url1.host, url1.port, url1.url_path, mode, hInst, hwnd);
        tp1.url = CreateTag(url1.prtcol, bars[0], url1.host);
        break;
    }
    tp1.hwnd = hwnd;

    return tp1;
}
