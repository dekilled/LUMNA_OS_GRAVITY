/* O que as partes do Gravity dividem entre si (as apps usam só o gravity.h). */
#ifndef GV_INTERNO_H
#define GV_INTERNO_H

#include <cairo.h>
#include <stddef.h>

#include "gravity.h"

#define GV_MAX_TECLAS 16

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/* ---- memória do quadro: textos e dados dos elementos, apagada a cada quadro ---- */
void *gv__quadro_alloc(size_t tamanho);
void gv__quadro_limpar(void);

/* ---- entrada do quadro (gv_app.c enche, os componentes lêem) ---- */
struct gv_entrada {
	float x, y;
	bool baixo;
	bool apertou;
	bool soltou;
	bool toque;
	uint32_t ativo;
	bool travado;
	bool direito;
	bool segurou;
	bool segurar_cancelado;
	double aperto_em;
	float aperto_x, aperto_y;
	uint32_t teclas[GV_MAX_TECLAS];
	int nteclas;
	char texto[128];
	uint32_t mods;
	uint32_t serial_tecla;
	float roda_x, roda_y;
	struct {
		bool chegou;
		char escrito[512];
		char compondo[256];
		int compondo_ini, compondo_fim;
		uint32_t apagar_antes, apagar_depois;
	} ime;
};
extern struct gv_entrada gv__entrada;

/* Internal APIs for focus, selection, images, writing, etc. Full header available in source. */
#endif
