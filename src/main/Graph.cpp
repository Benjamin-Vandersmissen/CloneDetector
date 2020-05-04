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
//TODO: outdated?
void
plot_clones(std::ostream &stream, const std::vector<Graph *> &graphs, bool onlyClones) {
//    stream << "digraph Circuit{\n";
//    auto clone_groups = getSelectCloneGroups(graphs);
//
//    std::map<unsigned, unsigned> clone_to_group;
//    std::map<unsigned, std::vector<edge_ptr> > clone_to_edges;
//    std::map<Graph*, std::vector<unsigned> > graph_to_clones;
//
//    std::vector<edge_ptr> clone_edges;
//
//    auto clone_id = 0;
//    for(unsigned group_id = 0; group_id < clone_groups.size(); ++ group_id){
//        auto clone_group = clone_groups[group_id];
//        for(const auto& subgraph : clone_group){
//            clone_to_group[clone_id] = group_id;
//            clone_to_edges[clone_id] = subgraph.edges();
//            clone_edges.insert(clone_edges.begin(), subgraph.edges().begin(), subgraph.edges().end());
//            for(const auto& graph : graphs){
//                if (! intersection(graph->edges(), subgraph.edges()).empty()) {
//                    graph_to_clones[graph].push_back(clone_id);
//                    break;
//                }
//            }
//            clone_id++;
//        }
//    }
//    for (const auto& graph : graphs){
//        stream << "subgraph \"cluster_" << graph->name() << "\" {\n";
//        for(const auto& node: graph->nodes()) {
//            if (!onlyClones)
//                stream << "\"" << node->getName() << "\";\n";
//        }
//
//        auto clones = graph_to_clones[graph];
//        for(auto clone : clones){
//            stream << "subgraph cluster_" << clone << " {\n";
//            stream << "label=\"Clone " << clone_to_group[clone] << "\"\n";
//            stream << "color=red\n";
//            for(const auto& edge : clone_to_edges[clone]){
//                stream <<"\"" <<  edge->from().first->getName() << "\" -> \"" << edge->to().first->getName() << "\" [label=\"" << edge->from().second << "/" << edge->to().second << "\"];\n";
//            }
//            stream << "}\n";
//        }
//
//        for(const auto& edge: graph->edges()){
//            if (!onlyClones and ! contains(clone_edges, edge))
//                stream <<"\"" <<  edge->from().first->getName() << "\" -> \"" << edge->to().first->getName() << "\" [label=\"" << edge->from().second << "/" << edge->to().second << "\"];\n";
//        }
//
//        stream << "label=\"Circuit: " << graph->name() << "\"\n";
//        stream << "}\n";
//    }
//    stream << "}";
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
    std::set<CandidateClone> subgraphs;
    for(const auto& edge: m_edges){
        subgraphs.emplace(edge);
    }
    auto iteration = 0;
    while(!subgraphs.empty()) {
        std::cout << iteration << std::endl;
        std::cout << "Before Pruning:" << subgraphs.size() << std::endl;
        prune(subgraphs, iteration);
        std::cout << "After Pruning:" << subgraphs.size() << std::endl;
        subgraphs = extend(subgraphs);
        std::cout << "After Extending:" << subgraphs.size() << std::endl;
        if (iteration > 0)
            removeCoveredGroupsHeuristic(iteration);
        else
            m_cloneGroups.erase(0); // not valuable information
        iteration ++;
    }
}

void Graph::prune(std::set<CandidateClone> &candidate_set, unsigned iteration) {
    std::map<std::string, std::vector<CandidateClone>> mapping;
    for(const auto& candidate : candidate_set){
        //Hacky code, we know that .representation() only caches the representation in the corresponding object without changing anything else
        //The key of a CandidateClone doesn't depend on the representation, so we can safely edit this value in our object
        auto& reference = const_cast<CandidateClone&>(candidate);
        auto representation = reference.representation();
        bool overlapping = false;
        for(const auto& other : mapping[representation])
            overlapping |= overlap(other, candidate);
        if (! overlapping)
            mapping[representation].push_back(candidate);
    }

    candidate_set.clear();

    for(const auto& pair : mapping){
        if(pair.second.size() > 1){ // a canonical label has more than 1 subGraph -> clones
            m_cloneGroups[iteration].insert(pair);
            candidate_set.insert(pair.second.begin(), pair.second.end());
        }
    }
    if (iteration == 0)
        m_cloned_edges = candidate_set;
}

std::set<CandidateClone> Graph::extend(const std::set<CandidateClone> &candidate_set) {
    std::set<CandidateClone> retValue;
    m_previous_parent_children = m_parent_children;
    m_parent_children.clear();
    for (const auto& candidate1 : candidate_set){
        for(const auto& candidate2 : m_cloned_edges){

            // 2 candidates can only merge if they are in the same circuit. The candidate_set is sorted on ascending circuit()
            // This means if candidate1.circuit() < candidate2.circuit(), they can never merge nor can candidate1 merge with a following candidate2'
            // because candidate1.circuit() <= candidate2.circuit() <= candidate2'.circuit()
            // If candidate1.circuit() > candidate2.circuit(), there might be another candidate2', such that candidate2.circuit() < candidate2'.circuit() == candidate1.circuit()
            if (candidate1.circuit() < candidate2.circuit())
                break;
            else if(candidate1.circuit() > candidate2.circuit())
                continue;

            if(candidate1.canMerge(candidate2)) {
                auto new_candidate = CandidateClone::merge(candidate1, candidate2);
                retValue.insert(new_candidate);
                auto reference = const_cast<CandidateClone&>(candidate1); // SEE COMMENT @ Graph::prune

                // Used in removeCoveredGraphs, for faster checking if two graphs are covered
                m_parent_children[reference.representation()].insert(new_candidate.representation());
            }
        }
    }
    return retValue;
}

void Graph::removeCoveredGroupsHeuristic(unsigned iteration) {
    auto& previousGroups = m_cloneGroups[iteration-1];
    auto& currentGroups = m_cloneGroups[iteration];

    if (iteration < 7){  // these clone groups aren't valuable
        m_cloneGroups.erase(iteration);
        return;
    }
    auto it = previousGroups.begin();
    while(it != previousGroups.end()){
        bool deleted = false;
        for(const auto& child : m_previous_parent_children[(*it).first]){
            if (currentGroups.find(child) == currentGroups.end())
                continue;
            if (currentGroups.at(child).size() == (*it).second.size()) {
                it = previousGroups.erase(it);
                deleted = true;
                break;
            }
        }
        if (!deleted) ++it;
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

    auto it = clones.begin();
    while(it != clones.end()){
        bool deleted = false;
        auto it2 = clones.rbegin();
        while(it2 != clones.rend()){  //loop from back to front, because the last clone groups have the most edges, and are most likely to cover the other group
            if (it2->begin()->edges().size() == it->begin()->edges().size())
                break;
            if (covered(*it, *it2)){
                deleted = true;
                it = clones.erase(it);
                break;
            }
            it2++;
        }

        if (!deleted) ++it;
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
    std::cout << clone_groups.size() << std::endl;
    retValue.insert(retValue.end(), clone_groups.begin(), clone_groups.end());  //TODO: make argument maybe?
    return retValue;
}

void Graph::print() {
    for (const auto& edge : m_edges){
        std::cout << edge->representation() << std::endl;
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
