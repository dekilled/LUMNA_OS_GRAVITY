# GraviTy — LEIA-ME da sintaxe

Um arquivo `.gt` descreve **um componente**: o estado e as funções em C, e o layout em tags. O pré-processador (`gt`) gera C que usa Clay para o layout e Gravity para tema, texto, ícone, foco e entrada.

Não é HTML. As tags parecem HTML para ser rápido de ler. O resultado é C compilado. Não existe DOM.

## Arquivo

Ordem fixa, cada bloco no máximo uma vez:

```html
<script>
/* C. Estado, funções, imports. */
</script>

<style>
/* pacotes de props nomeados. Não é CSS. */
</style>

<box id="raiz">
    <!-- árvore. Um único elemento raiz. -->
</box>
```

`<script>` e `<style>` são opcionais. A árvore não é.

Nome do arquivo vira o nome do componente: `explorador.gt` → `GtExplorador` e `gt_explorador_desenhar`.

## Funções e estado

O `<script>` é C11, com três regras do gerador.

1. **Estado** — variável global do script que não seja `const` vai para a struct do componente. No layout e nas funções você usa o nome puro (`caminho`, `item_ativo`). O gerador lê e escreve `s->caminho`.
2. **Função** — escrita como C normal. O gerador adiciona `GtX *s` como primeiro parâmetro e marca `static`, salvo se a linha anterior for `export`.
3. **`export`** — a função (ou a struct, se for o desenho) aparece no `.h` para o `main` ou para outro componente.

```html
<script>
#include "fs.h"

bool item_ativo;
char caminho[4096];
const int k_max = 256;          /* fica no .c, não entra na struct */

void selecionar(void) {
    item_ativo = !item_ativo;   /* o gerador troca por s->item_ativo */
}

export void gt_definir_caminho(const char *p); /* implementada no script, visível fora */
</script>
```

O que pode aparecer no script:

| Permitido | Vai para |
|---|---|
| `#include` | topo do `.c` gerado |
| `const` / `#define` / `typedef` / `enum` | topo do `.c` |
| variável simples (`bool`, inteiro, `float`, array de `char`, struct já definida) | campo da struct de estado |
| função | `static`, com `s` injetado |
| `import "caminho" as Nome` | ver abaixo |

Não escreva a struct de estado à mão. Não escreva `s->` no `.gt`.

Ponteiros alocados (`malloc`) são por sua conta: guarde o ponteiro num campo e liberte numa função que o `main` chama. O gerador não cria destrutor.

### Importar

```html
<script>
import "componentes/sidebar.gt" as Sidebar;
import "fs.h";
</script>

<box direction="row">
    <Sidebar caminho="caminho" ativo="item_ativo" />
    <Grade />
</box>
```

- `import "arquivo.gt" as Nome` — componente. Vira uma tag `<Nome .../>`. Cada prop é um argumento da função de desenho daquele componente. O estado dele é um campo embutido (`s->sidebar`).
- `import "arquivo.h"` — header C. Copiado como `#include`. Funções de lá são chamadas do script, não viram tag.

Props de componente são só entrada. Para o pai ler algo do filho, o filho exporta uma função.

## Como um valor é escrito

Atributo sem chaves é **literal**. Com `{{ }}` é **expressão C** (vale variável de estado, campo, chamada).

```html
<box width="240" bg="superficie" />
<box width="{{lado}}" bg="{{item_ativo ? 'selecionado' : 'superficie'}}" />
<text>{{nome}}</text>
<button onclick="selecionar">Documentos</button>
<button onclick="{{abrir(caminho)}}">Abrir</button>
```

Regras:

- Número nu = pixels lógicos (os do Gravity, não pixels físicos).
- Cor pode ser token do tema ou hexadecimal.
- `onclick` sem `{{ }}` é o nome da função. Com `{{ }}` é a expressão executada no clique.
- Texto filho de `<text>` e `<button>` é o rótulo. `{{nome}}` no meio de texto concatena.

### Tamanho

| No `.gt` | No C gerado |
|---|---|
| `width="240"` | `CLAY_SIZING_FIXED(240)` |
| `width="grow"` | `CLAY_SIZING_GROW(0)` |
| `width="fit"` | `CLAY_SIZING_FIT(0)` |
| `width="50%"` | `CLAY_SIZING_PERCENT(0.50f)` |
| `min-width="120"` | `.width = CLAY_SIZING_GROW(120)` quando combinado com grow; senão `FIT` com `min` |
| `max-width="480"` | limite `max` do sizing Clay |
| (omitido) | `FIT` naquele eixo |

`height` usa a mesma tabela.

`grow` come o espaço que sobra no eixo do pai. `fit` mede os filhos. Número trava. Porcentagem é fração do pai.

### Cor

Token (lê `gv_t` no quadro, então tema.ini muda sem recompilar):

