//
// Created by benji on 05/11/2019.
//

#ifndef CLONEDETECTOR_NODE_H
#define CLONEDETECTOR_NODE_H

#include <string>
#include <vector>
#include <map>
#include <exception>
#include <tuple>
#include "Components.h"


class Node {
private:
    std::string m_name;

    component_ptr m_component;

public:
    explicit Node(const component_ptr &component);

    /**
     * \brief Simple getter for the name
     * */
    const std::string &getName() const;

    component_ptr component() const;
};

using node_ptr = std::shared_ptr<Node>;

class Edge{
private:
    std::pair<node_ptr, unsigned> m_from;

    std::pair<node_ptr , unsigned> m_to;

public:
    Edge(const node_ptr& from, unsigned outport, const node_ptr &to, unsigned inport);

    /**
     * \brief Simple getter for m_from
     * */
    const std::pair<node_ptr, unsigned int> & from() const;

    /**
     * \brief Simple getter for m_to
     * */
    const std::pair<node_ptr, unsigned int> & to() const;
};

using edge_ptr = std::shared_ptr<Edge>;

class SubGraph;

class Graph{
protected:
    std::vector<node_ptr> m_nodes;

    std::vector<edge_ptr> m_edges;

    // # edges : {representation : clones, ... }
    std::map<unsigned, std::vector< std::pair<std::string, std::vector<SubGraph> > > > m_cloneGroups;
public:
    Graph()=default;

    /**
     * \brief Add a node to this graph if it isn't already present
     * */
    void addNode(const node_ptr& node);

    /**
     * \brief Add an edge to this graph only if both nodes it connects are already in the graph
     * */
    virtual void addEdge(const edge_ptr &edge);

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

    /**
     * \brief Prunes the working set so only SubGraphs that have a non-overlapping clone in the working set are still present in the working set. As byproduct, it populates m_cloneGroups[iteration]
     *
     * \returns The current working set of SubGraphs
     * */
    std::vector<SubGraph> prune(const std::vector<SubGraph> &subs, unsigned iteration);

    /**
     * \brief Extends the working set: for each SubGraph, make an extension with each possible edge such that the SubGraph is connected and no edges are duplicates
     *
     * \returns The current working set of SubGraphs
     * */
    std::vector<SubGraph> extend(const std::vector<SubGraph> &subs);

    /**
     * \brief Removes Clonegroups of size (iteration-1) if they are fully covered by a cloneGroup of size iteration
     * */
    void removeCoveredGroups(unsigned iteration);

    /**
     * \brief returns all CloneGroups as a vector of cloneGroups
     * */
    std::vector<std::vector<SubGraph>> getAllCloneGroups() const;

    friend std::vector<std::vector<SubGraph>> getCloneGroups(const std::vector<Graph*>& graphs);
};

/**
 * \brief Calculate the cloneGroups of a vector of Graphs, by merging them in one graph and running Graph::findClones, graph::getAllCloneGroups
 * */
std::vector<std::vector<SubGraph>> getCloneGroups(const std::vector<Graph*>& graphs);

class SubGraph : public Graph{
private:
    // node : unique name (in subGraph)
    std::map<node_ptr, std::string> m_mapping;

    // node name : count
    std::map<std::string, unsigned> m_counter;
public:
    explicit SubGraph(const edge_ptr &edge);

    /**
     * \brief return True if edge not in m_edges and edge can connect with another edge from m_edges
     * */
    bool canConnect(const edge_ptr &edge);

    /**
     * \brief Add an edge to m_edges, also add the nodes if they aren't in m_nodes
     * */
    void addEdge(const edge_ptr &edge) override;

    /**
     * \brief Try to calculate a canonical label
     * */
    std::string representation() const;

    /**
     * \brief remap all the nodes
     * */
    void remap();

    /**
     * \brief used to order edges based on their textual representation
     * */
    static bool compareEdges(const edge_ptr &e1, const edge_ptr &e2);

};

/**
 * \brief Test if the edges of two subgraphs overlap -> non-empty intersection
 * */
bool overlap(const SubGraph &sg1, const SubGraph &sg2);

/**
 * \brief Test if a subgraph is covered by a vector of subgraphs => 1 subgraph from the vector covers the subgraph -> the subgraph has all edges of the covered subgraph and one edge extra
 * */
bool covers(const SubGraph &sg, const std::vector<SubGraph>& cover);

/**
 * \brief generate a dot output for graphs, plots all the graphs in the same stream
 * */
void plot(std::ostream &stream, const std::vector<Graph* > &graphs, std::vector<std::string> names);

/**
 * \brief generate a dot output for graphs, plot all the graphs in the same stream and highlight the clones
 * */
void
plot_clones(std::ostream &stream, const std::vector<Graph *> &graphs, std::vector<std::string> names, bool onlyClones=false);

#endif //CLONEDETECTOR_NODE_H
