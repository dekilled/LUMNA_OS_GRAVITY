/*
 * As janelas abertas no compositor (wlr-foreign-toplevel-management): título, app id, se está
 * ativa ou minimizada, e os pedidos de ativar e fechar. É o que a dock e o alternador usam.
 * Só existe se o compositor tiver o protocolo (o Orbit tem).
 */
#include "gv_interno.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wayland-client.h>

#include "wlr-foreign-toplevel-management-unstable-v1-client-protocol.h"

#define GV_MAX_JANELAS 64

struct janela {
	struct zwlr_foreign_toplevel_handle_v1 *alca;
	gv_janela publica;
	char titulo[256], app_id[128];
	char titulo_novo[256], app_id_novo[128];
	bool ativa_nova, minimizada_nova, maximizada_nova, tela_cheia_nova;
	struct wl_output *saida;
};

static struct zwlr_foreign_toplevel_manager_v1 *gestor;
static struct wl_seat *seat;
static struct janela janelas[GV_MAX_JANELAS];
static int njanelas;
static uint32_t proximo_id = 1;

/* Core window management for Orbit compositor integration. Full implementation continues. */
