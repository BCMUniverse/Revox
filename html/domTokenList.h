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
#include "../utils/filaStrs.h"

#ifndef _DOMTOKENLIST_H_
#define _DOMTOKENLIST_H_

typedef struct _DomTokenList DOMTokenList;

struct _DomTokenList {
    unsigned long length;
    filaString *tokens;
};

DOMTokenList* criaDOMTokenList();
unsigned long DTLgetLength(DOMTokenList *);
char* DTLgetValue(DOMTokenList *);
void DTLsetValue(DOMTokenList *, char*);

char* DTLitem(DOMTokenList *, unsigned long index);
bool DTLcontains(DOMTokenList *, char *token);
void DTLadd(DOMTokenList *, char** tokens);
void DTLremove(DOMTokenList *, char **tokens);
bool DTLtoggle(DOMTokenList *, char *token, bool force);
void DTLreplace(DOMTokenList *, char *token, char *newToken);
bool DTLsupports(DOMTokenList *, char *token);

#endif // _DOMTOKENLIST_H_
