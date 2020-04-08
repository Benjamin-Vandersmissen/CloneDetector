//
// Created by benji on 29/03/2020.
//

#include "Graph.h"
#include <iostream>

void plot(std::ostream &stream, const std::vector<Graph *> &graphs) {
    stream << "digraph Circuit{\n";
    for (const auto& graph : graphs){
        stream << "subgraph \"cluster_" << graph->name() << "\" {\n";
        for(const auto& node: graph->nodes())
            stream << "\"" << node->getName() << "\";\n";
        for(const auto& edge: graph->edges()){
            stream <<"\"" <<  edge->from().first->getName() << "\" -> \"" << edge->to().first->getName() << "\" [label=\"" << edge->from().second << "/" << edge->to().second << "\"];\n";
        }

        stream << "label=\"Circuit: " << graph->name() << "\"\n";
        stream << "}\n";
    }
    stream << "}";
}

//TODO: fix for overlapping clones
void
plot_clones(std::ostream &stream, const std::vector<Graph *> &graphs, bool onlyClones) {
    stream << "digraph Circuit{\n";
    auto clone_groups = getSelectCloneGroups(graphs);

    std::map<unsigned, unsigned> clone_to_group;
    std::map<unsigned, std::vector<edge_ptr> > clone_to_edges;
    std::map<Graph*, std::vector<unsigned> > graph_to_clones;

    std::vector<edge_ptr> clone_edges;

    auto clone_id = 0;
    for(unsigned group_id = 0; group_id < clone_groups.size(); ++ group_id){
        auto clone_group = clone_groups[group_id];
        for(const auto& subgraph : clone_group){
            clone_to_group[clone_id] = group_id;
            clone_to_edges[clone_id] = subgraph.edges();
            clone_edges.insert(clone_edges.begin(), subgraph.edges().begin(), subgraph.edges().end());
            for(const auto& graph : graphs){
                if (! intersection(graph->edges(), subgraph.edges()).empty()) {
                    graph_to_clones[graph].push_back(clone_id);
                    break;
                }
            }
            clone_id++;
        }
    }
    for (const auto& graph : graphs){
        stream << "subgraph \"cluster_" << graph->name() << "\" {\n";
        for(const auto& node: graph->nodes()) {
            if (!onlyClones)
                stream << "\"" << node->getName() << "\";\n";
        }

        auto clones = graph_to_clones[graph];
        for(auto clone : clones){
            stream << "subgraph cluster_" << clone << " {\n";
            stream << "label=\"Clone " << clone_to_group[clone] << "\"\n";
            stream << "color=red\n";
            for(const auto& edge : clone_to_edges[clone]){
                stream <<"\"" <<  edge->from().first->getName() << "\" -> \"" << edge->to().first->getName() << "\" [label=\"" << edge->from().second << "/" << edge->to().second << "\"];\n";
            }
            stream << "}\n";
        }

        for(const auto& edge: graph->edges()){
            if (!onlyClones and ! contains(clone_edges, edge))
                stream <<"\"" <<  edge->from().first->getName() << "\" -> \"" << edge->to().first->getName() << "\" [label=\"" << edge->from().second << "/" << edge->to().second << "\"];\n";
        }

        stream << "label=\"Circuit: " << graph->name() << "\"\n";
        stream << "}\n";
    }
    stream << "}";
}

void Graph::addNode(const node_ptr& node) {
    if (! contains(m_nodes, node))
        m_nodes.push_back(node);
}

void Graph::addEdge(const edge_ptr &edge) {
    if (m_circuit_to_edges.empty())
        m_circuit_to_edges[m_file+": "+m_name] = {};

    if (contains(m_nodes, edge->from().first) and contains(m_nodes, edge->to().first)) {
        if (edge->parent().empty()) {
            edge->parent(m_name);
            edge->file(m_file);
        }
        m_edges.push_back(edge);
        m_circuit_to_edges[m_file+": "+m_name].push_back(edge);
    }
}

const std::vector<node_ptr> & Graph::nodes() const {
    return m_nodes;
}

const std::vector<edge_ptr> & Graph::edges() const {
    return m_edges;
}


//based on algorithm eScan from https://www.researchgate.net/publication/221553495_Complete_and_accurate_clone_detection_in_graph-based_models
void Graph::findClones() {
    std::vector<CandidateClone> subgraphs;
    for(const auto& edge: m_edges){
        subgraphs.emplace_back(edge);
    }
    auto iteration = 0;
    while(true) {
        std::cout << "Before Pruning:" << subgraphs.size() << std::endl;
        subgraphs = prune(subgraphs, iteration);
        std::cout << "After Pruning:" << subgraphs.size() << std::endl;
        if(subgraphs.empty()) return;   // No potential clone pairs anymore
        subgraphs = extend(subgraphs);
        std::cout << "After Extending:" << subgraphs.size() << std::endl;
        if (iteration > 0) removeCoveredGroups(iteration);
        iteration ++;
    }
}

