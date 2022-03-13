#include <stdio.h>
#include <string.h>
#include "pokedex.h"
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
void imprimir_menu(bool encendido)
{
    FILE* archivo;
    if(encendido) archivo = fopen(RUTA_MENU, "r");
    else archivo = fopen(RUTA_MENU_APAGADO, "r");
    char c = (char)fgetc(archivo);
    while (c!=EOF)
    {
        printf("%c",c);
        c = (char)fgetc(archivo);
    }
    fclose(archivo);
}
/*
 * Recibe el pokedex y un input. Interpreta del input el pokemon o especie
 * del cual el usuario espera recibir informacion.
 * si pokemon_particular es false,
 */
void comando_info(pokedex_t* pokedex, char* input_entero, size_t tope_input, bool pokemon_particular)
{
    char nombre_pokemon[MAX_NOMBRE] = "";
    int numero_especie = -1;
    int leidos = 0;
    if(pokemon_particular)leidos = sscanf(input_entero+1," %i %[^\n]",&numero_especie,nombre_pokemon);
    else leidos = sscanf(input_entero+1," %i",&numero_especie);
    if(!leidos 
    || leidos == EOF 
    || (pokemon_particular && strcmp(nombre_pokemon, "")==0) ) 
    {
        printf("no pudo leer!\n");
        return;
    }
    pokedex_informacion(pokedex,numero_especie, (pokemon_particular) ? nombre_pokemon : "");
}
/*
 * Interactuar recibe el pokedex, un comando, un string input y su tamanio.
 * Llama a la funcion correspondiente al comando recibido si existe,
 * si no imprime un mensaje de error.
 * Si pudo identificar el comando exitosamente devuelve EXITO.
 * Si no devuelve ERROR.
 */
int interactuar(pokedex_t** pokedex,char comando, char* input_entero, size_t tope_input)
{
    switch(comando)
    {
        case INICIAR:
            if(!*pokedex) *pokedex = pokedex_prender();
            break;
        case GUARDAR:
            pokedex_apagar(*pokedex);
            break;
        case SALIR:
            if(*pokedex) pokedex_destruir(*pokedex);
            return SALIDA;
        case AYUDA:
            imprimir_menu(*pokedex != NULL);
            break;
        case AVISTAR:
            pokedex_avistar(*pokedex, RUTA_AVISTAR);
            break;
        case EVOLUCIONAR:
            pokedex_evolucionar(*pokedex, RUTA_EVOLUCIONAR);
            break;
        case CAPTURAS:
            pokedex_ultimos_capturados(*pokedex);
            break;
        case VISTAS:
            pokedex_ultimos_vistos(*pokedex);
            break;
        case INFO_ESPECIE:
            comando_info(*pokedex, input_entero, tope_input, false);
            break;

        case INFO_POKEMON:
            comando_info(*pokedex, input_entero, tope_input, true);
            break;
        default:
            printf("ERROR input\n");
            return ERROR;
    }
    return EXITO;
}
/*
 * Lee todo el input hasta newline en stdin. Devuelve su longitud.
 */
size_t leer_linea(char* input)
{
    fgets(input, MAX_INPUT, stdin);
    return strlen(input);
}
/*
 * Recibe un set de interacciones válidas y un doble puntero a pokedex.
 * Lee un comando de la consola. Si el primer caracter es un comando válido,
 * ejecuta la interacción. Si no, imprime 'Interaccion no valida'.
 * Si pudo interactuar con el comando exitosamente devuelve EXITO.
 * Si no devuelve ERROR.
 */
int interactuar_pokedex(pokedex_t** pokedex, char* interacciones, int tope)
{
    int resultado = EXITO;
    //obtener comando
    char input[MAX_INPUT];
    printf("hola, por favor ingrese un comando. Ingrese 'H' para obtener ayuda.\n--->");
    size_t tope_input = leer_linea(input);
    //buscar caracter en comandos
    bool encontrado = false;
    for(int i = 0; i < tope;i++)
    {
        if(input[0] == interacciones[i])
        {
            encontrado = true;
            resultado = interactuar(pokedex, input[0],input,tope_input);
            break;
        }
    }
    if(!encontrado) {
        printf("Interaccion no valida. :(\n");
    }
    return resultado;
}
void usar_pokedex()
{
    char interacciones_encendido[CANT_INTERACCIONES_ENCENDIDO] = INTERACCIONES_ENCENDIDO;
    char interacciones_apagado[CANT_INTERACCIONES_APAGADO] = INTERACCIONES_APAGADO;
    pokedex_t* pokedex = NULL;
    int resultado = 0;
    while(resultado!=SALIDA && resultado!=ERROR)
    {
        resultado = (pokedex) 
        ? interactuar_pokedex(&pokedex,interacciones_encendido, CANT_INTERACCIONES_ENCENDIDO) 
        : interactuar_pokedex(&pokedex,interacciones_apagado, CANT_INTERACCIONES_APAGADO) ;
    }
    printf("saliendo...\n");
}
int main()
{
    usar_pokedex();
}

