#include "draw.h"
#include "linea.h"
#include "figuras.h"
#include "../global.h"
#include "../estructuras/luz.h"

#include <stdint.h>
#include <stdlib.h>

void swapv4(Vec4 *a, Vec4 *b){
	Vec4 temp = *a;
	*a = *b;
	*b = temp;
}

void draw_trian(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color){
    draw_linea(x0, y0, x1, y1, color);
    draw_linea(x1, y1, x2, y2, color);
    draw_linea(x2, y2, x0, y0, color);
}

Vec3 normal_triangulo(Triangulo *trian){
	Vec3 AB = resta_vec3(vec4_to_vec3(&trian -> pos[1]), vec4_to_vec3(&trian -> pos[0]));
	Vec3 AC = resta_vec3(vec4_to_vec3(&trian -> pos[2]), vec4_to_vec3(&trian -> pos[0]));

	normalizar_vec3_inplace(&AB);
	normalizar_vec3_inplace(&AC);

	Vec3 normal = cross_vec3(AB, AC);
	normalizar_vec3_inplace(&normal);
	trian -> normal = normal;
	return normal;
}

Vec3 barycentric_pesos(Vec3 a, Vec3 b, Vec3 c, Vec3 p){
	Vec3 ab  =resta_vec3(b, a);
	Vec3 bc  =resta_vec3(c, b);
	Vec3 ac  =resta_vec3(c, a);
	Vec3 ap  =resta_vec3(p, a);
	Vec3 bp  =resta_vec3(p, b);

	float areaABC = (ab.unpack.x * ac.unpack.y - ab.unpack.y * ac.unpack.x);
	float alpha = (bc.unpack.x * bp.unpack.y - bp.unpack.x * bc.unpack.y) / areaABC;
	float beta = (ap.unpack.x * ac.unpack.y - ac.unpack.x * ap.unpack.y) / areaABC;
	float gamma = 1 - alpha - beta;

	Vec3 pesos = {{alpha, beta, gamma}};

	return pesos;
}

void tex_trian(Vec4 p1, TexturaUV tuv1, 
	       Vec4 p2, TexturaUV tuv2, 
	       Vec4 p3, TexturaUV tuv3, 
	       uint32_t *textura, int tw, int th,
	       float intensidad){
	
	if(p1.unpack.y > p2.unpack.y){
		swapv4(&p1, &p2);
		swapUV(&tuv1, &tuv2);
	}

	if(p2.unpack.y > p3.unpack.y){
		swapv4(&p2, &p3);
		swapUV(&tuv2, &tuv3);
	}

	if(p1.unpack.y > p2.unpack.y){
		swapv4(&p1, &p2);
		swapUV(&tuv1, &tuv2);
	}

	tuv1.v = 1.f - tuv1.v;
	tuv2.v = 1.f - tuv2.v;
	tuv3.v = 1.f - tuv3.v;

	// Flat bottom
	float m1 = 0;
	float m2 = 0;

	if(p2.unpack.y - p1.unpack.y != 0)
		m1 = -((float)(p2.unpack.y - p1.unpack.y) / (p1.unpack.x - p2.unpack.x));

	if(p3.unpack.y - p1.unpack.y != 0)
		m2 = ((float)(p3.unpack.y - p1.unpack.y) / (p3.unpack.x - p1.unpack.x));

	if(p2.unpack.y - p1.unpack.y != 0){
		for(int i = 0; i < (p2.unpack.y - p1.unpack.y); ++i){
			int xin = p1.unpack.x + (i / m1);
			int xen = p1.unpack.x + (i / m2);
			int y = p1.unpack.y + i;

			// Dibujar el trinagulo de derecha a izq por eso el swap
			if(xen < xin){
				int temp = xin;
				xin = xen;
				xen = temp;
			}

			for(int x = xin; x < xen; x++){
				draw_textura(x, y, p1, p2, p3, tuv1, tuv2, tuv3, textura, tw, th, intensidad);
			}
		}
	}

	// Flat top
	m1 = 0;
	m2 = 0;

	if(p3.unpack.y - p2.unpack.y != 0)
		m1 = -((p3.unpack.y - p2.unpack.y) / (float)(p3.unpack.x - p2.unpack.x));
	
	if(p3.unpack.y - p1.unpack.y != 0)
		m2 = -((p3.unpack.y - p1.unpack.y) / (float)(p3.unpack.x - p1.unpack.x));

	if(p3.unpack.y - p2.unpack.y != 0){
		for(int i = 0; i <= (p3.unpack.y - p2.unpack.y); i++){
			int xin = p3.unpack.x + (i / (float)m1);
			int xen = p3.unpack.x + (i / (float)m2);
			int y = p3.unpack.y - i;

			// Dibujar el triangulo de dereacha a izq por eso el swap
			if(xen < xin){
				int temp = xin;
				xin = xen;
				xen = temp;
			}

			for(int x = xin; x < xen; x++){
				draw_textura(x, y, p1, p2, p3, tuv1, tuv2, tuv3, textura, tw, th, intensidad);
			}
		}
	}
}

