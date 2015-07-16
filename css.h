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
	BCM Revox Engine -> Ano: 2015|Tipo: WebEngine
*/

#include "html.h"

#ifndef _CSS_H_
#define _CSS_H_

typedef enum _seltp{
    DEFAULT,
    ACTIVE,
    FOCUS,
    HOVER,
    VISITED
}selTipo;

typedef enum _modoCss{
    DEFAULT_MODE,
    EXTERNAL,
    INTERNAL,
    INLINE
}modCSS;

typedef struct _elemCss{
    char *prop, *valor;
    selTipo tipo;
    modCSS modo;
    struct _elemCss *ant, *prox;
}elemCSS;

typedef struct _listaCss{
    char *id, *classe;
    Elemts tag;
    elemCSS *inicio, *fim;
}listaCSS;

typedef struct _hashCss{
    listaCSS *vet;
    int tam;
}hashCSS;

#endif // _CSS_H_
