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

Node::Node(Component *component) : Node(component->m_name){
    m_component = component;
}

const std::string &Node::getName() const {
    return m_name;
}

Component *Node::component() const {
    return m_component;
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


//based on algorithm eScan from https://www.researchgate.net/publication/221553495_Complete_and_accurate_clone_detection_in_graph-based_models
void Graph::findClones() {
    std::vector<SubGraph*> subgraphs;
    for(const auto& edge: m_edges){
        subgraphs.push_back(new SubGraph(edge));
    }
    auto iteration = 0;
    while(m_cloneGroups.size() == iteration) { //TODO: remove clonegroup if it is a subgroup of a larger clonegroup
        subgraphs = prune(subgraphs, iteration);
        subgraphs = extend(subgraphs, iteration);
        iteration ++;
    }
}

std::vector<SubGraph *> Graph::prune(std::vector<SubGraph *> subs, unsigned iteration) {
    std::map<std::string, std::vector<SubGraph*>> mapping;
    for(auto sub : subs){
        auto representation = sub->representation();
        bool overlapping = false;
        for(auto other : mapping[representation])
            overlapping |= overlap(other, sub);
        if (! overlapping)
            mapping[representation].push_back(sub);
    }

    std::vector<SubGraph* > retValue;

    for(auto pair : mapping){
        if(pair.second.size() > 1){
            m_cloneGroups[iteration].push_back(pair.second);
            retValue.insert(retValue.end(), pair.second.begin(), pair.second.end());
        }
    }
    return retValue;
}

std::vector<SubGraph *> Graph::extend(std::vector<SubGraph *> subs, unsigned iteration) {
    std::vector<SubGraph*> retValue;
    for (auto sub : subs){
        for(auto edge : m_edges){
            if (sub->canConnect(edge)){
                auto new_sub = new SubGraph(*sub);
                new_sub->addEdge(edge);
                retValue.push_back(new_sub);
            }
        }
    }
    return retValue;
}

SubGraph::SubGraph(Edge *edge) {
    m_edges = {edge};
    m_nodes = {edge->from().first, edge->to().first};

    remap();
}

bool SubGraph::canConnect(Edge *edge) {
    return (contains(m_nodes, edge->from().first) || contains(m_nodes, edge->to().first)) and not(contains(m_edges, edge));
}

void SubGraph::addEdge(Edge *edge) {
    m_edges.push_back(edge);
    if (!contains(m_nodes, edge->from().first)) {
        m_nodes.push_back(edge->from().first);
    } else {
        m_nodes.push_back(edge->to().first);
    }
    remap();
}

std::string SubGraph::representation() const {
    std::vector<std::string> temp;
    std::string representation;
    for (const auto& edge : m_edges){
        temp.push_back(m_mapping.at(edge->from().first) + " -> " + m_mapping.at(edge->to().first) + " ("
                        + std::to_string(edge->from().second) + "/" + std::to_string(edge->to().second) + ")\n");
    }
    std::sort(temp.begin(), temp.end());
    for(const auto& value: temp)
        representation += value;
    return representation;
}

void SubGraph::remap() {
    std::sort(m_edges.begin(), m_edges.end(), compareEdges);

    m_mapping.clear();
    m_counter.clear();
    for(auto edge : m_edges) {
        if (m_mapping.find(edge->from().first) == m_mapping.end()){
            auto component = edge->from().first->component();
            m_mapping[edge->from().first] = component->name() + std::to_string(m_counter[component->name()]);
            m_counter[component->name()]++;
        }
        if (m_mapping.find(edge->to().first) == m_mapping.end()){
            auto component = edge->to().first->component();
            m_mapping[edge->to().first] = component->name() + std::to_string(m_counter[component->name()]);
            m_counter[component->name()]++;
        }
    }
}

bool SubGraph::compareEdges(Edge *e1, Edge *e2) {
    if (e1->from().first->component()->name() < e2->from().first->component()->name())
        return true;
    else if(e1->from().first->component()->name() == e2->from().first->component()->name()){
        if (e1->from().second < e2->from().second)
            return true;
        else if (e1->from().second == e2->from().second){
            if (e1->to().first->component()->name() < e2->to().first->component()->name())
                return true;
            else if (e1->to().first->component()->name() == e1->to().first->component()->name()){
                return e1->to().second < e2->to().second;
            }
        }
    }
    return false;
}

bool overlap(SubGraph *sg1, SubGraph* sg2) {
    return ! intersection(sg1->edges(), sg2->edges()).empty();
}