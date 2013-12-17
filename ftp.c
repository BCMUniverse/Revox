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
##################################################
## Hi, ##
## Cliente FTP Básico - C ##
## by Dark Side ##
## wallacediasferreira@hotmail.com ##
## Bye. ##
##################################################
*/

#include <omp.h>
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#define ERRO -1 // Constante de erro

/*
resp = retorno de funções;
bytes = bytes recebidos;
size = número bytes que podem ser escritos em uma string.
*/
unsigned long resp=0,bytes=0,size=0;
long p_id=-1; // Número de incremento da porta local

WSADATA wsadata; // Variível WSADATA
SOCKET sock,sockdata; //SOCKETS
struct sockaddr_in sockin,local,sockindata; //Estruturas sockaddr_in
struct hostent * host; // Estrutruta hostent

/*
servidor,usuario,senha = informações sobre a conta;
buffer = buffer para a recepção de dados;
localip = IP local;
ftp_ip = representação do IP Local no formato aceito em FTP.
*/
char servidor[0x32],usuario[0x14],senha[0x14],buffer[0x800],localip[0x10],ftp_ip[0x10];

char * env = NULL; //Buffer para envio de comandos
char * buffer_file = NULL; // Buffer para receber armazenar conteúdo de arquivos

unsigned long chunk = 0x1000,total=0; //Variíveis de controle : enviar e receber de arquivos

/*
erro = informa erros;
fecha_socket = fecha sockets;
conv_ip = converte o IP Local para o formato aceito no FTP;
sock_dados = abre conexão de dados;
iniciar = inicia comunicação com o servidor.
*/

void erro(const char*),fecha_socket(void),conv_ip(void),sock_dados (void),iniciar(void);
char * receber(void); // Função para receber dados

void listar(void); // Lista arquivos
int upload(void); // Faz upload de arquivos
int download(void); // Baixa arquivos
void deleta_arquivo(void); // Deleta arquivos
int ren_arq(void); // Renomeia arquivos
void muda_dir(void); // Muda de diretório
void cria_dir(void); // Cria diretório
void deleta_dir(void); // Deleta diretórios
void sair(); // Encerra

int InitFTP(char host2){
    strcpy(servidor, host2);
    iniciar(); // Inicia comunicação
    unsigned char op;
gogo: // Ponto de partida
    system("cls"); // Limpa a tela

// Tarefas
    puts("\nSelecione uma tarefa:");
    puts("[1] Listar arquivos");
    puts("[2] Fazer upload de um arquivo...");
    puts("[3] Fazer o download de um arquivo...");
    puts("[4] Deletar um arquivo");
    puts("[5] Renomear um arquivo");
    puts("[6] Mudar de diretorio ");
    puts("[7] Criar um diretorio ");
    puts("[8] Deletar um diretorio ");
    puts("[9] Disconectar ");
    printf("Opcao: ");
    op = getchar(); // Obtém opção

    if(WSAGetLastError() == WSAECONNRESET)  // Verifica se hí conexão
    {
        fecha_socket();
        erro("\nA conexão foi perdida!");
    }

    switch(op) // Verifica a tarefa selecionada
    {
    case '1': // Listar arquivos
        listar();
        goto gogo; // Volta ao ponto de partida
        break;

    case '2': // Upload
        if(upload() == ERRO) goto gogo;
        goto gogo;
        break;

    case '3': // Download
        if(download() == -1) goto gogo;
        goto gogo;
        break;

    case '4': // Deleta arquivos
        deleta_arquivo();
        goto gogo;
        break;

    case '5': // Renomeia arquivos
        if(ren_arq() == -1) goto gogo;
        goto gogo;
        break;

    case '6': // Muda de diretório
        muda_dir();
        goto gogo;
        break;

    case '7': // Cria diretórios
        cria_dir();
        goto gogo;
        break;

    case '8': // Deleta diretórios
        deleta_dir();
        goto gogo;
        break;

    case '9': // Encerra
        sair();
        break;

    default: // Nenhuma das opções acima
        goto gogo; // Volta ao ponto de partida
        break;

    }

    return 0;
}

////////////////////////////////////////////////////////////////////////////////

