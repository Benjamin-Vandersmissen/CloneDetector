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

    std::string m_representation;

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
     * \brief Calculate a canonical label and store it for further usage
     * */
    std::string representation();

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

    /**
     * \brief returns *filename* / *circuit*
     * */
    std::string circuit() const;

    /**
     * \brief returns the first edge in the set
     * */
    const edge_ptr& firstEdge() const;

    /**
     * \brief merge c1, by making a copy and adding c2
     * */
    static CandidateClone merge(const CandidateClone &c1, const CandidateClone &c2);
};

/**
 * \brief Test if the edges of two subgraphs overlap -> non-empty intersection
 * */
bool overlap(const CandidateClone &sg1, const CandidateClone &sg2);

/**
 * \brief Test if the edges of group1 are contained in the edges of group2
 * */
bool covered(const std::vector<CandidateClone>& group1, const std::vector<CandidateClone>& group2);
/**
  * \brief Calculate the covered nodes of a SubGraph, i.e. the nodes that have all outgoing or all incoming edges in the subgraph
  * */
 unsigned coveredNodes(const CandidateClone& sg);

#endif //CLONEDETECTOR_CLONE_H
