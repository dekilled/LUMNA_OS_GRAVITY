/*
 * O tema: as cores, a forma, o texto e o movimento do LUMNA, num sítio só. Um tema.ini muda o
 * que quiser, chave a chave (o que não aparece fica como está):
 *
 *   # ~/.config/gravity/tema.ini
 *   acento = #f472b6
 *   raio_grande = 18
 *   fonte = Noto Sans
 *   duracao_normal = 300
 *   curva = mola
 *   animacoes = sim
 */
#include "gv_interno.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <time.h>

gv_tema gv_t;

Clay_Color gv_cor(uint32_t rgba) {
	return (Clay_Color){ (rgba >> 24) & 0xff, (rgba >> 16) & 0xff, (rgba >> 8) & 0xff, rgba & 0xff };
}

Clay_Color gv_alfa(Clay_Color cor, float alfa) {
	cor.a = alfa * 255;
	return cor;
}

void gv_tema_padrao(gv_tema *t) {
	*t = (gv_tema){
		.fundo = gv_cor(0x141518ff),
		.superficie = gv_cor(0x1d1e22ff),
		.elevada = gv_cor(0x25262aff),
		.sob_cursor = gv_cor(0x2d2e33ff),
		.selecionado = gv_cor(0x2b2340ff),
		.borda = gv_cor(0x303238ff),
		.borda_controle = gv_cor(0x4a4c53ff),
		.texto = gv_cor(0xf4f1faff),
		.texto_secundario = gv_cor(0xbcb5ceff),
		.texto_suave = gv_cor(0x8e869fff),
		.acento = gv_cor(0xa78bfaff),
		.acento_cursor = gv_cor(0xb79fffff),
		.acento_premido = gv_cor(0x8f73dcff),
		.sobre_acento = gv_cor(0x160f26ff),
		.foco = gv_cor(0xc4b5fdff),
		.sucesso = gv_cor(0x78c6a3ff),
		.aviso = gv_cor(0xe4bb77ff),
		.perigo = gv_cor(0xe58b98ff),
		.info = gv_cor(0x86b8e8ff),
		.desativado = gv_cor(0x625b70ff),
		.divisor = gv_cor(0x8e869fff),
		.veu = gv_cor(0x0b0c0e99),
		.icone = gv_cor(0xf4f1faff),
		.raio_pequeno = 6,
		.raio_medio = 8,
		.raio_grande = 12,
		.espaco = 4,
		.traco_icone = 1.6f,
		.fonte = "Inter, Noto Sans, sans-serif",
		.tamanho_legenda = 13,
		.tamanho_corpo = 15,
		.tamanho_destaque = 17,
		.tamanho_titulo = 20,
		.duracao_rapida = 120,
		.duracao_normal = 200,
		.duracao_lenta = 320,
		.curva = GV_CURVA_SUAVE,
		.animacoes = true,
	};
}

/* Full implementation continues in subsequent commits for remaining functions (paleta_clara, contraste_alto, ler_cor, etc.) */
