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
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "bonus.h"
#include "hex.h"
#include "strs.h"
#include "typeparser.h"
#include "urlparser.h"
#include "downloads.h"

char manisec[][16] = {"CACHE:", "FALLBACK:", "SETTINGS:", "NETWORK:"};
char setts[][16] = {"fast", "prefer-online"};

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
    char *cont, buffer[12352], *aux = NULL, path[4096], date[64], *dr = NULL, defsetts[32];
    int i = 0;
    FILE *index, *output;
    long tam;

    dr = (char *)malloc(sizeof(char)*2048);
    getcwd(dr, 2048);
    strcat(dr, "\\cache\\index");
    if((index = fopen(dr, "r+"))==NULL){
        fprintf(stderr, "Erro: Arquivo Invalido!\r\nEndereco: %s\r\n", dr);
        return NULL;
    }
    while(fgets(buffer, strlen(buffer), index)!=NULL){
        if(aux!=NULL){
            limpaVetor(aux);
            aux = NULL;
        }
        aux = CopyManifst(buffer, &i);
        if(strcmp(aux, url)==0){
            if(aux!=NULL){
                limpaVetor(aux);
                aux = NULL;
            }
            aux = CopyManifst(buffer, &i);
            strcpy(path, aux);
            if(aux!=NULL){
                limpaVetor(aux);
                aux = NULL;
            }
            aux = CopyManifst(buffer, &i);
            strcpy(defsetts, aux);
            if(aux!=NULL){
                limpaVetor(aux);
                aux = NULL;
            }
            aux = CopyManifst(buffer, &i);
            strcpy(date, aux);
        }
    }
    fclose(index);
    if(strcmp(defsetts, setts[1])==0){
        return NULL;
    }
    if((output = fopen(path, "r+"))==NULL){
        fprintf(stderr, "Erro: Arquivo Invalido!\r\nEndereco: %s\r\n", path);
        return NULL;
    }
    tam = TamFile(output);
    cont = (char *)malloc(sizeof(char)*tam);
    fscanf(output, "%s", cont);
    fclose(output);
    free(dr);
    dr = NULL;

    return cont;
}

