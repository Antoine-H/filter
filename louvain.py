import logging
import argparse
import pickle
import community as community_louvain
import matplotlib.cm as cm
import matplotlib.pyplot as plt
import networkx as nx
from sklearn.metrics.cluster import normalized_mutual_info_score


if __name__ == "__main__":
    # Set parser
    PARSER = argparse.ArgumentParser(prog="main.py",
                                     description="""C""",)
    PARSER.add_argument("filename", help="Name of the file where the filtered"
                                         " graph is", type=str)
    PARSER.add_argument("-v", "--verbose", help="Turn on verbosity",
                        action="store_true")
    ARGS = PARSER.parse_args()

    # Set logger
    if ARGS.verbose:
        logging.basicConfig(format='%(message)s', level=10)
    else:
        logging.basicConfig(format='%(message)s', level=25)

    # load the graph
    G = nx.read_edgelist(ARGS.filename, nodetype=int,
                         data=(("weight", float),))

    # first compute the best partition
    partition = community_louvain.best_partition(G)

    # draw the graph
    # pos = nx.spring_layout(G)
    # # color the nodes according to their partition
    # cmap = cm.get_cmap('viridis', max(partition.values()) + 1)
    # nx.draw_networkx_nodes(G, pos, partition.keys(), node_size=40,
    #                        cmap=cmap, node_color=list(partition.values()))
    # nx.draw_networkx_edges(G, pos, alpha=0.5)
    # plt.show()

    with open("examples/lfr_gt", 'rb') as gt_file:
        truth = pickle.load(gt_file)

    # with open('examples/lfr_truth', 'r', encoding="UTF-8") as file:
    #     lines = file.readlines()
    #     truth = {}
    #     for line in lines:
    #         node, community = line.split()
    #         truth[int(node)-1] = int(community)

    # coms = {}
    # for a, b in zip(ecg.graph.vs, ecg.membership):
    #     coms[int(a['name'])-1] = b

    # coms_ = {}
    # for node in coms:
    #     if node in truth:
    #         coms_[node] = coms[node]

    # truth_ = {}
    # for node in truth:
    #     if node in coms_:
    #         truth_[node] = truth[node]

    truth_d = {}
    for i, community in enumerate(truth):
        for node in community:
            truth_d[node] = i

    # For some reason there are more nodes in truth than in partition. Maybe
    # the Louvain implementation considers the biggest cc?
    truth_d_pruned = {}
    for k, v in truth_d.items():
        if k in partition:
            truth_d_pruned[k] = v

    # print(sorted(partition.keys()) == sorted(truth_d_pruned.keys()))
    coms_sorted = [partition[i] for i in sorted(partition.keys())]
    truth_sorted = [truth_d_pruned[i] for i in sorted(truth_d_pruned.keys())]
    print(normalized_mutual_info_score(coms_sorted, truth_sorted))
    # print(f"Number of pairs (ecg) {g.ecount()}")
    # print('NMI with ECG:', normalized_mutual_info_score(ecg.membership, truth_sorted))
    # print('NMI with ECG:', normalized_mutual_info_score(ecg.membership, comm))
    # END1 = time.process_time()
