/* Exemplo de barra superior do LUMNA usando Gravity. */
#include "../src/gravity.h"

int main(void) {
	gv_app *app = gv_app_criar(&(gv_opcoes){ .titulo = "Barra", .tipo = GV_CAMADA });
	return gv_app_correr(app, NULL, NULL);
}
