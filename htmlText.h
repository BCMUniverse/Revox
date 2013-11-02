/*
	Este arquivo faz parte do BCM Revox Engine;

	BCM Revox Engine é Software Livre; você pode redistribui-lo e/ou
	modificá-lo dentro dos termos da Licença Pública Geru GNU como
	publicada pela Fundação do Software Livre (FSF); na versão 3 da Licença.
	Este programa é distribuído na esperança que possa ser util,
	mas SEM NENHUMA GARANTIA; sem uma garantia implicita de ADEQUAÇÂO a
	qualquer MERCADO ou APLICAÇÃO EM PARTICULAR. Veja a Licença Pública Geral
	GNU para maiores detalhes.
	Você deve ter recebido uma cópia da Licença Pública Geral GNU junto com
	este programa, se não, escreva para a Fundação do Software Livre(FSF) Inc.,
	51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

	BCM Revox Engine v0.1
	BCM Revox Engine -> Ano: 2013|Tipo: WebEngine
*/
#ifndef _HTMLTEXT_H_
#define _HTMLTEXT_H_

//Em Breve
typedef struct{
    char aux[32], aux2[32], html[BUF32KB], title[1024], head[10240], *body;
}Parsers;

//Cria Elementos para compara-los
char *CreateTag(char *inicio, char *tag, char *fim);
//Copia Substrings dentro de um string
char *SubString(char *str, int inicio, int fim);
//Copia várias substrings e remove-as dentro de uma string
char *SubString2(char *str, char *inicio, char *fim);
//Analisa documento HTML e converte em Texto
char *InitHTMLText(char *content);

#endif // _HTMLTEXT_H_
