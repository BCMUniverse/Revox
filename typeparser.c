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
	BCM Revox Engine -> Ano: 2014|Tipo: WebEngine
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>
#include <windows.h>
#include <commctrl.h>
#include "bonus.h"
#include "downloads.h"
#include "html.h"
#include "htmlText.h"
#include "manifest.h"
#include "plain.h"
#include "strs.h"
#include "typeparser.h"
#include "urlparser.h"

char types[][128] = {"text/html", "text/cache-manifest", "text/plain", "application/x-download", "application/octet-stream"};

type TypeBuster(char content[]){
    char *aux = NULL, TypFile[VKB], *vtyp = NULL;
    int i, j;

    for(i=0; content[i]!='\0';){
        if(aux!=NULL){
            limpaVetor(aux);
            aux = NULL;
        }
        aux = CopyManifst(content, &i);
        if(SearchString(aux, "Content-Type:")>-1){
            for(i=i, j=0; content[i]!='\r' && content[i]!='\n'; i++, j++){
                TypFile[j] = content[i];
            }
            break;
        }
    }
    for(j=0; j<5; j++){
        vtyp = strstr(TypFile, types[j]);
        if(vtyp!=NULL){
            return (type)j;
        }
    }
    return DOWNLOAD;
}

typec InitTypeParser(Type tp1, int mode){
    char *vtyp = NULL, path[2048] = {}, *content = NULL, szFilePathName[_MAX_PATH] = {""}, *aux = NULL;
    int i = 0, j = 0, k = 0;
    type typ1;
    typec result;
    url u1 = UrlParser(tp1.url);
    TCHAR szFilters[] = _T("Todos os arquivos (*.*)\0*.*\0\0");
    OPENFILENAME ofn = {0};

    result.manifst = UNCACHED;
    result.plain = NULL;
    for(j=0; j<5; j++){
        vtyp = strstr(tp1.content, types[j]);
        if(vtyp!=NULL){
            typ1 = (type)j;
        }
    }
    if(vtyp==NULL){
        typ1 = TypeBuster(tp1.content);
    }
    char path2[strlen(u1.url_path)];
    if((aux = strrchr(u1.url_path, '/'))!=NULL){
        k = aux-u1.url_path;
        for(i=0; u1.url_path[k]!='\0'; i++, k++){
            path2[i] = u1.url_path[k];
        }
    }
    switch(typ1){
    case THTML:
        //a função inithtmltext será substituida em breve!
        InitHTMLText(tp1.content);
        break;
    case MANIFEST:
        result.manifst = InitManifest(tp1.content, tp1.url);
        break;
    case PLAIN:
        result = InitPlain(tp1.content, tp1.url);
        break;
    case DOWNLOAD: case OCTETSTREAM: default:
        switch(mode){
        case 0:
            // Preencher a estrutura OPENFILENAME
            ZeroMemory(&ofn, sizeof(ofn));
            ofn.lStructSize = sizeof(OPENFILENAME);
            ofn.hwndOwner = tp1.hwnd;
            ofn.lpstrFilter = szFilters;
            ofn.lpstrFile = szFilePathName;  // Isto manterá o nome do arquivo
            ofn.lpstrDefExt = _T("");
            ofn.nMaxFile = _MAX_PATH;
            ofn.lpstrTitle = _T("Salvar Arquivo");
            ofn.Flags = OFN_OVERWRITEPROMPT;
            ofn.lpstrFileTitle = path2;

            // Abra o arquivo Salvar caixa de diálogo e escolher o nome do arquivo
            if(GetSaveFileName(&ofn)){
                content = TagBody(tp1.content);
                SaveFile(szFilePathName, content, "w+");
            }
            break;
        case 1:
            printf("Digite o local onde deseja salvar o arquivo(para salvar exemplo.txt, C:\exemplo_pasta\exemplo.txt): ");
            fgets(path, 2048, stdin);
            content = TagBody(tp1.content);
            SaveFile(path, content, "w+");
            break;
        default:
            fprintf(stderr, "Erro: Valor Invalido!\r\n");
        }
        break;
    }

    return result;
}

char *TagHeader(char *content){
    char header[8196];
    int i = 0, head = SearchString(content, "\r\n\r\n");

    for(i=0; i<=head; i++){
        header[i] = content[i];
    }

    return header;
}

char *TagBody(char *content){
    char body[strlen(content)-1024];
    int i = 0, j = 0, bodyn = SearchString(content, "\r\n\r\n"), tam = strlen(content);

    for(i=bodyn, j=0; i<tam; i++, j++){
        body[j] = content[i];
    }

    return body;
}
