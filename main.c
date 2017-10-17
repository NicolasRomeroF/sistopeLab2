#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

struct structHebra
{
	int id;
	int x;
	int y;
	char (*palabras)[128];
} typedef Hebra;




int N, M;
Hebra * hebras;
pthread_mutex_t** matrizMutex;
char** matriz;
int palabrasPorProceso;
FILE* salida;
char (*diccionario)[128];


int verificarSize(char* palabra, int x)
{
	int sizePalabra = strlen(palabra);
	if ((x + sizePalabra) >= M)
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
	for (i = x; i < sizePalabra; i++)
	{
		if (matriz[y][i] != ' ')
		{
			return 0;
		}
	}
	return 1;
}

void printMatriz(char** matriz, int N, int M)
{
	int i, j;
	for (i = 0; i < N; i++)
	{
		for (j = 0; j < M; j++)
		{
			printf("%c ", matriz[i][j]);
		}
		printf("\n");
	}
	printf("Fin print\n");
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
	for (i = 0; i < M; i++)
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
	for (i = 0; i < M; i++)
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

void ubicarPalabra(char* palabra, int x, int y) {
	int len = strlen(palabra);
	int i;
	for (i = x; i < len; i++) {
		matriz[y][i] = palabra[i - x];
	}
}

void* ubicar (void* id)
{
	//SI NO ES LA ULTIMA HEBRA HACER ESTO
	Hebra* hilo = (Hebra*)id;
	printf("Soy la hebra %d\n", hilo->id);
	int i;
	for (i = 0; i < palabrasPorProceso; i++) {
		hilo->x = rand() % N;
		hilo->y = rand() % M;
		if (verificarSize(hilo->palabras[i], hilo->x) && verificarVacio(hilo->palabras[i], hilo->x, hilo->y)) {
			ubicarPalabra(hilo->palabras[i], hilo->x, hilo->y);
		}
	}

}

void* identificar(void* id)
{

	Hebra* hilo = (Hebra*)id;
	printf("Soy la hebra %d\n", hilo->id);
}

/*int mallocPalabras(int size, int id)
{
	hebras[id].palabras = (char**)malloc(sizeof(char*)*128);

	if (hebras[id].palabras == NULL)
	{
		printf("No se puedo asignar memoria a palabras");
		return 1;
	}

	int i;
	for (i = 0; i < size; i++)
	{
		hebras[id].palabras[i] = (char*)malloc(sizeof(char) * size);
		if (hebras[id].palabras[i] == NULL)
		{
			printf("No se puedo asignar memoria a palabras");
			return 1;
		}
	}
	return 0;
}*/

void leerArchivo(FILE* entrada, int cantPalabras)
{
	diccionario=malloc(sizeof(*diccionario)*cantPalabras);
	char buffer[128];
	int i;
	for(i=0;i<cantPalabras;i++)
	{	
		fscanf(entrada,"%s",buffer);
		strcpy(diccionario[i],buffer);
	}
}

void printDiccionario(int cantPalabras)
{
	int i;
	printf("Diccionario: \n");
	for(i=0;i<cantPalabras;i++)
	{
		printf("%s\n",diccionario[i]);
	}
}

int asignarPalabras(FILE* entrada, int cantPalabras, int palabrasPorHebra, int cantHebras)
{
	int i, j;
	int cont = 0;
	for (i = 0; i < cantHebras - 1; i++)
	{
		hebras[i].palabras = malloc(sizeof(*(hebras[i].palabras))*palabrasPorHebra);
	
		for (j = 0; j < palabrasPorHebra; j++)
		{
			printf("i: %d j: %d\n", i, j);
			/*if (mallocPalabras(palabrasPorHebra, i) == 1)
			{
				return 1;
			}*/
			
			strcpy(hebras[i].palabras[j], diccionario[cont]);
			printf("hebrapalabra: %s diccionariocont: %s\n", hebras[i].palabras[j],diccionario[cont]);
		
			cont++;
		}
		
	}
	int resto = cantPalabras - (palabrasPorHebra * (cantHebras - 1));
	hebras[cantHebras-1].palabras = malloc(sizeof(*(hebras[cantHebras-1].palabras))*resto);
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


int main(int argc, char **argv)
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

	N = nMatriz;
	M = mMatriz;
	matriz = crearMatriz(nMatriz, mMatriz);
	matrizMutex = crearMatrizMutex(nMatriz, mMatriz);

	if (matriz == NULL || matrizMutex == NULL )
	{
		return -1;
	}
	FILE* entrada = fopen("diccionario", "r");
	if (entrada == NULL)
	{
		printf("No se pudo abrir el archivo dado");
		return -1;
	}



	printf("Matriz N: %d M: %d\n", nMatriz, mMatriz);
	printMatriz(matriz, nMatriz, mMatriz);
	hebras = (Hebra*)malloc(sizeof(Hebra) * hCant);

	palabrasPorProceso = cCant / hCant;
	printf("antes asignar\n");
	leerArchivo(entrada,cCant);
	printDiccionario(cCant);
	if (asignarPalabras(entrada, cCant, palabrasPorProceso, hCant) == 1)
	{
		return -1;
	}


	fclose(entrada);
	printPalabras(cCant, palabrasPorProceso, hCant);


	printf("owo\n");
	int i;
	pthread_t id[10];

	//



	printf("antes mutex\n");
	/*for (i = 0; i < 10; i++)
	{

		hebras[i].id = i;
		pthread_create(&id[i], NULL, identificar, (void*) (&hebras[i]));
		printf("i: %d\n", i);
	}*/
	

	printf("Fin creacion\n");

	/*for (i = 0; i < 10; i++)
	{
		int err =pthread_join(id[i], NULL);
		printf("err: %d, id: %lu\n",err,id[i]);
	}*/
	printf("fin join\n");
	//printMatriz(matriz, nMatriz, mMatriz);






	return 0;
}
