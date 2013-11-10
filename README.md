#BCM Revox Engine
### #TheNextEngine -> #TheRevox

BCM Revox Engine <http://revox.sf.net/>

##O que é Revox?

Revox é um renderizador web, escrito do zero por Cleber Matheus, que ler arquivos HTML e converte-os em páginas web. O Revox foi criado para programas que utilizavam dados da internet ou para exibir páginas web e ler arquivos HTML.

O Revox acessa os arquivos HTML no disco local ou através do protocolo HTTP e outros protocolos da internet, lê os arquivos recebidos e interpreta-os segundo o padrão definido pela Engine(nas próximas versões irá interpretar conforme os padrões do HTML5.1).

O BCM Revox Engine é distribuido pelo GNU General Public License - veja o arquivo LICENSE, acompanhamento para mais detalhes.

##Requisitos Mínimos

O Revox está sendo desenvolvido somente no Windows 7, porém pode funcionar em outras versões do Windows, em versões futuras, terá suporte a Linux, Mac OS X entre outros.

##Documentação:
Na pasta docs/ você poderá encontrar mais detalhadamente com utilizar cada função no seu código e como incorporar o Revox no seu projeto.

##Como construir o Revox

Primeiro, clone o repositório do Revox:

```bash
git clone https://github.com/BCMUniverse/Revox.git
```
Se não tiver o Codeblocks 12.11 instalado, faça o download e baixe-o no link abaixo:

```bash
http://sourceforge.net/projects/codeblocks/files/Binaries/12.11/Windows/codeblocks-12.11mingw-setup.exe
```

Abra o arquivo Revox.cbp no codeblocks e clique em Build ou compilar.
Pronto. A Biblioteca estará na pasta bin/Release ou bin/Dbg, compilada junto com a biblioteca estática.

##Utilizando o Revox

A instalação e utilização do Revox estará na pasta docs explicando detalhadamente como instala-lo e utilizá-lo.