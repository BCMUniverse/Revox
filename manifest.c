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
#include <io.h>
#include <stdio.h>
#include <time.h>
#include "downloads.h"
#include "hex.h"
#include "http.h"
#include "manifest.h"
#include "strptime.h"
#include "strs.h"
#include "urlparser.h"

char manisec[][16] = {"CACHE:", "FALLBACK:", "SETTINGS:", "NETWORK:"};
char setts[][16] = {"fast", "prefer-online"};
/**
*   A varíavel defcache define baseado nas prefefências do manifesto, se deve ou não salvar os arquivos indicado no manifesto.
*   defcache igual a 0 -> fast
*   defcache igual a 1 -> prefer-online
**/
int defcache = 0;

char *CopyManifst(char content[], int *i){
    char result[strlen(content)], c;
    int j = 0, k = *i;

    memset(result, 0, sizeof(result));
    if(content[k]=='#'){
        while(content[k]!='\r' && content[k]!='\n'){
            c = content[k++];
        }
    }
    else{
        while(content[k]!='\r' && content[k]!='\n' && content[k]!=' '){
            result[j++] = content[k++];
        }
    }
    while(content[k]=='\r' || content[k]=='\n' || content[k]==' '){
        c = content[k++];
    }
    *i = k;

    return result;
}

char *IsCached(char url[]){
    char *cont = NULL, *aux = NULL, path[4096] = {}, *dr = NULL, defsetts[32] = {}, *cont2 = NULL;
    int i = 0;

    dr = (char *)malloc(sizeof(char)*2048);
    getcwd(dr, 2048);
    strcat(dr, "\\cache\\index");
    if((cont = openFile(dr))==NULL){
        fprintf(stderr, "Erro: Arquivo Invalido!\r\nEndereco: %s\r\n", dr);
        return NULL;
    }
    for(i=0; cont[i]!='\0';){
        if(aux!=NULL){
            limpaVetor(aux);
            aux = NULL;
        }
        aux = CopyManifst(cont, &i);
        if(strcmp(aux, url)==0){
            if(aux!=NULL){
                limpaVetor(aux);
                aux = NULL;
            }
            aux = CopyManifst(cont, &i);
            strcpy(path, aux);
            if(aux!=NULL){
                limpaVetor(aux);
                aux = NULL;
            }
            aux = CopyManifst(cont, &i);
            strcpy(defsetts, aux);
            if(aux!=NULL){
                limpaVetor(aux);
                aux = NULL;
            }
            aux = CopyManifst(cont, &i);
            break;
        }
    }
    if(strcmp(defsetts, setts[1])==0){
        return NULL;
    }
    cont2 = openFile(path);
    free(dr);
    dr = NULL;

    return cont2;
}

