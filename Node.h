//
// Created by benji on 05/11/2019.
//

#ifndef CLONEDETECTOR_NODE_H
#define CLONEDETECTOR_NODE_H

#include <string>
#include <vector>
#include <map>
#include <exception>
#include "Components.h"

class ConstructionException: public std::exception{
private:
    std::string m_errmsg;
public:
    ConstructionException(const std::string& msg);

    const char* what() const noexcept ;
};


class Node {
private:
    std::string m_name;

    unsigned m_nr_ports;

    std::map<unsigned, Node*> m_incoming_nodes;

    std::vector<Node*> m_outgoing_nodes;

    static std::map<std::string, unsigned int> counter;

public:
    Node(std::string name);

    Node(Component* component);

    void addOutGoingConnection(Node *node, unsigned port);

    void addIncomingConnection(Node* node, unsigned port);

    std::vector<std::pair<unsigned int, Node *>> getIncomingNodes() const;

    const std::string &getName() const;
};


void plot(std::ostream &stream, const std::vector<std::vector<Node*> >& forest);

#endif //CLONEDETECTOR_NODE_H
