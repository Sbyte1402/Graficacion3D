#pragma once

#include "../math/vectores.h"
#include "../draw/figuras.h"

typedef struct _mesh{
	Vec3 *vertices;
	Vec3 *normales;
	Cara_t *indices;
	Triangulo *triangulos;

	Vec3 rotacion;
	Vec3 escala;
	Vec3 traslado;
}Mesh;

typedef enum{
	VERTICES = 2 << 0,
	NORMALES = 2 << 1,
	INDICES  = 2 << 2,
	TEXTURAS = 2 << 3,
}MeshOpciones;

Mesh loadMesh(const char* fiilePath, MeshOpciones opt);
