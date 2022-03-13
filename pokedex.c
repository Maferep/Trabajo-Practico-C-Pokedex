#include "lista.h"
#include "pokedex.h"
#include <stdio.h>
#include <string.h>
#define ERROR -1
#define EXITO 0
#define SI 'S'
#define NO 'N'
#define RUTA_POKEDEX "pokedex.txt"
#define ES_POKEMON 'P'
#define ES_ESPECIE 'E'
typedef struct scan 
{
    char tipo;
    particular_pokemon_t* pokemon;
    especie_pokemon_t* especie;
    bool error;
}scan_t;
typedef void (*destructor) (void*);
/*
 * Compara los numeros de especie de los pokemon. Si son los mismos, es la misma especie.
 */
int comparador_especie_pokemon(void* poke1, void* poke2){
    return ((especie_pokemon_t*)poke1)->numero - ((especie_pokemon_t*)poke2)->numero  ;
}
/*
 * Compara los nombres de pokemon particulares. 
 */
int comparador_particular_pokemon(void* poke1, void* poke2){
    return strcmp(    
        ((particular_pokemon_t*)poke1)->nombre,    
        ((particular_pokemon_t*)poke2)->nombre    
    );
}
void destructor_particular_pokemon(void* poke);
void destructor_especie_pokemon(void* poke);
void liberar_elementos_lista(lista_t* lista, destructor destruir);
/*
 * Recibe el nombre de un entrenador.
 * Reserva memoria para un pokedex y sus estructuras internas. 
 * Devuelve el pokedex. Si falla devuelve NULL.
 */
pokedex_t* pokedex_crear(char entrenador[MAX_NOMBRE]){
    pokedex_t* pokedex = calloc(1, sizeof(pokedex_t));
    if(!pokedex || !entrenador) return NULL;
    strcpy(pokedex->nombre_entrenador, entrenador);
    pokedex->ultimos_capturados = lista_crear();
    pokedex->ultimos_vistos = lista_crear();
    pokedex->pokemones = arbol_crear(comparador_especie_pokemon,destructor_especie_pokemon);
    return pokedex;
}
/*
 * Recibe un pokedex, libera su memoria y la de sus estructuras internas.
 */
void pokedex_destruir(pokedex_t* pokedex){
    //abb sabe liberar contenidos con destructor
    arbol_destruir(pokedex->pokemones);
    //loop listas de particulares y liberarlos
    liberar_elementos_lista(pokedex->ultimos_capturados, destructor_particular_pokemon);
    liberar_elementos_lista(pokedex->ultimos_vistos, destructor_particular_pokemon);
    //destruir listas
    lista_destruir(pokedex->ultimos_capturados);
    lista_destruir(pokedex->ultimos_vistos);
    //liberar resto de pokedex
    free(pokedex);
}
/*
 * Crea un espacio de memoria para una especie pokemon basada en la informacion recibida.
 * Si no puede reservar memoria devuelve NULL.
 */
especie_pokemon_t* copiar_especie(const especie_pokemon_t* info_especie)
{
    especie_pokemon_t* nueva_especie = calloc(1, sizeof(especie_pokemon_t));
    if(!nueva_especie) return NULL;
    strcpy(nueva_especie->nombre, info_especie->nombre);
    strcpy(nueva_especie->descripcion, info_especie->descripcion);
    nueva_especie->pokemones = info_especie->pokemones;
    nueva_especie->numero = info_especie->numero;
    return nueva_especie;
}
/*
 * Crea un espacio de memoria para un pokemon particular basado en la informacion recibida.
 * Si no puede reservar memoria devuelve NULL.
 */
particular_pokemon_t* copiar_pokemon(const particular_pokemon_t* info_particular)
{
    particular_pokemon_t* nuevo_pokemon = calloc(1, sizeof(especie_pokemon_t));
    if(!nuevo_pokemon) return NULL;
    strcpy(nuevo_pokemon->nombre, info_particular->nombre);
    nuevo_pokemon->nivel = info_particular->nivel;
    nuevo_pokemon->capturado = info_particular->capturado;
    return nuevo_pokemon;
}
/*
 * Busca una especie en el arbol de pokemones del pokedex que coincida con info_especie.
 */
especie_pokemon_t* buscar_especie(pokedex_t* pokedex, especie_pokemon_t* info_especie)
{
    return (especie_pokemon_t*)arbol_buscar(pokedex->pokemones, info_especie);
}
/* 
 * Busca una especie con la informacion recibida en el arbol de especies de pokedex.
 * Si no la encuentra crea una copia de la información recibida en memoria dinámica y la guarda en el arbol.
 * Si no existe y no pudo crear la especie, devuelve NULL.
 */
