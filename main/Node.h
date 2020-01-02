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

    // inport : (Node, outport)
    std::map<unsigned, std::pair<Node*, unsigned> > m_incoming_nodes;

    // outport : (Node, inport)
    std::map<unsigned, std::pair<Node*, unsigned> > m_outgoing_nodes;

    // name : occurences
    static std::map<std::string, unsigned int> counter;

public:
    Node(std::string name);

    Node(Component* component);

    /**
     * \brief add an outgoing connection from this node to another node
     * */
    void addOutGoingConnection(Node *node, unsigned inport, unsigned outport);

    /**
     * \brief add an incoming connection to this node from another node
     * */
    void addIncomingConnection(Node *node, unsigned inport, unsigned outport);

    /**
     * \brief returns the incoming nodes in the format {{inport, node, outport}..}
     * */
    std::vector<std::tuple<unsigned, Node*, unsigned>> getIncomingNodes() const;

    /**
     * \brief Simple getter for the name
     * */
    const std::string &getName() const;
};

/**
 * \brief generate a dot output for forests, plots all the forests in the same stream
 * */
void plot(std::ostream &stream, const std::vector<std::vector<Node *> > &forest, std::vector<std::string> names);

#endif //CLONEDETECTOR_NODE_H