status addCache(char content[], char address[]){
    char addrx[strlen(address)];
    strcpy(addrx, address);
    char *hexUrl = NULL, *cached = NULL, Time[64] = {}, *dr = NULL, path[2536] = {}, pathIndex[2536] = {}, *aux = NULL, file[8500] = {}, date[64] = {}, date2[64] = {};
    char *aux2 = NULL, *aux3 = NULL, ctmp[64] = {};
    int i = 0, j = 0, k = 0;
    time_t currentTime;
    struct tm tm1;

    strcpy(address, addrx);
    //Obter a index
    dr = (char *)malloc(sizeof(char)*2048);
    getcwd(dr, 2048);
    strcat(dr, "\\cache");
    sprintf(path, "mkdir %s", dr);
    system(path);
    printf("Pasta criada em: %s\r\n", dr);

    //Gerar o tempo atual antes de grava-lo na index
    currentTime = time(NULL);
    strftime(Time, 64, "%d/%m/%Y", localtime(&currentTime));
    sprintf(pathIndex, "%s\\index", dr);

    hexUrl = HexCreater(addrx);
    memset(&tm1, 0, sizeof(struct tm));
    if((cached = IsCached(address))!=NULL){
        for(i=0; cached[i]!='\0';){
            if(aux!=NULL){
                limpaVetor(aux);
                aux = NULL;
            }
            aux = CopyManifst(cached, &i);
            if(strcmp(aux, "Last-Modified:")==0){
                if(aux!=NULL){
                    limpaVetor(aux);
                    aux = NULL;
                }
                aux = CopyManifst(cached, &i);
                for(j=0; j<4; j++){
                    if(aux!=NULL){
                        limpaVetor(aux);
                        aux = NULL;
                    }
                    aux = CopyManifst(cached, &i);
                    strcat(date, aux);
                }
                strptime(date, "%d %m %Y %H:%M:%S", &tm1);
                limpaVetor(date);
                strftime(date, 64, "%d %m %Y", &tm1);
                Type buf = UrlConnect(address, 1, NULL, NULL);
                char cont[strlen(server_reply)];
                limpaVetor(cont);
                strcpy(cont, buf.content);
                aux2 = strtok(cont, " \r\n");
                while(aux2!=NULL){
                    aux2 = strtok(NULL, " \r\n");
                    if((aux3 = strstr(aux2, "Last-Modified:"))!=NULL){
                        int tmp = strlen(aux3);
                        for(j=15, k = 0; j<tmp || aux3[j]!='\r' && aux3[j]!='\n' && aux3[j]!='\0'; j++, k++){
                            ctmp[k] = aux3[j];
                        }
                        strptime(ctmp, "    %d %m %Y %H:%M:%S    ", &tm1);
                        limpaVetor(date2);
                        strftime(date2, 64, "%d %m %Y", &tm1);
                    }
                }
                if(strcmp(date, date2)>0){
                    SaveFile(file, cont, "w+");
                }
            }
        }
        return UNCACHED;
    }
    else{
        sprintf(file, "%s\\%s", dr, hexUrl);
        //Abre/cria a index e o arquivo no cache
        if(aux!=NULL){
            limpaVetor(aux);
            aux = NULL;
        }
        aux = (char *)malloc(sizeof(char)*((sizeof(address)+sizeof(file)+sizeof(Time))+31));
        limpaVetor(aux);
        strcpy(aux, address);
        strcat(aux, "\r\n");
        strcat(aux, file);
        strcat(aux, "\r\n");
        strcat(aux, setts[defcache]);
        strcat(aux, "\r\n");
        strcat(aux, Time);
        strcat(aux, "\r\n\r\n");
        SaveFile(pathIndex, aux, "a+");
        //Armazena o cache
        if(aux!=NULL){
            limpaVetor(aux);
            aux = NULL;
        }
        aux = (char *)malloc(sizeof(char)*(strlen(content)+8));
        limpaVetor(aux);
        strcpy(aux, content);
        strcat(aux, "\r\n");
        SaveFile(file, aux, "w+");
    }
    //limpa a memória
    free(aux);
    free(dr);
    free(cached);
    free(hexUrl);

    return UPDATEREADY;
}

