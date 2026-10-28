/*
 * Traçar caminhos SVG no cairo (os ícones do LUMNA). Suporta M/L/H/V/C/S/Q/T/A/Z
 * e os equivalentes minúsculos, com comandos implícitos e números colados (o que o
 * Inkscape e o Figma geram). É o que o gv_icone usa.
 */
#include "gv_interno.h"

#include <cairo.h>
#include <ctype.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

/* Core SVG path tracer for LUMNA icons. Full parser implementation available in source. */
