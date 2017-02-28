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
#include "../revox.h"

#ifndef _NODE_H_
#define _NODE_H_

typedef struct _ElementNode {
    char *namespaceURI, *prefix, *localName, *tagName;
    char *id, *className; // CEREactions
} Element;

typedef struct _attr {
    char *namespaceURI, *prefix, *localName, *name;
    char *value; // CEREactions
    Element *ownerElement;
    bool specified;
} attr;

#endif // _NODE_H_
