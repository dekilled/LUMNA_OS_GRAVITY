/* Exemplo de menu Wi-Fi do LUMNA. */
#include "../src/gravity.h"

int main(void) {
	gv_app *app = gv_app_criar(&(gv_opcoes){ .titulo = "Wi-Fi", .tipo = GV_CAMADA });
	return gv_app_correr(app, NULL, NULL);
}
