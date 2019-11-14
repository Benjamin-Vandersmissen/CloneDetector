//
// Created by benji on 05/11/2019.
//

#include "Node.h"
#include "XMLParser.h"

#include <utility>

std::map<std::string, unsigned int> Node::counter = {};


Node::Node(std::string name) {
    if (counter.find(name) == counter.end()){
        counter[name] = 0;
    }
    m_name = name + "_" + std::to_string(counter[name]++); // Append index to name and increment index
}

void Node::addOutGoingConnection(Node *node, unsigned port) {
    node->addIncomingConnection(this, port);
    this->m_outgoing_nodes.push_back(node);
}

Node::Node(Component *component) : Node(component->m_name){
    m_nr_ports = component->m_in.size();
}

void Node::addIncomingConnection(Node *node, unsigned port) {
    if (m_incoming_nodes.find(port) != m_incoming_nodes.end())
        throw ConstructionException("Port " + std::to_string(port) + " of " + m_name + " has multiple connected outputs");
    m_incoming_nodes[port] = node;
}

std::vector<std::pair<unsigned int, Node *>> Node::getIncomingNodes() const {
    std::vector<std::pair<unsigned, Node*> > out;
    for(const auto& pair : m_incoming_nodes){
        out.emplace_back(pair);
    }
    return out;
}

const std::string &Node::getName() const {
    return m_name;
}

const char *ConstructionException::what() const noexcept {
    return m_errmsg.c_str();
}

ConstructionException::ConstructionException(const std::string &msg) : m_errmsg(msg){}


void plot(std::ostream &stream, const std::vector<std::vector<Node *> > &forest) {
    stream << "digraph Circuit{\n";
    for (const auto& tree : forest){
        for (const auto& node : tree){
            auto incoming = node->getIncomingNodes();
            for(const auto& pair : incoming){
                auto port = pair.first;
                auto other = pair.second;
                stream << "\"" << other->getName() << "\" -> \"" << node->getName() << "\" [label=\" todo_OUT / " << port << "\"]\n";
            }
            if (incoming.empty())
                stream << "\"" << node->getName() << "\"" << std::endl;
        }
    }
    stream << "}";
}