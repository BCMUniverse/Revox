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
	BCM Revox Engine -> Ano: 2017|Tipo: WebEngine
*/
#include <stdbool.h>

#ifndef _FILA_STRS_H_
#define _FILA_STRS_H_

typedef struct _nodeString {
    char *strings;
    struct _nodeString *prox;
} nodeString;

typedef struct _filaString {
    unsigned long tam;
    nodeString *inicio, *fim;
} filaString;

filaString *criaFilaString();
void pushFS(filaString*, char*);
char* popFS(filaString*);
void pop2FS(filaString*, char*);
char* peekFS(filaString*);
bool isEmptyFS(filaString *);
void destroyFS(filaString *);
void printFS(filaString *);

#endif // _FILA_STRS_H_
