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
/*
    Baseado no código do site: http://www.news2news.com/vfp/?example=385&ver=vcpp.
    Acessado em 25 de Dezembro de 2013
*/
#ifndef _SOCKS_H_
#define _SOCKS_H_

//Inicia o socket
int InitSock();
//Obtem o ip
char *GetIP(char *address);
//Valida o Envelope
int ValidateEnvelope(char *h, char *r, char *s, char *ip);
//Versão modificada do ValidateEnvelope que devolve o ip convertido e pede somente o endereço
char *ValidEnvelope2(char *h);
//faz a cpnexão
int ConnectTo(int port, char *ip, SOCKET sockt);
//Envia os pacotes
int snd(char *txt, int resp, SOCKET sockt);

#endif // _SOCKS_H_
