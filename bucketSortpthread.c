#include <omp.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

struct bucket {
    int n_elem;
    int index; // [start : n_elem)
    int start; //starting point in B array
};

struct message{
    int *B;
    int start; 
    int size;
};
int cmpfunc(const void * a, const void * b);

void *sort(void *arg){
    struct message* m = (struct message*) arg;
    qsort(m->B + m->start, m->size, sizeof(int), cmpfunc);
}

int cmpfunc (const void * a, const void * b)
{
 return ( *(int*)a - *(int*)b );
}

int main(int argc, char *argv[]) {

    int *A, *B, *temp;
    int dim, n_buckets, i, w, j, limit;
    struct bucket* buckets; //array of buckets
    double t1; // Timing variables
    float total; //total time

    printf("Give length of array to sort \n");
    scanf("%d", &dim);
    printf("Give number of buckets \n");
    scanf("%d", &n_buckets);
    limit = 100000;
    w = (int)limit/n_buckets;
    A = (int *) malloc(sizeof(int)*dim);
    B = (int *) malloc(sizeof(int)*dim);
    
    buckets = (struct bucket *) calloc(n_buckets, sizeof(struct bucket));

    for(i=0;i<dim;i++) {
        A[i] = random() % limit;
    }
    

    t1 = omp_get_wtime();
    
    for (i=0; i<dim;i++){
        j = A[i]/w;
        if (j>n_buckets-1)
                j = n_buckets-1;
        buckets[j].n_elem++;
    }
    
    for (i=1; i<n_buckets;i++){
        buckets[i].index = buckets[i-1].index + buckets[i-1].n_elem;
        buckets[i].start = buckets[i-1].start + buckets[i-1].n_elem;
    }
    int b_index;
    for (i=0; i<dim;i++){
        j = A[i]/w;
        if (j > n_buckets -1)
                j = n_buckets-1;
        b_index = buckets[j].index++;
        B[b_index] = A[i];
    }   


    pthread_t pthread[n_buckets];
    struct message m[n_buckets];


    for(int myid = 0; myid < n_buckets; myid++){
       
        m[myid].B = B;
        m[myid].start = buckets[myid].start;
        m[myid].size = buckets[myid].n_elem;
        pthread_create(&pthread[myid], NULL, sort, (void *) &m[myid]);
    }

    for(int myid = 0; myid < n_buckets; myid++){
        pthread_join(pthread[myid], NULL);
    }


    temp = A; // swap the two arrays A and B
    A = B;
    B = temp;
    total = omp_get_wtime() - t1;

    printf("Sorting %d elements took %f ms\n", dim,total * 1000.0);
    return 0;
}