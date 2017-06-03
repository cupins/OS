#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

#define SIZE 2000


// global instantiated for simplicity
int T_SIZE, result[SIZE][SIZE], a[SIZE][SIZE], b[SIZE][SIZE];
// matrix multiply function
void *matrix_func(void *t);

int main(int argc, char *argv[])
{
	// command line args for threads 
	// threads should be dependent on architecture
    T_SIZE = atoi(argv[1]);
    int x, y;
	
	// a and b can be local variables if malloc
	
    /*
    a = malloc(SIZE*sizeof(long long));
    for (x = 0; x < SIZE; x++){
        a[x] = malloc(SIZE * sizeof(*a[x]));
    }
    b = malloc(SIZE*sizeof(long long));
    for (x = 0; x < SIZE; x++){
        b[x] = malloc(SIZE * sizeof(*b[x]));
    }
    */
    for (x = 0; x < SIZE; x++){
        for (y = 0; y < SIZE; y++){
            a[x][y] = rand();
            b[x][y] = rand();
        }
    }
    // Print both input matrices DEBUG
    /*
    for (x = 0; x < SIZE; x ++){
        for (y = 0; y < SIZE; y++){
            printf("%d ", a[x][y]);
        }
        printf("\n");
    }
 printf("\n");
    for (x = 0; x < SIZE; x ++){
        for (y = 0; y < SIZE; y++){
            printf("%d ", b[x][y]);
        }
        printf("\n");
    }
 printf("\n");

    */
    //dynamic 2d resultant matrix if local
    /*
    result = malloc(SIZE*sizeof(long long));
    for (x = 0; x < SIZE; x++){
        result[x] = malloc(SIZE * sizeof(*result[x]));
    }
    */
    //spawn threads
    time_t start, end;
    double cpu_time_used;
    time(&start);
    //mult
    int i, j , k, rowThread[T_SIZE];
    pthread_t n[T_SIZE];
    for (x = 0; x < T_SIZE; x++){
        rowThread[x] = x*(SIZE/T_SIZE);
        pthread_create(&n[x], NULL, matrix_func, &rowThread[x]);
    }
    // end mult
    time(&end);
    cpu_time_used = difftime(end, start);
    for (x = 0; x < T_SIZE; x++)
        pthread_join(n[x], NULL);
    printf("Time elapsed is %f \n", cpu_time_used);
    // debug resultant
    /*
    for (i = 0; i < SIZE; i ++){
        for (j = 0; j< SIZE; j++){
            printf("%d ", result[i][j]);
        }
        printf("\n");
    }
    */
    /*
    for (x = 0; x < SIZE; x++){
        result[x] = malloc(SIZE * sizeof(*result[x]));
    }
    free(result);
    for (x = 0; x < SIZE; x++){
        a[x] = malloc(SIZE * sizeof(*a[x]));
    }
    free(a);
    for (x = 0; x < SIZE; x++){
        b[x] = malloc(SIZE * sizeof(*b[x]));
    }
    free(b);
    */

    return 0;
}

void *matrix_func(void *t){

    int *ptr = (int*) t;
    int i,j,k;

    for(i = *ptr; i < (*ptr + (SIZE/T_SIZE)); i++)
        for(j = 0; j < SIZE; j++)
            for(k = 0; k < SIZE; k++)
                result[i][j] = result[i][j] + a[i][k] * b[k][j];

}

