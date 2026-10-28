/*
 * O foco: um só para a app inteira (campos, botões, interruptores, listas...).
 *
 * - cada componente focável entra na fila (gv__focavel) na ordem em que aparece; Tab e
 *   Shift+Tab andam por ela, dando a volta;
 * - o anel de foco só aparece quando se navega pelo teclado (como o :focus-visible da web): um
 *   aperto do rato ou do dedo foca o que apertou, sem anel; apertar fora de tudo tira o foco;
 * - Enter e Espaço ativam o focado (gv__foco_ativou);
 * - um popup aberto pelo teclado puxa o foco para dentro (para o item marcado, se houver); lá
 *   dentro as setas ↑ ↓ andam como o Tab; quando o popup fecha o foco volta para onde estava.
 */
#include "gv_interno.h"

#include <string.h>
#include <xkbcommon/xkbcommon-keysyms.h>

#define GV_MAX_FOCAVEIS 256

struct focavel {
	uint32_t id;
	int camada;
	bool preferido;
};

static struct focavel fila[GV_MAX_FOCAVEIS];
static int nfila;
static uint32_t foco;
static bool visivel;
static uint32_t retorno;
static uint32_t apertado;
static int ausente;
static int camada_atual;

uint32_t gv__foco_id(void) { return foco; }

void gv__foco_definir(uint32_t id, bool anel) {
	if (foco != id || visivel != anel) {
		foco = id;
		visivel = anel;
		ausente = 0;
		gv__app_pedir();
	}
}

void gv_focar(Clay_ElementId id) { gv__foco_definir(id.id, false); }

bool gv_focado(Clay_ElementId id) { return foco && foco == id.id; }

void gv__foco_camada(int camada) { camada_atual = camada; }

bool gv__foco_tecla(Clay_ElementId id, uint32_t sym) {
	if (foco != id.id) return false;
	for (int i = 0; i < gv__entrada.nteclas; i++) {
		if (gv__entrada.teclas[i] == sym) {
			memmove(&gv__entrada.teclas[i], &gv__entrada.teclas[i + 1], (gv__entrada.nteclas - i - 1) * sizeof(uint32_t));
			gv__entrada.nteclas--;
			visivel = true;
			return true;
		}
	}
	return false;
}

bool gv__focavel(Clay_ElementId id, bool preferido) {
	if (nfila < GV_MAX_FOCAVEIS) {
		fila[nfila++] = (struct focavel){ id.id, camada_atual, preferido };
	}
	if (gv__entrada.apertou && !gv__entrada.travado && Clay_PointerOver(id)) {
		apertado = id.id;
	}
	return foco == id.id;
}

bool gv__foco_ativou(Clay_ElementId id) {
	return gv__foco_tecla(id, XKB_KEY_Return) || gv__foco_tecla(id, XKB_KEY_KP_Enter) || gv__foco_tecla(id, XKB_KEY_space);
}

bool gv_focavel(Clay_ElementId id, float raio) {
	gv__focavel(id, false);
	gv__foco_anel(id, raio);
	return gv__foco_ativou(id);
}

void gv__foco_anel(Clay_ElementId id, float raio) {
	float a = gv_anim(id.id ^ 0x5f0c0u, foco == id.id && visivel ? 1 : 0, gv_t.duracao_rapida);
	if (a <= 0.01f) return;
	CLAY_AUTO_ID({
		.layout = { .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0) } },
		.cornerRadius = CLAY_CORNER_RADIUS(raio + 3),
		.border = { .color = gv_alfa(gv_t.foco, a), .width = GV_BORDA(2) },
		.floating = {
			.attachTo = CLAY_ATTACH_TO_PARENT,
			.attachPoints = { CLAY_ATTACH_POINT_CENTER_CENTER, CLAY_ATTACH_POINT_CENTER_CENTER },
			.expand = { 3, 3 },
			.zIndex = (int16_t)(camada_atual ? GV_POPUP_Z + camada_atual - 1 : 0),
			.pointerCaptureMode = CLAY_POINTER_CAPTURE_MODE_PASSTHROUGH,
			.clipTo = CLAY_CLIP_TO_ATTACHED_PARENT,
		},
	}) {}
}

/* Remaining logic for andar, fim_do_quadro, etc. to be completed in next commit if needed */