char *InitManifest(char content[], char url1[]){
    char *result = NULL, *aux = NULL, *aux2 = NULL, *cache = NULL, cache2[4096] = {}, hexUrl[8192] = {}, *cached = NULL, file[8500] = {}, Time[64] = {}, *dr = NULL, path[2200] = {};
    char pathIndex[2200] = {}, *tb = NULL, url2[strlen(url1)], cont[strlen(content)];
    FILE *output, *index;
    int i = 0, CacheManfst = 0;
    time_t currentTime;
    Type buffer;
    buffer.content = NULL;
    buffer.url = NULL;
    /**
    *   A varíavel defcache define baseado nas prefefências do manifesto, se deve ou não salvar os arquivos indicado no manifesto.
    *   defcache igual a 0 -> fast
    *   defcache igual a 1 -> prefer-online
    **/
    int defcache = 0;
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
    dr = (char *)malloc(sizeof(char)*2048);
    getcwd(dr, 2048);
    strcat(dr, "\\cache");
    sprintf(path, "mkdir %s", dr);
    system(path);
    printf("Pasta criada em: %s\r\n", dr);
    limpaVetor(url2);
    strcpy(url2, url1);
    CacheManfst = SearchString(cont, "CACHE MANIFEST");

    //Gerar o tempo atual antes de grava-lo na index
    currentTime = time(NULL);
    strftime(Time, 64, "%d/%m/%Y", localtime(&currentTime));
    sprintf(pathIndex, "%s\\index", dr);

    //Limpa Vetores não utuilizados
    limpaVetor(cache2);
    limpaVetor(file);
    limpaVetor(hexUrl);

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
            memset(hexUrl, 0, sizeof(hexUrl));
            strcpy(url1, url2);
            if(buffer.content!=NULL){
                limpaVetor(buffer.content);
                buffer.content = NULL;
            }
            switch(state){
            case 0: case 1:
                cache = UrlConstructor(url1, aux, 1);
                sprintf(hexUrl, "%s\0", HexCreater(cache));
                if((cached = IsCached(cache))!=NULL){
                    strcpy(result, cached);
                }
                else{
                    sprintf(file, "%s\\%s", dr, hexUrl);

                    sprintf(aux2, "%s\r\n%s\r\n%s\r\n%s\r\n\r\n", cache, file, setts[defcache], Time);
                    SaveFile(pathIndex, aux2, "a+");
                    //Abre/cria a index e o arquivo no cache
                    /*if((index = fopen(pathIndex, "a+"))==NULL){
                        fprintf(stderr, "Erro: Arquivo Invalido!\r\nEndereco: %s\r\n", pathIndex);
                        return NULL;
                    }
                    //Registra o cache na index
                    fprintf(index, "%s\r\n%s\r\n%s\r\n%s\r\n\r\n", cache, file, setts[defcache], Time);
                    //Fecha o arquivo
                    fclose(index);*/
                    buffer = UrlConnect(cache, 1, NULL, NULL);
                    if(strlen(buffer.content)==0){
                        return NULL;
                    }
                    if((tb = TypeBuster(buffer.content, THTML))==NULL){
                        return NULL;
                    }
                    sprintf(aux2, "Content-Type: %s\r\n\r\n%s\r\n", tb, buffer.content);
                    SaveFile(file, aux2, "w+");
                    /*if((output = fopen(file, "w+"))==NULL){
                        fprintf(stderr, "Erro: Arquivo Invalido!\r\nEndereco: %s\r\n", file);
                        return NULL;
                    }
                    //Armazena o cache
                    buffer = UrlConnect(cache, 1, NULL, NULL);
                    if(strlen(buffer.content)==0){
                        return NULL;
                    }
                    if((tb = TypeBuster(buffer.content, THTML))==NULL){
                        return NULL;
                    }
                    fprintf(output, "Content-Type: %s\r\n\r\n%s\r\n", tb, buffer.content);
                    //Fecha o arquivo
                    fclose(output);*/
                    //Anula o arquivo impedido modificação e invasão de dados
                    /*index = NULL;
                    output = NULL;*/
                }
                break;
            case 2:
                if(aux2!=NULL){
                    limpaVetor(aux2);
                    aux2 = NULL;
                }
                aux2 = CopyManifst(cont, &i);
                cache = UrlConstructor(url1, aux, 1);
                sprintf(cache2, "%s", UrlConstructor(url1, aux2, 1));
                sprintf(hexUrl, "%s", HexCreater(cache2));
                if((cached = IsCached(cache))!=NULL){
                    strcpy(result, cached);
                }
                else{
                    sprintf(file, "%s\\%s", dr, hexUrl);
                    //Abre/cria a index e o arquivo no cache
                    if((index = fopen(pathIndex, "a+"))==NULL){
                        fprintf(stderr, "Erro: Arquivo Invalido!\r\nEndereco: %s\r\n", pathIndex);
                        return NULL;
                    }
                    //Registra o cache na index
                    fprintf(index, "%s\r\n%s\r\n%s\r\n%s\r\n\r\n", cache2, file, setts[defcache], Time);
                    //Fecha o arquivo
                    fclose(index);
                    if((output = fopen(file, "w+"))==NULL){
                        fprintf(stderr, "Erro: Arquivo Invalido!\r\nEndereco: %s\r\n", file);
                        return NULL;
                    }
                    //Armazena o cache
                    buffer = UrlConnect(cache2, 1, NULL, NULL);
                    if(strlen(buffer.content)==0){
                        return NULL;
                    }
                    if((tb = TypeBuster(buffer.content, THTML))==NULL){
                        return NULL;
                    }
                    fprintf(output, "Content-Type: %s\r\n\r\n%s\r\n", tb, buffer.content);
                    //Fecha o arquivo
                    fclose(output);
                }
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
            }
        }
    }
    result = "CACHED";
    free(dr);
    dr = NULL;

    return result;
}
