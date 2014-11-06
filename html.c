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
#include "html.h"
#include "strs.h"
#include "typeparser.h"

char elemts[][16] = {"doctype", "--", "[CDATA[", "html", "head", "base", "link", "meta", "noscript", "script", "style", "template", "title", "body", "a", "abbr", "address", "area", "article", "aside", "audio", "b", "p"};

void alocaListaHtml(listaHtml *lista){
    lista->inicio = NULL;
    lista->fim = NULL;
    lista->tam = 0;
}

hashHtml *alocaHash(){
    hashHtml *h = (hashHtml *)malloc(sizeof(hashHtml));

    for(int i=0; i<100; i++){
        alocaListaHtml(&h->vet[i]);
    }
    return h;
}

filaHtml *alocaFila(){
    filaHtml *f = (filaHtml *)malloc(sizeof(filaHtml));

    f->inicio = NULL;
    f->fim = NULL;

    return f;
}

void insereElementoNaFila(filaHtml *fila, Elemts tagName){
    elemntFila *e = (elemntFila *)malloc(sizeof(elemntFila));

    e->tagName = tagName;
    if(fila->inicio == NULL){
        e->ant = fila->inicio;
        e->prox = fila->fim;
        fila->inicio = e;
    }
    else{
        e->ant = fila->fim;
        e->prox = NULL;
        fila->fim->prox = e;
    }
    fila->fim = e;
}

int insereElementoNoHash(hashHtml *h, char id[], char className[], char attrs[], char innerHtml[], Elemts tagName, filaHtml *f){
    elemntHtml *e = (elemntHtml *)malloc(sizeof(elemntHtml));

    strcpy(e->attrs, attrs);
    strcpy(e->className, className);
    strcpy(e->id, id);
    strcpy(e->innerHtml, innerHtml);
    e->tagName = tagName;
    e->isClosed = 0;
    insereElementoNaFila(f, tagName);

    return insereElementoNaListaHtml(&h->vet[tagName], e);
}

int insereElementoNaListaHtml(listaHtml *lista, elemntHtml* elemento){
    if(lista->inicio==NULL){
        elemento->ant = lista->inicio;
        elemento->prox = lista->fim;
        lista->inicio = elemento;
    }
    else{
        elemento->ant = lista->fim;
        elemento->prox = NULL;
        lista->fim->prox = elemento;
    }
    lista->fim = elemento;
    lista->tam++;

    return 0;
}

int filaVazia(filaHtml *fila){
    if(fila->inicio == NULL && fila->fim == NULL){
        return 1;
    }
    return 0;
}

void removeElementoDaFila(filaHtml *f){
    if(!filaVazia(f)){
        elemntFila *e = f->inicio;
        f->inicio = f->inicio->prox;
        if(f->inicio!=NULL){
            f->inicio->prox->ant = NULL;
        }
        else{
            f->fim = NULL;
        }
        e->tagName = UNKNOWN;
        free(e);
    }
}

int removeElementoDaListaHtml(listaHtml *lista, int pos){
    int i = 0;
    elemntHtml *elemento, *inCurse;

    if(lista->tam == 0){
        return -1;
    }
    if(pos == 1){ //remove o primeiro elemento
        elemento = lista->inicio;
        lista->inicio = lista->inicio->prox;
        if(lista->inicio == NULL){
            lista->fim = NULL;
        }
        else{
            lista->inicio->ant = NULL;
        }
    }
    else if(pos == lista->tam){ //remove o último elemento
        elemento = lista->fim;
        lista->fim->ant->prox = NULL;
        lista->fim = lista->fim->ant;
    }
    else{
        inCurse = lista->inicio;
        for(i = 1; i<pos; ++i){
            inCurse = inCurse->prox;
        }
        elemento = inCurse;
        inCurse->ant->prox = inCurse->prox;
        inCurse->prox->ant = inCurse->ant;
    }
    free(elemento->attrs);
    free(elemento->className);
    free(elemento->id);
    free(elemento->innerHtml);
    elemento->tagName = UNKNOWN;
    free(elemento);
    lista->tam--;

    return 0;
}

void excluirFila(filaHtml *fila){
    free(fila);
}

void excluirListaHtml(listaHtml *lista){
    while(lista->tam > 0){
        removeElementoDaListaHtml(lista, 1);
    }
}

char *copiaTag(char content[], int *i){
    char result[strlen(content)], c;
    int j = 0, k = *i;

    memset(result, 0, sizeof(result));
    while(content[k]!='>' && content[k]!=' ' && content[k]!='\t'){
        result[j++] = content[k++];
    }
    while(content[k]==' ' || content[k]=='\t'){
        c = content[k++];
    }
    if(content[k]=='>'){
        result[j] = content[k];
    }
    *i = k;

    return result;
}

