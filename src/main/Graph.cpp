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

void generateClones(std::vector<Graph*> graphs) {
    Graph totalGraph;
    for(auto graph : graphs){
        totalGraph.merge(*graph);
    }
    totalGraph.findClones();
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
    std::set<CandidateClone> candidates;
    for(const auto& edge: m_edges){
        candidates.insert(CandidateClone(edge));
    }
    findClonedEdges(candidates);

    auto i = 0;
    for(const auto& candidate: candidates){
        i++;
        discover(candidate);
        std::cout <<"Candidate" << i << std::endl;
        std::cout << this->m_candidate_count << std::endl;
    }
    std::cout << "GENERATED CANDIDATES: " << m_candidate_count << std::endl;
    std::cout << "UNIQUE CANDIDATES: " << m_temp_candidates.size() << std::endl;

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

void Graph::findClonedEdges(std::set<CandidateClone> &candidates) {
/**
 * Given a set of candidates containing a single edge, calculate the canonical label for each candidate,
 * populate m_cloned_edges and m_cloneGroups[0]
 * */

    std::map<std::string, std::set<CandidateClone> > cloned_edges_map;

    for(const auto& candidate : candidates){
        auto temp = const_cast<CandidateClone&>(candidate);
        auto label = temp.representation();
        cloned_edges_map[label].insert(candidate);
    }
    candidates.clear();
    for(const auto& pair : cloned_edges_map){
        if (pair.second.size() > 1){
            candidates.insert(pair.second.begin(), pair.second.end());
            m_cloneGroups[1].insert(pair);
        }
    }
    m_cloned_edges = candidates;
    this->m_candidate_count = candidates.size();
    this->m_temp_candidates = candidates;
}

void Graph::discover(const CandidateClone &fragment) {

    auto temp = const_cast<CandidateClone&>(fragment);
    auto clone_group = m_cloneGroups[fragment.edges().size()][temp.representation()];  // get Clonegroup(fragment)

    std::set<CandidateClone> candidates;

//    std::cout << "Fragment: ";
//    for(auto edge : fragment.edges())
//        std::cout << "edge_" << std::distance(m_cloned_edges.begin(), std::find(m_cloned_edges.begin(), m_cloned_edges.end(), CandidateClone(edge))) << " - ";
//    std::cout << "\n";
    std::set<CandidateClone> generated_from_fragment;
    for(const auto& other : clone_group){
        for(const auto& extension : m_cloned_edges) {
            if (extension.circuit() < other.circuit()) // ordered on circuit(), so lower -> skip, higher -> break
                continue;

            else if (extension.circuit() > other.circuit())
                break;

            if (other.canMerge(extension)) {
                auto merged = CandidateClone::merge(other, extension);
                if (other == fragment && merged.lastEdge() == extension.lastEdge())
                    generated_from_fragment.insert(merged);
                this->m_candidate_count++;
                candidates.insert(merged);
                m_temp_candidates.insert(merged);
                if (this->m_candidate_count%1000 == 0) std::cout << this->m_candidate_count <<" " << this->m_temp_candidates.size() <<  std::endl;
            }
        }
    }
//    for(auto candidate : candidates){
//        for(auto edge : candidate.edges())
//            std::cout << "edge_" << std::distance(m_cloned_edges.begin(), std::find(m_cloned_edges.begin(), m_cloned_edges.end(), CandidateClone(edge))) << " - ";;
//        std::cout << "\n";
//    }
    std::map<std::string, std::set<CandidateClone>> temp_clone_group;
    for(const auto& candidate : candidates){
        auto temp = const_cast<CandidateClone&>(candidate);
        bool overlapping = false;
        for(const auto& temp_candidate : temp_clone_group[temp.representation()]) {
            overlapping |= overlap(temp_candidate, candidate);
        }
        if (!overlapping)
            temp_clone_group[temp.representation()].insert(candidate);
    }

    auto it = temp_clone_group.begin();
    candidates.clear();
    while(it != temp_clone_group.end()) { //remove candidates with only one clone.
        if ((*it).second.size() == 1)
            it = temp_clone_group.erase(it);
        else {
            auto temp = it->second;
            candidates.merge(temp);
            ++it;
        }
    }


    m_cloneGroups[fragment.edges().size()+1].insert(temp_clone_group.begin(), temp_clone_group.end());

	generated_from_fragment = intersection(generated_from_fragment, candidates);
    for(const auto& new_fragment : generated_from_fragment)
        discover(new_fragment);
//    std::cout << "<-\n";
}

std::vector<std::vector<CandidateClone>> Graph::getAllCloneGroups() const {
    std::vector<std::vector<CandidateClone>> clones;
    for(const auto& pair : m_cloneGroups){ // all clone groups with same iteration
        for(const auto& group : pair.second){ // a clone group
            auto temp = std::vector<CandidateClone>(group.second.begin(), group.second.end());
            clones.push_back(temp);
        }
    }

    auto it = clones.begin();
    while(it != clones.end()){
        bool deleted = false;
        auto it2 = clones.rbegin();
        while(it2 != clones.rend()){  //loop from back to front, because the last clone groups have the most edges, and are most likely to cover the other group
            if (it2->empty()) {
                clones.erase(it2.base());
                it2++;
                continue;
            }
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

//void Graph::print() {
//    for (const auto& edge : m_edges){
//        std::cout << edge->representation() << std::endl;
//    }
//}

//Graph::Graph(std::string name, std::string file) : m_name(std::move(name)), m_file(std::move(file)){}
//
//const std::string &Graph::name() const {
//    return m_name;
//}
