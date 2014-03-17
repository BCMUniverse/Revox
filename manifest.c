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
#include <time.h>
#include "bonus.h"
#include "hex.h"
#include "strs.h"
#include "typeparser.h"
#include "urlparser.h"

char manisec[][16] = {"CACHE:", "FALLBACK:", "SETTINGS:", "NETWORK:"};
char setts[][16] = {"prefer-online", "fast"};

char *CopyManifst(char content[], int *i){
    char result[4096], c;
    int j, k = *i;

    if(content[k]=='#'){
        while(content[k]!='\n' || content[k]!='\r'){
            c = content[k];
            k++;
        }
    }
    else{
        for(k=*i, j=0; content[k]!=' ' || content[k]!='\r' || content[k]!='\n'; k++, j++){
            result[j] = content[k];
        }
    }
    while(content[k]=='\n' || content[k]=='\r' || content[k]==' '){
        c = content[k];
        k++;
    }
    *i = k;

    return result;
}

char *IsCached(char url[]){
    char *cont, *contIndex, *aux, c, path[4096], date[64];
    int aux2, i = 0;
    FILE *index, *output;
    long tam;

    if((index = fopen(".\\cache\\index", "r+"))==NULL){
        fprintf(stderr, "Erro: Arquivo Invalido!\r\n");
        return NULL;
    }
    tam = TamFile(index);
    contIndex = (char *)malloc(sizeof(char)*tam);
    fscanf(output, "%s", contIndex);
    aux2 = SearchString(contIndex, url);
    if(aux2!=-1){
        fseek(index, aux2, SEEK_SET);
        do{
            fscanf(index, "%c", c);
        }while(c!='\n' || c!='\r' || c!=' ');
        do{
            fscanf(index, "%c", path[i]);
            i++;
        }while(path[i]!='\n' || path[i]!='\r' || path[i]!=' ');
        i = 0;
        do{
            fscanf(index, "%c", date[i]);
            i++;
        }while(date[i]!='\n' || date[i]!='\r' || date[i]!=' ');
    }
    else{
        return NULL;
    }
    if((output = fopen(path, "r+"))==NULL){
        fprintf(stderr, "Erro: Arquivo Invalido!\r\n");
        return NULL;
    }
    tam = TamFile(output);
    cont = (char *)malloc(sizeof(char)*tam);
    fscanf(output, "%s", cont);
    fclose(output);
    fclose(index);

    return cont;
}

char *InitManifest(char content[], char url1[], char tipo[]){
    char *result, *aux, cache[4096], hexUrl[8192], *cached, file[12288], Time[64];
    FILE *output, *index;
    int i, j, k, cache1, fallbck, setts, net;
    struct tm *defTime;
    time_t currentTime;
    /*
        A varíavel defcache define baseado nas prefefências do manifesto, se deve ou não salvar os arquivos indicado no manifesto.
        defcache igual a 0 -> fast
        defcache igual a 1 -> prefer-online
    */
    int defcache = 0;

    cache1 = SearchString(content, manisec[0]);
    fallbck = SearchString(content, manisec[1]);
    setts = SearchString(content, manisec[2]);
    net = SearchString(content, manisec[3]);

    for(i=0; content[i]!='\0'; i++){
        aux = CopyManifst(content, &i);
        if(strcmp(aux, manisec[0])==0 && i!=fallbck && i!=setts && i!=net){
            aux = CopyManifst(content, &i);
            if(aux[0]!='#'){
                strcpy(cache, UrlConstructor(url1, aux));
                strcpy(hexUrl, HexCreater(cache));
                if((cached = IsCached(cache))!=NULL){
                    strcpy(result, cached);
                }
                else{
                    sprintf(file, ".\\cache\\%s", hexUrl);
                    //Abre ou cria a index e o arquivo no cache
                    if((output, fopen(file, "w+"))==NULL){
                        fprintf(stderr, "Erro: Arquivo Invalido!\r\n");
                        return NULL;
                    }
                    if((index, fopen(".\\cache\\index", "a+"))==NULL){
                        fprintf(stderr, "Erro: Arquivo Invalido!\r\n");
                        return NULL;
                    }
                    //Gerar o tempo atual antes de grava-lo na index
                    currentTime = time(NULL);
                    defTime = localtime(&currentTime);
                    strftime(Time, 64, "%d/%m/%Y", defTime);
                    //Registra o cache na index
                    fprintf(index, "%s\r\n%s\r\n%s\r\n\r\n", url1, HexCreater(cache), time);
                    //Armazena o cache
                    fprintf(output, "Content-Type: %s\r\n%s\r\n", tipo, UrlConnect(cache, 1, NULL, NULL));
                    //Fecha os arquivos
                    fclose(index);
                    fclose(output);
                }
            }
            else{
                CopyManifst(content, &i);
            }
        }
        if(strcmp(aux, manisec[1])==0 && i!=cache1 && i!=setts && i!=net){}
        if(strcmp(aux, manisec[2])==0 && i!=fallbck && i!=cache1 && i!=net){}
    }
    strcpy(result, "CACHED");

    return result;
}
