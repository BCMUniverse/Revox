#BCM Revox Engine
### #TheNextEngine -> #TheRevox

[![Join the chat at https://gitter.im/BCMUniverse/Revox](https://badges.gitter.im/Join%20Chat.svg)](https://gitter.im/BCMUniverse/Revox?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

BCM Revox Engine <http://revox.sf.net/>

##O que � Revox?

Revox � um renderizador web, escrito do zero por Cleber Matheus, que ler arquivos HTML e converte-os em p�ginas web. O Revox foi criado para programas que utilizavam dados da internet ou para exibir p�ginas web e ler arquivos HTML.

O Revox acessa os arquivos HTML no disco local ou atrav�s do protocolo HTTP e outros protocolos da internet, l� os arquivos recebidos e interpreta-os segundo o padr�o definido pela Engine(nas pr�ximas vers�es ir� interpretar conforme os padr�es do HTML5.1).

O BCM Revox Engine � distribuido pelo GNU General Public License - veja o arquivo LICENSE, acompanhamento para mais detalhes.

##Requisitos M�nimos

O Revox est� sendo desenvolvido somente no Windows 7, por�m pode funcionar em outras vers�es do Windows, em vers�es futuras, ter� suporte a Linux, Mac OS X entre outros.

##Documenta��o:
Na pasta docs/ voc� poder� encontrar mais detalhadamente com utilizar cada fun��o no seu c�digo e como incorporar o Revox no seu projeto.

##Como construir o Revox

Primeiro, clone o reposit�rio do Revox:

```bash
git clone https://github.com/BCMUniverse/Revox.git
```
Se n�o tiver o Codeblocks 12.11 instalado, fa�a o download e baixe-o no link abaixo:

```bash
http://sourceforge.net/projects/codeblocks/files/Binaries/12.11/Windows/codeblocks-12.11mingw-setup.exe
```

Abra o arquivo Revox.cbp no codeblocks e clique em Build ou compilar.
Pronto. A Biblioteca estar� na pasta bin/Release ou bin/Dbg, compilada junto com a biblioteca est�tica.

##Instalando e Utilizando o Revox

- Para instalar o Revox no projeto, deve-se copiar o arquivo dll para a pasta de arquivos compilados do projeto ou a pasta bin do Revox na pasta bin do seu projeto.
- Para utilizar o Revox no seu projeto deve seguir os passos que est�o na pasta docs.

##Copyright

Copyright 2013, 2014 Cleber Matheus
GNU General Public License v3.0

Contato: clebermatheus@outlook.com

###Autor(es)

Cleber Matheus