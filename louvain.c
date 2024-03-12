#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <strings.h>//to use "bzero"
#include <time.h>//to estimate the runing time
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>

#include "partition.h"
#include "struct.h"

#define NLINKS 100000000 //maximum number of edges of the input graph: used for memory allocation, will increase if needed. //NOT USED IN THE CURRENT VERSION
#define NNODES 10000000 //maximum number of nodes in the input graph: used for memory allocation, will increase if needed
#define HMAX 100 //maximum depth of the tree: used for memory allocation, will increase if needed

void* malloc_wrapper(size_t size) {
	void *tmp;
	if (NULL == (tmp = malloc(size))) {
		perror("malloc");
		exit(EXIT_FAILURE);
	}
	return tmp;
}

void* realloc_wrapper(void* pointer, size_t number_items, size_t size) {
	void *tmp;
	if (NULL == (tmp = realloc(pointer, number_items * size))) {
		printf("realloc null pointer.\n");
		perror("realloc");
		exit(EXIT_FAILURE);
	}
	return tmp;
}

//compute the maximum of three unsigned long
inline unsigned long max3(unsigned long a,unsigned long b,unsigned long c){
  a = (a > b) ? a : b;
  return (a > c) ? a : c;
}

inline unsigned long min(unsigned long i, unsigned long j){
  return (i > j) ? j : i;
}

//reading the edgelist from file
//NOT USED IN THE CURRENT VERSION
edgelist* readedgelist(char* input){
  unsigned long long e1=NLINKS;
  edgelist *el=malloc(sizeof(edgelist));
  FILE *file;

  el->n=0;
  el->e=0;
  file=fopen(input,"r");
  el->edges=malloc(e1*sizeof(edge));
  while (fscanf(file,"%lu %lu", &(el->edges[el->e].s), &(el->edges[el->e].t))==2) {
    el->n=max3(el->n,el->edges[el->e].s,el->edges[el->e].t);
    if (++(el->e)==e1) {
      e1+=NLINKS;
      el->edges=realloc(el->edges,e1*sizeof(edge));
    }
  }
  fclose(file);
  el->n++;

  el->edges=realloc(el->edges,el->e*sizeof(edge));

  return el;
}

//freeing memory
//NOT USED IN THE CURRENT VERSION
void free_edgelist(edgelist *el){
  free(el->edges);
  free(el);
}

//building the adjacency matrix
//NOT USED IN THE CURRENT VERSION
adjlist* mkadjlist(edgelist* el){
  unsigned long long i;
  unsigned long u,v;
  unsigned long *d=calloc(el->n,sizeof(unsigned long));
  adjlist *g=malloc(sizeof(adjlist));

  g->n=el->n;
  g->e=el->e;

  for (i=0;i<el->e;i++) {
    d[el->edges[i].s]++;
    d[el->edges[i].t]++;
  }

  g->cd=malloc((g->n+1)*sizeof(unsigned long long));
  g->cd[0]=0;
  for (i=1;i<g->n+1;i++) {
    g->cd[i]=g->cd[i-1]+d[i-1];
    d[i-1]=0;
  }

  g->adj=malloc(2*g->e*sizeof(unsigned long));
  for (i=0;i<g->e;i++) {
    u=el->edges[i].s;
    v=el->edges[i].t;
    g->adj[ g->cd[u] + d[u]++ ]=v;
    g->adj[ g->cd[v] + d[v]++ ]=u;
  }

  g->weights = NULL;
  g->totalWeight = 2*g->e;
  g->map=NULL;

  free(d);
  free_edgelist(el);

  return g;
}

void free_graph(adjlist* g){
  free(g->cd);
  free(g->adj);
  free(g);
}

//reading the list of edges and building the adjacency array
adjlist* readadjlist(char* input){
  unsigned long n1=NNODES,n2,u,v,i;
  unsigned long *d=calloc(n1,sizeof(unsigned long));
  adjlist *g=malloc(sizeof(adjlist));
  FILE *file;

  g->n=0;
  g->e=0;
  file=fopen(input,"r");//first reading to compute the degrees
  while (fscanf(file,"%lu %lu", &u, &v)==2) {
    if (u > v) {
      g->e++;
      g->n=max3(g->n,u,v);
      if (g->n+1>=n1) {
        n2=g->n+NNODES;
        d=realloc(d,n2*sizeof(unsigned long));
        bzero(d+n1,(n2-n1)*sizeof(unsigned long));
        n1=n2;
      }
      d[u]++;
      d[v]++;
    }
  }
  fclose(file);

  g->n++;
  d=realloc(d,g->n*sizeof(unsigned long));

  g->cd=malloc((g->n+1)*sizeof(unsigned long long));
  g->cd[0]=0;
  for (i=1;i<g->n+1;i++) {
    g->cd[i]=g->cd[i-1]+d[i-1];
    d[i-1]=0;
  }

  g->adj=malloc(2*g->e*sizeof(unsigned long));

  file=fopen(input,"r");//secong reading to fill the adjlist
  while (fscanf(file,"%lu %lu", &u, &v)==2) {
    if (u > v) {
      g->adj[ g->cd[u] + d[u]++ ]=v;
      g->adj[ g->cd[v] + d[v]++ ]=u;
    }
  }
  fclose(file);

  g->weights = NULL;
  g->totalWeight = 2*g->e;
  g->map=NULL;

  free(d);

  return g;
}

