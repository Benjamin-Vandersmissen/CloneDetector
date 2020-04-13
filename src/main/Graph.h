//
// Created by benji on 29/03/2020.
//

#ifndef CLONEDETECTOR_GRAPH_H
#define CLONEDETECTOR_GRAPH_H

#include "Node.h"
#include "Clone.h"

class Graph{
private:
    std::vector<node_ptr> m_nodes;

    std::vector<edge_ptr> m_edges;

    // # edges : {representation : clones, ... }
    std::map<unsigned, std::map< std::string, std::vector<CandidateClone> > > m_cloneGroups;

    std::string m_name; // circuit name

    std::string m_file; // file name

    // maps a circuit name to all edges
    std::map<std::string, std::vector<edge_ptr>> m_circuit_to_edges;

    // a set of all clones of one edge
    std::set<CandidateClone> m_cloned_edges;

    std::map<std::string, std::set<std::string>> m_parent_children;

    std::map<std::string, std::set<std::string>> m_previous_parent_children;
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

    /**
     * \brief Prunes the working set so only Candidate Clones that have a non-overlapping clone in the working set are still present in the working set. As byproduct, it populates m_cloneGroups[iteration]
     *
     * \returns The current working set of Candidate Clones
     * */
    void prune(std::set<CandidateClone> &candidate_set, unsigned iteration);

    /**
     * \brief Extends the working set: for each CandidateClone, make an extension with each possible edge such that the Candidate is connected and no edges are duplicates
     *
     * \returns The current working set of Candidate
     * */
    std::set<CandidateClone> extend(const std::set<CandidateClone> &candidate_set);

    /**
     * \brief Removes Clonegroups of size (iteration-1) if they are fully covered by a cloneGroup of size iteration
     * */
    void removeCoveredGroups(unsigned iteration);

    /**
     * \brief returns all CloneGroups as a vector of cloneGroups
     * */
    std::vector<std::vector<CandidateClone>> getAllCloneGroups() const;

	/**
	* \brief print the representation of the graph
	*/
    void print();

    friend std::vector<std::vector<CandidateClone>> getCloneGroups(const std::vector<Graph*>& graphs);

    friend std::vector<std::vector<CandidateClone>> getSelectCloneGroups(const std::vector<Graph*>& graphs);

    void merge(const Graph& graph);
};

/**
 * \brief generate a dot output for graphs, plots all the graphs in the same stream
 * */
void plot(std::ostream &stream, const std::vector<Graph *> &graphs);

/**
 * \brief generate a dot output for graphs, plot all the graphs in the same stream and highlight the clones
 * */
void
plot_clones(std::ostream &stream, const std::vector<Graph *> &graphs, bool onlyClones=false);

/**
 * \brief Calculate the cloneGroups of a vector of Graphs, by merging them in one graph and running Graph::findClones, graph::getAllCloneGroups
 * */
std::vector<std::vector<CandidateClone>> getCloneGroups(const std::vector<Graph*>& graphs);

/**
 * \brief Calculate the cloneGroups of a vector of Graphs and filtering them
 * */
 std::vector<std::vector<CandidateClone>> getCloneGroups(const std::vector<Graph*>& graphs);

#include "Node.h"
#include "Clone.h"

#endif //CLONEDETECTOR_GRAPH_H
