#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "abb.h"
#define EXITO 0
#define FRACASO -1
#define INTERRUPCION -2
/* 
 * Recibe una funcion comparador que puede comparar los elementos,
 * y funcion destructor para liberar la memoria del elemento al ser destruido.
 * Crea el arbol y reserva la memoria necesaria de la estructura.
 * Devuelve el arbol creado.
 * Si recibe un comparador NULL devuelve NULL.
 * 
 */
abb_t* arbol_crear(abb_comparador comparador, abb_liberar_elemento destructor)
{
    if(!comparador) return NULL;
    abb_t* arbol = malloc(sizeof(abb_t));
    if(!arbol) return NULL;
    arbol->nodo_raiz = NULL;
    arbol->comparador = comparador;
    arbol->destructor = destructor;
    return arbol;
}
/* 
 * Reserva memoria para un nodo y le asigna el elemento recibido.
 * Devuelve la direccion de la memoria reservada.
 * Si no pudo reservar la memoria devuelve NULL.
 */
static nodo_abb_t* nodo_crear(void* elemento)
{
    nodo_abb_t* nodo = calloc(1, sizeof(nodo_abb_t));
    if(!nodo) return NULL;
    nodo->elemento = elemento;
    return nodo;
}
/* 
 * Recibe un puntero a puntero al nodo, un comparador, y un elemento.
 * Si *p_nodo es nulo, hago *p_nodo la direccion del nodo nuevo.
 * Si existe *p_nodo, intento insertar a la izquierda o derecha y devuelvo el resultado.
 * Si recibe un puntero a puntero nulo, o no logra crear un nodo nuevo, devuelve FRACASO.
 */
int nodo_insertar(abb_comparador comparador, nodo_abb_t** p_nodo, void* elemento)
{   
    if(!p_nodo)  return FRACASO;
    //si *p_nodo es nulo, ubico el nodo nuevo en *p_nodo.
    if(!*p_nodo) 
    {
        nodo_abb_t* nodo_auxiliar = nodo_crear(elemento); 
        if(!nodo_auxiliar) return FRACASO;
        *p_nodo = nodo_auxiliar;
        return EXITO;
    }
    //si existe el nodo, comparar su elemento con el nuestro, e insertar en un hijo
    else if(comparador(elemento, (*p_nodo)->elemento)>0 )
        return nodo_insertar(comparador,&((*p_nodo)->derecha),elemento);
    else
        return nodo_insertar(comparador,&((*p_nodo)->izquierda),elemento);
}
/*
 * Recibe un arbol y un elemento.
 * Si logra insertar devuelve EXITO, si no devuelve FRACASO.
 * Si el arbol es NULL devuelve FRACASO.
 */
int arbol_insertar(abb_t* arbol, void* elemento)
{
    if(!arbol) return FRACASO;
    return nodo_insertar(arbol->comparador, &(arbol->nodo_raiz), elemento);
}
/*
 * Halla el menor nodo de un abb recursivamente.
 */
nodo_abb_t** rec_minimo_arbol(nodo_abb_t** p_nodo) //p. ej. &(arbol->raiz)
{
    //condicion de corte
    if(!p_nodo||!*p_nodo) return NULL; 
    //entrar al nodo izquierdo
    if((*p_nodo)->izquierda) return rec_minimo_arbol(&(*p_nodo)->izquierda);
    //si no tiene, retorna a si mismo
    else return p_nodo;
}
/*
 *Devuelve la direccion del puntero al nodo heredero del nodo dado (menor de los mayores)
 */
nodo_abb_t** sucesor(nodo_abb_t** p_nodo_a_borrar)
{
    if(!p_nodo_a_borrar||!*p_nodo_a_borrar || !(*p_nodo_a_borrar)->derecha)
        return NULL;
    return rec_minimo_arbol(&(*p_nodo_a_borrar)->derecha);
}
//revisa si el puntero derecha del nodo es null.
bool tiene_hijo_derecho(nodo_abb_t* nodo)
{
    return nodo->derecha!=NULL;
}
//revisa si el puntero derecha del nodo es null.
bool tiene_hijo_izquierdo(nodo_abb_t* nodo)
{
    return nodo->izquierda!=NULL;
}
/*
 * Recibe un nodo con un solo subarbol hijo
 * en su direccion ubica a su unico subarbol hijo, y libera su memoria.
 * Devuelve un puntero al hijo que lo reemplazo.
 * Si no tiene subarboles hijos, libera el nodo y devuelve NULL.
 */
