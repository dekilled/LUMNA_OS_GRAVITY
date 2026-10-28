/*
 * O Gravity no Wayland: a superfície (janela ou camada), os buffers em memória partilhada, a
 * escala do ecrã, o ponteiro, o toque e o teclado, e o ciclo dos quadros.
 *
 * Um quadro só é desenhado quando algo mudou (entrada, tamanho, uma animação a correr) e ao
 * ritmo do ecrã: depois de cada commit o próximo espera o frame callback do compositor.
 */
#define _GNU_SOURCE
#include "gv_interno.h"

#include <errno.h>
#include <fcntl.h>
#include <linux/input-event-codes.h>
#include <math.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/inotify.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/timerfd.h>
#include <time.h>
#include <unistd.h>
#include <wayland-client.h>
#include <wayland-cursor.h>
#include <xkbcommon/xkbcommon-compose.h>
#include <xkbcommon/xkbcommon.h>
#include <xkbcommon/xkbcommon-keysyms.h>

#include "ext-session-lock-v1-client-protocol.h"
#include "wlr-layer-shell-unstable-v1-client-protocol.h"
#include "xdg-shell-client-protocol.h"

#define GV_MAX_SAIDAS 8
#define GV_MAX_VIGIAS 16
#define GV_MAX_POPUPS 4
#define GV_POPUP_MARGEM 28

struct gv_buffer {
	struct wl_buffer *wl;
	void *dados;
	size_t tamanho;
	int largura, altura;
	bool ocupado;
	cairo_surface_t *superficie;
};

struct gv_saida {
	struct wl_output *wl;
	uint32_t global;
	int escala;
	bool dentro;
	char nome[64];
};

struct gv_popup {
	uint32_t id;
	bool aberto;
	bool declarado;
	bool configurado;
	struct wl_surface *sup;
	struct xdg_surface *xdg_sup;
	struct xdg_popup *xdg_popup;
	struct gv_buffer buffers[2];
	int x, y;
	int w, h;
	Clay_BoundingBox ancora;
	gv_popup_opcoes opcoes;
	double desde;
	uint32_t token;
};

struct gv_vigia {
	int fd;
	void (*pronto)(gv_app *app, int fd, void *dados);
	void *dados;
};

struct gv_app {
	gv_opcoes opcoes;
	struct wl_display *display;
	struct wl_registry *registo;
	struct wl_compositor *compositor;
	struct wl_shm *shm;
	struct wl_seat *seat;
	struct xdg_wm_base *xdg;
	struct zwlr_layer_shell_v1 *camadas;
	struct ext_session_lock_manager_v1 *bloqueio;
	struct wl_surface *superficie;
	struct xdg_surface *xdg_superficie;
	struct xdg_toplevel *janela;
	struct zwlr_layer_surface_v1 *camada;
	struct wl_callback *quadro_cb;
	struct wl_pointer *ponteiro;
	struct wl_touch *toque;
	struct wl_keyboard *teclado;
	struct wl_cursor_theme *cursores;
	struct wl_surface *cursor_sup;
	uint32_t ponteiro_serial;
	uint32_t aperto_serial;
	bool maximizada;
	int32_t dedo;
	struct xkb_context *xkb;
	struct xkb_keymap *mapa;
	struct xkb_state *estado;
	struct xkb_compose_state *compor;
	int repete_fd;
	uint32_t repete_tecla;
	int repete_taxa, repete_atraso;
	double acordar;
	struct tecla_pendente {
		uint32_t sym;
		char utf8[16];
		uint32_t mods;
	} pendentes[64];
	int npendentes;
	bool quadro_com_acao;
	uint32_t mods_reais;
	struct gv_buffer buffers[2];
	int largura, altura;
	int escala;
	bool configurado;
	bool precisa;
	bool a_correr;
	int codigo;
	struct gv_vigia vigias[GV_MAX_VIGIAS];
	int nvigias;
	struct gv_popup popups[GV_MAX_POPUPS];
	float ponteiro_dx, ponteiro_dy;
	bool ponteiro_no_popup;
	float dedo_dx, dedo_dy;
	bool dedo_no_popup;
	uint32_t xdg_versao;
	uint32_t entrada_id;
	bool saidas_mudaram;
	Clay_BoundingBox entrada_caixa;
	Clay_Arena arena;
	double ultimo_quadro;
};

struct gv_entrada gv__entrada = { .x = -1, .y = -1 };
static gv_app *atual;

double gv_agora(void) {
	static struct timespec inicio;
	struct timespec t;
	clock_gettime(CLOCK_MONOTONIC, &t);
	if (!inicio.tv_sec && !inicio.tv_nsec) {
		inicio = t;
	}
	return (t.tv_sec - inicio.tv_sec) + (t.tv_nsec - inicio.tv_nsec) / 1e9;
}

void gv_app_redesenhar(gv_app *app) {
	if (!app) app = atual;
	if (app) app->precisa = true;
}

void gv__app_acordar_em(double segundos) {
	if (!atual) return;
	double quando = gv_agora() + segundos;
	if (atual->acordar <= 0 || quando < atual->acordar) {
		atual->acordar = quando;
	}
}

/* Full buffer management, event listeners, and main loop continue in subsequent updates. */
