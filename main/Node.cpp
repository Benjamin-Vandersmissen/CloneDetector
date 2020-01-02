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

void Node::addOutGoingConnection(Node *node, unsigned inport, unsigned outport) {
    node->addIncomingConnection(this, inport, outport);
    this->m_outgoing_nodes[outport] = {node, inport};
}

Node::Node(Component *component) : Node(component->m_name){}

void Node::addIncomingConnection(Node *node, unsigned inport, unsigned outport) {
    if (m_incoming_nodes.find(inport) != m_incoming_nodes.end())
        throw ConstructionException("Port " + std::to_string(inport) + " of " + m_name + " has multiple connected outputs");
    m_incoming_nodes[inport] = {node, outport};
}

std::vector<std::tuple<unsigned, Node*, unsigned>>  Node::getIncomingNodes() const {
    std::vector<std::tuple<unsigned, Node*, unsigned> > out;
    for(const auto& pair : m_incoming_nodes){
        out.emplace_back(pair.first, pair.second.first, pair.second.second);
    }
    return out;
}

const std::string &Node::getName() const {
    return m_name;
}

const char *ConstructionException::what() const noexcept {
    return m_errmsg.c_str();
}

ConstructionException::ConstructionException(std::string msg) : m_errmsg(std::move(msg)){}


void plot(std::ostream &stream, const std::vector<std::vector<Node *> > &forest, std::vector<std::string> names) {
    stream << "digraph Circuit{\n";
    auto name_it = names.begin();
    for (const auto& tree : forest){
        stream << "subgraph cluster_" << *name_it << " {\n";
        for (const auto& node : tree){
            auto incoming = node->getIncomingNodes();
            for(const auto& tuple : incoming){
                auto inport = std::get<0>(tuple);
                auto other = std::get<1>(tuple);
                auto outport = std::get<2>(tuple);
                stream << "\"" << other->getName() << "\" -> \"" << node->getName() << "\" [label=\" " << outport << " / " << inport << "\"]\n";
            }
            if (incoming.empty())
                stream << "\"" << node->getName() << "\"" << std::endl;
        }
        stream << "label=\"Circuit: " << *name_it << "\"\n";
        std::advance(name_it, 1);
        stream << "}\n";
    }
    stream << "}";
}