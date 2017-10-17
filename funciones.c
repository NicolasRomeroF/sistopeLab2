#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include "funciones.h"
#include "structs.h"

int N, M;
Hebra * hebras;
pthread_mutex_t** matrizMutex;
pthread_mutex_t bloquear, desbloquear, printear;
char** matriz;
int palabrasPorProceso;
FILE* salida;
char (*diccionario)[128];
int dPrint;



int verificarSize(char* palabra, int y)
{
	int sizePalabra = strlen(palabra);
	if ((y + sizePalabra) >= M)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

int verificarVacio(char* palabra, int x, int y)
{
	int sizePalabra = strlen(palabra);
	int i;
	for (i = y; i < sizePalabra+y; i++)
	{
		if (matriz[x][i] != ' ')
		{
			return 0;
		}
	}
	return 1;
}

void mayusculas(char* palabra)
{
    int i;
    for(i=0;i<strlen(palabra);i++){
        palabra[i]=toupper(palabra[i]);
    }
}

char letraRandom()
{
	char c;
    c = rand()%26;
    c+=97;
    return c;
}

void rellenarMatriz()
{
	int i,j;
	for(i=0;i<N;i++)
	{
		for(j=0;j<M;j++)
		{
			if(matriz[i][j]==' ')
			{
				matriz[i][j]=letraRandom();
			}
		}
	}
}

void printMatriz(char** matriz, int N, int M)
{
	printf("\nInicio Matriz\n\n");
	int i, j;
	for (i = 0; i < N; i++)
	{
		for (j = 0; j < M; j++)
		{
			printf("%c", matriz[i][j]);
		}
		printf("\n");
	}
	printf("\nFin Matriz\n");
}

char** crearMatriz(int N, int M)
{
	char** matriz = (char**)malloc(sizeof(char*)*N);
	int i, j;
	if (matriz == NULL)
	{
		printf("ERROR: No se pudo asignar memoria a la matriz\n");
		return NULL;
	}
	for (i = 0; i < N; i++)
	{
		matriz[i] = (char*)malloc(sizeof(char) * M);
		if (matriz[i] == NULL)
		{
			printf("ERROR: No se pudo asignar memoria a la matriz\n");
			return NULL;
		}
	}
	for (i = 0; i < N; i++)
	{
		for (j = 0; j < M; j++)
		{
			matriz[i][j] = ' ';
		}
	}
	return matriz;
}

pthread_mutex_t** crearMatrizMutex(int N, int M)
{
	pthread_mutex_t** matriz = (pthread_mutex_t**)malloc(sizeof(pthread_mutex_t*)*N);
	int i;
	if (matriz == NULL)
	{
		printf("ERROR: No se pudo asignar memoria a la matriz de mutex\n");
		return NULL;
	}
	for (i = 0; i < N; i++)
	{
		matriz[i] = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t) * M);
		if (matriz[i] == NULL)
		{
			printf("ERROR: No se pudo asignar memoria a la matriz mutex\n");
			return NULL;
		}
	}
	return matriz;
}



void ubicarPalabra(char* palabra, int x, int y)
{
	int len = strlen(palabra);
	int i;
	mayusculas(palabra);
	int cont = 0;
	for (i = y; i < len+y; i++) {
		matriz[x][i] = palabra[cont];
		cont++;
		//printMatriz(matriz, N, M);
	}
	if(dPrint==1)
	{
		pthread_mutex_lock(&printear);
		printf("Se inserto la palabra: %s\n",palabra);
		printMatriz(matriz, N, M);
		pthread_mutex_unlock(&printear);
	}
}

void bloquearMutex(int x, int y, int sizePalabra)
{
	int i;
	if (y + sizePalabra < M)
	{
		for (i = y; i < sizePalabra+y; i++)
		{
			pthread_mutex_lock(&matrizMutex[x][i]);
		}
	}
}

void desbloquearMutex(int x, int y, int sizePalabra)
{
	int i;
	if (y + sizePalabra < M)
	{
		for (i = y; i < sizePalabra+y; i++)
		{
			pthread_mutex_unlock(&matrizMutex[x][i]);
		}
	}
}

