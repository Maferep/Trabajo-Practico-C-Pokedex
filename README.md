# algo2tp2
Este es un programa compuesto de un TDA pokedex, y una interfase de usuario de consola.
Con esta se puede encender un pokedex, guardar su información en un texto,
avistar y evolucionar pokemon, y mostrar ciertas especies o pokemon por pantalla.

La implementación del pokedex utiliza los TDAs de lista y arbol. El TDA lista viene modificado 
de la entrega original para la clase, pues ahora incluye una función lista_borrar_elemento
que permite pasar un comparador y encontrar un elemento de la lista si existe, para luego borrarlo.
Fue agregado para simplificar el código de procesar_evolucion, y en general porque
tiene sentido que una lista tenga esta funcionalidad ya preconstruida.

lista.c y lista.h conforman la lista. abb.c y abb.h conforman el abb. Los tres archivos .txt
(evoluciones, avistamientos y pokedex)
son archivos de ejemplo, y después de ser editados se puede usar el comendo make clean
para devolverlos a su estado anterior. 
Los archivos txt "menu" y "menu_apagado" contienen los mensajes que se imprimen al
llamar el comando 'ayuda', dependiendo si el pokedex esta encendido o apagado.

El programa se compila y corre con 'make' , que utiliza las líneas:

gcc *.c -Wall -Werror -Wconversion -std=c99 -g -o pokedex

para compilar, y

valgrind  -s --leak-check=full ./pokedex

para correr.
