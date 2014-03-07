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
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bonus.h"
#include "hex.h"
#include "typeparser.h"
#include "urlparser.h"

char manisec[][16] = {"CACHE:", "FALLBACK:", "SETTINGS:"};
char setts[][16] = {"prefer-online", "fast"};

char *CopyManifst(char content[], int *i){
    char result[4096];
    int j, k;

    for(k=*i, j=0; content[k]!=' ' || content[k]!='\r' || content[k]!='\n'; k++, j++){
        result[j] = content[k];
    }
    *i = k;

    return result;
}

char *InitManifest(char content[], char url1[]){
    char *result, *aux, cache[4096], *apt;
    FILE *output, *index;
    int i, j, k;
    /*
        A varíavel defcache define baseado nas prefefências do manifesto, se deve ou não salvar os arquivos indicado no manifesto.
        defcache igual a 0 -> fast
        defcache igual a 1 -> prefer-online
    */
    int defcache = 0;

    if((index = fopen(".\\cache\\index", "r+"))==NULL){
        fprintf(stderr, "Erro: Arquivo Invalido!\r\n");
        return "\0";
    }
    for(i=0; content[i]!='\0'; i++){
        aux = CopyManifst(content, &i);
        if(strcmp(aux, manisec[0])==0){
            aux = CopyManifst(content, &i);
            strcpy(cache, aux);
            if(strcmp(index, HexCreater(aux))==0){
                if((output = fopen(cache, "a+"))==NULL){
                    fprintf(stderr, "Erro: Arquivo Invalido!\r\n");
                    return "\0";
                }
                fclose(output);
            }
            if((output = fopen(cache, "a+"))==NULL){
                fprintf(stderr, "Erro: Arquivo Invalido!\r\n");
                return "\0";
            }
            fclose(output);
        }
        if(strcmp(aux, manisec[1])==0){}
        if(strcmp(aux, manisec[2])==0){}
    }
    fclose(index);

    return result;
}
