#let preprocessor() = {
  [
    O pré-processador é a primeira fase do compilador. Sua responsabilidade é preparar o
    código-fonte para a análise léxica, realizando duas operações em sequência: remoção
    de comentários e minificação.

    == Remoção de Comentários

    A função `remove_comments` elimina todos os comentários presentes no código-fonte.
    São tratados dois estilos de comentário:\

    *Comentários de bloco* (`/* ... */`): o código percorre o buffer caractere a caractere
    procurando a sequência de abertura `/*`. Ao encontrá-la, localiza o fechamento `*/`
    correspondente e substitui todos os caracteres intermediários, inclusive comentários em
    linha ou início de comentários de blocos, com exceção das quebras de linha, que são
    preservadas para manter a numeração de linhas correta para diagnósticos de erro.\

    *Comentários de linha* (`// ...`): após o tratamento dos blocos, uma expressão regular do
    padrão `//[^\n]*` substitui cada comentário de linha por uma string vazia.

    == Minificação

    A função `minify` compacta o código removendo espaços e caracteres de controle desnecessários,
    sem quebrar tokens válidos. O algoritmo percorre o buffer e aplica as seguintes regras:

    - Quebras de linha são sempre preservadas no resultado.
    - Caracteres visíveis (não-espaço, não-controle) são copiados diretamente.
    - Espaços e caracteres de controle são descartados, com uma exceção: se o último caractere já inserido no resultado *e* o próximo caractere a ser lido forem ambos alfanuméricos, um espaço é inserido entre eles para evitar a fusão indevida de tokens.
  ]
}