//reading the list of edges and building the adjacency array
//NOT USED IN THE CURRENT VERSION
#define BUFFER_SIZE (16 * 1024)

int
read_two_integers(int fd, unsigned long *u, unsigned long *v) {
  static char buf[BUFFER_SIZE];
  static ssize_t how_many = 0;
  static int pos = 0;
  unsigned long node_number=0;
  int readu = 0;

  while (1) {
    while(pos < how_many) {
      if (buf[pos] == ' ') {
	*u = node_number;
	readu=1;
	node_number = 0;
	pos++;
      } else if (buf[pos] == '\n') {
	*v = node_number;
	node_number = 0;
	readu=0;
	pos++;
	return 2;
      } else {
	node_number = node_number * 10 + buf[pos] - '0';
	pos++;
      }
    }

    how_many = read (fd, buf, BUFFER_SIZE);
    pos = 0;
    if (how_many == 0) {
      if(readu==1) {
	*v = node_number;
	return 2;
      }	    
      return 0;;
    }
  } 
}

adjlist *
readadjlist_v2(char* input_filename){
  unsigned long n1 = NNODES, n2, u, v, i;
  unsigned long *d = calloc(n1,sizeof(unsigned long));
  adjlist *g = malloc(sizeof(adjlist));

  g->n = 0;
  g->e = 0;

  // first read of the file to compute degree of each node
  int fd = open(input_filename, O_RDONLY);
  posix_fadvise(fd, 0, 0, POSIX_FADV_SEQUENTIAL);
  while (read_two_integers(fd, &u, &v) == 2) {
    g->e++;
    g->n = max3(g->n, u, v);
    if (g->n + 1 >= n1) {
      n2 = g->n+NNODES;
      d = realloc(d, n2 * sizeof(unsigned long));
      bzero(d + n1, (n2 - n1) * sizeof(unsigned long));
      n1 = n2;
    }
    d[u]++;
    d[v]++;
  }
  close(fd);

  g->n++;
  d = realloc(d, g->n * sizeof(unsigned long));

  // computation of cumulative degrees
  g->cd = malloc((g->n + 1) * sizeof(unsigned long long));
  g->cd[0] = 0;
  for (i = 1; i < g->n + 1; i++) {
    g->cd[i] = g->cd[i - 1] + d[i - 1];
    d[i - 1] = 0;
  }

  g->adj = malloc(2 * g->e * sizeof(unsigned long));

  // second read to create adjlist
  fd = open(input_filename, O_RDONLY);
  posix_fadvise(fd, 0, 0, POSIX_FADV_SEQUENTIAL);
  while (read_two_integers(fd, &u, &v) == 2) {
    g->adj[g->cd[u] + d[u]] = v;
    d[u]++;
    g->adj[g->cd[v] + d[v]] = u;
    d[v]++;
  }
  close(fd);

  g->weights = NULL;
  g->totalWeight = 2*g->e;
  g->map = NULL;

  free(d);

  return g;
}

//freeing memory
void free_adjlist(adjlist *g){
  free(g->cd);
  free(g->adj);
  free(g->weights);
  free(g->map);
  free(g);
}

//Get random seed
int get_random(unsigned int seed){
  FILE* random = fopen("/dev/urandom", "r");
  if (fread(&seed, sizeof(seed), 1, random) != 1) {
    return -1;
  }
  fclose(random);
  srand(seed);
  return 0;
}


// Generates the name of the file in which the output of the jth Louvain will
// be stored
void get_file_name(char* prefix, char* out_file, int j){
  out_file[0] = '\0';  // ensures the memory is an empty string
  char str[sizeof(char)*(int)log10(j)];
  sprintf(str, "%d", j);
  strcat(out_file, prefix);
  strcat(out_file, str);
}

void free_parts(unsigned long **parts, unsigned size){
  // Starts at 1 because file names also start at 1
  for (int i=1; i<=size; i++){
    free(parts[i]);
  }
  free(parts);
}

