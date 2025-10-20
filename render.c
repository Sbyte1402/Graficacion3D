#include "render.h"
#include "draw/draw.h"
#include "global.h"
#include "math/matrix.h"
#include "draw/figuras.h"
#include "math/vectores.h"
#include "color/colores.h"
#include "memoria/memoria.h"

#include <math.h>
#include <stdio.h>

#define N_PUNTOS 9 * 9 * 9
#define N_CARAS 6 * 2

Triangulo cubo_triangulos[N_CARAS];

Vec3 camara;
Vec3 rotaciones;
Vec3 escalamiento;
const float fovf = 630;

Vec2 *punto_seleccionado = NULL;

void transformar(void){
	for(int m = 0; m < array_size(estadosrender.meshes); ++m){
		free_array(estadosrender.meshes[m].triangulos);
		estadosrender.meshes[m].triangulos = 0;

		// Por cada cara del buffer
		int num_caras = array_size(estadosrender.meshes[m].indices);
		for(int i = 0; i < num_caras; i++){
			Cara_t punto_cara = estadosrender.meshes[m].indices[i];
			Vec3 cara_vertice[3];

			cara_vertice[0] = estadosrender.meshes[m].vertices[punto_cara.a - 1];
			cara_vertice[1] = estadosrender.meshes[m].vertices[punto_cara.b - 1];
			cara_vertice[2] = estadosrender.meshes[m].vertices[punto_cara.c - 1];

			Triangulo triangulo_proyectado;
			Vec3 vertices_transformados[3];
			// Por cada vertice
			for(int j = 0; j < 3; j++){
				Vec3 punto = cara_vertice[j];

				// Matriz de transformacion
				Mat4 mt = mat4_eye();
				// Escalamos
				mat4_push_escala(&mt, estadosrender.meshes[m].escala);
				// Rotamos
				mat4_push_rotar(&mt, estadosrender.meshes[m].rotacion);
				// Trasladamos
				mat4_push_traslado(&mt, estadosrender.meshes[m].traslado);

				Vec4 p = {{punto.unpack.x, punto.unpack.y, punto.unpack.z, 1.f}};

				p = mat4_dot_vec4(&mt, &p);
				punto = vec4_to_vec3(&p);
				vertices_transformados[j] = punto;
			}

			// Back-face culling
			if(!back_face_culling(camara, vertices_transformados))
				continue;

			for(int j = 0; j < 3; ++j){
				Vec4 pp = {{vertices_transformados[j].unpack.x, 
					    vertices_transformados[j].unpack.y, 
					    vertices_transformados[j].unpack.z, 
					    1.f}};

				Mat4 PM = mat4_matriz_proyeccion(fovf, estadosrender.w_height / (float)estadosrender.w_width, 1.f, 100.f);
				Vec4 punto_proyectado = proyeccion(&PM, pp);

				// Escala ya que los puntos estan entre 0-1
				punto_proyectado.unpack.y *= estadosrender.w_height / 2.f;
				punto_proyectado.unpack.x *= estadosrender.w_width / 2.f;

				// Centrar
				punto_proyectado.unpack.y += estadosrender.w_height / 2.f;
				punto_proyectado.unpack.x += estadosrender.w_width / 2.f;

				// Generar triangulos
				triangulo_proyectado.pos[j].unpack.x = punto_proyectado.unpack.x;
				triangulo_proyectado.pos[j].unpack.y = punto_proyectado.unpack.y;
			}
			pushto_array(estadosrender.meshes[m].triangulos, triangulo_proyectado);
		}
	}
}

void render_input(void){
    if(estadosrender.evento.type == SDL_EVENT_QUIT){
        estadosrender.run = 0;
    }

    //if(estadosrender.evento.type == SDL_EVENT_MOUSE_BUTTON_DOWN){
    //    // Buscar el pivote mas cerca
    //    // Obtener referencia de la figura
    //    // Modificar

    //    // printf("(%.3f, %.3f)\n", estadosrender.evento.motion.x, estadosrender.evento.motion.y);

    //    Vec2 *pivote = pivote_mas_cerca((Vec2){{estadosrender.evento.motion.x,
    //                                           estadosrender.evento.motion.y}},
    //                                           estadosrender.figuras_temp_buffer,
    //                                           20.f);

    //    if(!punto_seleccionado){
    //        punto_seleccionado = pivote_mas_cerca((Vec2){{estadosrender.evento.motion.x,
    //                                                      estadosrender.evento.motion.y}},
    //                                                      estadosrender.figuras_temp_buffer,
    //                                                      20.f);
    //    }

    //    if(punto_seleccionado){
    //        printf("Punto cercano (%.3f, %.3f)\n",pivote -> unpack.x, pivote -> unpack.y);
    //    }

    //} else if (estadosrender.evento.type == SDL_EVENT_MOUSE_BUTTON_UP){
    //    // Modificar la figura
    //    if(punto_seleccionado){
    //        punto_seleccionado->unpack.x = estadosrender.evento.motion.x;
    //        punto_seleccionado->unpack.y = estadosrender.evento.motion.y;
    //        punto_seleccionado = NULL;
    //    }
    //}
}

