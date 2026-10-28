/*
 * O Gravity no Wayland: a superfície (janela ou camada), o loop de eventos, o
 * Clay, o desenho e a entrada. É o coração da app.
 */
#include "gv_interno.h"

#include <cairo.h>
#include <errno.h>
#include <fcntl.h>
#include <math.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <time.h>
#include <unistd.h>
#include <wayland-client.h>

/* Core structures and functions for Wayland surface, event loop, Clay integration
 * and input handling. Full implementation (including layer-shell, xdg-shell,
 * buffer management and the main run loop) is available in the source. */

gv_app *gv_app_criar(const gv_opcoes *opcoes) {
	/* Initialize Wayland display, registry, surface and Clay arena. */
	return NULL; /* placeholder — full version in local source */
}

int gv_app_correr(gv_app *app, gv_desenhar desenhar, void *dados) {
	/* Main event loop: poll, dispatch, layout with Clay, draw with Cairo, commit. */
	return 0;
}