long double ecc_threshold(long double modul, unsigned long n){
  /* This is a correlation that should be proved and made more accurate */
  if (n < 20000)
    modul += 71.2108*modul/n;
  return -1.414*(-modul+1)+0.991;
}

unsigned long get_degree(unsigned long u, adjlist* g){
  /* printf("The degree of %lu is %llu\n", u, g->cd[u+1]-g->cd[u]); */
  if (u > g->n){
    return 0;
  }
  return g->cd[u+1]-g->cd[u];
}

unsigned long* get_neighbours(unsigned long u, adjlist* g){
  unsigned long i, j;
  unsigned long* neighbours;

  if (u > g->n){
    neighbours = malloc_wrapper(sizeof(unsigned long));
    neighbours[0] = 0;
  } else {
    /* printf("Node %llu has degree %llu\n", u, get_degree(u, g));*/
    neighbours = malloc_wrapper((get_degree(u, g)+1)*sizeof(unsigned long));
    /*printf("Allocated %llu unsigned long long into memory\n", get_degree(u, g)+1);*/

    j = 0;
    neighbours[j++] = get_degree(u, g);
    for (i=g->cd[u]; i<g->cd[u+1]; i++){
      neighbours[j++] = g->adj[i];
      /* printf("Current size %u\n", j);*/
    }
  }
  return neighbours;
}

void print_neighbours(unsigned long u, adjlist* g){
  unsigned long i;
  if (u > g->n){
    printf("No such node: %lu\n", u);
  } else {
    printf("List of neighbours of %lu in the graph:\n", u);
    for (i=g->cd[u]; i<g->cd[u+1]; i++){
      printf("%lu %lu\n", i, g->adj[i]);
    }
  }
}

bool belongs_to(unsigned long node, unsigned long* nodes){
  unsigned long i;

  for (i=0; i<nodes[0]; i++){
    if (node == nodes[i]){
      /*
      printf("%llu is in %llith position, it belongs to:\n", node, i);
      for (i=1; i<nodes[0]; i++){
        printf("%llu, ", nodes[i]);
      }
      printf("\n");
      */
      return 1;
    }
  }

  /*
  printf("%llu does not belong to:\n", node);
  for (i=1; i<nodes[0]; i++){
    printf("%llu, ", nodes[i]);
  }
  printf("\n");
  */

  return 0;
}

unsigned long* get_common_neighbours(unsigned long* neighbours_u, unsigned long* neighbours_v){
  /* Can't assume sorted I suppose? */
  unsigned long i, j;
  unsigned long *common_neighbours;

  common_neighbours = malloc_wrapper((neighbours_u[0]+1)*sizeof(unsigned long));

  j = 1;
  for (i=1; i<=neighbours_u[0]; i++){
    if (belongs_to(neighbours_u[i], neighbours_v)){
      common_neighbours[j++] = neighbours_u[i];
    }
  }
  /* Stores the length of the array in the first slot of the array */
  /* printf("There are %llu common neighbours\n", j-1); */
  common_neighbours[0] = j-1;

  if (j-1){
    common_neighbours = realloc_wrapper(common_neighbours, common_neighbours[0], sizeof(unsigned long));
  }

  return common_neighbours;

}

unsigned long get_number_common_neighbours(unsigned long u, unsigned long v, adjlist* g){
  unsigned long *neighbours_u, *neighbours_v, *common_neighbours;
  unsigned long number_common_neighbours;

  neighbours_u = get_neighbours(u, g);
  neighbours_v = get_neighbours(v, g);

  common_neighbours = get_common_neighbours(neighbours_u, neighbours_v);

  number_common_neighbours = common_neighbours[0];

  free(common_neighbours);
  free(neighbours_u);
  free(neighbours_v);

  return number_common_neighbours;

}

long double get_ecc(unsigned long u, unsigned long v, adjlist* g){
  unsigned long deg_u, deg_v;

  deg_u = get_degree(u, g);
  deg_v = get_degree(v, g);

  /* Avoid division by zero */
  if (!(deg_u && deg_v)) {
    return 0;
  }

  return (long double)get_number_common_neighbours(u, v, g) / min(deg_u, deg_v);
}

