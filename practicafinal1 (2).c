//Integrantes: Angel Miguel Sanchez Perez, Guillermo Carreto Sanchez, Jayme Said Hernandez Barbosa
//18/12/2023

//Bibliotecas
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Estructura del nodo del arbol 
typedef struct nodo {
    char caracter;               // Caracter almacenado en el nodo 
    int frecuencia;              // Frecuencia del caracter en el texto original
    struct nodo* izquierda;      // Puntero al hijo izquierdo
    struct nodo* derecha;        // Puntero al hijo derecho
} Nodo;

// Estructura de la lista ligada utilizada para construir el arbol
typedef struct lista {
    Nodo* nodo;                  // Puntero al nodo
    struct lista* siguiente;    // Puntero al siguiente nodo en la lista
} Lista;

// Funcion para crear una lista ligada vacia
Lista* crear_lista() {
    return NULL;
}

// Funcion para insertar un nodo en la lista ordenada por frecuencia
void insertar_ordenado(Lista** lista, Nodo* nodo) {
    Lista* nuevo = (Lista*)malloc(sizeof(Lista));
    nuevo->nodo = nodo;
    nuevo->siguiente = NULL;

    // Caso especial: la lista esta vacia o el nodo tiene menor frecuencia que el primer nodo
    if (*lista == NULL || nodo->frecuencia < (*lista)->nodo->frecuencia) {
        nuevo->siguiente = *lista;
        *lista = nuevo;
    } else {
        Lista* temp = *lista;
        // buscar la posicion adecuada para insertar el nuevo nodo
        while (temp->siguiente != NULL && temp->siguiente->nodo->frecuencia < nodo->frecuencia) {
            temp = temp->siguiente;
        }
        nuevo->siguiente = temp->siguiente;
        temp->siguiente = nuevo;
    }
}

// Funcion para extraer y eliminar el nodo con la menor frecuencia de la lista
Nodo* extraer_minimo(Lista** lista) {
    Nodo* temp = (*lista)->nodo;
    Lista* eliminar = *lista;
    *lista = (*lista)->siguiente;
    free(eliminar);
    return temp;
}

// Funcion para construir el arbol a partir de la lista
Nodo* construir_arbol(Lista** lista) {
    while (*lista && (*lista)->siguiente) {
        Nodo* izquierda = extraer_minimo(lista);
        Nodo* derecha = extraer_minimo(lista);

        Nodo* nuevo = (Nodo*)malloc(sizeof(Nodo));
        nuevo->caracter = '\0';
        nuevo->frecuencia = izquierda->frecuencia + derecha->frecuencia;
        nuevo->izquierda = izquierda;
        nuevo->derecha = derecha;

        insertar_ordenado(lista, nuevo);
    }
    return extraer_minimo(lista);
}

// Funcion recursiva para imprimir los codigos
void imprimir_codigos_recursivo(Nodo* raiz, int cod[], int index, FILE* archivo_salida) {
    if (raiz->izquierda) {
        cod[index] = 0;
        imprimir_codigos_recursivo(raiz->izquierda, cod, index + 1, archivo_salida);
    }

    if (raiz->derecha) {
        cod[index] = 1;
        imprimir_codigos_recursivo(raiz->derecha, cod, index + 1, archivo_salida);
    }

    if (!raiz->izquierda && !raiz->derecha) {
        fprintf(archivo_salida, "%c: Frecuencia = %d, Codigo = ", raiz->caracter, raiz->frecuencia);
        for (int i = 0; i < index; i++) {
            fprintf(archivo_salida, "%d", cod[i]);
        }
        fprintf(archivo_salida, "\n");
    }
}

// Funcion para imprimir todos los codigos
void imprimir_codigos(Nodo* raiz, FILE* archivo_salida) {
    int cod[100];
    imprimir_codigos_recursivo(raiz, cod, 0, archivo_salida);
}

// Funcion para liberar la memoria utilizada por el arbol
void liberar_arbol(Nodo* raiz) {
    if (raiz) {
        liberar_arbol(raiz->izquierda);
        liberar_arbol(raiz->derecha);
        free(raiz);
    }
}

// Prototipo de la funcion recursiva para codificar una cadena
void codificar_cadena_recursivo(Nodo* raiz, char caracter, int cod[], int* index, FILE* archivo_salida);

// Funcion para codificar una cadena y escribirla en el archivo de salida
void codificar_cadena(Nodo* raiz, char* cadena, FILE* archivo_salida) {
    fprintf(archivo_salida, "Cadena codificada: ");
    for (int i = 0; i < strlen(cadena); i++) {
        char caracter = cadena[i];
        int cod[100];
        int index = 0;
        codificar_cadena_recursivo(raiz, caracter, cod, &index, archivo_salida);
    }
    fprintf(archivo_salida, "\n");
}

// Funcion recursiva para codificar un caracter en la cadena
void codificar_cadena_recursivo(Nodo* raiz, char caracter, int cod[], int* index, FILE* archivo_salida) {
    if (raiz) {
        if (raiz->caracter == caracter) {
            for (int i = 0; i < *index; i++) {
                fprintf(archivo_salida, "%d", cod[i]);
            }
            fprintf(archivo_salida, " ");
            return;
        }

        cod[*index] = 0;
        (*index)++;
        codificar_cadena_recursivo(raiz->izquierda, caracter, cod, index, archivo_salida);
        (*index)--;

        cod[*index] = 1;
        (*index)++;
        codificar_cadena_recursivo(raiz->derecha, caracter, cod, index, archivo_salida);
        (*index)--;
    }
}

