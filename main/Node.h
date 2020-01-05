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

    const std::pair<node_ptr, unsigned int> & from() const;

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

    void addNode(const node_ptr& node);

    virtual void addEdge(const edge_ptr &edge);

    const std::vector<node_ptr> & nodes() const;

    const std::vector<edge_ptr> & edges() const;

    void findClones();

    std::vector<SubGraph> prune(const std::vector<SubGraph> &subs, unsigned iteration);

    std::vector<SubGraph> extend(const std::vector<SubGraph> &subs);

    void removeCoveredGroups(unsigned iteration);

    std::vector<std::vector<SubGraph>> getAllCloneGroups() const;

    friend std::vector<std::vector<SubGraph>> getCloneGroups(const std::vector<Graph*>& graphs);
};

std::vector<std::vector<SubGraph>> getCloneGroups(const std::vector<Graph*>& graphs);

class SubGraph : public Graph{
private:
    std::map<node_ptr, std::string> m_mapping;

    std::map<std::string, unsigned> m_counter;
public:
    explicit SubGraph(const edge_ptr &edge);

    bool canConnect(const edge_ptr &edge);

    void addEdge(const edge_ptr &edge) override;

    std::string representation() const;

    void remap();

    static bool compareEdges(const edge_ptr &e1, const edge_ptr &e2);

};

bool overlap(const SubGraph &sg1, const SubGraph &sg2);

bool covers(const SubGraph &sg, const std::vector<SubGraph>& cover);

/**
 * \brief generate a dot output for forests, plots all the forests in the same stream
 * */
void plot(std::ostream &stream, const std::vector<Graph* > &graphs, std::vector<std::string> names);

void
plot_clones(std::ostream &stream, const std::vector<Graph *> &graphs, std::vector<std::string> names, bool onlyClones=false);

#endif //CLONEDETECTOR_NODE_H
