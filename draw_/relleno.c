#include "draw.h"
#include "figuras.h"
#include "../global.h"
#include "../estructuras/pila.h"

#include <stdlib.h>

uint32_t getColor(Vec2 *pixel);

void fill_figura(void *figura, uint32_t fill_color){
    Figuras *figuraCast = (Figuras*)figura;
    switch(figuraCast -> data.type){
        case CIRC:{
            Circulo *circCast = (Circulo*)figuraCast;
            fill_flood(&(circCast -> pos), fill_color);
            break;}
        case CUADRO:{
            Cuadro *cuadCast = (Cuadro*)figuraCast;

            int xM = cuadCast -> pos.unpack.x + (cuadCast -> w / 2);
            int yM = cuadCast -> pos.unpack.y + (cuadCast -> h / 2);

            Vec2 puntoM = {{xM, yM}};
            puntoM.unpack.x = xM;
            puntoM.unpack.y = yM;

            fill_flood(&puntoM, fill_color);
            break;}
        case TRIAN:{
            Triangulo *trianCast = (Triangulo*)figuraCast;
            fill_triangulo(trianCast -> pos[0],
			   trianCast -> pos[1],
			   trianCast -> pos[2],
			   fill_color);
            break;}
        case LINEA:
            break;
        case CURVA:
            break;
    }
}

void fill_flood(Vec2 *pInicial, uint32_t fill_color){
    Pila *pila = (Pila*)calloc(1, sizeof(Pila));

    int total_pixeles = estadosrender.w_height * estadosrender.w_width;
    char *verificado = (char*)calloc(total_pixeles, sizeof(char));

    Vec2 *pixel = malloc(sizeof(Vec2));
    pixel->unpack.x = pInicial->unpack.x;
    pixel->unpack.y = pInicial->unpack.y;

    push(pila, pixel);

    uint32_t colorObj = getColor(pixel);

    while(pila -> cima != NULL){
        pixel = (Vec2*)pop(pila);
        int x = pixel -> unpack.x;
        int y = pixel -> unpack.y;

        int idx = y * estadosrender.w_width + x;

        if(x >= 0 && x < estadosrender.w_width &&
           y >= 0 && y < estadosrender.w_height &&
           verificado[idx] == 0 &&
           getColor(pixel) == colorObj)
        {
            draw_pixel(x, y, fill_color);
            verificado[idx] = 1;

            Vec2 vecinos[4] = {{{1, 0}},
                               {{-1, 0}},
                               {{0, 1}},
                               {{0, -1}}};

            for(int i = 0; i < 4; i++){
                int newX = x + vecinos[i].unpack.x;
                int newY = y + vecinos[i].unpack.y;

                if(newX >= 0 && newX < estadosrender.w_width &&
                   newY >= 0 && newY < estadosrender.w_height)
                {
                    int pixelPos = newY * estadosrender.w_width + newX;
                    if(verificado[pixelPos] == 0){
                        Vec2 *objetivo = malloc(sizeof(Vec2));
                        objetivo->unpack.x = newX;
                        objetivo->unpack.y = newY;
push(pila, objetivo);
                    }
                }
            }
        }
        free(pixel);
    }

    free(verificado);
    freePila(pila);
    free(pila);
}

void fill_triangulo(Vec4 p1, Vec4 p2, Vec4 p3, uint32_t color){
	if(p1.unpack.y > p2.unpack.y){
		swapv4(&p1, &p2);
	}

	if(p2.unpack.y > p3.unpack.y){
		swapv4(&p2, &p3);
	}

	if(p1.unpack.y > p2.unpack.y){
		swapv4(&p1, &p2);
	}

	float m1 = 0;
	float m2 = 0;

	if(p2.unpack.y - p1.unpack.y != 0){
		m1 = -((float)(p2.unpack.y - p1.unpack.y) / (p1.unpack.x - p2.unpack.x));
	}

	if(p3.unpack.y - p1.unpack.y != 0){
		m2 = ((float)(p3.unpack.y - p1.unpack.y) / (p3.unpack.x - p1.unpack.x));
	}
	
	if(p2.unpack.y - p1.unpack.y != 0){
		for(int i = 0; i < (p2.unpack.y - p1.unpack.y); ++i){
			int xin = p1.unpack.x + (i / m1);
			int xen = p1.unpack.x + (i / m2);
			int y = p1.unpack.y + i;

			if(xen < xin){
				int temp = xin;
				xin = xen;
				xen = temp;
			}

			for(int x = xin; x < xen; x++){
				draw_trian_pixel(x, y, color, p1, p2, p3);
			}
		}
	}

	m1 = 0;
	m2 = 0;

	if(p3.unpack.y - p2.unpack.y != 0){
		m1 = -((p3.unpack.y - p2.unpack.y) / (float)(p3.unpack.x - p2.unpack.x));
	}

	if(p3.unpack.y - p1.unpack.y != 0){
		m2 = -((p3.unpack.y - p1.unpack.y) / (float)(p3.unpack.x - p1.unpack.x));
	}

	if(p3.unpack.y - p2.unpack.y != 0){
		for(int i = 0; i <= (p3.unpack.y - p2.unpack.y); i++){
			int xin = p3.unpack.x + (i / (float)m1);
			int xen = p3.unpack.x + (i / (float)m2);
			int y = p3.unpack.y - i;

			if(xen < xin){
				int temp = xin;
				xin = xen;
				xen = temp;
			}

			for(int x = xin; x < xen; x++){
				draw_trian_pixel(x, y, color, p1, p2, p3);
			}
		}
	}
}

uint32_t getColor(Vec2 *pixel){
    return estadosrender.color_buffer[(int)pixel->unpack.y * estadosrender.w_width + (int)pixel->unpack.x];
}


