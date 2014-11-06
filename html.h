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
#include "revox.h"

#ifndef _HTML_H_
#define _HTML_H_

typedef struct _listaHtml listaHtml;
typedef struct _pilhaHtml pilhaHtml;
typedef struct _elemntHtml elemntHtml;
typedef struct _hash hashHtml;
typedef struct _elemntFila elemntFila;
typedef struct _fila filaHtml;

extern char elemts[][16];

typedef enum _Elemts{
    DOCTYPE,
    COMMENTS,
    CDATA,
    HTML,
    HEAD,
    BASE,
    LINK,
    META,
    NOSCRIPT,
    SCRIPT,
    STYLE,
    TEMPLATE,
    TITLE,
    BODY,
    A,
    ABBR,
    ADDRESS,
    AREA,
    ARTICLE,
    ASIDE,
    AUDIO,
    B,
    P,
    UNKNOWN
} Elemts;

struct _elemntHtml{
    char *id, *className, *attrs, *innerHtml;
    Elemts tagName;
    int quantFilhos, isClosed;
    struct _elemntHtml *ant, *prox;
};

struct _listaHtml{
    elemntHtml *inicio, *fim;
    int tam;
};

struct _hash{
    listaHtml vet[100];
};

struct _elemntFila{
    Elemts tagName;
    struct _elemntFila *ant, *prox;
};

struct _fila{
    elemntFila *inicio, *fim;
};

/**
*	Aloca a lista.
*
*	@param lista* listaHtml - lista a ser alocada.
**/
void alocaListaHtml(listaHtml *lista);

hashHtml *alocaHash();

filaHtml *alocaFila();

/**
*	Insere o elemento na fila.
*
*	@param * filaHtml - fila onde o elemento será inserido.
*	@param tagName Elemts - elemento que será adicionado.
**/
void insereElementoNaFila(filaHtml *, Elemts tagName);

int insereElementoNoHash(hashHtml *h, char id[], char className[], char attrs[], char innerHtml[], Elemts tagName, filaHtml *f);

/**
*	Insere um elemento na lista.
*
*	@param lista* listaHtml - lista onde o elemento será inserido.
*	@param elemento* elemntHtml - adiciona o elemento.
*	@return int - retorna -1 se houver um erro na função, senão retorna 0.
**/
int insereElementoNaListaHtml(listaHtml *lista, elemntHtml* elemento);

/**
*	Verifica se a fila está vazia.
*
*	@param filaHtml* - fila a ser verificada.
*	@return int - retorna 1 se a sila estiver vazia, senão retorna 0.
**/
int filaVazia(filaHtml *f);

/**
*	Remove o elemento da fila.
*
*	@param filaHtml* - fila onde está o elemento a ser removido.
**/
void removeElementoDaFila(filaHtml *);

/**
*	Remove o elemento da lista.
*
*	@param lista listaHtml* - lista onde está o elemento a ser removido.
*	@param pos int - posição do elemento na lista.
*	@return int - retorna -1 se houver algum problema, senão retorna 0.
**/
int removeElementoDaListaHtml(listaHtml *lista, int pos);

/**
*	Exclui a pilha.
*
*	@param pilha pilhaHtml* - pilha que será excluída.
**/
void excluirPilhaHtml(pilhaHtml *pilha);

/**
*	Exclui a lista.
*
*	@param lista listaHtml* - lista que será excluída.
**/
void excluirListaHtml(listaHtml *lista);

/**
*	Copia elemento do tipo html de uma string.
*
*	@param content char[] - conteúdo da string.
*	@param i int* - posição inicial da string a ser copiada.
*	@return char* - retorna o elemento copiado.
**/
char *copiaTag(char content[], int *i);

/**
*	Analisador HTML.
*
*	@param content char[] - conteúdo html em formato de uma string.
*	@param url char[] - endereço deste conteúdo.
*	@return listaHtml* - retorna a lista criada.
**/
hashHtml *htmlParser(char content[], char url[]);

#endif // _HTML_H_
