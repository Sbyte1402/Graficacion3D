#include "luz.h"
#include "../memoria/memoria.h"

#include <stdint.h>

uint32_t luz_intensidad(uint32_t color, float porcentaje){
	if (porcentaje < 0.3f) porcentaje = 0.3f; 
	if (porcentaje > 1.0f) porcentaje = 1.0f;

	uint8_t r = (color & 0xFF000000) >> 24;
	uint8_t g = (color & 0x00FF0000) >> 16;
	uint8_t b = (color & 0x0000FF00) >> 8;
	uint8_t a = (color & 0x000000FF);

	r = (uint8_t)(r * porcentaje);
	g = (uint8_t)(g * porcentaje);
	b = (uint8_t)(b * porcentaje);

	return (r << 24) |
	       (g << 16) |
	       (b << 8) |
	       (a);
}