static especie_pokemon_t* actualizar_especies(pokedex_t* pokedex, const especie_pokemon_t* info_especie)
{
    especie_pokemon_t* especie = (especie_pokemon_t*) arbol_buscar(pokedex->pokemones, (void*)info_especie);
    if(!especie)
    {
        especie = copiar_especie(info_especie);
        if(!especie) 
        {
            printf("No pudo copiar.\n"); return NULL;
        }
        especie->pokemones = lista_crear();
        if(arbol_insertar(pokedex->pokemones, especie) == ERROR)
        {
            printf("ERROR INSERCION!\n");
            destructor_especie_pokemon(especie);
            return NULL;
        }
    }
    return especie;
}
/* 
 * Crea una copia del pokemon y lo inserta en la lista de particulares de la especie recibida.
 * Si fracasa, devuelve NULL. Si no devuelve un puntero al pokemon insertado.
 */
static particular_pokemon_t* agregar_pokemon_a_especie(especie_pokemon_t* especie, particular_pokemon_t* info_pokemon)
{
    particular_pokemon_t* pokemon = copiar_pokemon(info_pokemon);
    if(!pokemon) return NULL;
    else if(lista_insertar(especie->pokemones, pokemon)==ERROR)
    {
        destructor_particular_pokemon(pokemon);
        return NULL;
    }
    return pokemon;
}
/* 
 * Crea una copia del pokemon y lo inserta en la lista de vistos.
 * Si fracasa, devuelve NULL. Si no devuelve un puntero al pokemon insertado.
 */
static particular_pokemon_t* agregar_pokemon_a_vistos(pokedex_t* pokedex, particular_pokemon_t* info_pokemon)
{
    particular_pokemon_t* pokemon = copiar_pokemon(info_pokemon);
    if(!pokemon) return NULL;
    else if (lista_encolar(pokedex->ultimos_vistos, pokemon)==ERROR)
    {
        destructor_particular_pokemon(pokemon);
        return NULL;
    }
    return pokemon;
}
/* 
 * Crea una copia del pokemon y lo inserta en la lista de capurados.
 * Si fracasa, devuelve NULL. Si no devuelve un puntero al pokemon insertado.
 */
static particular_pokemon_t* agregar_pokemon_a_capturados(pokedex_t* pokedex, particular_pokemon_t* info_pokemon)
{
    particular_pokemon_t* pokemon = copiar_pokemon(info_pokemon);
    if(!pokemon) return NULL;
    else if(lista_apilar(pokedex->ultimos_capturados, pokemon)==ERROR)
    {
        destructor_particular_pokemon(pokemon);
        return NULL;
    }
    return pokemon;
}
/*
 * Abre el archivo de avistamientos y trata de leerlo com el formato
 * NOMBRE_ESPECIE;DESC_ESPECIE;NOMBRE_POKEMON;NIVEL_POKEMON;CAPTURADO (S/N)
 * Si encuentra un error interrumpe la lectura a medio camino y devuelve ERROR.
 */
int pokedex_avistar(pokedex_t* pokedex, char ruta_archivo[MAX_RUTA])
{
    if(!pokedex || !ruta_archivo) return ERROR;
    FILE* archivo = fopen(ruta_archivo,"r");
    if(!archivo) return ERROR;
    especie_pokemon_t info_especie = {0};
    particular_pokemon_t info_pokemon = {0};
    char capturado = '\0';
    int leidos;
    while(true) 
    {
        leidos=fscanf(archivo,"%i;%100[^;];%100[^;];%100[^;];%i;%c ", 
                            &info_especie.numero,
                            info_especie.nombre,
                            info_especie.descripcion, 
                            info_pokemon.nombre,
                            &info_pokemon.nivel, 
                            &capturado);
        if(leidos == EOF) break;
        if(leidos <= 5) 
        {
            fclose(archivo);
            return ERROR;
        }
        if(capturado == SI) 
        {
            printf("pokemon capturado!\n");
            info_pokemon.capturado = true;
        }
        else if(capturado == NO) info_pokemon.capturado = false;
        else 
        {
            fclose(archivo);
            return ERROR;
        }
        printf("Procesado: %s (%i): '%s' \n", 
                            info_especie.nombre, 
                            info_especie.numero, 
                            info_pokemon.nombre);

        especie_pokemon_t* mi_especie = actualizar_especies(pokedex, &info_especie);
        if(mi_especie==NULL) // Fallo actualizar_especies
        {
            printf("Error actualizando la especie.\n"); //DEBUG
            fclose(archivo);
            return ERROR;
        }
        agregar_pokemon_a_especie(mi_especie, &info_pokemon);
        agregar_pokemon_a_vistos(pokedex, &info_pokemon);
        if(info_pokemon.capturado) 
            agregar_pokemon_a_capturados(pokedex, &info_pokemon);
        
    }
    fclose(archivo);
    //limpiar archivo entero.
    archivo = fopen(ruta_archivo,"w");
    fclose(archivo);
    return EXITO;
}
/*
 * Recibe una especie y los datos de un pokemon particular.
 * Busca entre los pokemones de la especie al pokemon cuyos datos coincidan con el particular mediante el comparador de pokemones.
 * Si lo encuentra, devuelve un puntero a el. Si no devuelve null.
 */
