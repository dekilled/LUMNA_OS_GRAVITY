/* Exemplo de app Emma usando Gravity. */
#include "../src/gravity.h"

int main(void) {
	gv_app *app = gv_app_criar(&(gv_opcoes){ .titulo = "Emma" });
	return gv_app_correr(app, NULL, NULL);
}
