//
// Created by benji on 29/03/2020.
//

#ifndef CLONEDETECTOR_GRAPH_H
#define CLONEDETECTOR_GRAPH_H

#include <set>
#include "Node.h"
#include "Clone.h"

class Graph{
protected:
    std::vector<node_ptr> m_nodes;

    std::vector<edge_ptr> m_edges;

    // # edges : {representation : clones, ... }
    std::map<unsigned, std::map<std::string, std::set<CandidateClone> > > m_cloneGroups;

    std::string m_name;

    std::string m_file;

    // maps a circuit name to all edges
    std::map<std::string, std::vector<edge_ptr>> m_circuit_to_edges;

    std::map<std::string, std::set<CandidateClone> >m_cloned_edges;
public:
    Graph()=default;

    explicit Graph(std::string name, std::string file);

    /**
     * \brief Simple getter for m_name
     * */
    const std::string& name() const;

    /**
     * \brief Add a node to this graph if it isn't already present
     * */
    void addNode(const node_ptr& node);

    /**
     * \brief Add an edge to this graph only if both nodes it connects are already in the graph
     * */
    void addEdge(const edge_ptr &edge);

    /**
     * \brief Simple getter for m_nodes
     * */
    const std::vector<node_ptr> & nodes() const;

    /**
     * \brief Simple getter for m_edges
     * */
    const std::vector<edge_ptr> & edges() const;

    /**
     * \brief run the e-scan algorithm to populate m_cloneGroups, based on Pham, et al. (2009). Complete and accurate clone detection in graph-based models. 276-286. 10.1109/ICSE.2009.5070528.
     * */
    void findClones();

    void discover(const CandidateClone& fragment);

    void findClonedEdges(std::set<CandidateClone> &candidates);

    void merge(const Graph& graph);
};

void generateClones(std::vector<Graph*> graphs);

/**
 * \brief generate a dot output for graphs, plots all the graphs in the same stream
 * */
void plot(std::ostream &stream, const std::vector<Graph *> &graphs);

/**
 * \brief generate a dot output for graphs, plot all the graphs in the same stream and highlight the clones
 * */
void
plot_clones(std::ostream &stream, const std::vector<Graph *> &graphs, bool onlyClones=false);

#include "Node.h"
#include "Clone.h"

#endif //CLONEDETECTOR_GRAPH_H
