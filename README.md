# JAVINHA

## Instruções de Compilação

O arquivo CMakeLists.txt contém as instruções para gerar o arquivo Makefile final de compilação do projeto. Para compilar o Javinha, você precisa:

- Criar um diretório vazio *build* e entrar nele: `mkdir build && cd build`;
- Especificar o modo de compilação: `cmake -DCMAKE_BUILD_TYPE=[Debug/Release] ..`;
- Compilar: `cmake --build .`;

Existem 2 modos de compilação disponíveis para o Javinha: o "Debug" foca em ajudar os desenvolvedores do compilador, adicionando flags necessárias para gerar símbolos de debugação e evitar completamente as otimizações dos compiladores GCC/Clang, de modo a nos permitir debugar de forma fiel o código que desenvolvemos. Já o modo "Release" faz o contrário, removendo quaisquer símbolos de debug e indicando o uso de otimizações agressivas de código, reduzindo o tamanho do binário. 

Obs.: não é necessário utilizar o CMake para gerar novos Makefiles sempre! Após modificações no código só precisamos dar o último comando (`cmake --build .`) para visualizarmos nossas alterações. O uso do comando que vem antes desse é indicado em 2 situações: 1) queremos mudar o modo de compilação (sair de Release para Debug e vice-versa); ou 2) fizemos alguma modificação no CMakeLists.txt (inclusão de biblioteca, mudança de flags, etc.)