void iniciar(){
    printf(">> Servidor: %50s\r\n", servidor); // Servidor
    //scanf("%50s",servidor);

    resp = WSAStartup(MAKEWORD(1,1),&wsadata); // Tenta inicializar Winsock
    if(resp == ERRO)
        erro("Erro ao inicializa WinSock."); // Função que mostra erro

    host = gethostbyname("localhost"); // Obtém host local

// Copia IP para a estrutura sockaddr_in: local
    memcpy(&local.sin_addr,host->h_addr_list[0],host->h_length);

    snprintf(localip,16,"%s",inet_ntoa(local.sin_addr)); // Grava o IP local na variível
    conv_ip(); // Converte o IP para o formato usando no FTP

    resp =(sock = socket(AF_INET,SOCK_STREAM,0)); // Tenta criar socket
    if(resp == ERRO)
        erro("Erro ao criar socket.");

    sockin.sin_family = AF_INET; // Famàlia do socket
    sockin.sin_port=htons(21); // Porta do FTP

    host = gethostbyname(servidor); // Tenta obter host do servidor
    if(host == NULL)
        erro("Erro ao obter host.");

    memcpy(&sockin.sin_addr,host->h_addr_list[0],host->h_length);

    resp = connect(sock,(struct sockaddr*)&sockin,sizeof(sockin)); // Tenta se conectar
    if(resp == ERRO)
        erro("Erro ao se conectar no servidor.");

    puts(receber()); // Mostra o banner => Dados inicias logo após a conexão

    printf("Nome de usuario: "); // Usuírio
    scanf("%20s",usuario);

    printf("Senha: "); // Senha
    scanf("%20s",senha);

// Envia usuírio: USER USUARIO
    size = strlen(usuario)+8;
    env = (char*)malloc(size);
    snprintf(env,size,"USER %s\r\n",usuario);
    send(sock,env,strlen(env),0);
    free(env);

    if(strstr(receber(),"331") == 0) // Verifica se não houve pedido da senha
    {
        fecha_socket();
        erro("Erro no processo de login");
    }

// Envia senha: PASS SENHA
    size = strlen(senha) + 8;
    env = (char*)malloc(size);
    snprintf(env,size,"PASS %s\r\n",senha);
    send(sock,env,strlen(env),0);
    free(env);

    if(strstr(receber(),"230") == 0) // Verifica se não houve login
    {
        fecha_socket();
        erro("O login falhou.");
    }

}

////////////////////////////////////////////////////////////////////////////////

void sock_dados() // Socket para conexão de dados
{
    p_id++; // Incrementa a porta = 1000 + p_id

    resp =(sockdata = socket(AF_INET,SOCK_STREAM,0)); // Tenta criar socket de dados
    if(resp == ERRO)
    {
        fecha_socket();
        erro("Erro ao criar socket.");
    }

    sockindata.sin_family = AF_INET;
    sockindata.sin_port=htons(1000+p_id); // Configura porta local de acordo com 1000 + p_id

// Tenta configurar socket
    resp = bind(sockdata,(struct sockaddr*)&sockindata,sizeof(sockindata));
    if(resp == ERRO)
    {
        fecha_socket();
        erro("Erro ao configurar socket de dados.");
    }
    listen(sockdata,1); // Aguarda a conexão de dados com o servidor
}

////////////////////////////////////////////////////////////////////////////////

// Recebe dados do socket principal
char * receber()
{
    memset(buffer,0x0,0x800); // Limpa o buffer
    bytes = recv(sock,buffer,0x800,0); // Recebe dados
    if(!bytes)  // Caso não tenha recebido dados, encerra
    {
        fecha_socket();
        erro("Erro ao receber dados");
    }
    return buffer; // Retorna os dados recebidos
}

////////////////////////////////////////////////////////////////////////////////

// Substitui . por , no ip. Ex: 127.0.0.1 = 127,0,0,1
// O ip deve ser enviado nesse formato para conexões de dados

void conv_ip()
{
    int i;
    strncpy(ftp_ip,localip,16);
    for(i=0; i<strlen(ftp_ip); i++)
        if(ftp_ip[i] == '.') ftp_ip[i] = ',';
}

////////////////////////////////////////////////////////////////////////////////

// Lista os arquivos e diretório
void listar()
{
    sock_dados(); // Abre conexão de dados
    size = 0x20;
    env = (char*)malloc(size);
    snprintf(env,size,"PORT %s,3,%d\r\nLIST\r\n",ftp_ip,232+p_id);


// Envia comando LIST + PORT:
    /*
    LIST
    PORT xxx,xxx,xxx,xxx,Y,YYY

    xxx,xxx,xxx,xxx = IP
    Y,YYY = grupos dos últimos bytes(2 em 2) que formam a porta, em decimal
    */

    send(sock,env,strlen(env),0); // Aqui -> envia
    puts(receber());

    while((sockdata = accept(sockdata,0,0)) == ERRO) // Enquanto não houver conexão
        Sleep(10); // Aguarda

    bytes = 1; // Bytes = 1 => validar loop

    while(bytes)  // Enquanto estiver recebendo
    {
        memset(buffer,0x0,0x800); // Limpa o buffer
        bytes = recv(sockdata,buffer,0x800,0); // Obtém dados
        puts(buffer); //Mostra-os
    }

    closesocket(sockdata); // Fecha socket de conexão
    puts(receber()); // Recebe dados do socket principal
    free(env);
    system("pause"); // Efetua uma pausa

}

