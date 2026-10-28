/* Updated gv_app.c — core structures already present.
 * Buffer management and main event loop skeleton added below.
 */
#include "gv_interno.h"

/* ... previous structures ... */

static struct gv_buffer *buffer_livre(struct gv_buffer *bufs, int n) {
	for (int i = 0; i < n; i++) {
		if (!bufs[i].ocupado) return &bufs[i];
	}
	return NULL;
}

/* Full buffer creation, Wayland listeners and gv_app_correr loop continue in next updates. */
