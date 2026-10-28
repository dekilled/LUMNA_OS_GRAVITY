/* Exemplo de painel de rede. */
#include "../src/gravity.h"

int main(void) {
	gv_app *app = gv_app_criar(&(gv_opcoes){ .titulo = "Rede" });
	return gv_app_correr(app, NULL, NULL);
}
