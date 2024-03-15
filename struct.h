#ifndef STRUCT_H
#define STRUCT_H

typedef struct {
  unsigned long s;
  unsigned long t;
} edge;


typedef struct {
  unsigned long s;
  unsigned long t;
  long double w;
} weighted_edge;


//edge list structure:
typedef struct {
  unsigned long n;//number of nodes
  unsigned long long e;//number of edges
  unsigned long long emax;//max number of edges
  edge *edges;//list of edges
  unsigned long *map;//map[u]=original label of node u
} edgelist;


//edge list structure:
typedef struct {
  unsigned long n;//number of nodes
  unsigned long long e;//number of edges
  unsigned long long emax;//max number of edges
  edge *edges;//list of edges
  unsigned long long *cd;//cumulative degree cd[0]=0 length=n+1
  unsigned long *adj;//concatenated lists of neighbors of all node
  long double *weights;//concatenated lists of weights of neighbors of all nodes
  long double totalWeight;//total weight of the links
  unsigned long *map;//map[u]=original label of node u
} adjlist;

#endif