nodo_abb_t *liberar_nodo_de_un_hijo(nodo_abb_t **nodo)
{
    nodo_abb_t* auxiliar = *nodo;
    nodo_abb_t* sucesor = tiene_hijo_derecho(*nodo) ? (*nodo)->derecha : (*nodo)->izquierda;
    *nodo = sucesor;
    free(auxiliar);
    return sucesor;
}
/*
 * Recibe la direccion p_nodo a un puntero al nodo.
 * Si el nodo tiene dos hijos, escoge un heredero izquierda->derecha para reemplazarlo.
 * Si tiene un solo hijo, conecta este hijo a su abuelo.
 * Si la direccion o su punteros son nulas, devuelve FRACASO y no destruye nada.
 */
int nodo_destruir_uno(nodo_abb_t** p_nodo_raiz, abb_liberar_elemento destructor)
{
    if(p_nodo_raiz==NULL||(*p_nodo_raiz)==NULL) return FRACASO;
    //destruyo contenido del nodo
    destructor((*p_nodo_raiz)->elemento);
    if(tiene_hijo_derecho(*p_nodo_raiz) && tiene_hijo_izquierdo(*p_nodo_raiz)) 
    {
        nodo_abb_t** p_heredero = sucesor(p_nodo_raiz);
        if(!p_heredero || !(*p_heredero)) return FRACASO; 
        //mudar elemento del heredero
        (*p_nodo_raiz)->elemento = (*p_heredero)->elemento;
        //el heredero tiene un hijo derecho o NULL. Quien apunta al heredero ahora apunta a su hijo
        liberar_nodo_de_un_hijo(p_heredero);
    } 
    else liberar_nodo_de_un_hijo(p_nodo_raiz);
    return EXITO;
}
/*
 *Recibe la direccion p_nodo a un puntero al nodo desde el cual se comienza a buscar.
 *Busca en un abb el nodo con el elemento dado. 
 *Si lo encuentra devuelve una referencia al puntero (lo cual permite cambiarlo.)
 *Si no lo encuentra devuelvle NULL.
 *Si p_nodo es nula, o *p_nodo es nula, devuelve NULL.

                * -> !elem
               * *
              /   \
             *     * -> !elem
            * *   *(*) RETURN 
                     \
                      * -> elemento
                     * *
*/
nodo_abb_t** nodo_buscar_puntero(nodo_abb_t** p_nodo, void* elemento, abb_comparador comparador)
{
    if(!p_nodo || !*p_nodo) return NULL;
    if(comparador(elemento, (*p_nodo)->elemento )==0) return p_nodo;

    if (comparador(elemento, (*p_nodo)->elemento )>0)
        return nodo_buscar_puntero(&((*p_nodo)->derecha), elemento, comparador);
    else 
        return nodo_buscar_puntero(&((*p_nodo)->izquierda), elemento, comparador);
}

int arbol_borrar(abb_t* arbol, void* elemento)
{
    if(!arbol) return FRACASO;
    //busqueda recursiva del nodo a borrar
    nodo_abb_t** p_nodo_a_borrar = nodo_buscar_puntero(&(arbol->nodo_raiz), elemento, arbol->comparador);
    //destruir sin destruir hijos
    if(!p_nodo_a_borrar) return FRACASO;
    if(nodo_destruir_uno(p_nodo_a_borrar, arbol->destructor)==FRACASO) return FRACASO;
    return EXITO;
}
/* 
 * Recibe un nodo raiz, un elemento y un comparador.
 * Si el elemento coincide con el elemento del nodo (su comparador devuelve 0)
 * devuelve el nodo raiz recibido.
 * Si el elemento es 'mayor' al de la raiz, sigue buscando por la derecha.
 * Si no, sigue buscando por la izquierda.
 * Si recibe una raiz nula, devuelve NULL.
*/
nodo_abb_t* nodo_buscar(nodo_abb_t* raiz, void* elemento, abb_comparador comparador)
{ 
    //cond. de corte
    if(!raiz) return NULL;
    //elementos coinciden
    if(comparador(elemento, raiz->elemento )==0) 
        return raiz;
    //elementos no coinciden
    else if (comparador(elemento, raiz->elemento )>0) 
        return nodo_buscar(raiz->derecha, elemento, comparador);
    else 
        return nodo_buscar(raiz->izquierda, elemento, comparador);
}
/*
 * Recibe un arbol y un elemento.
 * Si el elemento coincide con el elemento de un nodo, 
 * devuelve el elemento encontrado.
 * Si no, devuelve NULL.
 * Si recibe un arbol nulo devuelve NULL.
 */