particular_pokemon_t* especie_buscar_particular(especie_pokemon_t* especie,const particular_pokemon_t* info_poke)
{
    if(!especie || !info_poke) return NULL;
    lista_iterador_t* it = lista_iterador_crear(especie->pokemones);
    while(lista_iterador_tiene_siguiente(it))
    {
        particular_pokemon_t* actual = (particular_pokemon_t*) lista_iterador_siguiente(it);
        if(comparador_particular_pokemon((void*)info_poke,actual)==0) 
        {
            lista_iterador_destruir(it);
            return actual ;
        }
    }
    lista_iterador_destruir(it);
    return NULL;
}
/*
 * Recibe un pokedex, la información de un pokemon evolucionado, su anterior y nueva especie.
 * Si conozco al pokemon y lo tengo en los particulares de esa especie, 
 * puedo evolucionarlo.
 * 
 * Devuelvo error si no tengo a la especie, al pokemon particular, o si nolo tengo capturado.
 */
int procesar_evolucion(pokedex_t* pokedex, particular_pokemon_t* info_particular, especie_pokemon_t* info_anterior,  especie_pokemon_t* info_evolucion)
{
    //reviso que tengo a este pokemon listado
    especie_pokemon_t* anterior = arbol_buscar(pokedex->pokemones, info_anterior);
    if(!anterior) return ERROR;
    //busco pokemon particular sin evolucionar en su especie
    particular_pokemon_t* mi_pokemon_particular = especie_buscar_particular(anterior, info_particular);
    if(mi_pokemon_particular==NULL) return ERROR;
    if(!(mi_pokemon_particular->capturado)) return ERROR;
    //borro pokemon de su vieja especie
    lista_borrar_elemento(anterior->pokemones, mi_pokemon_particular, comparador_particular_pokemon);
    //lo pongo en su especie nueva
    especie_pokemon_t* evolucion = actualizar_especies(pokedex, info_evolucion);
    if(evolucion) agregar_pokemon_a_especie(evolucion, mi_pokemon_particular);
    destructor_particular_pokemon(mi_pokemon_particular);
    return evolucion ? EXITO : ERROR;
}
/*
 * Función que dado un archivo, deberá cargar en la Pokedex a los
 * Pokémon que evolucionaron.
 *
 * La evolución de Pokémon no afecta a la pila de capturados ni a la
 * cola de vistos. Un Pokémon que no está marcado como capturado no
 * puede evolucionar.
 *
 * Devuelve 0 si tuvo éxito o -1 si se encuentra algún error durante
 * el proceso (por ejemplo si no existe la especie, el Pokémon
 * específico o si el Pokémon no fue capturado).
 */
