/* A implementação do Clay (vendor/clay, zlib), compilada uma vez para o Gravity inteiro. */
#define CLAY_IMPLEMENTATION
#include "clay.h"

void gv__clay_limpar_medidas(void) {
	Clay_ResetMeasureTextCache();
	Clay_GetCurrentContext()->booleanWarnings.maxTextMeasureCacheExceeded = false;
}
