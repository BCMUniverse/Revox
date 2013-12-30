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

	BCM Revox Engine v0.2
	BCM Revox Engine -> Ano: 2013|Tipo: WebEngine
*/
#ifndef _MAIL_H_
#define _MAIL_H_

typedef struct _mail mail;
typedef enum _hmail hmail;
typedef enum _HeadMail HeadMail;

//Envia o email pelo smtp
int SendMail(mail sml);
//Envia o email por interface gráfica
char *InitMailGUI(char *host, char *others, int port, HINSTANCE hInst, HWND hwnd);
//Envia o email por texto
char *InitMailText(char *host, char *others, int port);
//Exibe a janela Remetente
BOOL CALLBACK AboutDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);

#endif // _MAIL_H_
