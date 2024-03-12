This program takes an undirected graph as input, and outputs the corresponding
*filtered* graph, which is a graph with the sames nodes, but where two nodes
share an edge if their ECC is above a certain threshold in the starting graph.

This filtered graph should have a stronger community structure, and is meant to
be given as input to a (consensual) community detection algorithm, such as
Louvain, ECG, or something else.

## Reference:

Fast unfolding of communities in large networks. Vincent D. Blondel, Jean-Loup
Guillaume, Renaud Lambiotte, Etienne Lefebvre. J. Stat. Mech. P10008, 2008.

Huchet, A., Guillaume, J. L., & Ghamri-Doudane, Y. (2023, June). On filtering
the noise in consensual communities. In International Conference on
Computational Science (pp. 655-670). Cham: Springer Nature Switzerland.

## To compile:

type "make"

## To execute:

"./louvain infile outfile number_of_runs"
- "infile" should contain an undirected unweighted graph: one edge on each line
  (two unsigned long (ID of the 2 nodes) separated by a space).
- "outfile": will contain the resulting partition: one node and its community
  id on each line.
- "number_of_runs": The number of times Louvain is executed to compute the
  average modularity, which is used to compute the threshold on the ECC.

## Example

- python lfr.py 10000 0.6
- ./louvain examples/lfr_graph examples/filtered_graph 10
- python louvain.py examples/filtered_graph

- python louvain.py examples/lfr_graph

## Initial contributors:  

Jean-Loup Guillaume
March 2020
jean-loup.guillaume@univ-lr.fr

Antoine Huchet
March 2024
antoine.huchet@univ-lr.fr
