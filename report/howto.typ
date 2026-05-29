#let howto() = {
  [
    O compilador Javinha pode ser executado de duas formas diferentes: via `javinha_debug`, onde são impressos os logs detalhados de cada etapa, ou `javinha_release`, onde são exibidos somente os erros encontrados e os resultados da análise léxica/sintática.

    Para gerar os executáveis, crie um diretório "build" e o acesse com o comando `mkdir build && cd build`. Então, insira `cmake ..` para gerar o Makefile. Então, basta inserir `make` que serão gerados os 2 executáveis mencionados anteriormente.

    Para executar, basta inserir `./javinha_release {p1} {p2}`, onde `p1` é o caminho até o código-fonte enquanto `p2` é o nome do arquivo de output que será gerado contendo a lista de tokens e a tabela de símbolos.
  ]
}