void draw_textura(int x, int y, 
		  Vec4 a, Vec4 b, Vec4 c, 
		  TexturaUV t1, TexturaUV t2, TexturaUV t3, 
		  uint32_t *textura, int tw, int th,
		  float intensidad){
	
	Vec3 p = {{x, y, 1.f}};
	Vec3 pesos = barycentric_pesos(
			vec4_to_vec3(&a),
			vec4_to_vec3(&b),
			vec4_to_vec3(&c),
			p);
	
	float alpha = pesos.unpack.x;
	float beta = pesos.unpack.y;
	float gamma = pesos.unpack.z;

	// Interpolar
	float iU = (t1.u / a.unpack.w) * alpha + (t2.u / b.unpack.w) * beta + (t3.u / c.unpack.w) * gamma;
	float iV = (t1.v / a.unpack.w) * alpha + (t2.v / b.unpack.w) * beta + (t3.v / c.unpack.w) * gamma;

	float iinvW = (1.f / a.unpack.w) * alpha + (1.f / b.unpack.w) * beta + (1.f / c.unpack.w) * gamma;
	iU /= (iinvW);
	iV /= (iinvW);

	int texIDX = abs((int)(iU * tw)) % tw;
	int texIDY = abs((int)(iV * th)) % th;

	iinvW = 1.f - iinvW;
	if(iinvW < estadosrender.z_buffer[estadosrender.w_width * y + x]){
		uint32_t colorOriginal = textura[tw * texIDY + texIDX];
		uint32_t colorConLuz = luz_intensidad(colorOriginal, intensidad);

		draw_pixel(x, y, colorConLuz);
		estadosrender.z_buffer[estadosrender.w_width * y + x] = iinvW;
	}	
}

void draw_trian_pixel(int x, int y,
		      uint32_t color,
		      Vec4 p1, Vec4 p2, Vec4 p3){
	
	Vec3 p = {{x, y, 1.f}};
	Vec3 pesos = barycentric_pesos(
			vec4_to_vec3(&p1),
			vec4_to_vec3(&p2),
			vec4_to_vec3(&p3),
			p);

	float alpha = pesos.unpack.x;
	float beta  = pesos.unpack.y;
	float gamma = pesos.unpack.z;

	float iinvW = (1.f / p1.unpack.w) * alpha + (1.f / p2.unpack.w) * beta + (1.f / p3.unpack.w) * gamma;

	if(iinvW < estadosrender.z_buffer[y * estadosrender.w_width + x]){
		draw_pixel(x, y, color);
		estadosrender.z_buffer[y * estadosrender.w_width + x] = iinvW;
	}
}

void swap(Vec3 *p1, Vec3 *p2){
	Vec3 temp = *p1;
	*p1 = *p2;
	*p2 = temp;
}

void swapUV(TexturaUV *p1, TexturaUV *p2){
	TexturaUV temp = *p1;
	*p1 = *p2;
	*p2 = temp;
}
