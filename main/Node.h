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

class ConstructionException: public std::exception{
private:
    std::string m_errmsg;
public:
    explicit ConstructionException(std::string  msg);

    [[nodiscard]] const char* what() const noexcept override ;
};


class Node {
private:
    std::string m_name;

    Component* m_component;

public:
    // name : occurences
    static std::map<std::string, unsigned int> counter;

    Node(std::string name);

    Node(Component* component);

    /**
     * \brief Simple getter for the name
     * */
    const std::string &getName() const;

    Component *component() const;
};

class Edge{
private:
    std::pair<Node*, unsigned> m_from;

    std::pair<Node*, unsigned> m_to;

public:
    Edge(Node* from, unsigned outport, Node* to, unsigned inport);

    const std::pair<Node *, unsigned int> &from() const;

    const std::pair<Node *, unsigned int> &to() const;
};

class SubGraph;

class Graph{
protected:
    std::vector<Node*> m_nodes;

    std::vector<Edge*> m_edges;

    std::map<unsigned, std::vector< std::vector<SubGraph*> > > m_cloneGroups;
public:
    Graph();

    void addNode(Node* node);

    virtual void addEdge(Edge* edge);

    const std::vector<Node *> &nodes() const;

    const std::vector<Edge *> &edges() const;

    void findClones();

    std::vector<SubGraph *> prune(std::vector<SubGraph *> subs, unsigned iteration);

    std::vector<SubGraph*> extend(std::vector<SubGraph*> subs, unsigned iteration);
};

class SubGraph : public Graph{
private:
    std::map<Node*, std::string> m_mapping;

    std::map<std::string, unsigned> m_counter;
public:
    SubGraph(Edge* edge);

    bool canConnect(Edge* edge);

    void addEdge(Edge* edge) override;

    std::string representation() const;

    void remap();

    static bool compareEdges(Edge* e1, Edge* e2);

};

bool overlap(SubGraph *sg1, SubGraph* sg2);

/**
 * \brief generate a dot output for forests, plots all the forests in the same stream
 * */
void plot(std::ostream &stream, const std::vector<Graph* > &forest, std::vector<std::string> names);

#endif //CLONEDETECTOR_NODE_H
