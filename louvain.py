import argparse
import community
import networkx as nx
from sklearn.metrics.cluster import normalized_mutual_info_score
import partition_networkx
# pip install partition-networkx


if __name__ == "__main__":
    # Set parser
    PARSER = argparse.ArgumentParser(prog="main.py",
                                     description="""C""",)
    PARSER.add_argument("filename", help="Name of the file where the filtered"
                                         " graph is", type=str)
    PARSER.add_argument("-v", "--verbose", help="Turn on verbosity",
                        action="store_true")
    ARGS = PARSER.parse_args()

    # load the graph
    G = nx.read_edgelist(ARGS.filename, nodetype=int,
                         data=(("weight", float),))

    # Compute the best partition with Louvain
    louvain = community.best_partition(G)
    ecg = community.ecg(G, ens_size=16, resolution=1.0)

    # Read the ground truth
    truth = {}
    with open("community.dat", 'rb') as gt_file:
        lines = gt_file.readlines()
        for line in lines:
            node, community = line.split()
            truth[int(node)] = int(community)

    # For some reason there are more nodes in truth than in partition. Maybe
    # the Louvain implementation considers the biggest cc?
    truth_pruned = {}
    for k, v in truth.items():
        if k in louvain:
            truth_pruned[k] = v

    # print(sorted(partition.keys()) == sorted(truth_d_pruned.keys()))
    louvain_sorted = [louvain[i] for i in sorted(louvain.keys())]
    truth_sorted = [truth_pruned[i] for i in sorted(truth_pruned.keys())]
    ecg_sorted = [ecg.partition[i] for i in sorted(ecg.partition.keys())]
    print("NMI Louvain", normalized_mutual_info_score(louvain_sorted, truth_sorted))
    print("NMI ECG", normalized_mutual_info_score(ecg_sorted, truth_sorted))
