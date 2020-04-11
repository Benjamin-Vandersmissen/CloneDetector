//
// Created by benji on 29/03/2020.
//

#ifndef CLONEDETECTOR_CLONE_H
#define CLONEDETECTOR_CLONE_H

#include "Node.h"

class CandidateClone;

class CandidateClone{
private:

	// maps a node to the outgoing edges
    std::map<node_ptr, std::vector<edge_ptr> > m_graph;

	// nodes with outgoing edges
    std::vector<node_ptr> m_outgoing_nodes;

	// nodes with incoming edges
    std::vector<node_ptr> m_incoming_nodes;

    std::set<edge_ptr> m_edges;

    edge_ptr m_last_added;

    /**
     * \brief DFS implementation that makes the representation
     * */
    std::string representation_dfs(const node_ptr &node) const;

public:
    CandidateClone()=default;

    explicit CandidateClone(const edge_ptr &edge);

    /**
     * \brief return True if edge not in m_edges and edge can connect with another edge from m_edges
     * */
    bool canConnect(const edge_ptr &edge) const;

    /**
     * \brief Add an edge to m_edges, also add the nodes if they aren't in m_nodes
     * */
    void addEdge(const edge_ptr &edge);

    /**
     * \brief Try to calculate a canonical label
     * */
    std::string representation() const;

    /**
     * \brief size of intersection of edges = size of edges - 1
     * */
    bool canMerge(const CandidateClone& sg) const;

    /**
     * \brief simple getter for edges
     * */
    const std::set<edge_ptr> &edges() const;

    /**
     * \brief simple getter for nodes
     * */
    std::vector<node_ptr> nodes() const;

    bool operator==(const CandidateClone& other) const;

    bool operator<(const CandidateClone& other) const;

    std::string circuit() const;

    edge_ptr lastEdge() const;

    edge_ptr lastAdded() const;

    bool isGeneratingParent(const CandidateClone& other) const;
};

CandidateClone merge(const CandidateClone& c1, const CandidateClone& c2);
/**
 * \brief Test if the edges of two subgraphs overlap -> non-empty intersection
 * */
bool overlap(const CandidateClone &sg1, const CandidateClone &sg2);

/**
 * \brief Test if a subgraph is covered by a vector of subgraphs => 1 subgraph from the vector covers the subgraph -> the subgraph has all edges of the covered subgraph and one edge extra
 * */
bool covers(const CandidateClone &sg, const std::vector<CandidateClone>& cover);

/**
  * \brief Calculate the covered nodes of a SubGraph, i.e. the nodes that have all outgoing or all incoming edges in the subgraph
  * */
 unsigned coveredNodes(const CandidateClone& sg);

#endif //CLONEDETECTOR_CLONE_H
