# Plano — GraviTy Framework, construído junto com o Explorador

O framework não nasce genérico e vazio. Cada marco existe porque uma tela do LUMNA Explorer precisa dela. O que não for usado pelo explorador fica de fora até existir um segundo app.

Abordagem fechada: **pré-processador**. Arquivo `.gt` (layout estilo tags + `<script>` em C) vira `.c`/`.h` que chamam Clay e Gravity. Em runtime não há interpretador.

## O que já existe

- `gravity/` — UI imediata em C (Clay + Wayland). O código gerado **só** pode chamar essa API.
- `compositor/` — Orbit. O explorador é cliente, não mexe no compositor.
- `framework/componentes/sidebar.gt` — esboço do formato. A sintaxe oficial está em [LEIA-ME.md](LEIA-ME.md).

## Pipeline

```text
.app .gt
   │  1. léxico + parser  →  AST (script, style, árvore de tags)
   │  2. resolve imports, classes, componentes
   │  3. checa props (tag desconhecida ou prop ilegal = erro com linha)
   │  4. gera C
   ▼
.c / .h   →  compila com gravity →  binário
```

Três módulos, nesta ordem, sem pular:

| Módulo | Arquivo | Faz |
|---|---|---|
| Parser | `framework/preprocessor/gt_parse.c` | `.gt` → AST |
| Checagem | `framework/preprocessor/gt_check.c` | tags, props, tipos de valor, imports |
| Geração | `framework/preprocessor/gt_emit.c` | AST → C |

Entrada de linha de comando:

```text
gt explorador.gt -o build/explorador.c
```

Um `.gt` gera um par. O `main` da app é escrito à mão em C e só chama a função de desenho gerada.

## Forma do C gerado

Cada arquivo vira um componente com estado próprio.

```c
typedef struct {
    bool item_ativo;
    char caminho[4096];
} GtExplorador;

void gt_explorador_desenhar(GtExplorador *s);
```

Regras de geração:

- Variáveis do `<script>` que não são `const` entram na struct.
- Funções do `<script>` viram funções `static` e recebem `GtExplorador *s` como primeiro argumento, injetado pelo gerador. No `.gt` a função é escrita normal (`void abrir(void)`), sem o ponteiro.
- `const` e `typedef` / `#include` passam direto para o `.c`.
- O desenho é uma função chamada todo quadro. Nada de objeto retido. Clay recalcula o layout.
- `id` vira `CLAY_ID("...")`. Sem `id`, o gerador cria um id estável a partir do caminho na árvore (`sidebar/docs`).
- Expressão em atributo (`selected="s->item_ativo"` no .gt é só `selected="item_ativo"`) vira o C correspondente dentro de `if` ou do argumento.

## Marcos (o explorador puxa o framework)

### Marco 0 — Contrato
Este plano + [LEIA-ME.md](LEIA-ME.md). Sem código de gerador além do necessário para travar a sintaxe.

### Marco 1 — Pipeline mínimo que compila
Tags: `box`, `text`, `button`, `icon`.
Props: `id`, `direction`, `width`, `height`, `padding`, `gap`, `bg`, `color`, `size`, `radius`, `onclick`.
Script: funções copiadas, uma variável `bool`.
Prova: `componentes/sidebar.gt` gera C que compila contra `gravity/` e desenha a coluna Espaços.

### Marco 2 — Casca do explorador (mockup da grade)
Arquivo `apps/explorador/explorador.gt`.
Layout: `sidebar` | coluna (`topbar` + `grade`) | `painel`.
Tags novas exigidas por esta tela: `scroll`, `image`, `if`.
Dados: lista estática de pastas e arquivos (ainda sem `readdir`).
Componentes: `Sidebar`, `TopBar`, `Grade`, `Cartao`, `Painel`.
Prova: janela com a estrutura dos mockups (grade de pastas roxas, painel direito, barra de cima).

### Marco 3 — Estado e lista
- `for` sobre um array.
- Clique num cartão preenche o painel (tipo, itens, caminho, modificado).
- `selected` e `if` reativos ao estado.
Prova: selecionar “Projetos” muda o painel sem recompilar lógica à mão.

### Marco 4 — Caminho e menu de contexto
Telas: edição de caminho com sugestões; menu do botão direito.
Tags novas: `input`, `floating` (ou prop `float` em `box`).
Prova: Ctrl+L foca o campo; digitar `Pro` lista sugestões; botão direito abre o menu com submenus (Fixar na barra lateral).

### Marco 5 — Configuração
Telas: Exibição e editor da barra lateral.
Tags novas: `toggle`, `slider`, `select`.
Estado: modo da grade (grade / lista / lista grande), tamanho do ícone, ocultos, extensões.
Prova: mudar o slider de 128 px altera `width`/`height` dos cartões no quadro seguinte.

### Marco 6 — Disco de verdade
`import` de C existente (`gravity/servicos` ou um `apps/explorador/fs.c` com `opendir`/`stat`).
O `<script>` chama essas funções; o layout não conhece POSIX.
Prova: abrir `~/Documentos` lista o diretório real.

## Ordem de trabalho dentro de cada marco

1. Escrever o `.gt` da tela, mesmo que o gerador ainda não aceite a tag.
2. Se a tag não existe, acrescentar a linha dela no LEIA-ME **antes** de codar o emit.
3. Estender parser, checagem e emit só dessa tag.
4. Compilar o C gerado com um `main` de 15 linhas (`gv_app_criar` + `gv_app_correr`).
5. Só então partir para a próxima tela.

## Fora de escopo até o marco 6

- Interpretador em runtime.
- CSS de verdade (cascata, especificidade, media query). Classe é só um pacote de props.
- Hot reload. Recompilar é o ciclo.
- Bindings que não sejam C (nada de linguagem de script própria).
- Layout absoluto como padrão. Posição x/y só em flutuante (menu, sugestão, popup).

## Critério de “framework pronto para o segundo app”

Um segundo `.gt` (por exemplo a barra ou o painel de volume) compila sem mudar o gerador, só compondo as tags do LEIA-ME. Aí o contrato está fechado.
