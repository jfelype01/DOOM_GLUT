# Projeto: Doom Clone em C com FreeGLUT

Este projeto é uma implementação de um jogo 3D estilo Doom, utilizando a biblioteca *FreeGLUT* para renderização com OpenGL e a biblioteca *stb_image.h* para carregamento de texturas. O objetivo foi criar uma engine simples baseada em setores e paredes, inspirada na renderização de jogos clássicos como Doom e Duke Nukem 3D.

O desenvolvimento foi realizado utilizando a *IDE Visual Studio*, com suporte para projetos em C e integração com bibliotecas externas.

---

## 🎮 Funcionalidades

- Renderização 3D em primeira pessoa usando projeção 2D baseada em setores
- Sistema de setores e paredes (level design baseado em polígonos)
- Texturas nos pisos, tetos e paredes com carregamento via stb_image
- Detecção de colisão com paredes
- Movimentação estilo FPS com teclado e mouse
- Carregamento de mapas de arquivos externos
- Sistema básico de entidades (ex: inimigos ou objetos no mapa)

---

## 🛠️ Tecnologias Utilizadas

- *Linguagem:* C
- *Renderização:* OpenGL com FreeGLUT
- *Imagens:* [stb_image.h](https://github.com/nothings/stb)
- *IDE:* [Visual Studio](https://visualstudio.microsoft.com/)
- *Bibliotecas:*
  - [FreeGLUT](http://freeglut.sourceforge.net/)
  - [stb_image.h](https://github.com/nothings/stb)

---


## 🖥️ Como Executar (Visual Studio)

1. Baixe ou clone este repositório.
2. Abra o arquivo doom-clone.sln no *Visual Studio*.
3. Certifique-se de que as bibliotecas *FreeGLUT* e *OpenGL* estão corretamente configuradas:
   - Adicione os arquivos .lib e .dll nas pastas corretas do projeto ou configure nas propriedades do projeto.
   - Inclua o diretório onde está o stb_image.h.
4. Compile e execute o projeto (pressione Ctrl + F5 ou clique em "Local Windows Debugger").

---

## 🕹️ Controles

- *W / A / S / D:* Movimento
- *Mouse:* Olhar para os lados
- *Mouse esquerdo* Atirar
- *Tecla 1* Pistola
- *Tecla 2* Shotgun
- *ESC:* Sair do jogo

---

## 🙋‍♂️ Créditos

- Artur Fernandes Braga
- Fabrício Alves Ribeiro
- João Felype Morais Vieira