void clear_color_buffer(){
    for(int y = 0; y < estadosrender.w_height; ++y){
        for(int x = 0; x < estadosrender.w_width; ++x){
            draw_pixel(x, y, estadosrender.clear_color.hex);
        }
    }
}

void copy_buffer_to_texture(){
    SDL_UpdateTexture(estadosrender.texture, 
                      NULL, 
                      estadosrender.color_buffer, 
                      (int)(estadosrender.w_width * sizeof(int)));

    SDL_RenderTexture(estadosrender.renderer, 
                      estadosrender.texture, 
                      NULL, 
                      NULL);
}

void _Init(){
	camara.unpack.z = -5.f;

	// Cargar mesh
	Mesh cubo = loadMesh("assets/cube.obj", VERTICES | INDICES);

	pushto_array(estadosrender.meshes, cubo);

	estadosrender.meshes[0].traslado.unpack.z = 5.f;

	estadosrender.meshes[0].rotacion.unpack.x = 0.f;
	estadosrender.meshes[0].rotacion.unpack.y = 0.f;
	estadosrender.meshes[0].rotacion.unpack.z = 0.f;
	estadosrender.meshes[0].escala.unpack.x = 0.003f;
	estadosrender.meshes[0].escala.unpack.y = 0.003f;
	estadosrender.meshes[0].escala.unpack.z = 0.003f; 
	
	//En espacio local, crear el cubo
	// int p = 0;
	// for(float x = -1; x <= 1; x += 0.25){
	//	for(float y = -1; y <= 1; y += 0.25){
	//		for(float z = -1; z <= 1; z += 0.25){
	//			Vec3 punto = {{x, y, z}};
	//			cubo_puntos[p++] = punto;
	//		}
	//	}
	//}
}

void update(){
	estadosrender.meshes[0].rotacion.unpack.x += 0.001f;
	estadosrender.meshes[0].rotacion.unpack.y += 0.001f;
	estadosrender.meshes[0].rotacion.unpack.z += 0.001f;

	transformar();
}

void render_frame(){
	// Por cada mesh
	for(int m = 0; m < array_size(estadosrender.meshes); ++m){
		// Por cada triangulo
		int num_trian = array_size(estadosrender.meshes[m].triangulos);
		for(int i = 0; i < num_trian; i++){
			// Vertices de los triangulos
			Triangulo trian = estadosrender.meshes[m].triangulos[i];

			fill_cuadro(trian.pos[0], 4, 4, 0xFF00FFFF, 0xFF00FFFF);
			fill_cuadro(trian.pos[1], 4, 4, 0xFF00FFFF, 0xFF00FFFF);
			fill_cuadro(trian.pos[2], 4, 4, 0xFF00FFFF, 0xFF00FFFF);
			// Lineas de los triangulos
		
			draw_trian(trian.pos[0].unpack.x, trian.pos[0].unpack.y,
				   trian.pos[1].unpack.x, trian.pos[1].unpack.y,
				   trian.pos[2].unpack.x, trian.pos[2].unpack.y, 0xFF00FFFF);

		}
	}
		SDL_RenderPresent(estadosrender.renderer);
}

Vec2* pivote_mas_cerca(Vec2 mp, Figuras *figs, float umbral){
    
    int a = 0;
    int b = array_size(figs) - 1;
    
    float low;
    float high;
    float centro;
    for(int i = 0; i < array_size(figs); i++){
        int r = (a + b) / 2;
        low = distanciav2(mp, figs[a].cuadro.pos);
        high = distanciav2(mp, figs[b].cuadro.pos);
        centro = distanciav2(mp, figs[r].cuadro.pos);

        if(fabs(low) <= umbral){
            return &figs[a].cuadro.pos;
        } else if (fabs(high) <= umbral) {
            return &figs[b].cuadro.pos;
        } else if (fabs(centro) <= umbral){
            return &figs[r].cuadro.pos;
        }

        if(r < b){
            b = r;
        } else if (r > a){
            a = r;
        }
    }

    return NULL;
}

int back_face_culling(Vec3 camara, Vec3 *puntos){
	Vec3 BA = resta_vec3(puntos[1], puntos[0]);
	normalizar_vec3_inplace(&BA);
	Vec3 CA = resta_vec3(puntos[2], puntos[0]);
	normalizar_vec3_inplace(&CA);

	Vec3 N = cross_vec3(BA, CA);
	Vec3 Ray = resta_vec3(camara, puntos[0]);
	normalizar_vec3_inplace(&Ray);

	float cull = dot_vec3(N, Ray);

	if(cull < 0)
		return 0;

	return 1;
}