void* ubicar (void* id)
{
	//SI NO ES LA ULTIMA HEBRA HACER ESTO
	Hebra* hilo = (Hebra*)id;
	int i = 0;
	int validar = 1;
	while (i < hilo->cantPalabras) {
		while (validar==1)
		{
			hilo->x = rand() % N;
			hilo->y = rand() % M;
			pthread_mutex_lock(&bloquear);
			bloquearMutex(hilo->x, hilo->y, strlen(hilo->palabras[i]));
			pthread_mutex_unlock(&bloquear);
			if (verificarSize(hilo->palabras[i], hilo->y) && verificarVacio(hilo->palabras[i], hilo->x, hilo->y)) {
			
				ubicarPalabra(hilo->palabras[i], hilo->x, hilo->y);
				validar=0;
			}
			pthread_mutex_lock(&desbloquear);
			desbloquearMutex(hilo->x, hilo->y, strlen(hilo->palabras[i]));
			pthread_mutex_unlock(&desbloquear);
		}
		validar=1;
		i++;
	}

	//HACEEER CASO PARA ULTIMAAAAA HEBRAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA

}

void* identificar(void* id)
{

	Hebra* hilo = (Hebra*)id;
	printf("Soy la hebra %d\n", hilo->id);
}


void leerArchivo(FILE* entrada, int cantPalabras)
{
	diccionario = malloc(sizeof(*diccionario) * cantPalabras);
	char buffer[128];
	int i;
	for (i = 0; i < cantPalabras; i++)
	{
		fscanf(entrada, "%s", buffer);
		strcpy(diccionario[i], buffer);
	}
}

void printDiccionario(int cantPalabras)
{
	int i;
	printf("Diccionario: \n");
	for (i = 0; i < cantPalabras; i++)
	{
		printf("%s\n", diccionario[i]);
	}
}

int asignarPalabras(FILE* entrada, int cantPalabras, int palabrasPorHebra, int cantHebras)
{
	int i, j;
	int cont = 0;
	for (i = 0; i < cantHebras - 1; i++)
	{
		hebras[i].palabras = malloc(sizeof(*(hebras[i].palabras)) * palabrasPorHebra);
		hebras[i].cantPalabras=palabrasPorHebra;

		for (j = 0; j < palabrasPorHebra; j++)
		{
			/*if (mallocPalabras(palabrasPorHebra, i) == 1)
			{
				return 1;
			}*/

			strcpy(hebras[i].palabras[j], diccionario[cont]);

			cont++;
		}

	}
	
	int resto = cantPalabras - (palabrasPorHebra * (cantHebras - 1));
	hebras[cantHebras - 1].palabras = malloc(sizeof(*(hebras[cantHebras - 1].palabras)) * resto);
	hebras[cantHebras - 1].cantPalabras=resto;
	
	for (j = 0; j < resto; j++)
	{
		/*if (mallocPalabras(resto, cantHebras - 1) == 1)
		{
			return 1;
		}*/

		strcpy(hebras[cantHebras - 1].palabras[j], diccionario[cont]);
		cont++;
	}

	return 0;
}

void mutexInit()
{
	int i, j;
	for (i = 0; i < N; i++)
	{
		for (j = 0; j < M; j++)
		{
			pthread_mutex_init(&matrizMutex[i][j], NULL);
		}
	}
	pthread_mutex_init(&bloquear, NULL);
	pthread_mutex_init(&desbloquear, NULL);
	pthread_mutex_init(&printear, NULL);
}


void printPalabras(int cantPalabras, int palabrasPorHebra, int cantHebras)
{
	int i, j;
	for (i = 0; i < cantHebras - 1; i++)
	{
		for (j = 0; j < palabrasPorHebra; j++)
		{

			printf("hebra %d palabra %d %s\n", i, j, hebras[i].palabras[j]);

		}
	}

	int resto = cantPalabras - (palabrasPorHebra * (cantHebras - 1));

	printf("resto: %d\n", resto);

	for (j = 0; j < resto; j++)
	{

		printf("hebra %d palabra %d %s\n", cantHebras - 1, j, hebras[cantHebras - 1].palabras[j]);
	}
}

int sizePalabraLarga(int cantPalabras)
{
	int max = 0;
	int i,largo;
	for(i=0;i<cantPalabras;i++)
	{
		largo=strlen(diccionario[i]);
		if(largo>max)
		{
			max=largo;
		}
	}
	return max;
}