`fundo` `superficie` `elevada` `sob_cursor` `selecionado` `borda` `borda_controle` `texto` `texto_secundario` `texto_suave` `acento` `acento_cursor` `acento_premido` `sobre_acento` `foco` `sucesso` `aviso` `perigo` `info` `desativado` `divisor` `veu` `icone`

Hex: `#RRGGBB` ou `#RRGGBBAA` → `gv_cor(0xRRGGBBAA)`.

```html
<box bg="elevada" border="1 borda" />
<text color="texto_suave">6 pastas</text>
<box bg="#a78bfa" />
```

### Espaço, raio, borda, direção

| Prop | Valores | C |
|---|---|---|
| `direction` | `row` (padrão) ou `column` | `CLAY_LEFT_TO_RIGHT` / `CLAY_TOP_TO_BOTTOM` |
| `padding` | `16` ou `8 12` ou `8 12 8 12` (topo direita base esquerda) | `CLAY_PADDING_ALL` ou padding por lado |
| `gap` | número | `.childGap` |
| `align` | `start` `center` `end` | alinhamento no eixo cruzado (`childAlignment`) |
| `justify` | `start` `center` `end` | alinhamento no eixo principal |
| `radius` | número | `CLAY_CORNER_RADIUS(n)` |
| `border` | `largura token` | borda Clay nessa cor |
| `scroll` | `true` | contentor com scroll do Clay + gesto do Gravity |
| `opacity` | `0`–`1` | `gv_alfa(cor, valor)` na cor de fundo e do texto |

### Posição

O layout normal **não tem x/y**. Os filhos entram na ordem, no eixo do pai. Isso é o modelo do Clay (flex), não CSS absoluto.

`x` e `y` só valem com `float="true"`. Aí o elemento sai do fluxo e ancora no pai:

```html
<box float="true" x="12" y="{{menu_y}}" z="20" bg="elevada" radius="12">
    <!-- menu de contexto -->
</box>
```

| Prop | Efeito |
|---|---|
| `float` | `CLAY` floating, `attachTo = PARENT` |
| `x` `y` | offset em pixels lógicos |
| `z` | `zIndex` |
| `anchor` | `center` `top-left` `top-right` `bottom-left` `bottom-right` |

Menu, sugestão de caminho e popover usam isso. Grade, sidebar e painel não.

## Classes

Classe não cascateia. É um pacote de props aplicado antes dos atributos do elemento. Atributo ganha da classe.

```html
<style>
.escura {
    bg: elevada;
    radius: 12;
    padding: 12;
    direction: column;
    gap: 8;
}
.item {
    padding: 8 12;
    radius: 8;
    bg: superficie;
}
.item-ativo {
    bg: selecionado;
}
</style>

<box class="escura" width="260">
    <button class="item item-ativo">Documentos</button>
</box>
```

Várias classes se somam da esquerda para a direita. Não há seletor, nem `:hover`. Hover e pressão ficam a cargo do componente (`button`, `toggle`), que já anima com `gv_anim`.

## Tags

Todas aceitam `id`, `class`, `if`. `if="expr"` envolve a tag num `if (expr)` no C. Se a expressão for falsa, a tag não existe naquele quadro.

### box

Contentor. É a tag do `CLAY(...)`.

Aceita: `direction` `width` `height` `min-width` `max-width` `min-height` `max-height` `padding` `gap` `align` `justify` `bg` `radius` `border` `scroll` `opacity` `float` `x` `y` `z` `anchor`.

Filhos: qualquer tag.

```html
<box direction="column" width="grow" height="grow" bg="fundo" padding="16" gap="12">
</box>
```

### text

Texto. Não tem filhos de tag, só texto e `{{ }}`.

| Prop | Valores |
|---|---|
| `size` | `legenda` `corpo` (padrão) `destaque` `titulo` ou um número |
| `color` | token ou hex |
| `wrap` | `true` (padrão) `false` |
| `mono` | `true` `false` |

`size` nomeado lê `gv_t.tamanho_*`. Número vira `fontSize` direto.

### icon

Ícone do mapa do Gravity (`gv_icone`). Sem filhos.

| Prop | Valores |
|---|---|
| `name` | nome do ícone (`home`, `folder`, `file`, `search`, …) |
| `size` | pixels (padrão 24) |
| `color` | token ou hex (padrão `icone`) |

### image

| Prop | Valores |
|---|---|
| `src` | caminho ou `{{expr}}` |
| `width` `height` | tamanho |
| `radius` | recorte |

### button

Linha clicável. Já trata hover, foco e `onclick`.

| Prop | Valores |
|---|---|
| `onclick` | nome de função ou `{{expr}}` |
| `icon` | nome do ícone, desenhado à esquerda do rótulo |
| `selected` | expressão; fundo `selecionado` quando verdadeira |
| `disabled` | expressão |
| `variant` | `plain` (padrão) `accent` `ghost` `danger` |

Aceita também as props de caixa: `width` `height` `padding` `radius` `bg`.

Filho: texto do rótulo. Ícone extra, se precisar de outro além de `icon`, é um `<icon>` filho.

### input

Campo de texto do Gravity.