void* arbol_buscar(abb_t* arbol, void* elemento)
{
    if(!arbol) return NULL;
    nodo_abb_t* nodo = nodo_buscar(arbol->nodo_raiz, elemento, arbol->comparador);
    if(!nodo) return NULL;
    return nodo->elemento;
}
/*
 * Recibe un arbol.
 * Si es nulo o no tiene nodos, devuelve NULL.
 * Si no, devuelve el contenido de su raiz.
 */
void* arbol_raiz(abb_t* arbol)
{
    if(!arbol) return NULL;
    if(!arbol->nodo_raiz) return NULL;
    return arbol->nodo_raiz->elemento;
}
/*
 * Determina si el arbol esta vacio.
 * Devuelve true si esta vacio o el arbol es NULL, false si el arbol tiene elementos.
 */
bool arbol_vacio(abb_t* arbol)
{
    if(!arbol) return true;
    if(!arbol->nodo_raiz) return true;
    else return false;
}
/*
 *Recibe un nodo, un puntero 'array' y su tamanio. 
 *Recorre recursivamente, en inorden, los subarboles del nodo. 
 *Inserta los 'n' elementos del subarbol izquierdo en  la direccion 'array'.
 *Luego el elemento se inserta a si mismo en la direccion array + n.
 *Finalmente inserta los 'm' elementos del subarbol derecho en la direccion 'array + n + 1'.
 *Devuelve n + 1 + m o tamanio_array si se le acaba el espacio.

          array
            |
            v
  -|-|-|-|-|i|i|i|i|i|i|i|i|elem|d|d|d|d|d|d|-|-|-|-|

           |-------n-------|    |-----m-----|

           |------------(RETURN)------------|
           |-----------(tamanio_array)--------------|
*/
int nodo_recorrido_inorden(nodo_abb_t* nodo, void** array, int tamanio_array)
{
    if(!nodo|| tamanio_array <=0) return 0;
    int insertados = 0;
    //recursion por la izquierda
    if (nodo->izquierda)
        insertados += nodo_recorrido_inorden(nodo->izquierda, array + insertados, tamanio_array - insertados);
    if(tamanio_array <= insertados) return insertados;
    //insercion si me queda espacio
    array[insertados] = nodo->elemento;
    insertados++;
    //recursion por la derecha
    if (nodo->derecha)
        insertados += nodo_recorrido_inorden(nodo->derecha, array + insertados, tamanio_array - insertados);
    return insertados;
}
/*
 *Recibe un nodo, un puntero 'array' y su tamanio. 
 *Recorre recursivamente, en preorden, los subarboles del nodo. 
 *El elemento se inserta a si mismo en la direccion array.
 *Inserta los 'n' elementos del subarbol izquierdo en  la direccion 'array + 1'.
 *Finalmente inserta los 'm' elementos del subarbol derecho en la direccion 'array + 1 + n'.
 *Devuelve 1 + n + m o tamanio_array si se le acaba el espacio.

          array
            |
            v
  -|-|-|-|-|elem|i|i|i|i|i|i|i|i|d|d|d|d|d|d|-|-|-|-|

                |-------n-------|-----m-----|

           |------------(RETURN)------------|
           |-----------(tamanio_array)--------------|
*/
int nodo_recorrido_preorden(nodo_abb_t* nodo, void** array, int tamanio_array)
{
    int insertados =0;
    //insercion
    array[insertados] = nodo->elemento; 
    insertados++;
    //recursion por la izquierda
    if(nodo->izquierda) insertados += nodo_recorrido_preorden(nodo->izquierda, array+insertados, tamanio_array-insertados);
    //recursion por la derecha
    if(nodo->derecha) insertados += nodo_recorrido_preorden(nodo->derecha, array+insertados, tamanio_array-insertados);
    return insertados;
}
/*         
 * Recibe un nodo, un puntero 'array' y su tamanio. 
 * Recorre recursivamente, en postorden, los subarboles del nodo. 
 * Inserta los 'n' elementos del subarbol izquierdo en  la direccion 'array'.
 * Inserta los 'm' elementos del subarbol derecho en la direccion 'array + n'.
 * Finalmente el elemento se inserta a si mismo en la direccion 'array + n + m'.
 * Devuelve n + m + 1 o tamanio_array si se le acaba el espacio.

          array
            |
            v
  -|-|-|-|-|i|i|i|i|i|i|i|i|d|d|d|d|d|d|elem|-|-|-|-|

           |-------n-------|-----m-----|

           |------------(RETURN)------------|
           |-----------(tamanio_array)--------------|
*/
int nodo_recorrido_postorden(nodo_abb_t* nodo, void** array, int tamanio_array)
{
    if(!nodo|| tamanio_array <=0) return 0;
    int insertados =0;
    //recursion por la izquierda
    if(nodo->izquierda) 
        insertados += nodo_recorrido_postorden(nodo->izquierda, array+insertados, tamanio_array-insertados);
    //recursion por la derecha
    if(nodo->derecha) 
        insertados += nodo_recorrido_postorden(nodo->derecha, array+insertados, tamanio_array-insertados);
    //insercion si me queda espacio
    if(tamanio_array > insertados)
    {
        array[insertados] = nodo->elemento;
        insertados++;
    } 
    return insertados;
}
/*
 * Recibe un arbol, un array y su tamanio.
 * Devuelve 0 si el arbol es nulo, su raiz es nula, el arreglo es nulo o su tamanio es negativo.
 * Si no, devuelve la cantidad de elementos del arbol que puede llenar en el array
 * con recorrido inorden.
 */
