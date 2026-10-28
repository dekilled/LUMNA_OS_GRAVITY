/*
 * Os comandos do Clay no cairo: retângulos de cantos redondos (com sombra macia, se o elemento
 * tem gv_sombra), bordas, texto, recortes e os elementos próprios do Gravity (ícone,
 * deslizante, interruptor).
 */
#include "gv_interno.h"

#include <math.h>

static void cor(cairo_t *cr, Clay_Color c) {
	cairo_set_source_rgba(cr, c.r / 255.0, c.g / 255.0, c.b / 255.0, c.a / 255.0);
}

static void redondo(cairo_t *cr, double x, double y, double w, double h, Clay_CornerRadius r) {
	double max = fmin(w, h) / 2;
	double tl = fmin(r.topLeft, max), tr = fmin(r.topRight, max);
	double bl = fmin(r.bottomLeft, max), br = fmin(r.bottomRight, max);
	cairo_new_sub_path(cr);
	cairo_arc(cr, x + w - tr, y + tr, tr, -M_PI / 2, 0);
	cairo_arc(cr, x + w - br, y + h - br, br, 0, M_PI / 2);
	cairo_arc(cr, x + bl, y + h - bl, bl, M_PI / 2, M_PI);
	cairo_arc(cr, x + tl, y + tl, tl, M_PI, 3 * M_PI / 2);
	cairo_close_path(cr);
}

static Clay_CornerRadius todos(float r) {
	return (Clay_CornerRadius){ r, r, r, r };
}

/* Sombra macia: camadas cada vez maiores e mais fracas (o cairo não tem desfoque). */
static void sombra(cairo_t *cr, Clay_BoundingBox b, Clay_CornerRadius r, const struct gv_extra *s) {
	const int passos = 12;
	double d = s->sombra.desfoque;
	for (int i = passos; i >= 1; i--) {
		double f = (double)i / passos;
		double cresce = d * f;
		double alfa = s->sombra.alfa * (1 - f) * (1 - f) * 2.2 / passos;
		cairo_set_source_rgba(cr, 0, 0, 0, alfa);
		Clay_CornerRadius rr = { r.topLeft + cresce, r.topRight + cresce, r.bottomLeft + cresce, r.bottomRight + cresce };
		redondo(cr, b.x - cresce, b.y - cresce + s->sombra.desloca_y, b.width + 2 * cresce, b.height + 2 * cresce, rr);
		cairo_fill(cr);
	}
}

/* Core drawing functions for icons, sliders, switches, etc. Full file available in source. */