////////////////////////////////////////////////////////////////////////////////

// Faz upload de um arquivo
int upload()
{
    printf("Fazer upload do arquivo: ");
    char local_file[0x50];
    scanf("%80s",local_file); // Obtém caminho do arquivo local

    FILE *fp = fopen(local_file,"rb"); // Tenta abrà-lo
    if(fp ==0)
    {
        printf("Erro ao abrir o arquivo %s\n",local_file);
        system("pause");
        return -1; // Encerra função com código -1
    }

    send(sock,"TYPE I\r\n",8,0); // Define conexão biníria
    receber();

    sock_dados(); // Inicializa conexão de dados
    size = 0x1D;
    env = (char*)malloc(size);
    snprintf(env,size,"PORT %s,3,%d\r\n",ftp_ip,232+p_id);

    send(sock,env,strlen(env),0); //Envia comando LIST + PORT
    puts(receber());
    free(env);

    size = strlen(local_file)+8;
    env = (char*)malloc(size);
    snprintf(env,size,"STOR %s\r\n",local_file);

// Envia comando STOR => Upload de arquivos
// Exemplo: STOR arquivo.lol

    send(sock,env,strlen(env),0);
    puts(receber());
    free(env);

    while((sockdata = accept(sockdata,0,0)) == ERRO) // Enquanto não houver conexão de dados
        Sleep(10); // Aguarda

    puts("Enviando arquivo..."); // Inicia envio do arquivo => fim do loop acima

    fseek(fp,0,SEEK_END); // Seta read/write ao fim
    unsigned long tam = ftell(fp); // Obtém tamanho do arquivo
    rewind(fp); // Seta read/write ao inàcio

    while(ftell(fp) < tam)  // Loop até que todo o arquivo seja lido
    {
        if(tam - ftell(fp) < chunk) // Calcula espaço disponàvel para leitura: 4096 Bytes = padrão
            chunk = tam - ftell(fp);

        buffer_file = (char*)malloc(chunk); // Aloca espaço no buffer
        fread(buffer_file,chunk,1,fp); // Lê bloco de bytes
        Sleep(10); // Pequena pausa
        send(sockdata,buffer_file,chunk,0); // Envia bloco de bytes
        printf("Enviando %lu de %lu\n",ftell(fp),tam); // Mostra status
        free(buffer_file); // Libera espaço previamente alocado
    }

    fclose(fp); // Fecha arquivo
    closesocket(sockdata);
    puts(receber());
    system("pause");

    return 0;
}

////////////////////////////////////////////////////////////////////////////////

int download()
{
    printf("Fazer download do arquivo: ");
    char remote_file[0x32];
    scanf("%50s",remote_file); // Obtém caminho do arquivo remoto

    sock_dados();
    size = 0x1D;
    env = (char*)malloc(size);
    snprintf(env,size,"PORT %s,3,%d\r\n",ftp_ip,232+p_id);

// Envia comando LIST + PORT
    send(sock,env,strlen(env),0);
    printf(receber());
    free(env);

    send(sock,"TYPE I\r\n",8,0);
    receber();

    size = strlen(remote_file)+8;
    env = (char*)malloc(size);
    snprintf(env,size,"RETR %s\r\n",remote_file);

// Envia comando RETR => Download de arquivos
// Exemplo: RETR arquivo.lol

    send(sock,env,strlen(env),0);
    free(env);

    char str_resp[0x32];
    snprintf(str_resp,0x32,"%s",receber()); // Armazena a resposta do servidor no buffer

    if(strstr(str_resp,"150")==0)  // Se não tiver código 150 (Sucesso)
    {
        puts(str_resp); // Mostra erro gerado pelo servidor
        printf("Erro ao obter o arquivo %s\n",remote_file);
        closesocket(sockdata);
        system("pause");
        return -1; // Encerra função com código -1
    }

    FILE *fp2 = fopen(remote_file,"wb"); // Tenta abrir o arquivo local
    if(fp2 ==0)  //
    {
        printf("Erro ao abrir o arquivo %s\n",remote_file);
        system("pause");
        return -1; // Encerra função com código -1
    }

    while((sockdata = accept(sockdata,0,0)) == ERRO) // Enquanto não houver conexão de dados
        Sleep(10); // Aguarda

    buffer_file = (char*)malloc(chunk); // Aloca espaço referente ao chunk (4096 bytes)
    bytes = 1; // Bytes = 1 => validar loop
    total = 0; // Total recebido
    puts("Recebendo arquivo...");
    while(bytes) // Enquanto houverem bytes (bytes > 0)
    {
        memset(buffer_file,0x0,chunk); // Limpa buffer
        bytes = recv(sockdata,buffer_file,chunk,0); // Recebe dados
        fwrite(buffer_file,bytes,1,fp2); // Escreve-os no arquivo local
        total+=bytes; // Incrementa total
    }

// Mostra bytes totais
    printf("Total recebido: %lu Bytes\n",total);

    fclose(fp2); // Fecha arquivo local
    closesocket(sockdata);
    puts(receber());
    free(buffer_file); // Libera espaço alocado no buffer
    system("pause");
    return 0;
}