int pokedex_evolucionar(pokedex_t* pokedex, char ruta_archivo[MAX_RUTA])
{
    if(!pokedex || !ruta_archivo) return ERROR;
    FILE* archivo = fopen(ruta_archivo,"r");
    if(!archivo) return ERROR;
    particular_pokemon_t info_particular;
    especie_pokemon_t info_anterior, info_evolucion;
    int leidos=fscanf(archivo,"%i;", &info_anterior.numero);
    while(leidos!=EOF) 
    {
        //leo el nombre del particular y el numero de su especie
        leidos = fscanf(archivo,"%[^;];%i;%[^;];%[^\n]\n",info_particular.nombre,&info_evolucion.numero,info_evolucion.nombre,info_evolucion.descripcion);
        if(leidos==EOF || leidos < 4)
        {
            printf("No pudo leer evolucion.\n");
            fclose(archivo);
            //limpiar archivo
            archivo = fopen(ruta_archivo,"w");
            fclose(archivo);
            return ERROR;
        }
        if(procesar_evolucion(pokedex, &info_particular,&info_anterior, &info_evolucion)==ERROR) 
        {
            printf("No pudo procesar evolucion.\n");
            fclose(archivo);
            //limpiar archivo
            archivo = fopen(ruta_archivo,"w");
            fclose(archivo);
            return ERROR;
        }
        leidos=fscanf(archivo,"%i;", &info_anterior.numero);
    }
    fclose(archivo);
    //limpiar archivo
    archivo = fopen(ruta_archivo,"w");
    fclose(archivo);
    return EXITO;
}
/* 
 * Recorre la lista de ultimos capturados, imprime y saca los pokemones correspondientes.
 */
void pokedex_ultimos_capturados(pokedex_t* pokedex)
{
    if(!pokedex) return;
    if(lista_vacia(pokedex->ultimos_capturados)) printf("No hay pokemones capturados.\n");
    while(!lista_vacia(pokedex->ultimos_capturados))
    {
        particular_pokemon_t* pokemon = (particular_pokemon_t*) lista_tope(pokedex->ultimos_capturados);
        if(!pokemon) return;
        printf("CAPTURADO;%s, nivel %i\n",pokemon->nombre, pokemon->nivel);
        destructor_particular_pokemon(pokemon);
        lista_desapilar(pokedex->ultimos_capturados);
    }
}
/* 
 * Recorre la lista de ultimos vistos, imprime y saca los pokemones correspondientes.
 */
void pokedex_ultimos_vistos(pokedex_t* pokedex)
{
    if(!pokedex) return;
    if(lista_vacia(pokedex->ultimos_vistos)) printf("No hay vistos para mostrar.\n");
    while(!lista_vacia(pokedex->ultimos_vistos))
    {
        particular_pokemon_t* pokemon = (particular_pokemon_t*) lista_primero(pokedex->ultimos_vistos);
        if(!pokemon) return;
        printf("VISTO: %s, nivel %i\n",pokemon->nombre, pokemon->nivel);
        destructor_particular_pokemon(pokemon);
        lista_desencolar(pokedex->ultimos_vistos);
    }
}
/* 
 * Recibe una lista y libera sus elementos con la funcion destruir.
 */
void liberar_elementos_lista(lista_t* lista, destructor destruir){
    lista_iterador_t* iterador = lista_iterador_crear(lista);
    while(lista_iterador_tiene_siguiente(iterador))
    {
        destruir(lista_iterador_siguiente(iterador));
    }
    lista_iterador_destruir(iterador);
}
//destructores, TODO DOC
void destructor_particular_pokemon(void* poke){
    free((particular_pokemon_t*)poke);
}
void destructor_especie_pokemon(void* poke){
    //iterar lista de pokemon particulares y liberarlos
    lista_t* lista_pokes = ((especie_pokemon_t*)poke)->pokemones; 
    lista_iterador_t* mis_pokes = lista_iterador_crear(lista_pokes);
    while(lista_iterador_tiene_siguiente(mis_pokes))
    {
        destructor_particular_pokemon(lista_iterador_siguiente(mis_pokes));
    }
    lista_iterador_destruir(mis_pokes);
    //destruir lista y liberar especie
    lista_destruir(lista_pokes);
    free((especie_pokemon_t*)poke);
}
/* 
 * Imprime los pokemones en la lista si coinciden con nombre_pokemon, o si
 * imprimir_todos es true.
 */
void imprimir_particulares(lista_t* pokemones, bool imprimir_todos, char nombre_pokemon[MAX_NOMBRE])
{
    if(!pokemones) return;
    bool encontrado = false;
    lista_iterador_t* it = lista_iterador_crear(pokemones);
    if(!it) return;
    
    while(lista_iterador_tiene_siguiente(it)){
        particular_pokemon_t* actual = (particular_pokemon_t*)lista_iterador_siguiente(it);
        if(imprimir_todos||strcmp(actual->nombre, nombre_pokemon)==0){
            printf("*Nombre del pokemon: %s\n", actual->nombre  );
            encontrado = true;
        }
        if(!imprimir_todos && encontrado) break;
    }
    lista_iterador_destruir(it);
    if(!imprimir_todos && !encontrado) printf("no encontro pokemon nombrado %s\n", nombre_pokemon);
}
/* 
 * imprime la información de los pokemon.
 */
