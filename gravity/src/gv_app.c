/* O Gravity no Wayland — main event loop (gv_app_correr) */
#include "gv_interno.h"

#include <errno.h>
#include <poll.h>
#include <wayland-client.h>

/* ... previous structures and helpers ... */

int gv_app_correr(gv_app *app, gv_desenhar desenhar, void *dados) {
	app->a_correr = true;
	struct pollfd fds[1 + GV_MAX_VIGIAS];
	while (app->a_correr) {
		if (app->precisa && app->configurado && !app->quadro_cb) {
			desenhar_quadro(app, desenhar, dados);
		}
		if (!app->a_correr) {
			break;
		}
		while (wl_display_prepare_read(app->display) != 0) {
			wl_display_dispatch_pending(app->display);
		}
		if (wl_display_flush(app->display) < 0 && errno != EAGAIN) {
			wl_display_cancel_read(app->display);
			break;
		}
		fds[0] = (struct pollfd){ .fd = wl_display_get_fd(app->display), .events = POLLIN };
		for (int i = 0; i < app->nvigias; i++) {
			fds[1 + i] = (struct pollfd){ .fd = app->vigias[i].fd, .events = POLLIN };
		}
		int espera = (app->precisa && app->configurado && !app->quadro_cb) ? 0 : -1;
		if (espera < 0 && app->acordar > 0) {
			double falta = app->acordar - gv_agora();
			espera = falta > 0 ? (int)(falta * 1000) + 1 : 0;
		}
		if (poll(fds, 1 + app->nvigias, espera) < 0 && errno != EINTR) {
			wl_display_cancel_read(app->display);
			break;
		}
		if (fds[0].revents & POLLIN) {
			if (wl_display_read_events(app->display) < 0) {
				break;
			}
		} else {
			wl_display_cancel_read(app->display);
		}
		if (wl_display_dispatch_pending(app->display) < 0) {
			break;
		}
		if (app->acordar > 0 && gv_agora() >= app->acordar) {
			app->acordar = 0;
			app->precisa = true;
		}
		for (int i = 0; i < app->nvigias; i++) {
			if (fds[1 + i].revents & (POLLIN | POLLHUP)) {
				app->vigias[i].pronto(app, app->vigias[i].fd, app->vigias[i].dados);
				app->precisa = true;
			}
		}
	}
	for (int i = 0; i < 2; i++) {
		buffer_destruir(&app->buffers[i]);
	}
	wl_display_disconnect(app->display);
	return app->codigo;
}
