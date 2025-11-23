#pragma once

#include "../draw/figuras.h"
#include "../math/vectores.h"

typedef struct _mesh{
	Vec3 *vertices;
	Vec3 *normales;

	Cara_t *indices;
	Cara_t *n_indices;

	Triangulo *triangulos;
	TexturaUV *texturaUV;
	Textura textura;
	
	Vec3 rotacion;
	Vec3 escala;
	Vec3 traslado;
}Mesh;

typedef enum{
	VERTICES = 2 << 0,
	NORMALES = 2 << 1,
	INDICES  = 2 << 2,
	UV = 2 << 3,
}MeshOpciones;

Mesh loadMesh(const char* filePath, MeshOpciones opt);
