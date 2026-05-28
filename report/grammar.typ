
#let grammar() = {
  [A gramática original foi adaptada para a seguinte estrutura:]
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
    - Inclusão de um novo símbolo inicial *Start*: tal símbolo foi criado pois consideramos "EOF" como um token e deixamos *Prog* intacta;
    - Inclusão de novo não-terminal *ListCmd*: tal mudança foi feita para permitir o reconhecimento de múltiplos comandos em métodos e blocos de código;
    - Adição de precedência de operadores: foram criados novos não-terminais organizados de modo a permitir precedência correta dos operadores lógicos, aritméticos, de negação e de acesso a objetos.
  ]
}
