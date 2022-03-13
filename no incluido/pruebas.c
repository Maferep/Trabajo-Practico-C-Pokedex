#include "pokedex.h"
#include <stdio.h>

#define ERROR -1
#define EXITO 0
#define SALIDA -2

#define INICIAR 'I'
#define GUARDAR 'G'
#define SALIR 'S'
#define AYUDA 'H'
#define AVISTAR 'A'
#define EVOLUCIONAR 'E'
#define CAPTURAS 'C'
#define VISTAS 'V'
#define INFO_ESPECIE 'M'
#define INFO_POKEMON 'P'

#define INTERACCIONES_ENCENDIDO {GUARDAR,SALIR,AYUDA,AVISTAR,EVOLUCIONAR,CAPTURAS,VISTAS,INFO_ESPECIE,INFO_POKEMON}
#define INTERACCIONES_APAGADO {INICIAR,SALIR,AYUDA}
#define MAX_INTERACCIONES 20
#define MAX_INPUT 200

#define CANT_INTERACCIONES_ENCENDIDO 9
#define CANT_INTERACCIONES_APAGADO 3

#define RUTA_AVISTAR "avistamientos.txt"
#define RUTA_EVOLUCIONAR "evoluciones.txt"
#define RUTA_MENU "menu.txt"
#define RUTA_MENU_APAGADO "menu_apagado.txt"
int comparador(void* e1, void* e2)
{
    return *(int*) e1 - *(int*)e2;
}
int pruebas_lista()
{
    lista_t* lista = lista_crear();
    int numerito [10] = {10,20,30,40,50,60,70,80,90, 420};
    printf("insertar numerito\n");
    for(int i = 0; i < 10; i++)
    {
        lista_insertar(lista,numerito + i);
        printf("cantidad elementos = %li \n",lista_elementos(lista));
    }
    printf("borrar elemento al final\n");
    lista_borrar(lista);
    printf("cantidad elementos = %li \n",lista_elementos(lista));
    lista_insertar(lista,numerito);
    printf("inserto.cantidad elementos = %li \n",lista_elementos(lista));
    printf("borrando por elemento,,,\n");
    int* b = lista_borrar_elemento(lista, numerito + 5, comparador);
    if(b) printf("borré %i. cantidad elementos = %li \n",*b, lista_elementos(lista));
    else printf ("error lista\n");
    lista_destruir(lista);
    return EXITO;
}
bool imprimir_especie(void* especie, void* contador)
{
    especie_pokemon_t* mi_especie = (especie_pokemon_t*) especie ;
    *(int*) contador = *(int*) contador + 1;
    printf("Especie: %s. numero: %i.\nPokemones: \n",mi_especie->nombre, mi_especie->numero);
    lista_iterador_t* it = lista_iterador_crear(mi_especie->pokemones);
    while(lista_iterador_tiene_siguiente(it))
    {
        printf("* %s\n", ((particular_pokemon_t*)lista_iterador_siguiente(it))->nombre  );
    }
    lista_iterador_destruir(it);
    return false;
}
void mostrar_pokedex(pokedex_t* pokedex)
{
    printf("---------MOSTRAR POKEMON-----\n");
    int* contador = malloc(sizeof(int)); *contador = 0;
    abb_con_cada_elemento(pokedex->pokemones,ABB_RECORRER_INORDEN, imprimir_especie,contador);
    printf("-------------contador:%i------------\n", *contador);
    free(contador);
}
int prueba_pokedex_avistar()
{
    printf("prueba avistar\n");
    int ret = EXITO;
    char nombre[MAX_NOMBRE] = "peddrito";
    pokedex_t* p = pokedex_crear(nombre);
    if(!p) return ERROR;
    mostrar_pokedex(p);
    if(pokedex_avistar(p, RUTA_AVISTAR)==ERROR) ret = ERROR;
    mostrar_pokedex(p);
    pokedex_destruir(p);
    return ret;
}
int prueba_pokedex_evo()
{
    int ret = EXITO;
    pokedex_t* p = pokedex_crear("pedrito2");
    mostrar_pokedex(p);
    if(pokedex_avistar(p, RUTA_AVISTAR)==ERROR) printf("ERROR avistando en prueba evo\n");
    if(pokedex_evolucionar(p, RUTA_EVOLUCIONAR)==ERROR) ret = ERROR;
    mostrar_pokedex(p);
    pokedex_destruir(p);
    return ret;
}
/*
Prueba autocontenida. Crea pokedex, avista, apaga y prende. 
*/
int prueba_pokedex_prender()
{
    pokedex_t* p = pokedex_crear("pedrito2");
    if(!p) return ERROR;
    printf("creó pokedex p\n");
    if(pokedex_avistar(p, RUTA_AVISTAR)==ERROR) printf("ERROR avistando en prueba prender\n");
    printf("pokedex luego de avistar:\n");
    mostrar_pokedex(p);
    if(pokedex_apagar(p)==ERROR) return ERROR;
    pokedex_destruir(p);
    p = pokedex_prender();
    if(!p) {
        printf("ERROR prendiendo");
        return ERROR;
    }
    printf("pokedex luego de apagar y prender:\n");
    mostrar_pokedex(p);
    pokedex_destruir(p);
    return EXITO;
}
/*
Genera archivo de avistamiento.
*/
void creador_avistamientos(char* ruta, const int seed)
{
    FILE* archivo = fopen(ruta,"w");
    char particular[]="%i;especie%i;descripcion...;nombre%i;%i;N\n";
    char capturado[]="%i;especie%i;descripcion...;capturado%i;%i;S\n";
    for(int especie = seed, pokemon = seed; especie < seed+10; especie+=2)
    {
        while (pokemon % 8 != 0)
        {
            fprintf(archivo, particular, especie, especie, pokemon, pokemon %5 + 1);
            pokemon+=2;
        }
        fprintf(archivo, capturado, especie, especie, pokemon,pokemon %5 + 1);
        pokemon+=2;
    }
    fclose(archivo);
}
/*
Genera archivo de evoluciones.
*/
void creador_evo(char* ruta, int seed)
{
    FILE* archivo = fopen(ruta,"w");
    int especie_nueva;
    char evolucion[]="%i;nombre%i;%i;especie%i;descripcion...\n";
    char evolucionable[]="%i;capturado%i;%i;especie%i;descripcion...\n";
    for(int especie_vieja = seed, pokemon = seed; especie_vieja < seed+10; especie_vieja+=2)
    {
        especie_nueva = especie_vieja + 1;
        while (pokemon % 8 != 0)
        {
            fprintf(archivo, evolucion, especie_vieja, pokemon,especie_nueva,especie_nueva);
            pokemon+=2;
        }
        fprintf(archivo, evolucionable, especie_vieja, pokemon,especie_nueva,especie_nueva);
        pokemon+=2;
    }
    fclose(archivo);
}
/*
Genera archivo de pokemones.
*/
void creador_pokedex(int seed)
{
    char ruta1[MAX_RUTA] = "avistamientosTEMP.txt";
    //char ruta2[MAX_RUTA] = "evolucionesTEMP.txt";
    pokedex_t* p = pokedex_crear("panchito\n");
    creador_avistamientos(ruta1, 200);
    pokedex_avistar(p, ruta1);
    pokedex_apagar(p);
    pokedex_destruir(p);
}
int generar_archivos(int seed)
{
    creador_avistamientos(RUTA_AVISTAR,seed);
    creador_evo(RUTA_EVOLUCIONAR, seed);
    creador_pokedex(seed+400);
    return EXITO;
}