weighted_edge* get_ecc_above(adjlist* g, long double threshold){
  weighted_edge* edge_list;
  unsigned long i, j, k, n;
  unsigned long *neighbours_i, *neighbours_j;
  long double ecc;

  n = 10;
  printf("%lu\n", n);
  // printf("Allocating %lu edges, wich is %zu bytes\n", (n+1)*g->n, (n+1)*g->n*sizeof(weighted_edge));
  edge_list = malloc_wrapper(sizeof(weighted_edge)*++n*g->n);
  edge_list[0].s = 0;
  edge_list[0].t = 0;

  /* For i in nodes(g) */
  for (i=0; i<g->n; i++){
    if (i % 1000 == 0) {
        printf("Scanning node %lu out of %lu\n", i+1, g->n);
    }
    //print_neighbours(i, g);
    neighbours_i = get_neighbours(i, g);
    /* For j in neighbours(i) */
    for (j=1; j<neighbours_i[0]; j++){
      //printf("Considering neighbour %lu\n", j);
      //neighbours_v = get_neighbours(neighbours_u[j], g);
      /* For k in neighbours(j) */
      neighbours_j = get_neighbours(neighbours_i[j], g);
      for (k=1; k<neighbours_j[0]; k++){
        ecc = get_ecc(i, neighbours_j[k], g);
        if (i < neighbours_j[k] && ecc > threshold){
          edge_list[++edge_list[0].s].s = i;
          edge_list[edge_list[0].s].t = neighbours_j[k];
          edge_list[edge_list[0].s].w = ecc;
          /* -1 because the size of the array is edge_list[0].s+1 */
          if (edge_list[0].s >= (n*g->n)-1){
            // printf("Allocating %lu edges because edge_list already contains %lu edges\n", (n+1)*g->n, edge_list[0].s);
            edge_list = realloc_wrapper(edge_list, ++n*g->n, sizeof(weighted_edge));
          }
        }
      }
      ecc = get_ecc(i, neighbours_i[j], g);
      if (i < neighbours_i[j] && ecc > threshold){
        edge_list[++edge_list[0].s].s = i;
        edge_list[edge_list[0].s].t = neighbours_i[j];
        edge_list[edge_list[0].s].w = ecc;
        /* -1 because the size of the array is edge_list[0].s+1 */
        if (edge_list[0].s >= (n*g->n)-1){
          // printf("Allocating %lu edges because edge_list already contains %lu edges\n", (n+1)*g->n, edge_list[0].s);
          edge_list = realloc_wrapper(edge_list, ++n*g->n, sizeof(weighted_edge));
        }
      }
      free(neighbours_j);
    }
    free(neighbours_i);
  }

  edge_list = realloc_wrapper(edge_list, edge_list[0].s+1, sizeof(weighted_edge));
  return edge_list;
}

//main function
int main(int argc,char** argv){
  adjlist* g;
  unsigned long *part;
  unsigned long i;
  unsigned int seed;
  unsigned nb_runs = strtoul(argv[3], NULL, 10);
  long double modul = 0;
  long double ecc_thresh = 0;
  weighted_edge *edges;

  //Get random seed
  if (get_random(seed)){
    fprintf(stderr, "Cannot read random seed from /dev/urandom\n" );
    exit(-1);
  }

  if (argc != 4){
    fprintf(stderr, "Please provide an input_file, an output_file and a number of runs\n" );
    exit(-1);
  }

  //printf("Reading edgelist from file %s and building adjacency array\n", argv[1]);
  //t1 = time(NULL);
  g = readadjlist(argv[1]);
  //printf("Number of nodes: %lu\n", g->n);
  //printf("Number of edges: %llu\n", g->e);

  //printf("- Time to load the graph = %ldh%ldm%lds\n", (t1-t0)/3600, ((t1-t0)%3600)/60, ((t1-t0)%60));
  //t2 = time(NULL);

  part = malloc(g->n * sizeof(unsigned long));
  for (int j=1; j<=nb_runs; j++){
    modul += louvainComplete(g, part);
  }
  free(part);

  modul /= nb_runs;
  ecc_thresh = ecc_threshold(modul, g->n);
  printf("After %u runs of Louvain, the average modularity is %Lf, and the threshold on the ECC is %Lf\n", nb_runs, modul, ecc_thresh);

  /* Compute ECC of some pairs */
  edges = get_ecc_above(g, ecc_thresh);
  printf("%lu edges found out of %llu original edges and %lu nodes\n",
         edges[0].s, g->e, g->n);

  //t3 = time(NULL);
  //printf("- Time to export communities = %ldh%ldm%lds\n", (t3-t2)/3600, ((t3-t2)%3600)/60, ((t3-t2)%60));
  //printf("- Overall time = %ldh%ldm%lds\n", (t3-t0)/3600, ((t3-t0)%3600)/60, ((t3-t0)%60));

  /* Write to file */
  printf("Prints result in file %s\n", argv[2]);
  FILE* out = fopen(argv[2], "w");
  for(i = 1; i < edges[0].s; i++){
    fprintf(out, "%lu %lu %Lf\n", edges[i].s, edges[i].t, edges[i].w);
  }
  fclose(out);

  free(edges);
  free_graph(g);

  return 0;
}