status InitManifest(char content[], char url1[]){
    char *aux = NULL, *aux2 = NULL, *cache = NULL, cache2[4096] = {}, *cache3 = NULL, *cont2 = NULL;
    char url2[strlen(url1)], cont[strlen(content)];
    int i = 0, CacheManfst = 0;
    status result = 0;
    Type buffer;
    buffer.content = NULL;
    buffer.url = NULL;
    /**
    *   A varíavel state define o estado atual que está sendo utilizados os dados do arquivo recebido
    *   state igual a 0 -> Explicito
    *   state igual a 1 -> Cache
    *   state igual a 2 -> Fallback
    *   state igual a 3 -> Settings
    *   state igual a 4 -> Network
    **/
    int state = 0;

    limpaVetor(cont);
    strcpy(cont, content);
    limpaVetor(url2);
    strcpy(url2, url1);
    CacheManfst = SearchString(cont, "CACHE MANIFEST");
    addCache(cont, url1);

    //Limpa Vetores não utuilizados
    limpaVetor(cache2);

    //Analisa o manifesto
    for(i=0; cont[i]!='\0';){
        if(aux!=NULL){
            limpaVetor(aux);
            aux = NULL;
        }
        aux = CopyManifst(cont, &i);
        if(i>CacheManfst+14 && strcmp(aux, "\0")!=0){
            if(aux[0]=='#' || strstr(aux, "MANIFEST")!=NULL){
                if(aux!=NULL){
                    limpaVetor(aux);
                    aux = NULL;
                }
                aux = CopyManifst(cont, &i);
            }
            if(strcmp(aux, "")==0 || strcmp(aux, "\0")==0){
                if(aux!=NULL){
                    limpaVetor(aux);
                    aux = NULL;
                }
                aux = CopyManifst(cont, &i);
            }
            if(strcmp(aux, manisec[0])==0){
                if(aux!=NULL){
                    limpaVetor(aux);
                    aux = NULL;
                }
                aux = CopyManifst(cont, &i);
                state = 1;
            }
            else{
                if(strcmp(aux, manisec[1])==0){
                    if(aux!=NULL){
                        limpaVetor(aux);
                        aux = NULL;
                    }
                    aux = CopyManifst(cont, &i);
                    state = 2;
                }
                else{
                    if(strcmp(aux, manisec[2])==0){
                        if(aux!=NULL){
                            limpaVetor(aux);
                            aux = NULL;
                        }
                        aux = CopyManifst(cont, &i);
                        state = 3;
                    }
                    else if(strcmp(aux, manisec[3])==0){
                        if(aux!=NULL){
                            limpaVetor(aux);
                            aux = NULL;
                        }
                        aux = CopyManifst(cont, &i);
                        state = 4;
                    }
                }
            }
            if(cache!=NULL){
                limpaVetor(cache);
                cache = NULL;
            }
            limpaVetor(cache2);
            strcpy(url1, url2);
            if(buffer.content!=NULL){
                limpaVetor(buffer.content);
                buffer.content = NULL;
            }
            switch(state){
            case 0: case 1:
                cache = UrlConstructor(url1, aux, 1);
                cache3 = (char *)malloc(sizeof(char)*(strlen(cache)+1));
                limpaVetor(cache3);
                strcpy(cache3, cache);
                buffer = UrlConnect(cache, 1, NULL, NULL);
                if(strlen(buffer.content)==0){
                    break;
                }
                strcpy(cache, cache3);
                cont2 = (char *)malloc(sizeof(char)*(sizeof(server_reply)));
                limpaVetor(cont2);
                strcpy(cont2, server_reply);
                result = addCache(cont2, cache);
                break;
            case 2:
                if(aux2!=NULL){
                    limpaVetor(aux2);
                    aux2 = NULL;
                }
                aux2 = CopyManifst(cont, &i);
                cache = UrlConstructor(url1, aux, 1);
                sprintf(cache2, "%s", UrlConstructor(url1, aux2, 1));
                cache3 = (char *)malloc(sizeof(char)*(strlen(cache2)+1));
                limpaVetor(cache3);
                strcpy(cache3, cache2);
                buffer = UrlConnect(cache2, 1, NULL, NULL);
                if(strlen(buffer.content)==0){
                    break;
                }
                strcpy(cache2, cache3);
                cont2 = (char *)malloc(sizeof(char)*(sizeof(server_reply)));
                limpaVetor(cont2);
                strcpy(cont2, server_reply);
                result = addCache(cont2, cache2);
                break;
            case 3:
                if(strcmp(aux, setts[0])==0){
                    defcache = 0;
                }
                else{
                    if(strcmp(aux, setts[1])==0){
                        defcache = 1;
                    }
                }
                break;
            case 4:
                break;
            default:
                fprintf(stderr, "Erro: Estado invalido!\r\n");
                result = UNCACHED;
            }
        }
    }

    return result;
}
