//
// Created by benji on 29/03/2020.
//
#include "Clone.h"

//TODO: phase out nodes(), use m_graph instead?
CandidateClone::CandidateClone(const edge_ptr &edge) {
    addEdge(edge);
}

bool CandidateClone::canConnect(const edge_ptr &edge) const{
    auto nodes = this->nodes();
    return (contains(nodes, edge->from().first) || contains(nodes, edge->to().first)) and not(contains(m_edges, edge));
}

void CandidateClone::addEdge(const edge_ptr &edge) {
    auto from = edge->from().first;
    auto to = edge->to().first;
    if (m_graph.find(from) == m_graph.end()) m_graph[from] = {edge};

    else{
        auto& vec = m_graph[from];
        bool inserted = false;
        for(auto it = vec.begin(); it != vec.end(); ++it) {
            const auto& edge1 = *it;
            if (edge1->representation() > edge->representation()) {
                vec.insert(it, edge);
                inserted = true;
                break;
            }
        }
        if(!inserted) vec.push_back(edge);
    }
    if (!contains(m_outgoing_nodes, from) and !contains(m_incoming_nodes, from)) m_outgoing_nodes.push_back(from);

    if (contains(m_outgoing_nodes, to)) m_outgoing_nodes.erase(std::find(m_outgoing_nodes.begin(), m_outgoing_nodes.end(), to));

    if(!contains(m_incoming_nodes, to)) m_incoming_nodes.push_back(to);

    if (m_graph.find(to) == m_graph.end()) m_graph[to] = {};

    m_edges.insert(edge);
}

// Assume no loops in the graph
std::string CandidateClone::representation_dfs(const node_ptr &node) const{
    std::string representation;
    std::vector<std::string> representations;
    for(const auto& edge : m_graph.at(node)){
        representations.push_back(edge->representation() + "  " + representation_dfs(edge->to().first));
    }
    std::sort(representations.begin(), representations.end());
    for(const auto& s : representations){
        representation += s + "  ";
    }
    return representation + ">"; // > is backtrack symbol
}

std::string CandidateClone::representation(){
    if (m_representation.empty()) {
        std::vector<std::string> representations;
        for (const auto &node : m_outgoing_nodes) {
            representations.push_back(representation_dfs(node));
        }
        std::string representation;
        std::sort(representations.begin(), representations.end());
        for (const auto &s : representations) {
            representation += s + "  ";
        }
        m_representation = representation;
    }
    return m_representation;
}

bool CandidateClone::canMerge(const CandidateClone &sg) const{
    // sg.firstEdge needs to be higher, so for example A->B->C-> D is only generated from A->B->C, not from B->C->D
    // We will use this for removing the duplicate groups
    return canConnect(sg.firstEdge());
}

const std::set<edge_ptr> &CandidateClone::edges() const {
    return m_edges;
}

std::vector<node_ptr> CandidateClone::nodes() const {
    std::vector<node_ptr > nodes;
    for (const auto& pair : m_graph)
        nodes.push_back(pair.first);
    return nodes;
}

bool CandidateClone::operator==(const CandidateClone &other) const {
    return this->circuit() == other.circuit() && this->m_graph == other.m_graph && this->m_edges == other.m_edges;
}

bool CandidateClone::operator<(const CandidateClone &other) const {
    if(this->circuit() < other.circuit())
        return true;
    else if (this->circuit() > other.circuit())
        return false;

    else return std::tie(this->m_graph, this->m_edges) < std::tie(other.m_graph, other.m_edges);
}

std::string CandidateClone::circuit() const {
    return firstEdge()->file() + ": " + firstEdge()->parent();
}

const edge_ptr &CandidateClone::firstEdge() const {
    return *(m_edges.begin());
}

CandidateClone CandidateClone::merge(const CandidateClone &c1, const CandidateClone &c2) {
    CandidateClone c3;
    auto edges = ::merge(c1.edges(), c2.edges());

    for(const auto& edge: edges){
        c3.addEdge(edge);
    }
    return c3;
}

edge_ptr CandidateClone::lastEdge() const {
    return *m_edges.rbegin();
}

bool CandidateClone::isGeneratingParent(const CandidateClone &other) const {
    return this->edges().size() == other.edges().size() + 1 && intersection(this->edges(), other.edges()).size() == other.edges().size() && this->lastEdge() != other.lastEdge();
}

bool overlap(const CandidateClone &sg1, const CandidateClone &sg2) {
    return ! intersection(sg1.edges(), sg2.edges()).empty();
}


bool covered(const std::vector<CandidateClone> &group1, const std::vector<CandidateClone> &group2) {
    auto edges1 = std::set<edge_ptr>{};
    auto edges2 = std::set<edge_ptr>{};

    for(const auto& clone : group1){
        edges1.insert(clone.edges().begin(), clone.edges().end());
    }
    for(const auto& clone : group2){
        edges2.insert(clone.edges().begin(), clone.edges().end());
    }

    return intersection(edges1, edges2).size() == edges1.size();
}

unsigned coveredNodes(const CandidateClone &sg) {
    auto edges = sg.edges();
    auto nodes = sg.nodes();
    auto map_out = std::map<node_ptr, std::vector<bool>>();
    auto map_in = std::map<node_ptr, std::vector<bool>>();
    for (const auto &node : nodes) {
        auto in = node->component()->getInputPorts().size();
        auto out = node->component()->getOutPorts().size();
        map_in[node] = std::vector<bool>(in);
        map_out[node] = std::vector<bool>(out);
    }
    for (const auto &edge : edges) {
        map_in[edge->to().first][edge->to().second] = true;
        map_out[edge->from().first][edge->from().second] = true;
    }
    unsigned retValue = 0;
    for(const auto& node : nodes){
        if(!contains(map_in[node], false) or !contains(map_out[node], false)) //if the vector doesn't contains 0, all the entries are 1
            retValue++;
    }
    return retValue;
}