void pokedex_informacion(pokedex_t* pokedex, int numero_pokemon, char nombre_pokemon[MAX_NOMBRE]){
    //busco especie en mi arbol, si existe la imprimo
    especie_pokemon_t info_especie;
    info_especie.numero = numero_pokemon;
    especie_pokemon_t* mi_especie = buscar_especie(pokedex, &info_especie);
    if(!mi_especie) 
    {
        printf("no existe la especie!\n");
        return;
    }
    printf("Especie: %s. numero: %i.\n",mi_especie->nombre, mi_especie->numero);
    //recorro mi lista. si el nombre es vacío imprimo todos los pokemon, 
    //si no imprimo el primero que tenga el mismo nombre si existe
    bool imprimir_todos = (strcmp(nombre_pokemon, "")==0);
    imprimir_particulares(mi_especie->pokemones, imprimir_todos, nombre_pokemon);
}

bool guardar_datos_especie(void* especie, void* archivo)
{
    fprintf((FILE*) archivo, "E;%s;%i;%s\n", ((especie_pokemon_t*)especie)->nombre,
                                            ((especie_pokemon_t*)especie)->numero,
                                            ((especie_pokemon_t*)especie)->descripcion);
    lista_iterador_t* it=lista_iterador_crear(((especie_pokemon_t*)especie)->pokemones);
    while(lista_iterador_tiene_siguiente(it))
    {
        particular_pokemon_t* poke = lista_iterador_siguiente(it);
        fprintf((FILE*) archivo, "P;%s;%i;%c\n", poke->nombre, poke->nivel, (poke->capturado) ? SI : NO);
    }
    lista_iterador_destruir(it);
    return false;
}
int pokedex_apagar(pokedex_t* pokedex)
{
    //abrir nuevo archivo para escribir
    FILE* archivo = fopen(RUTA_POKEDEX, "w");
    if(!archivo) return ERROR;
    //imprimir nombre de dueño
    fprintf(archivo, "%s\n",pokedex->nombre_entrenador);
    //recorrer arbol preorden e imprimir a la especie y sus particulares
    abb_con_cada_elemento(pokedex->pokemones, ABB_RECORRER_PREORDEN, guardar_datos_especie,archivo);
    fclose(archivo);
    return EXITO;
}
static scan_t* leer_linea(FILE* archivo,scan_t* scan)
{
    if(!scan) return NULL;
    scan->error = false;
    char capturado = '\0';
    int leidos = 0;
    scan->tipo = (char)fgetc(archivo);
    switch (scan->tipo)
    {
        case ES_POKEMON:
            scan->pokemon = malloc(sizeof(particular_pokemon_t));
            leidos = fscanf(archivo,";%[^;];%i;%c\n",
                        scan->pokemon->nombre,
                        &scan->pokemon->nivel, 
                        &capturado);
            if(capturado==NO) scan->pokemon->capturado = false;
            else if(capturado==SI) scan->pokemon->capturado = true;
            break;
        case ES_ESPECIE:
            scan->especie = malloc(sizeof(especie_pokemon_t));
            leidos = fscanf(archivo,";%[^;];%i;%[^\n]\n",
                        scan->especie->nombre,
                        &scan->especie->numero,
                        scan->especie->descripcion);
            scan->especie->pokemones = lista_crear();
            break;
        default:
            scan->error  = true;
    }
    if(leidos == EOF) scan->error = true;
    return scan;
}
pokedex_t* pokedex_prender()
{
    char nombre[100] = "\0";
    FILE* archivo = fopen(RUTA_POKEDEX, "r");
    if(!archivo) 
    {
        printf("Pokedex no encontrado. Por favor cargue un archivo %s.\n", RUTA_POKEDEX);
        return NULL;
    }
    fscanf(archivo,"%[^\n]\n", nombre);
    pokedex_t* pokedex = pokedex_crear(nombre);
    scan_t scan;
    scan.error = false;
    int resultado = EXITO;
    leer_linea(archivo, &scan);
    while(!(scan.error) && resultado !=ERROR){
        if(scan.tipo == 'E') resultado = arbol_insertar(pokedex->pokemones, scan.especie);
        if(scan.tipo == 'P') resultado = lista_insertar(scan.especie->pokemones, scan.pokemon);
        leer_linea(archivo, &scan);
    }
    fclose(archivo);
    return pokedex;
}
