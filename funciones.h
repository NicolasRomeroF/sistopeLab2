#ifndef _FUNCIONES_H_
#define _FUNCIONES_H_

#include <pthread.h>
#include <stdio.h>
#include "structs.h"


void printMatrizArchivo(FILE* salida, char** matriz, int N, int M);
int verificarSize(char* palabra, int y);
int verificarVacio(char* palabra, int x, int y);
void mayusculas(char* palabra);
char letraRandom();
void rellenarMatriz();
void printMatriz(char** matriz, int N, int M);
char** crearMatriz(int N, int M);
pthread_mutex_t** crearMatrizMutex(int N, int M);
void ubicarPalabra(char* palabra, int x, int y);
void bloquearMutex(int x, int y, int sizePalabra);
void desbloquearMutex(int x, int y, int sizePalabra);
void* ubicar (void* id);
void* identificar(void* id);
void leerArchivo(FILE* entrada, int cantPalabras);
void printDiccionario(int cantPalabras);
int asignarPalabras(FILE* entrada, int cantPalabras, int palabrasPorHebra, int cantHebras);
void mutexInit();
void printPalabras(int cantPalabras, int palabrasPorHebra, int cantHebras);
int sizePalabraLarga(int cantPalabras);
int comprobarEntradas(int cantHebras, int cantPalabras, int sizeN, int sizeM);
int inicio(int argc, char **argv);
void freeMatrices();


#endif

