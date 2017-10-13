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
	char** palabras;
} typedef Hebra;


int N, M;
Hebra * hebras;
pthread_mutex_t mutex;
char** matriz;
int palabrasPorProceso;


int verificarSize(char* palabra, int x)
{
	int sizePalabra = strlen(palabra);
	if ((x + sizePalabra) >= N)
	{
		return 0;
	}
	else
	{
		return 1;
	}
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

void ubicarPalabra(char* palabra,int x){
	int len=strlen(palabra);
	int i;
	for(i=x;i<len;i++){
		matriz[x][i]=palabra[i-x];
	}
}

void* ubicar (void* id)
{
	//SI NO ES LA ULTIMA HEBRA HACER ESTO
	int* id1 = (int*) id;
	printf("Soy la hebra %d\n", *id1);
	Hebra hebra = hebras[*id1];
	pthread_mutex_lock(&mutex);
	int i;
	for(i=0;i<palabrasPorProceso;i++){
		hebra.x=rand()%N;
		hebra.y=rand()%M;
		if(verificarSize(hebra.palabras[i],hebra.x)){
			ubicarPalabra(hebra.palabras[i],hebra.x);
		}
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
			abort ();
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
	if (matriz == NULL)
	{
		return -1;
	}

	printf("Matriz N: %d M: %d\n", nMatriz, mMatriz);
	printMatriz(matriz, nMatriz, mMatriz);
	hebras=(Hebra*)malloc(sizeof(Hebra)*hCant);
	int i;
	pthread_t id[10];
	int id1 = 0;
	pthread_mutex_init(&mutex,NULL);
	palabrasPorProceso = cCant/hCant;

	for (i = 0; i < 10; i++)
	{

		hebras[i].id=i;
		pthread_create(&id[i], NULL, ubicar, (void*) &id1);
		id1++;
	}
	
	for (i = 0; i < 10; i++)
	{
		pthread_join(id[i], NULL);
	}
	printMatriz(matriz,nMatriz,mMatriz);






	return 0;
}