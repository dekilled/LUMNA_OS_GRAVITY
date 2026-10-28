/* Exemplo de lançador do LUMNA usando Gravity. */
#include "../src/gravity.h"

int main(void) {
	gv_app *app = gv_app_criar(&(gv_opcoes){ .titulo = "Lançador" });
	return gv_app_correr(app, NULL, NULL);
}