hashHtml *htmlParser(char content[], char url[]){
    char *body = NULL, *aux = NULL, *aux2 = NULL, *id = NULL, *className = NULL, *attrs = NULL, *cont = NULL;
    Elemts tagName = UNKNOWN;
    int i = 0, j = 0, k = 0;
    hashHtml *h = alocaHash();
    filaHtml *f = alocaFila();

    body = TagBody(content);
    while(body[i]!='\0'){
        if(aux!=NULL){
            limpaVetor(aux);
            aux = NULL;
        }
        aux = copiaTag(body, &i);
        if(aux[j]=='<' && aux[j+1]=='!'){
            /* desnecessário, por enquanto
            if(elem!=NULL){
                limpaVetor(elem);
                elem = NULL;
            }
            elem = (char *)malloc(strlen(aux)+1);
            while(aux[j]!='>'){
                elem[j] = aux[j];
                j++;
            }
            elem[j] = aux[j];*/
            j = 2;
            if(aux2!=NULL){
                limpaVetor(aux2);
                aux2 = NULL;
            }
            aux2 = copiaString(aux, &j, ' ', '>');
            for(k=0; k<3; k++){
                if(strcmp(aux2, elemts[k])==0){
                    tagName = (Elemts)(k);
                    break;
                }
            }
            if(tagName==UNKNOWN){
                for(k=0; aux2[k]!='\0'; k++){
                    if(aux2[k]>64 && aux2[k]<91){
                        aux2[k] += 32;
                    }
                }
                for(k=0; k<3; k++){
                    if(strcmp(aux2, elemts[k])==0){
                        tagName = (Elemts)(k);
                        break;
                    }
                }
            }
            switch(tagName){
            case DOCTYPE:
                if(attrs!=NULL){
                    limpaVetor(attrs);
                    attrs = NULL;
                }
                attrs = copiaString(aux, &j, '>', '\0');
                insereElementoNoHash(h, "\0", "\0", attrs, "\0", tagName, f);
                break;
            case COMMENTS:
                if(cont!=NULL){
                    limpaVetor(cont);
                    cont = NULL;
                }
                cont = copiaString(aux, &j, '>', '\0');
                insereElementoNoHash(h, "\0", "\0", "\0", cont, tagName, f);
                break;
            case CDATA:
                //Mais em Breve!
                insereElementoNoHash(h, "\0", "\0", "\0", "\0", tagName, f);
                break;
            default:
                fprintf(stderr, "Erro: Elemento Invalido!\r\n");
            }
        }
        else if(aux[j]=='<' && aux[j+1]=='/'){
            j = 2;
            if(aux2!=NULL){
                limpaVetor(aux2);
                aux2 = NULL;
            }
            aux2 = copiaString(aux, &j, ' ', '>');
            for(k=3; k<23; k++){
                if(strcmp(aux2, elemts[k])==0){
                    tagName = (Elemts)(k);
                    break;
                }
            }
            if(tagName==UNKNOWN){
                for(k=0; aux2[k]!='\0'; k++){
                    if(aux2[k]>64 && aux2[k]<91){
                        aux2[k] += 32;
                    }
                }
                for(k=3; k<23; k++){
                    if(strcmp(aux2, elemts[k])==0){
                        tagName = (Elemts)(k);
                        break;
                    }
                }
            }
            switch(tagName){
            case HTML:
                //Marcar isCLosed == 1
                break;
            case HEAD:
                //Marcar isCLosed == 1
                break;
            default:
                fprintf(stderr, "Erro: Elemento Invalido!\r\n");
            }
        }
        else{
            j = 1;
            if(aux2!=NULL){
                limpaVetor(aux2);
                aux2 = NULL;
            }
            aux2 = copiaString(aux, &j, ' ', '>');
            for(k=3; k<23; k++){
                if(strcmp(aux2, elemts[k])==0){
                    tagName = (Elemts)(k);
                    break;
                }
            }
            if(tagName==UNKNOWN){
                for(k=0; aux2[k]!='\0'; k++){
                    if(aux2[k]>64 && aux2[k]<91){
                        aux2[k] += 32;
                    }
                }
                for(k=3; k<23; k++){
                    if(strcmp(aux2, elemts[k])==0){
                        tagName = (Elemts)(k);
                        break;
                    }
                }
            }

            //preparando para inserir elemento na lista e na pilha
            if(attrs!=NULL){
                limpaVetor(attrs);
                attrs = NULL;
            }
            attrs = copiaString(aux, &j, '>', '\0');
            if(id!=NULL){
                limpaVetor(id);
                id = NULL;
            }
            if((k = SearchString(attrs, "id="))!=-1){
                k++;
                id = copiaString(attrs, &k, '\"', '\'');
            }
            if(className!=NULL){
                limpaVetor(className);
                className = NULL;
            }
            if((k = SearchString(attrs, "class="))!=-1){
                k++;
                className = copiaString(attrs, &k, '\"', '\'');
            }
            if(cont!=NULL){
                limpaVetor(cont);
                cont = NULL;
            }
            cont = (char *)malloc(sizeof(char)*2048);
            for(k = 0; body[i]!='<'; k++){
                if(k==(sizeof(cont)-1)){
                    cont = realloc(cont, sizeof(char)*2048);
                }
                cont[k] = body[i++];
            }
            insereElementoNoHash(h, id, className, attrs, cont, tagName, f);
        }
        j = 0;
    }

    return h;
}
