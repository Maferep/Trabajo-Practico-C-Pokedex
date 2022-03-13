#include "lista.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#define EXITO 0
#define FRACASO -1

typedef struct nodo
{
    struct nodo* siguiente;
    void* contenido;
}nodo_t;

struct lista
{
    nodo_t* inicio;
    size_t tamanio;
    nodo_t* final;
};
struct lista_iterador
{
    lista_t* lista;
    nodo_t* nodo;
};
/*
 *Recibe un puntero a una lista creada y una posición.
 *Devuelve true si posicion esta en el rango [0, lista->tamanio] inclusive
 */
static bool posicion_en_rango(lista_t* lista, size_t posicion)
{
    return (posicion<=(lista->tamanio));
}
/* 
 * Recibe un puntero a una lista creada.
 * Devuelve true si la lista está vacía o false en caso contrario.
 */
bool lista_vacia(lista_t* lista) 
{ 
    if(lista) return lista->tamanio == 0; 
    return true;
} 
/*
 * Recibe un puntero a una lista creada.
 * Devuelve la cantidad de elementos almacenados en la lista.
 */
size_t lista_elementos(lista_t* lista)
{
    if(lista) return (lista->tamanio);
    return 0;
}
/*
Asigna memoria a un nodo, le asigna un contenido y lo hace apuntar a NULL.
devuelve el nodo si fue creado. Si falla devuelve NULL.
*/
static nodo_t* nodo_crear(void* contenido)
{
    nodo_t* nodo = malloc(sizeof(nodo_t)); 
    if(!nodo) return NULL;
    nodo->contenido = contenido;
    nodo->siguiente = NULL;
    return nodo;
}
/*
 *Busca el nodo [indice] desde 0 hasta tamanio-1 y lo devuelve.
 * si la posición provista es inválida, devuelve NULL.
 */
static nodo_t* lista_acceder_nodo(lista_t* lista, size_t indice)
{
    if(!lista || indice >= lista->tamanio || lista_vacia(lista)) 
        return NULL;
    if(indice==lista->tamanio-1)
    {
        return lista->final;
    }
    nodo_t* nodo_actual = lista->inicio;
    for(size_t i = 0; i < indice; i++)
    {
        nodo_actual = nodo_actual->siguiente; 
    }
    return nodo_actual;
}

/*
 * Crea la lista reservando la memoria necesaria.
 * Devuelve un puntero a la lista creada o NULL en caso de error.
 */
lista_t* lista_crear()
{
    lista_t* nueva = malloc(sizeof(lista_t));
    if(!nueva)
    {
        printf("ERROR creación lista\n");
        return NULL;
    }
    nueva->inicio = NULL;
    nueva->final = NULL;
    nueva->tamanio = 0;
    return nueva;
}
/*
 * Libera la memoria reservada por la lista.
 */
void lista_destruir(lista_t* lista)
{
    if(!lista)
    {
        printf("Lista no existe\n");
        return;
    }
    nodo_t* auxiliar;
    nodo_t* actual = lista->inicio;
    while(actual)
    {
        auxiliar = actual->siguiente;
        free(actual);
        actual = auxiliar;
    }
    free(lista);
}
/*
 * Inserta un elemento en la posicion indicada, donde 0 es insertar
 * como primer elemento y 1 es insertar luego del primer elemento.  
 * En caso de no existir la posicion indicada, lo inserta al final.
 * Devuelve 0 si pudo insertar o -1 si no pudo.
 */
int lista_insertar_en_posicion(lista_t* lista, void* elemento, size_t posicion)
{   
    if(!lista) 
        return FRACASO;
    nodo_t* nuevo_nodo = nodo_crear(elemento);
    if(!nuevo_nodo) 
        return FRACASO;
    if(!posicion_en_rango(lista, posicion))
        posicion = lista->tamanio;
    
    nodo_t* anterior = lista_acceder_nodo(lista, posicion-1); 
    //1. A donde apunta mi nodo nuevo?
    if(anterior) nuevo_nodo->siguiente =anterior->siguiente;
    else nuevo_nodo->siguiente=lista->inicio;
    //2. quien apunta a mi nodo?
    if(!anterior) 
        lista->inicio = nuevo_nodo;
    else 
        anterior->siguiente = nuevo_nodo;
    
    (lista->tamanio)++;
    if(!(nuevo_nodo->siguiente))
        lista->final = nuevo_nodo;
    return EXITO;
}
/*
 * Inserta un elemento al final de la lista.
 * Devuelve 0 si pudo insertar o -1 si no pudo.
 */
int lista_insertar(lista_t* lista, void* elemento)
{
    if(!lista)
        return FRACASO;
    return lista_insertar_en_posicion(lista, elemento,lista->tamanio);
    
}
/*
 * Quita de la lista el elemento que se encuentra en la posición
 * indicada, donde 0 es el primer elemento.  
 * En caso de no existir esa posición se intentará borrar el último
 * elemento.  
 * Devuelve 0 si pudo eliminar algún elemento o -1 si no pudo.
 */
