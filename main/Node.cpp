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

Node::Node(Component *component) : Node(component->m_name){}

const std::string &Node::getName() const {
    return m_name;
}

const char *ConstructionException::what() const noexcept {
    return m_errmsg.c_str();
}

ConstructionException::ConstructionException(std::string msg) : m_errmsg(std::move(msg)){}


void plot(std::ostream &stream, const std::vector<Graph*> &graphs, std::vector<std::string> names) {
    stream << "digraph Circuit{\n";
    auto name_it = names.begin();
    for (const auto& graph : graphs){
        stream << "subgraph cluster_" << *name_it << " {\n";
        for(const auto& node: graph->nodes())
            stream << "\"" << node->getName() << "\";\n";
        for(const auto& edge: graph->edges()){
            stream <<"\"" <<  edge->from().first->getName() << "\" -> \"" << edge->to().first->getName() << "\" [label=\"" << edge->from().second << "/" << edge->to().second << "\"];\n";
        }

        stream << "label=\"Circuit: " << *name_it << "\"\n";
        std::advance(name_it, 1);
        stream << "}\n";
    }
    stream << "}";
}

Edge::Edge(Node *from, unsigned outport, Node *to, unsigned inport): m_from{from, outport}, m_to{to, inport} {}

const std::pair<Node *, unsigned int> &Edge::from() const {
    return m_from;
}

const std::pair<Node *, unsigned int> &Edge::to() const {
    return m_to;
}

Graph::Graph() {}

void Graph::addNode(Node *node) {
    if (! contains(m_nodes, node))
        m_nodes.push_back(node);
}

void Graph::addEdge(Edge *edge) {
    if (contains(m_nodes, edge->from().first) and contains(m_nodes, edge->to().first))
        m_edges.push_back(edge);
}

const std::vector<Node *> &Graph::nodes() const {
    return m_nodes;
}

const std::vector<Edge *> &Graph::edges() const {
    return m_edges;
}
