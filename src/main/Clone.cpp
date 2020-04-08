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
            if (edge1->text(false) > edge->text(false)) {
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

    m_edges.push_back(edge);
}

// Assume no loops in the graph
std::string CandidateClone::representation_dfs(const node_ptr &node) const{
    std::string representation;
    std::vector<std::string> representations;
    for(const auto& edge : m_graph.at(node)){
        representations.push_back(edge->text(false) + "  " + representation_dfs(edge->to().first));
    }
    std::sort(representations.begin(), representations.end());
    for(const auto& s : representations){
        representation += s + "  ";
    }
    return representation + ">"; // > is backtrack symbol
}

std::string CandidateClone::representation() const {
    std::vector<std::string> representations;
    for(const auto& node : m_outgoing_nodes){
        representations.push_back(representation_dfs(node));
    }
    std::string representation;
    std::sort(representations.begin(), representations.end());
    for(const auto& s : representations){
        representation += s + "  ";
    }
    return representation ;
}

bool CandidateClone::canMerge(const CandidateClone &sg) const{
    if (this->edges().size() == 1 and sg.edges().size() == 1)
        return canConnect(sg.edges()[0]);
    return this->edges().size() == sg.edges().size() and intersection(this->edges(), sg.edges()).size() == this->edges().size()-1;
}

const std::vector<edge_ptr> &CandidateClone::edges() const {
    return m_edges;
}

std::vector<node_ptr> CandidateClone::nodes() const {
    std::vector<node_ptr > nodes;
    for (const auto& pair : m_graph)
        nodes.push_back(pair.first);
    return nodes;
}

bool CandidateClone::operator==(const CandidateClone &other) const {
    return this->m_graph == other.m_graph && this->m_edges == other.m_edges;
}

std::string CandidateClone::circuit() const {
    return m_edges[0]->file() + ": " + m_edges[0]->parent();
}

bool overlap(const CandidateClone &sg1, const CandidateClone &sg2) {
    return ! intersection(sg1.edges(), sg2.edges()).empty();
}

bool covers(const CandidateClone &sg, const std::vector<CandidateClone> &cover) {
    for (const auto& sg2 : cover){
        if (intersection(sg.edges(), sg2.edges()).size() == sg.edges().size()) return true;
    }

    return false;
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