| Prop | Valores |
|---|---|
| `value` | nome do campo `char[]` do estado (obrigatório) |
| `placeholder` | texto |
| `onchange` | função chamada quando o texto muda |
| `onsubmit` | função no Enter |
| `multiline` | `true` `false` |

### scroll

Atalho de `<box scroll="true">`. Mesmas props de `box`.

### spacer

Espaço vazio.

| Prop | Valores |
|---|---|
| `size` | pixels no eixo do pai |
| `grow` | `true` — come o resto |

### toggle

Interruptor.

| Prop | Valores |
|---|---|
| `value` | nome de um `bool` do estado |
| `onchange` | função |

### slider

| Prop | Valores |
|---|---|
| `value` | nome de um `float` ou `int` (0–1, ou na faixa `min`–`max`) |
| `min` `max` | números |
| `onchange` | função |

### select

Lista fechada. Filhos `<option value="grade">Grade</option>`.

| Prop | Valores |
|---|---|
| `value` | nome do campo (inteiro ou enum) |
| `onchange` | função |

### if

```html
<if test="item_ativo">
    <Painel />
</if>
```

`test` é expressão C. Sem props visuais.

### for

```html
<for item="arq" in="arquivos" index="i">
    <Cartao nome="{{arq.nome}}" onclick="{{escolher(i)}}" />
</for>
```

`in` é o nome de um array no estado mais a contagem `arquivos_n` (convenção: campo `T arquivos[N]` e `int arquivos_n`). O gerador emite o `for` em C. `item` e `index` só existem dentro do bloco.

## Componente novo

1. Criar `componentes/nome.gt` com script + um elemento raiz.
2. Props de entrada são declaradas no script com o prefixo que o gerador reconhece:

```html
<script>
prop const char *nome;
prop bool ativo;
prop void (*onclick)(void);
</script>
```

`prop` não entra no estado persistente. É argumento de `gt_nome_desenhar`.

3. Usar com `import "componentes/nome.gt" as Nome` e a tag `<Nome nome="{{arq.nome}}" ativo="{{i == selecionado}}" onclick="escolher" />`.

Props `onclick` do tipo função recebem a função do pai. O gerador passa o ponteiro certo, já com o `s` do pai preso num wrapper.

## O que cada tela do explorador usa

| Tela | Tags |
|---|---|
| Grade (pastas e arquivos) | `box` `scroll` `for` `icon` `image` `text` `button` |
| Barra lateral | `box` `button` `icon` `text` |
| Caminho com sugestões | `input` `box float` `for` `if` |
| Menu de contexto | `box float` `button` `if` |
| Exibição | `toggle` `slider` `select` `text` |
| Editor da barra | `for` `toggle` `button` `input` |

Se uma tela precisar de tag que não está nesta lista, a tag entra primeiro aqui, depois no gerador.

## Exemplo fechado — o que o gerador emite

Entrada `sidebar.gt`:

```html
<script>
bool item_ativo;
void selecionar(void) { item_ativo = !item_ativo; }
</script>

<box id="sidebar" direction="column" width="260" height="grow" bg="elevada" padding="12" gap="8">
    <text size="titulo">Espaços</text>
    <button icon="folder" selected="item_ativo" onclick="selecionar">Documentos</button>
</box>
```

Saída (forma, não byte a byte):

```c
typedef struct { bool item_ativo; } GtSidebar;

static void gt_sidebar_selecionar(GtSidebar *s) {
    s->item_ativo = !s->item_ativo;
}

void gt_sidebar_desenhar(GtSidebar *s) {
    CLAY(CLAY_ID("sidebar"), {
        .layout = {
            .layoutDirection = CLAY_TOP_TO_BOTTOM,
            .sizing = { .width = CLAY_SIZING_FIXED(260), .height = CLAY_SIZING_GROW(0) },
            .padding = CLAY_PADDING_ALL(12),
            .childGap = 8
        },
        .backgroundColor = gv_t.elevada
    }) {
        /* text size titulo → gv_t.tamanho_titulo, cor gv_t.texto */
        /* button → componente Gravity; onclick chama gt_sidebar_selecionar(s) */
    }
}
```

O `main` da app não é gerado:

```c
static GtSidebar sidebar;
static void desenhar(gv_app *app, void *dados) {
    (void)app; (void)dados;
    gt_sidebar_desenhar(&sidebar);
}
int main(void) {
    gv_app *app = gv_app_criar(&(gv_opcoes){ .titulo = "LUMNA Explorer", .largura = 1200, .altura = 700 });
    return gv_app_correr(app, desenhar, NULL);
}
```

## Erros que o `gt` deve recusar

- Tag que não está neste arquivo.
- Prop que a tag não lista.
- `x` / `y` sem `float="true"`.
- `width` que não é número, `grow`, `fit` ou `N%`.
- Cor que não é token nem hex.
- `onclick` apontando para função que não existe no script nem num `#include`.
- Dois elementos raiz.
- `import` de `.gt` sem `as`.
