#import "grammar.typ": grammar
#import "preprocessor.typ": preprocessor
#import "lexer.typ": lexer
#import "parser.typ": parser

#set document(title: [Especificações do Compilador "Javinha"])

#set text(font: "New Computer Modern", size: 12pt)

#set par(
  justify: true,
  leading: 0.52em,
  first-line-indent: 1em,
)

#set heading(numbering: "1.")

#show title: set text(size: 18pt)
#show title: set align(center)
#show title: set block(below: 3em, above: 3em)
#show heading: set text(size: 13pt, weight: "regular")
#show heading: smallcaps

#grid(
  columns: 1fr,
  align(center)[
    Universidade Federal do Rio Grande do Norte \
    Departamento de Matemática e Informática Aplicada \
    DIM0164 - Compiladores \
    Andriel Vinicius de Medeiros Fernandes, Jeremias Pinheiro de Araújo Andrade, Lucas Vinicius Dantas de Medeiros, Maria Paz Marcato
  ]
)


#title()

= Gramática
#grammar()

= Pré-Processador
#preprocessor()

= Análise Léxica
#lexer()

= Análise Sintática
#parser()
