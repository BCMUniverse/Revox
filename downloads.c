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
#include "bonus.h"

char *openFile(char caminho[]){
    char *arquivo = NULL, temp[TKB] = {};
    FILE *file;

    arquivo = (char *)malloc(sizeof(char)*TKB);
    limpaVetor(arquivo);
    if((file = fopen(caminho, "r"))==NULL){
        fprintf(stderr, "Erro: Arquivo Invalido!\r\nEndereco: %s\r\n", caminho);
        return NULL;
    }
    while((fgets(temp, TKB, file))!=NULL){
        strcat(arquivo, temp);
        arquivo = (char *)realloc(arquivo, sizeof(char)*TKB);
    }
    fclose(file);
    file = NULL;
    return arquivo;
}

void SaveFile(char path[], char content[], char tipo[]){
    FILE *file;

    //abre o arquivo
    if((file = fopen(path, tipo))==NULL){
        fprintf(stderr, "Erro: Arquivo Invalido!\r\nEndereco: %s\r\n", path);
        return;
    }
    //Armazena o conteúdo
    fprintf(file, "%s\r\n", content);
    //Fecha o arquivo
    fclose(file);
    file = NULL;
}
