
# Projeto exemplo

Palestra "Arquitetura de sistemas embarcados além do código-fonte" apresentada no [Seminário de Sistemas Embarcados e IoT 2023](https://embarcados.com.br/seminario-de-sistemas-embarcados-e-iot-2023/)

## Descrição

Este é um projeto básico de sistemas embarcados criado para exemplificação de conceitos apresentados durante a palestra:
- hardware
  * Arm Cortex-M STM32F103
  * placa Blue Pill
- linguagens: C++ e C
- inclusão de dependências externas
  * FreeRTOS
  * libopencm3
- CMake para execução de builds
- definição de container Docker para ambiente de desenvolvimento
- automação de testes com GitHub Actions

As instruções fornecidas a seguir são voltadas para sistemas operacionais  (especialmente distribuições baseadas em Debian), mas podem ser utilizadas como base para outros sistemas operacionais.

## Utilização recomendada

É recomendado utilizar o editor de código Visual Studio Code para maior facilidade de uso deste projeto. Neste caso os requisitos básicos são:
- Visual Studio Code instalado
- Docker instalado e funcional

Execute este comando para validar o funcionamento do Docker:
```console
$ docker run hello-world
```

Consulte a documentação do software para informações sobre instalação e configuração.

Após isto, basta abrir a pasta do projeto no VS Code e confirmar o pop-up que irá aparecer perguntado se deseja reabrir o workspace no devcontainer. Este processo deve levar alguns minutos na primeira execução. Instale as extensões recomendadas pelo workspace. Após isto, utilize o menu lateral CMake para configuar e compilar o projeto.

## Debug com o VS Code

Atualmente a funcionalidade de debug deste projeto não oferece suporte para conectar-se a um debugger utilizando o devcontainer. Para isto, é necessário utilizar o VS Code de forma nativa, instalando algumas dependências para a compilação e utilização do projeto.


**Instalação das dependências:**
```console
$ apt-get install --no-install-recommends -y \
gcc-arm-none-eabi \
libnewlib-arm-none-eabi \
libstdc++-arm-none-eabi-newlib \
gdb-multiarch \
cmake \
make
```

Agora, basta abrir a pasta pelo VS Code e recusar a utilização do devcontainer. Instale as extensões recomendadas. Utilize o menu lateral CMake para configurar e compilar o projeto.

É fornecida uma configuração de debug para a placa Blue Pill e debugger Black Magic Probe, basta iniciar o debug com o perfil `Cortex Debug` ou pressionando `F5`.

## Compilação nativa (sem utilizar o VS Code e devcontainer)

Este projeto pode ser desenvolvido e compilado de forma nativa, sem utilizar os setups apresentados anteriormente.

**Instalar pacotes necessários:**
```console
$ apt-get install --no-install-recommends -y \
gcc-arm-none-eabi \
libnewlib-arm-none-eabi \
libstdc++-arm-none-eabi-newlib \
gdb-multiarch \
cmake \
make
```

**Configurar o projeto e compilar:**
```bash
$ cmake -B build .
$ cmake --build build
```
