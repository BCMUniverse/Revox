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
#ifndef _FTP_H_
#define _FTP_H_

int InitFTP(char host2);
void listar(void); // Lista arquivos
int upload(void); // Faz upload de arquivos
int download(void); // Baixa arquivos
void deleta_arquivo(void); // Deleta arquivos
int ren_arq(void); // Renomeia arquivos
void muda_dir(void); // Muda de diretório
void cria_dir(void); // Cria diretório
void deleta_dir(void); // Deleta diretórios
void sair(); // Encerra

/*
erro = informa erros;
fecha_socket = fecha sockets;
conv_ip = converte o IP Local para o formato aceito no FTP;
sock_dados = abre conexão de dados;
iniciar = inicia comunicação com o servidor.
*/

void erro(const char*),fecha_socket(void),conv_ip(void),sock_dados (void),iniciar(void);
char *receber(void); // Função para receber dados

#endif // _FTP_H_
