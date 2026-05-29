#let lexer() = {
  [
    O analisador léxico é a segunda fase do compilador, executada logo após o pré-processador.
    Sua responsabilidade é percorrer o código fonte já preparado e convertê-lo em uma sequência
    de _tokens_, cada um carregando seu tipo, seu valor e sua posição (linha e coluna) no texto original.

    == Tokens

    Cada token é representado pela estrutura `Token`, composta pelos campos:

    - `type`: categoria do token, definida pelo enumerado `TokenType`;
    - `value`: lexema original extraído do código fonte;
    - `line` e `column`: posição no arquivo de entrada.

    As categorias reconhecidas são:

    #table(
      columns: (auto, 1fr),
      stroke: 0.5pt,
      [*Categoria*], [*Descrição*],
      [`KEYWORD`],    [Palavra reservada da linguagem.],
      [`IDENTIFIER`], [Nome definido pelo programador.],
      [`NUMBER`],     [Literal numérico inteiro ou decimal.],
      [`OPERATOR`],   [Operador aritmético, relacional ou lógico.],
      [`DELIMITER`],  [Símbolo de pontuação ou delimitação de bloco.],
      [`END_OF_FILE`],[Marcador sintético inserido ao fim da tokenização.],
      [`UNKNOWN`],    [Caractere não reconhecido pela gramática léxica.],
    )

    == Palavras Reservadas

    As 14 palavras reservadas da linguagem são armazenadas em um array estático de `string_view`
    e verificadas por busca linear durante a tokenização de identificadores:

    ```
    class   public   static   void     int
    String  boolean  if       else     while
    return  new      this     extends
    ```

    == Operadores e Delimitadores

    Os operadores e delimitadores simples são indexados em um mapa estático `char → TokenType`.
    O único operador de dois caracteres reconhecido é `&&`, tratado antes da consulta ao mapa simples.

    #table(
      columns: (auto, auto, 1fr),
      stroke: 0.5pt,
      [*Símbolo*], [*Categoria*], [*Descrição*],
      [`+` `-` `*` `=` `!` `>` `&`], [`OPERATOR`],  [Operadores aritméticos, relacionais e lógicos.],
      [`&&`],                         [`OPERATOR`],  [Operador lógico AND (dois caracteres).],
      [`;` `,` `.`],                  [`DELIMITER`], [Separadores de instrução, argumento e acesso a membro.],
      [`(` `)` `{` `}` `[` `]`],     [`DELIMITER`], [Delimitadores de bloco, parâmetro e índice.],
    )

    == Funcionamento Interno

    A classe `Lexer` mantém o estado da varredura em três campos: `pos` (posição atual no buffer),
    `line` e `col` (linha e coluna para diagnósticos). A tokenização é coordenada pelo método
    `tokenize()`, cujo laço principal classifica cada caractere e delega a um dos três métodos especializados:

    === Identificadores e Palavras Reservadas

    O método `identifier()` consome caracteres alfanuméricos e sublinhados enquanto `peek()` os
    retornar, formando o lexema. Ao final, `is_keyword()` consulta o array de palavras reservadas
    por busca linear; se o lexema for encontrado, o token recebe a categoria `KEYWORD`. Caso contrário,
    o identificador é registrado na tabela de símbolos com a categoria `"IDENTIFIER"` e o token
    recebe a categoria `IDENTIFIER`.

    A expressão regular que descreve os identificadores válidos é:

    $ ["a"-"z""A"-"Z""_"]["a"-"z""A"-"Z""0"-"9""_"]^* $

    === Literais Numéricos

    O método `number()` consome dígitos decimais consecutivos. Se, ao término, o próximo caractere
    for um ponto (`.`), ele é consumido junto com os dígitos seguintes, permitindo a representação
    de literais de ponto flutuante. O resultado é sempre um único token `NUMBER`.

    A expressão regular correspondente é:

    $ ["0"-"9"]^+("."["0"-"9"]^+)? $

    O método `tokenize()` inclui ainda uma verificação preventiva: se um bloco de dígitos for
    seguido imediatamente por uma letra ou sublinhado (ex.: `123abc`), um erro léxico é reportado,
    pois essa sequência não constitui um identificador válido na linguagem.

    === Operadores e Delimitadores

    O método `op_or_delim()` consome o caractere atual e forma um lexema de um único caractere.
    Em seguida, concatena-o com o caractere seguinte e verifica se a combinação está no array de
    operadores duplos. Atualmente apenas `&&` é reconhecido como operador duplo; nesse caso, o
    segundo caractere é consumido e o token recebe o lexema `"&&"`. Para os demais casos, o tipo
    é determinado pelo mapa `single_tokens`.

    == Tabela de Símbolos

    Ao tokenizar um identificador não reservado, o `Lexer` insere-o na `SymbolTable` interna,
    registrando nome, categoria e linha de primeira ocorrência. A tabela rejeita inserções
    duplicadas silenciosamente, preservando sempre a entrada da primeira ocorrência. Ao término
    da análise léxica, a tabela pode ser transferida para fases posteriores via `move_symbols()`,
    sem cópia.

    == Tratamento de Erros

    O método `tokenize()` retorna um `std::variant` que carrega, de forma exclusiva, ou o
    vetor de tokens (em caso de sucesso) ou um vetor de mensagens de erro (em caso de falha).
    Dois tipos de erro léxico são reportados:

    - *Identificador inválido:* sequência iniciada por dígito seguida de caractere alfabético
      (ex.: `2x`). O número é tokenizado normalmente, mas o erro é registrado.
    - *Caractere inválido:* qualquer caractere não pertencente ao alfabeto da linguagem
      (ex.: `@`, `#`, `$`). O caractere é descartado via `advance()` e o erro é reportado
      com linha e coluna precisas.

    Se ao menos um erro for detectado durante a varredura, a variante de retorno conterá a
    lista de erros e nenhum token será repassado às fases seguintes.
  ]
}
