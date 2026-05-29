#let parser() = {
  [
    O Analisador Sintático é o componente responsável por verificar
    se a sequência de tokens gerada pelo Analisador Léxico está de
    acordo com as regras gramaticais da linguagem compilada,através
    da análise da estrutura das sentenças. Este projeto implementou o
    Algoritmo de Earley, um parser de propósito geral capaz de
    reconhecer qualquer gramática livre de contexto (GLC).

    == Algoritmo de Earley

    Proposto por Jay Earley em 1970, o algoritmo de Earley é um parser
    baseado em programação dinâmica capaz de analisar qualquer gramática
    livre de contexto em tempo $O(n^3)$ no caso geral, $O(n^2)$ para
    gramáticas não-ambíguas e $O(n)$ para gramáticas não-ambíguas e bem
    estruturadas. O algoritmo mantém uma tabela de estados, composta de $n+1$ conjuntos de estados. Cada estado tem a forma:

    $ [A -> alpha dot beta, j] $

    Onde $A -> alpha beta$ é uma produção da gramática, o ponto ($dot$) indica
    o limite da análise de uma determinada produção e $j$ é a posição
    de início do reconhecimento do item. Três operações fazem o chart
    crescer:
    - Predição: quando o próximo símbolo esperado é um não-terminal, adiciona todos os itens de Earley correspondentes às produções desse não terminal.
    - Varredura: quando o próximo símbolo esperado é um terminal e  corresponde ao token atual da entrada, avança o ponto e adiciona o item ao próximo conjunto.
    - Completação: quando um item está completo, avança o ponto de todos os itens que estavam esperando esse não-terminal.

    == Implementação

    === Função Principal

    A função `earley_parser` é o ponto de entrada do parser. Ela orquestra
    todo o processo de análise sintática seguindo o algoritmo de Earley
    clássico:
    - Inicialização: cria o chart com $n+1$ posições e insere o estado inicial  $[S -> dot α, 0]$ em `chart[0]`.
    - Loop Principal: itera de i=0 até i=n, e em cada coluna executa um laço interno até que nenhum novo estado seja adicionado (ponto fixo).
    - Para cada estado na coluna atual, se completo, chama `complete`; se o próximo símbolo é não-terminal, chama `predict`; se terminal, chama `scan`.
    - Detecção de erros: se `chart[i+1]` estiver vazio após processar `chart[i]`, um erro sintático é detectado e o mecanismo de recuperação é ativado.
    - Resultado: ao final, chama `has_ended()` para verificar se a análise  foi bem sucedida.

    === Predição:

    A função `predict` implementa a operação
    de Predição. Dado um estado cujo próximo símbolo esperado é
    um símbolo não-terminal, ela busca todas as produções desse
    não-terminal na gramática e insere novos itens em `chart[it]` com o ponto no início. Esse mecanismo permite que o parser calcule previamente quais estruturas gramaticais podem aparecer na
    posição atual.

    === Varredura

    A função `scan` implementa a operação de Varredura. Ela compara
    o próximo símbolo esperado pelo estado com o token na posição `it`
    da entrada, convertido via `map_token`. Depois disso, se
    coincidem, avança o ponto e insere o novo estado em
    `chart[it+1]`. Também é tratado o caso especial de produções
    `lambda`, onde o ponto avança sem consumir nenhum token.

    === Completação

    A função `complete` implementa a operação de Completação. Quando
    um item está completo (ponto ao final da produção), ela volta
    ao conjunto `chart[state.start]` e procura todos os itens que
    estavam esperando o não-terminal recém completado. Para cada
    um desses itens, cria uma versão com o ponto avançado e insere
    em `chart[it]`.

    === Verificação de aceite

    A função `has_ended` verifica se a análise foi bem-sucedida. Ela
    percorre `chart[n]`, que é a última coluna, em busca de um estado
    completo com `lhs` igual ao símbolo inicial da gramática e start
    igual a 0. Isso significa que a gramática reconheceu toda a
    sequência de tokens a partir do início.

    === Mapeamento de tokens

    A função estática `map_token` converte os tokens do analisador
    léxico para o tipo T, que é o terminal da gramática. Ela realiza
    um mapeamento exaustivo cobrindo todos os tipos de tokens
    reconhecidos pela linguagem alvo. Tal função foi elaborada pois o Analisador Léxico gera tokens de forma "isolada" sem considerar o contexto dos símbolos gramaticais.

    === Mecanismo de recuperação de erros

    No parser, o erro é detectado quando, após processar completamente o
    `chart[i]`, o conjunto `chart[i+1]` permanece vazio. Isso indica que
    nenhum estado da coluna $i$ conseguiu consumir o token da posição
    `tolens[i]`. Nesse momento, o parser registra o erro com informações sobre a posição, o token inesperado e os terminais que eram
    esperados.

    A estratégia adotada é baseada na técnica de recuperação por pânico. O algoritmo funciona da seguinte
    maneira:
    - Avança na entrada procurando um token de sincronização: ponto e  vírgula, chave de fechamento ou colchete de fechamento.
    - Para cada candidato $k$ encontrado, retrocede no chart procurando uma coluna anterior c onde algum estado estivesse esperando esse exato token de sincronização.
    - Se encontrado, restaura `chart[k]` com o conteúdo de `chart[c]` e  retoma a análise a partir do token $k$, descartando os tokens entre $i$ e $k-1$.

    Essa abordagem é conservadora e eficaz, pois ao reutilizar um estado
    anterior do chart, o parser retoma a análise em um contexto
    gramaticalmente coerente, permitindo detectar erros subsequentes de
    forma eficiente.
  ]
}

#parser()