int arbol_recorrido_inorden(abb_t* arbol, void** array, int tamanio_array)
{
    if( !arbol || !(arbol->nodo_raiz) || !array || tamanio_array<=0 ) return 0;
    return nodo_recorrido_inorden(arbol->nodo_raiz, array, tamanio_array);
}
/*
 * Recibe un arbol, un array y su tamanio.
 * Devuelve 0 si el arbol es nulo, su raiz es nula, el arreglo es nulo o su tamanio es negativo.
 * Si no, devuelve la cantidad de elementos del arbol que puede llenar en el array
 * con recorrido preorden.
 */
int arbol_recorrido_preorden(abb_t* arbol, void** array, int tamanio_array)
{
    if( !arbol || !(arbol->nodo_raiz) || !array || tamanio_array<=0 ) return 0;
    return nodo_recorrido_preorden(arbol->nodo_raiz, array, tamanio_array);
}
/*
 * Recibe un arbol, un array y su tamanio.
 * Devuelve 0 si el arbol es nulo, su raiz es nula, el arreglo es nulo o su tamanio es negativo.
 * Si no, devuelve la cantidad de elementos del arbol que puede llenar en el array
 * con recorrido postorden.
 */
int arbol_recorrido_postorden(abb_t* arbol, void** array, int tamanio_array)
{
    if( !arbol || !(arbol->nodo_raiz) || !array || tamanio_array<=0 ) return 0;
    return nodo_recorrido_postorden(arbol->nodo_raiz, array, tamanio_array);
}
/*
 * Recibe la direccion p_nodo a un puntero al nodo.
 * Destruye a un nodo y sus subarboles con destructor.
 * Si la direccion o su punteros son nulas, devuelve NULL y no destruye nada.
 */
int nodo_destruir(nodo_abb_t** p_nodo, abb_liberar_elemento destructor)
{
    if(p_nodo==NULL) return FRACASO;
    else if ((*p_nodo)==NULL) return FRACASO;
    if((*p_nodo)->izquierda) nodo_destruir(&(*p_nodo)->izquierda, destructor);
    if((*p_nodo)->derecha) nodo_destruir(&(*p_nodo)->derecha, destructor);
    if(destructor) destructor((*p_nodo)->elemento);
    free(*p_nodo);
    *p_nodo = NULL;
    return EXITO;
}
/*
 * Recibe un arbol. Si es no nulo, destruye sus elementos con el destructor y libera la memoria.
 */
