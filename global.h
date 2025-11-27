#pragma once

#include <stdint.h>
#include <SDL3/SDL.h>
#include "estructuras/mesh.h"
#include "color/colores.h"
#include "draw/figuras.h"

typedef struct _estado{
    SDL_Event evento;
    SDL_Texture *texture;
    SDL_Renderer *renderer;

    float *z_buffer;

    int run;
    int w_width;
    int w_height;

    Mesh *meshes;
    Color clear_color;
    uint32_t *color_buffer;
    float dt;
    // Figuras *figuras_buffer;
    // Figuras *figuras_temp_buffer;
}EstadoRender;

extern EstadoRender estadosrender;