////////////////////////////////////////////////////////////////////////////////

void deleta_arquivo()
{
    printf("Deletar arquivo: ");
    char del_file[0x20];
    scanf("%32s",del_file); // Obtém caminho do arquivo remoto

    size = strlen(del_file)+8;
    env = (char*)malloc(size);
    snprintf(env,size,"DELE %s\r\n",del_file);

// Envia comando DELE
// Exemplo: DELE arquivo.lol

    send(sock,env,strlen(env),0);
    puts(receber());

    free(env);
    system("pause");

}

////////////////////////////////////////////////////////////////////////////////

int ren_arq()
{
    printf("Renomear arquivo de: ");
    char ren_file[0x20];
    scanf("%32s",ren_file); // Caminho do arquivo remoto => renomear

    printf("Renomear arquivo para: ");
    char to_file[0x20];
    scanf("%32s",to_file); // Novo nome

    size = strlen(ren_file)+8;
    env = (char*)malloc(size);

// Envia comando RNFR
// Exemplo: RNFR arquivo_de.lol

    snprintf(env,size,"RNFR %s\r\n",ren_file);
    send(sock,env,strlen(env),0);
    free(env);

    if(strstr(receber(),"350") == 0)  // Se o código não for 350 (Sucesso)
    {
        puts("O arquivo especificado nao existe.");
        system("pause");
        return -1; // Encerra função com código -1
    }

    size = strlen(to_file)+8;
    env = (char*)malloc(size);

// Envia comando RNTO
// Exemplo: RNTO arquivo_para.lol

    snprintf(env,size,"RNTO %s\r\n",to_file);
    send(sock,env,strlen(env),0);
    free(env);

    puts(receber());
    system("pause");

    return 0;
}

////////////////////////////////////////////////////////////////////////////////

void muda_dir()
{
    send(sock,"PWD\r\n",5,0); // Mostra diretório atual
    puts(receber());

    char dir[0x20];
    printf("Mudar diretorio: "); // Mudar para...
    scanf("%32s",dir);

    size = strlen(dir)+7;
    env = (char*)malloc(size);

// Envia comando CWD
// Exemplo: CWD dir1

    snprintf(env,size,"CWD %s\r\n",dir);
    send(sock,env,strlen(env),0);

    puts(receber());
    free(env);
    system("pause");
}

////////////////////////////////////////////////////////////////////////////////


void cria_dir()
{
    printf("Criar diretorio: ");
    char cre_dir[0x20];
    scanf("%32s",cre_dir); // Nome do diretório => criar

    size = strlen(cre_dir)+7;
    env = (char*)malloc(size);
    snprintf(env,size,"MKD %s\r\n",cre_dir);

// Envia comando MKD
// Exemplo: MKD meu_dir

    send(sock,env,strlen(env),0);

    puts(receber());
    free(env);
    system("pause");

}

////////////////////////////////////////////////////////////////////////////////

void deleta_dir()
{
    printf("Deletar diretorio: ");
    char del_dir[0x20];
    scanf("%32s",del_dir); // Nome diretório => deletar

    size = strlen(del_dir)+7;
    env = (char*)malloc(size);
    snprintf(env,size,"RMD %s\r\n",del_dir);

// Envia comando RMD
// Exemplo: RMD meu_dir

    send(sock,env,strlen(env),0);

    puts(receber());
    free(env);
    system("pause");
}

void sair()
{
    send(sock,"QUIT\r\n",6,0); // Comando QUIT

    puts(receber());
    fecha_socket();
    exit(0);
}

////////////////////////////////////////////////////////////////////////////////

// Mostra erros e encerra
void erro(const char * erro)
{
puts(erro); // Mostra o erro
getch(); // Pausa
exit(1); // Encerra programa com código 1
}

////////////////////////////////////////////////////////////////////////////////

// Fecha sockets
void fecha_socket()
{
closesocket(sock); // Fecha o socket principal
closesocket(sockdata); // Fecha socket de dados
WSACleanup(); // Finaliza WinSock
}