int lista_borrar_de_posicion(lista_t* lista, size_t posicion)
{
    if(!lista || lista_vacia(lista)) 
        return FRACASO;
    if(posicion >= lista->tamanio)
        posicion = lista->tamanio - 1;
    nodo_t* previo = lista_acceder_nodo(lista, posicion-1); 
    nodo_t* a_borrar = previo ? previo->siguiente : lista->inicio ;
    nodo_t* posterior = a_borrar->siguiente;
    //desconecto nodo a borrar
    if(previo)
        previo->siguiente = posterior;
    else 
        lista->inicio = posterior;
    //si era el último, reasigno lista->final
    if(!posterior) 
        lista->final = previo;
    //libero y achico lista
    free(a_borrar);
    (lista->tamanio)--;
    return EXITO;
}
/*
 * Borra el elemento al final de la lista.
 * Devuelve 0 si pudo borrar o -1 si no pudo.
 */
int lista_borrar(lista_t* lista)
{
    if(!lista)
        return FRACASO;
    return lista_borrar_de_posicion(lista,(lista->tamanio) - 1);
}
/*
 * FUNCION ADICIONAL. Recorre la lista buscando el elemento. Si lo encuentra lo elimina.
 */
void* lista_borrar_elemento(lista_t* lista, void* elemento, lista_comparador comparador)
{
    if(!lista || lista_vacia(lista)) 
        return NULL;
    nodo_t* nodo_actual = lista->inicio;
    void* borrado = NULL;
    for(size_t i = 0; i < lista->tamanio; i++)
    {
        if(comparador(nodo_actual->contenido, elemento)==0)
        {
            borrado = nodo_actual->contenido;
            lista_borrar_de_posicion(lista, i);
            break;
        }
        nodo_actual = nodo_actual->siguiente; 
    }
    return borrado;
}
/*
 * Devuelve el elemento en la posicion indicada, donde 0 es el primer
 * elemento.
 *
 * Si no existe dicha posicion devuelve NULL.
 */
void* lista_elemento_en_posicion(lista_t* lista, size_t posicion)
{
    nodo_t* nodo=lista_acceder_nodo(lista,posicion);
    if(nodo)return nodo->contenido;
    else return NULL;
}
/* 
 * Devuelve el último elemento de la lista o NULL si la lista se
 * encuentra vacía.
 */
void* lista_ultimo(lista_t* lista)
{
    if(!lista) return NULL;
    return lista_elemento_en_posicion(lista, lista->tamanio - 1);
}
/* 
 * Apila un elemento.
 * Devuelve 0 si pudo o -1 en caso contrario.
 */
int lista_apilar(lista_t* lista, void* elemento)
{
    return lista_insertar_en_posicion(lista, elemento, 0); 
}
/* 
 * Desapila un elemento.
 * Devuelve 0 si pudo desapilar o -1 si no pudo.
 */
int lista_desapilar(lista_t* lista)
{
    return lista_borrar_de_posicion(lista, 0); 
}
/*
 * Devuelve el elemento en el tope de la pila o NULL
 * en caso de estar vacía.
 */
void* lista_tope(lista_t* lista)
{
    return lista_elemento_en_posicion(lista, 0);
}
/* 
 * Encola un elemento.
 * Devuelve 0 si pudo encolar o -1 si no pudo.
 */
int lista_encolar(lista_t* lista, void* elemento)
{
    return lista_insertar(lista, elemento);
}
/* 
 * Desencola un elemento.
 * Devuelve 0 si pudo desencolar o -1 si no pudo.
 */
int lista_desencolar(lista_t* lista)
{
    return lista_borrar_de_posicion(lista, 0);
}
/*
 * Devuelve el primer elemento de la cola o NULL en caso de estar
 * vacía.
 */
void* lista_primero(lista_t* lista)
{
    return lista_elemento_en_posicion(lista, 0);
}
/*
 * Crea un iterador para una lista. El iterador creado es válido desde
 * el momento de su creación hasta que no haya mas elementos por
 * recorrer o se modifique la lista iterada (agregando o quitando
 * elementos de la lista).
 *
 * Devuelve el puntero al iterador creado o NULL en caso de error.
 */
lista_iterador_t* lista_iterador_crear(lista_t* lista)
{
    if(!lista) return NULL;
    lista_iterador_t* iterador = malloc(sizeof(lista_iterador_t));
    if(!iterador) 
        return NULL;
    iterador->lista = lista;
    iterador->nodo = lista->inicio;
    return iterador;
}
/*
 * Devuelve true si hay mas elementos sobre los cuales iterar o false
 * si no hay mas.
 */
bool lista_iterador_tiene_siguiente(lista_iterador_t* iterador)
{
    if(!iterador) 
        return false;
    return iterador->nodo != NULL;
}
/*
 * Devuelve el próximo elemento disponible en la iteración.
 * En caso de error devuelve NULL.
 */
void* lista_iterador_siguiente(lista_iterador_t* iterador)
{
    if(!iterador || !lista_iterador_tiene_siguiente(iterador)) 
        return NULL;
    void* contenido = iterador->nodo->contenido;
    iterador->nodo = iterador->nodo->siguiente;
    return contenido;
}
/*
 * Libera la memoria reservada por el iterador.
 */
void lista_iterador_destruir(lista_iterador_t* iterador)
{
    if(iterador) 
        free(iterador);
}
/*
 * Iterador interno. Recorre la lista e invoca la funcion con cada
 * elemento de la misma.
 */
void lista_con_cada_elemento(lista_t* lista, void (*funcion)(void*, void*), void *contexto)
{
    nodo_t* nodo_actual = lista->inicio;
    int i = 0;
    while(nodo_actual)
    {
        funcion(nodo_actual->contenido, contexto);
        nodo_actual = nodo_actual->siguiente;
        i++;
    }
}