// Funcion para escribir los bits correspondientes a la codificacion de la cadena
void escribir_bits(Nodo* raiz, char* cadena, FILE* archivo_salida_bits) {
    for (int i = 0; i < strlen(cadena); i++) {
        char caracter = cadena[i];
        int cod[100];
        int index = 0;
        codificar_cadena_recursivo(raiz, caracter, cod, &index, archivo_salida_bits);
    }
}

// Funcion para decodificar los bits y escribir el resultado en el archivo de texto
void decodificar_bits(Nodo* raiz, FILE* archivo_entrada_bits, FILE* archivo_salida_texto) {
    Nodo* nodo_actual = raiz;
    int bit;

    while ((bit = fgetc(archivo_entrada_bits)) != EOF) {
        if (bit == '0') {
            nodo_actual = nodo_actual->izquierda;
        } else if (bit == '1') {
            nodo_actual = nodo_actual->derecha;
        }

        if (nodo_actual->izquierda == NULL && nodo_actual->derecha == NULL) {
            // Llegamos a una hoja, escribimos el caracter en el archivo de salida
            fprintf(archivo_salida_texto, "%c", nodo_actual->caracter);
            // Reiniciamos para empezar desde la raiz en la proxima iteracion
            nodo_actual = raiz;
        }
    }
}

// Funcion principal del programa
int main() {
    char nombreArchivoEntrada[100];
    char nombreArchivoSalida[100];

    // Recibe el nombre del archivo a leer
    printf("Introduce el nombre del archivo de entrada: ");
    scanf("%s", nombreArchivoEntrada);

    // Recibe el nombre del archivo a crear
    printf("Introduce el nombre del archivo de salida: ");
    scanf("%s", nombreArchivoSalida);

    // Abre el archivo a leer
    FILE* archivo_entrada = fopen(nombreArchivoEntrada, "r");
    if (archivo_entrada == NULL) {
        perror("Error al abrir el archivo de entrada");
        return 1;
    }

    // Crea el archivo de salida
    FILE* archivo_salida = fopen(nombreArchivoSalida, "w");
    if (archivo_salida == NULL) {
        perror("Error al abrir el archivo de salida");
        fclose(archivo_entrada);
        return 1;
    }

    char cadena[1000];

    // Mensaje si no se puede abrir el archivo a leer
    if (fgets(cadena, sizeof(cadena), archivo_entrada) == NULL) {
        perror("Error al leer el archivo de entrada");
        fclose(archivo_entrada);
        fclose(archivo_salida);
        return 1;
    }

    // Cierra el archivo de entrada
    fclose(archivo_entrada);

    // Elimina el caracter de nueva línea de fgets
    cadena[strcspn(cadena, "\n")] = 0;

    int frecuencia[256] = {0};
    for (int i = 0; i < strlen(cadena); i++) {
        frecuencia[(unsigned char)cadena[i]]++;//le quita el valor negativo del signo y solo guarda el positivo, para contar bien la frecuencia
    }

    Lista* lista = crear_lista();
    for (int i = 0; i < 256; i++) {
        if (frecuencia[i] > 0) {
            Nodo* nuevo = (Nodo*)malloc(sizeof(Nodo));
            nuevo->caracter = i;
            nuevo->frecuencia = frecuencia[i];
            nuevo->izquierda = NULL;
            nuevo->derecha = NULL;
            insertar_ordenado(&lista, nuevo);
        }
    }

    Nodo* raiz = construir_arbol(&lista);
    imprimir_codigos(raiz, archivo_salida);
    codificar_cadena(raiz, cadena, archivo_salida);

    // Crea el archivo donde ira solo la cadena codificada
    FILE* archivo_salida_bits = fopen("bits.txt", "w");
    if (archivo_salida_bits == NULL) {
        perror("Error al abrir el archivo de bits");
        fclose(archivo_entrada);
        fclose(archivo_salida);
        return 1;
    }

    escribir_bits(raiz, cadena, archivo_salida_bits);

    // Cierra el archivo de bits
    fclose(archivo_salida_bits);

    printf("Proceso completado. Resultados guardados en %s\n", nombreArchivoSalida);

    // Decodificacion
    FILE* archivo_entrada_bits = fopen("bits.txt", "r");
    if (archivo_entrada_bits == NULL) {
        perror("Error al abrir el archivo de bits");
        fclose(archivo_entrada);
        fclose(archivo_salida);
        return 1;
    }

    // Nuevo archivo donde ira el resultado de la decodificación
    FILE* archivo_salida_texto = fopen("texto_decodificado.txt", "w");
    if (archivo_salida_texto == NULL) {
        perror("Error al abrir el archivo de salida de texto decodificado");
        fclose(archivo_entrada);
        fclose(archivo_entrada_bits);
        fclose(archivo_salida);
        return 1;
    }

    decodificar_bits(raiz, archivo_entrada_bits, archivo_salida_texto);

    // Libera la memoria del arbol
    liberar_arbol(raiz);

    // Cierra los archivos
    fclose(archivo_salida);
    fclose(archivo_entrada_bits);
    fclose(archivo_salida_texto);

    return 0;
}