int comprobarEntradas(int cantHebras, int cantPalabras, int sizeN, int sizeM)
{
	if(cantHebras<1)
	{
		printf("La cantidad de hebras de ser mayor o igual a 1\n");
		return 0;
	}
	if(cantPalabras<1)
	{
		printf("La cantidad de palabras de ser mayor o igual a 1\n");
		return 0;
	}
	if(sizeN<1 || sizeM<1)
	{
		printf("Las dimensiones de la matriz deben ser mayores o iguales a 1\n");
		return 0;
	}
	if(cantPalabras>sizeN)
	{
		printf("Existe una o mas palabras que no caben horizontalmente en la matriz\n");
		return 0;
	}
	/*if(sizePalabraLarga(cantPalabras)>sizeM)
	{
		printf("Es necesario que la cantidad de filas de la matriz sea igual o mayor a la cantidad de palabras\n");
		return 0;
	}	*/
	
	return 1;
}

int inicio(int argc, char **argv)
{
	/*
	i: Archivo de entrada
	h: cantidad de hebras
	c: cantidad de palabras
	n: tamanio n
	m: tamanio m
	s: archivo de salida
	d: mostrar por pantalla
	*/


	//Aqui declaramos las variables a recibir por medio de optarg
	char* iName;
	int hCant = 0;
	int cCant = 0;
	int nMatriz;
	int mMatriz;
	char* sName;
	int dFlag = 0;
	int index;
	int c;

	opterr = 0;

	//Con este ciclo se obtienen los argumentos ingresados por consola y se asignan
	//a las variables anteriormente declaradas
	while ((c = getopt (argc, argv, "i:h:c:n:m:s:d")) != -1) {
		switch (c) {
		case 'i':
			iName = optarg;
			break;
		case 'h':
			sscanf(optarg, "%d", &hCant);
			break;
		case 'c':
			sscanf(optarg, "%d", &cCant);
			break;
		case 'n':
			sscanf(optarg, "%d", &nMatriz);
			break;
		case 'm':
			sscanf(optarg, "%d", &mMatriz);
			break;
		case 's':
			sName = optarg;
			break;
		case 'd':
			dFlag = 1;
			break;

		//Caso en que se ingrese una opcion desconocida
		case '?':
			if (optopt == 'i' || optopt == 'h' || optopt == 'c' || optopt == 'n' || optopt == 'm' || optopt == 's') {
				fprintf (stderr, "Opcion -%c requiere un argumento.\n", optopt);
			}
			else if (isprint (optopt)) {
				fprintf (stderr, "Opcion desconocida `-%c'.\n", optopt);
			}
			else {
				fprintf (stderr, "Caracter desconocido `\\x%x'.\n", optopt);

			}
			return 1;
		default:
			return 1;
		}

	}

	//Se muestran por pantalla los datos ingresados.
	printf ("iName = %s, hCant = %d, cCant = %d, nMatriz = %d, mMatriz = %d, sName = %s, dFlag = %d\n", iName, hCant, cCant, nMatriz, mMatriz, sName, dFlag);

	//Se muestra mensaje en caso de ingresar un argumento que no existe
	for (index = optind; index < argc; index++) {
		printf ("Argumento no existente %s\n", argv[index]);
	}
	
	if(comprobarEntradas(hCant,cCant,nMatriz,mMatriz)!=1)
	{
		return -1;
	}
	
	if(hCant>cCant)
	{
		hCant=cCant;
	}
	N = nMatriz;
	M = mMatriz;
	dPrint=dFlag;

	matriz = crearMatriz(nMatriz, mMatriz);
	matrizMutex = crearMatrizMutex(nMatriz, mMatriz);
	if (matrizMutex == NULL)
	{
		return -1;
	}


	mutexInit();

	if (matriz == NULL || matrizMutex == NULL )
	{
		return -1;
	}
	FILE* entrada = fopen(iName, "r");
	if (entrada == NULL)
	{
		printf("No se pudo abrir el archivo dado");
		return -1;
	}


	hebras = (Hebra*)malloc(sizeof(Hebra) * hCant);

	palabrasPorProceso = cCant / hCant;

	leerArchivo(entrada, cCant);
	//printDiccionario(cCant);
	if (asignarPalabras(entrada, cCant, palabrasPorProceso, hCant) == 1)
	{
		return -1;
	}


	fclose(entrada);
	//printPalabras(cCant, palabrasPorProceso, hCant);

;
	int i;
	pthread_t id[10];


	srand(time(NULL));

	for (i = 0; i < hCant; i++)
	{

		hebras[i].id = i;
		pthread_create(&id[i], NULL, ubicar, (void*) (&hebras[i]));
	
	}



	for (i = 0; i < hCant; i++)
	{
		int err =pthread_join(id[i], NULL);
	}
	
	rellenarMatriz();
	printMatriz(matriz, nMatriz, mMatriz);
	
	printf("Fin programa\n");






	return 0;
}
