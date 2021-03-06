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
#ifndef _STRS_H_
#define _STRS_H_

typedef struct _elemento{
	char c;
	struct _elemento *ant, *prox;
}elemento;

typedef struct _pilha{
	elemento *topo;
}pilha;

int SearchString(char content[], char subStr[]);
int streql(char *str1, char *str2);
char *copiaLinha(char content[], int *i);
char *copiaString(char content[], int *i, char fim1, char fim2);
pilha *alocaPilha();
void insereElementoNaPilha(pilha *pilha, char c);
int pilhaVazia(pilha *pilha);
void removeElementoDaPilha(pilha *pilha, elemento *e);
void excluirPilha(pilha *pilha);

#endif // _STRS_H_
