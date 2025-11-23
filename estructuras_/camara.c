#include "camara.h"
#include "../math/matrix.h"
#include "../math/vectores.h"

Mat4 look_at(Vec3 eye, Vec3 target, Vec3 arriba){
	Vec3 fwd = normalizar_vec3(resta_vec3(target, eye));
	Vec3 rht = normalizar_vec3(cross_vec3(arriba, fwd));
	Vec3 up = normalizar_vec3(cross_vec3(fwd, rht));

	return (Mat4){{rht.unpack.x, rht.unpack.y, rht.unpack.z, -dot_vec3(rht, eye),
		        up.unpack.x,  up.unpack.y,  up.unpack.z, -dot_vec3(up, eye),
		       fwd.unpack.x, fwd.unpack.y, fwd.unpack.z, -dot_vec3(fwd, eye),
	                     0,           0,            0,                1}};
}
