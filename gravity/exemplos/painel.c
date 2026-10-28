/* Exemplo de painel de controle do LUMNA. */
#include "../src/gravity.h"

int main(void) {
	gv_app *app = gv_app_criar(&(gv_opcoes){ .titulo = "Painel" });
	return gv_app_correr(app, NULL, NULL);
}
