#pragma once

#include "../math/matrix.h"
#include "../math/vectores.h"

typedef struct _camara{
	Vec3 posicion;
	Vec3 direccion;
	Vec3 velocidad_fwd;

	float yaw;
}Camara;

Mat4 look_at(Vec3 eye, Vec3 target, Vec3  arriba);
