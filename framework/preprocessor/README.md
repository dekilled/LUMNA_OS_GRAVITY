# Pré-processador GraviTy

Transforma arquivos `.gt` em código C + Clay.

## Formato .gt (exemplo)

```html
<script>
bool aberto = false;
void toggle(void) { aberto = !aberto; }
</script>

<box id="sidebar" padding="16">
    <button onclick="toggle()">Menu</button>
    <text if="aberto">Conteúdo</text>
</box>
```

O pré-processador gera funções C que usam as macros CLAY.
