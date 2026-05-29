
#let grammar() = {
  [
    == Gramática Final
    A gramática original foi adaptada para a seguinte estrutura:]
  ```bnf
  Start    -> Prog 'EOF'
  Prog     -> MainC DefCl
  MainC    -> 'class' Id '{' 'public' 'static' 'void' 'main' '(' 'String' '[' ']' Id ')' '{' ListCmd '}' '}'
  DefCl    -> 'class' Id '{' DefVar DefMet '}' DefCl
            | 'class' Id 'extends' Id '{' DefVar DefMet '}' DefCl
            | λ
  DefVar   -> Type Id ';' DefVar
            | λ
  DefMet   -> 'public' Type Id '(' Args ')' '{' DefVar ListCmd 'return' Exp ';' '}' DefMet
            | 'public' Type Id '(' ')' '{' DefVar ListCmd 'return' Exp ';' '}' DefMet
            | λ
  Type     -> 'int' '[' ']'
            | 'boolean'
            | 'int'
            | Id
  Args     -> Type Id
            | Type Id ',' Args
  ListCmd  -> Cmd ListCmd | λ
  Cmd      -> '{' ListCmd '}'
            | 'if' '(' Exp ')' Cmd 'else' Cmd
            | 'while' '(' Exp ')' Cmd
            | 'System' '.' 'out' '.' 'println' '(' Exp ')' ';'
            | Id '=' Exp ';'
            | Id '[' Exp ']' '=' Exp ';'
  Exp      -> Greater Exp2
  Exp2     -> '&&' Greater Exp2 | λ
  Greater  -> Add Greater2
  Greater2 -> '>' Add Greater2 | λ
  Add      -> Mul Add2
  Add2     -> '+' Mul Add2 | '-' Mul Add2 | λ
  Mul      -> Neg Mul2
  Mul2     -> '*' Neg Mul2 | λ
  Neg      -> '!' Neg | Obj
  Obj      -> ObjAtom ObjMet
  ObjMet   -> '.' 'length' ObjMet
            | '.' Id '(' ListExp ')' ObjMet
            | '[' Exp ']' ObjMet
            | λ
  ObjAtom  -> 'new' Id '(' ')'
            | 'new' 'int' '[' Exp ']'
            | '(' Exp ')'
            | 'true'
            | 'false'
            | Id
            | Number
            | 'this'
  ListExp  -> Exp ',' ListExp
            | Exp
            | λ
  Id       -> Char Word
            | Char
  Char     -> 'a' | ... | 'z' | 'A' | ... | 'Z'
  Word     -> '_' | Char Word | Number Word | Char | Number
  Number   -> '0' | '1' | ... | '9'
  ```

  [As principais mudanças consistiram em:
    - Inclusão de um novo símbolo inicial *Start*: tal símbolo foi criado o símbolo "EOF" é interpretado como um token e foi feita a decisão de deixar a variável *Prog* intacta;
    - Inclusão de novo não-terminal *ListCmd*: tal mudança foi feita para permitir o reconhecimento de múltiplos comandos em métodos e blocos de código;
    - Adição de precedência de operadores: foram criados novos não-terminais organizados de modo a permitir precedência correta dos operadores lógicos, aritméticos, de negação e de acesso a objetos.
  ]
  [
    == Representação em Código
    A representação da gramática no código foi feita com a criação das seguintes estruturas:

    #table(
      columns: (auto, 0.2fr, 1fr),
      stroke: 0.5pt,
      [*Nome*], [*Tipo*], [*Descrição*],
      [SymbolType], [Enum], [Tipos dos símbolos gramaticais: Não-terminal, terminal ou lambda],
      [NT], [Enum], [Associa cada não-terminal da gramática a um inteiro],
      [T], [Enum], [Associa cada terminal da gramática a um inteiro],
      [GSymbol], [Struct], [Representa, de fato, um símbolo de acordo com seu tipo e valor numérico],
      [GProduction], [Struct], [Representa 1 produção da gramática, formada por uma sequência de até 24 GSymbols],
      [GRule], [Struct], [Representa todo o conjunto de produções para um não-terminal],
    )

    Além disso, foram criadas funções auxiliares `symbol_to_string` e `type_to_string` para uso no debug, convertendo cada um dos enums mencionados em Strings.
  ]
}
