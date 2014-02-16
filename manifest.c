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
#include "typeparser.h"
#include "urlparser.h"

char *InitManifest(char content[], char url1[]){
    char *result, c, buffer[2048], host[2048], *aux, *cont2, *cache, onbuff[2048], *fallbck, *settings;
    FILE *output, *index;
    int i, j, k;
    /*
        A varíavel defcache define baseado nas prefefências do manifesto, se deve ou não salvar os arquivos indicado no manifesto.
        defcache igual a 0 -> fast
        defcache igual a 1 -> prefer-online
    */
    int defcache = 0;

    if((output = fopen("./cache/manifest1", "a+"))==NULL){
        fprintf(stderr, "Erro: Arquivo Invalido!\r\n");
        return "\0";
    }
    for(i=0; content[i]!='\0'; i++){
        cache = strstr(content, "CACHE:");
        fallbck = strstr(content, "FALLBACK:");
        settings = strstr(content, "SETTINGS:");
        if(settings!=NULL){
            for(i=i; i!=(fallbck-content) || i!=(cache-content); i++){}
        }
        if(cache!=NULL){
            for(i=i; i!=(fallbck-content) || i!=(settings-content); i++){
                while(content[i]!='\n'){
                    i++;
                }
                i++;
                for(i=i, j=0; content[i]!='\r' || content[i]!='\n'; i++, j++){
                    buffer[j] = content[i];
                }
                for(k=0; k<11; k++){
                    aux = strstr(buffer, ports[k]);
                    if(aux!=NULL){
                        strcpy(host, buffer);
                        break;
                    }
                }
                if(aux==NULL){
                    strcpy(host, "/");
                    strcat(host, buffer);
                }
                cont2 = InitTypeParser(UrlConnect(host, 1, NULL, NULL), 1);
                fprintf(output, "%s\r\n", cont2);
            }
        }
        if(fallbck!=NULL){
            for(i=i; i!=(cache-content) || i!=(settings-content); i++){
                while(content[i]!='\n'){
                    i++;
                }
                i++;
                for(i=i, j=0; content[i]!=' '; i++, j++){
                    onbuff[j] = content[i];
                }
                for(i=i, j=0; content[i]!='\r' || content[i]!='\n'; i++, j++){
                    buffer[j] = content[i];
                }
                for(k=0; k<11; k++){
                    aux = strstr(buffer, ports[k]);
                    if(aux!=NULL){
                        strcpy(host, buffer);
                        break;
                    }
                }
                if(aux==NULL){
                    strcpy(host, "/");
                    strcat(host, buffer);
                }
                cont2 = InitTypeParser(UrlConnect(host, 1, NULL, NULL), 1);
                fprintf(output, "%s\r\n", cont2);
            }
        }
    }
    fclose(output);

    return result;
}
