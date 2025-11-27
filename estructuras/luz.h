#pragma once

#include "../math/vectores.h"
#include "../color/textura.h"
#include "mesh.h"

#include <stdint.h>

typedef struct _luz{
	Vec3 direccion;
}Luz;

uint32_t luz_intensidad(uint32_t color, float porcentaje);
Textura luzIntesidadTextura(Mesh textura, float porcentaje);
