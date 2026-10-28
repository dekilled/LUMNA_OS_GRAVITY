/* Continued gv_app.c — event loop skeleton */
#include "gv_interno.h"

/* ... previous content ... */

int gv_app_correr(gv_app *app, gv_desenhar desenhar, void *dados) {
	if (!app) return 1;
	atual = app;
	app->a_correr = true;

	while (app->a_correr) {
		/* poll Wayland + vigias + timer */
		/* dispatch events */
		/* if app->precisa: Clay_BeginLayout / EndLayout / draw / commit */
		/* wait for frame callback */
	}

	return app->codigo;
}

/* Full poll loop, listeners and buffer commit logic continue in next updates. */