void arbol_destruir(abb_t* arbol)
{
    if(!arbol) return;
    nodo_destruir(&(arbol->nodo_raiz), arbol->destructor);
    free(arbol);
}
/* 
 * Recibe un nodo no nulo, un puntero a funcion y una variable extra. 
 * Recorre recursivamente, en inorden, los subarboles del nodo. 
 * En cada nodo, ejecuta la funcion con el elemento del nodo y el extra.
 * Detiene la operacion si funcion devuelve true, y devuelve INTERRUPCION.
 * Si termina el arbol sin que la funcion devuelva true, devuelve EXITO.
*/
int rec_iterador_inorden(nodo_abb_t* nodo, bool (*funcion)(void*, void*), void* extra)
{
    //recursion por la izquierda
    if (nodo->izquierda)
        if(rec_iterador_inorden(nodo->izquierda, funcion, extra)==INTERRUPCION) return INTERRUPCION;
    //llamada a funcion
    if(funcion(nodo->elemento, extra)) return INTERRUPCION;
    //recursion por la derecha
    if (nodo->derecha)
        if(rec_iterador_inorden(nodo->derecha, funcion, extra)==INTERRUPCION) return INTERRUPCION;
    return EXITO;
}
/* 
 * Recibe un nodo no nulo, un puntero a funcion y una variable extra. 
 * Recorre recursivamente, en preorden, los subarboles del nodo. 
 * En cada nodo, ejecuta la funcion con el elemento del nodo y el extra.
 * Detiene la operacion si funcion devuelve true, y devuelve INTERRUPCION.
 * Si termina el arbol sin que la funcion devuelva true, devuelve EXITO.
*/
int rec_iterador_preorden(nodo_abb_t* nodo, bool (*funcion)(void*, void*), void* extra)
{
    //llamada a funcion
    if(funcion(nodo->elemento, extra)) return INTERRUPCION;
    //recursion por la izquierda
    if (nodo->izquierda)
        if(rec_iterador_preorden(nodo->izquierda, funcion, extra)==INTERRUPCION) return INTERRUPCION;
    //recursion por la derecha
    if (nodo->derecha)
        if(rec_iterador_preorden(nodo->derecha, funcion, extra)==INTERRUPCION) return INTERRUPCION;
    return EXITO;
}
/* 
 * Recibe un nodo no nulo, un puntero a funcion y una variable extra. 
 * Recorre recursivamente, en postorden, los subarboles del nodo. 
 * En cada nodo, ejecuta la funcion con el elemento del nodo y el extra.
 * Detiene la operacion si funcion devuelve true, y devuelve INTERRUPCION.
 * Si termina el arbol sin que la funcion devuelva true, devuelve EXITO.
*/
int rec_iterador_postorden(nodo_abb_t* nodo, bool (*funcion)(void*, void*), void* extra)
{
    //recursion por la izquierda
    if (nodo->izquierda)
        if(rec_iterador_postorden(nodo->izquierda, funcion, extra)==INTERRUPCION) return INTERRUPCION;
    //recursion por la derecha
    if (nodo->derecha)
        if(rec_iterador_postorden(nodo->derecha, funcion, extra)==INTERRUPCION) return INTERRUPCION;
    //llamada a funcion
    if(funcion(nodo->elemento, extra)) return INTERRUPCION;
    return EXITO;
}
/*
 * Recibe un arbol, una funcion de dos parametros, un parametro extra, y una variable 'recorrido'.
 * Dependiendo de 'recorrido', hace un recorrido inorden, preorden o postorden y
 * llama a la funcion para cada elemento. Si la funcion devuelve true interrumpe la operacion.
 * Los recorridos validos son: ABB_RECORRER_INORDEN, ABB_RECORRER_PREORDEN
 * y ABB_RECORRER_POSTORDEN.
*/
void abb_con_cada_elemento(abb_t* arbol, int recorrido, bool (*funcion)(void*, void*), void* extra)
{
    if(!arbol || !(arbol->nodo_raiz) || !funcion) return;
    switch(recorrido)
    {
        case ABB_RECORRER_INORDEN: 
            rec_iterador_inorden(arbol->nodo_raiz,funcion, extra);
            break;
        case ABB_RECORRER_PREORDEN: 
            rec_iterador_preorden(arbol->nodo_raiz,funcion, extra); 
            break;
        case ABB_RECORRER_POSTORDEN: 
            rec_iterador_postorden(arbol->nodo_raiz,funcion, extra);
            break;
    }
}