std::vector<CandidateClone> Graph::prune(const std::vector<CandidateClone>& subs, unsigned iteration) {
    std::map<std::string, std::vector<CandidateClone>> mapping;
    for(const auto& sub : subs){
        auto representation = sub.representation();
        bool overlapping = false;
        for(const auto& other : mapping[representation])
            overlapping |= overlap(other, sub);
        if (! overlapping)
            mapping[representation].push_back(sub);
    }

    std::vector<CandidateClone > retValue;

    for(auto pair : mapping){
        if(pair.second.size() > 1){ // a canonical label has more than 1 subGraph -> clones
            m_cloneGroups[iteration].push_back(pair);
            retValue.insert(retValue.end(), pair.second.begin(), pair.second.end());
        }
    }
    return retValue;
}

std::vector<CandidateClone> Graph::extend(const std::vector<CandidateClone> &subs) {
    std::vector<CandidateClone> retValue;
    // if and only if subs[i] is a clone of size k, sub[j] is a clone of size k, intersection(sub[i], sub[j]) has a size of k-1, then union(sub[i][, sub[j]) is a candidate clone
    for (auto i = 0; i < subs.size(); ++i){
        for(auto j = i+1; j < subs.size(); ++j){
            if(subs[i].canMerge(subs[j])) {
                auto edge = difference(subs[j].edges(), subs[i].edges())[0];
                auto new_sub = subs[i];
                new_sub.addEdge(edge);
                retValue.push_back(new_sub);
            }
        }
    }
    return retValue;
}

void Graph::removeCoveredGroups(unsigned iteration) {
    auto& previousGroups = m_cloneGroups[iteration-1];
    auto& currentGroups = m_cloneGroups[iteration];

    std::vector<std::size_t > to_delete;

    for(auto i = 0; i  < previousGroups.size(); ++i){
        for(auto j = static_cast<long>(currentGroups.size())-1; j >= 0; --j){ //TODO: why is this backwards
            auto group1 = previousGroups[i];
            auto group2 = currentGroups[j];

            bool covered = true;
            for(const auto& subgraph : group1.second){
                covered &= covers(subgraph, group2.second);
                if (!covered)   break;
            }
            if (covered){
                to_delete.push_back(i);
                break;
            }
        }
    }
    std::reverse(to_delete.begin(), to_delete.end());
    for(auto index : to_delete){
        previousGroups.erase(previousGroups.begin()+index);
    }
    if(previousGroups.empty())  this->m_cloneGroups.erase(iteration-1);
}

std::vector<std::vector<CandidateClone>> Graph::getAllCloneGroups() const {
    std::vector<std::vector<CandidateClone>> clones;
    for(const auto& pair : m_cloneGroups){ // all clone groups with same iteration
        for(const auto& group : pair.second){ // a clone group
            clones.push_back(group.second);
        }
    }
    return clones;
}

std::vector<std::vector<CandidateClone>> getCloneGroups(const std::vector<Graph*> &graphs) {
    Graph total_graph;
    for(const auto& graph: graphs){
        total_graph.merge(*graph);
    }
    total_graph.findClones();
    return total_graph.getAllCloneGroups();
}

std::vector<std::vector<CandidateClone>> getSelectCloneGroups(const std::vector<Graph *> &graphs) {
    auto clone_groups = getCloneGroups(graphs);
    std::vector<std::vector<CandidateClone>> retValue;
    for (auto group : clone_groups){
        if(coveredNodes(group.front()) > 7) // test only front of group, because all other clones in group are identical in form
            retValue.push_back(group);
    }
    return retValue;
}

void Graph::print() {
    for (const auto& edge : m_edges){
        std::cout << edge->text(false) << std::endl;
    }
}

Graph::Graph(std::string name, std::string file) : m_name(std::move(name)), m_file(std::move(file)){}

const std::string &Graph::name() const {
    return m_name;
}

void Graph::merge(const Graph &graph) {
    for (auto pair : graph.m_circuit_to_edges)
        m_circuit_to_edges.insert(pair);
    m_edges.insert(m_edges.begin(), graph.m_edges.begin(), graph.m_edges.end());
    m_nodes.insert(m_nodes.begin(), graph.m_nodes.begin(), graph.m_nodes.end());
}
