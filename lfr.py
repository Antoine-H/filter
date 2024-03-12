from itertools import product
import pickle
import logging
import argparse
import networkit as nk
import networkx as nx


def make_lfr(mu, n):
    """ Generates an LFR graph with the same parameters as the e article """
    lfr = nk.generators.LFRGenerator(n)
    lfr.generatePowerlawDegreeSequence(20, 50, -2)
    lfr.generatePowerlawCommunitySizeSequence(10, 100, -3)
    lfr.setMu(mu)
    lfrG = lfr.generate()
    edges = list()
    for i, j in lfrG.iterEdges():
        # print(i, j)
        edges.append((i, j))

    partitions = lfr.getPartition()
    communities = list()
    for i in partitions.getSubsetIds():
        # print(partitions.getMembers(i))
        communities.append(partitions.getMembers(i))

    graph = nx.Graph()
    graph.add_edges_from(edges)

    return graph, communities, get_truth(graph, communities)


def get_pairs(communities):
    pairs = list()
    for community in communities:
        pairs += list(product(community, repeat=2))
    return pairs


def get_truth(graph, communities):
    pairs = get_pairs(communities)
    truth = [[0 for _ in range(len(graph)+1)] for _ in range(len(graph)+1)]
    for (i, j) in pairs:
        truth[i][j] = 1
        truth[j][i] = 1
    return truth


def write_graph(graph):
    nx.write_edgelist(graph, "examples/lfr_graph", data=False)


def write_communities(communities):
    gt_file = open("examples/lfr_gt", 'wb')
    pickle.dump(communities, gt_file)
    gt_file.close()


def write_truth(truth):
    gt_file = open("examples/lfr_truth", 'wb')
    pickle.dump(truth, gt_file)
    gt_file.close()


if __name__ == "__main__":
    # Set parser
    PARSER = argparse.ArgumentParser(prog="main.py",
                                     description="""C""",)
    PARSER.add_argument("n", help="Number of nodes.", type=int)
    PARSER.add_argument("mu", help="Fraction of inter-community edges incident"
                                   " to each node. This value must be in the "
                                   "interval [0, 1].", type=float)
    PARSER.add_argument("-v", "--verbose", help="Turn on verbosity",
                        action="store_true")
    ARGS = PARSER.parse_args()

    # Set logger
    if ARGS.verbose:
        logging.basicConfig(format='%(message)s', level=10)
    else:
        logging.basicConfig(format='%(message)s', level=25)

    g, c, t = make_lfr(ARGS.mu, ARGS.n)
    write_graph(g)
    write_communities(c)
    write_truth(t)
