//
// Created by benji on 05/11/2019.
//

#include "Node.h"
#include "XMLParser.h"

#include <utility>

Node::Node(const component_ptr &component) {
    m_name = component->uniqueName();
    m_component = component;
}

const std::string &Node::getName() const {
    return m_name;
}

component_ptr Node::component() const {
    return m_component;
}


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

//TODO: fix for overlapping clones
void
plot_clones(std::ostream &stream, const std::vector<Graph *> &graphs, std::vector<std::string> names, bool onlyClones) {
    stream << "digraph Circuit{\n";
    auto name_it = names.begin();
    auto clone_groups = getCloneGroups(graphs);

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
        stream << "subgraph cluster_" << *name_it << " {\n";
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

        stream << "label=\"Circuit: " << *name_it << "\"\n";
        std::advance(name_it, 1);
        stream << "}\n";
    }
    stream << "}";
}

Edge::Edge(const node_ptr& from, unsigned outport, const node_ptr &to, unsigned inport): m_from{from, outport}, m_to{to, inport} {}

const std::pair<node_ptr, unsigned int> & Edge::from() const {
    return m_from;
}

const std::pair<node_ptr, unsigned int> & Edge::to() const {
    return m_to;
}

void Graph::addNode(const node_ptr& node) {
    if (! contains(m_nodes, node))
        m_nodes.push_back(node);
}

void Graph::addEdge(const edge_ptr &edge) {
    if (contains(m_nodes, edge->from().first) and contains(m_nodes, edge->to().first))
        m_edges.push_back(edge);
}

const std::vector<node_ptr> & Graph::nodes() const {
    return m_nodes;
}

const std::vector<edge_ptr> & Graph::edges() const {
    return m_edges;
}


//based on algorithm eScan from https://www.researchgate.net/publication/221553495_Complete_and_accurate_clone_detection_in_graph-based_models
void Graph::findClones() {
    std::vector<SubGraph> subgraphs;
    for(const auto& edge: m_edges){
        subgraphs.emplace_back(edge);
    }
    auto iteration = 0;
    while(true) {
        subgraphs = prune(subgraphs, iteration);
        if(subgraphs.empty()) return;   // No potential clone pairs anymore
        subgraphs = extend(subgraphs);
        if (iteration > 0) removeCoveredGroups(iteration);
        iteration ++;
    }
}

std::vector<SubGraph> Graph::prune(const std::vector<SubGraph> &subs, unsigned iteration) {
    std::map<std::string, std::vector<SubGraph>> mapping;
    for(const auto& sub : subs){
        auto representation = sub.representation();
        bool overlapping = false;
        for(const auto& other : mapping[representation])
            overlapping |= overlap(other, sub);
        if (! overlapping)
            mapping[representation].push_back(sub);
    }

    std::vector<SubGraph > retValue;

    for(auto pair : mapping){
        if(pair.second.size() > 1){ // a canonical label has more than 1 subGraph -> clones
            m_cloneGroups[iteration].push_back(pair);
            retValue.insert(retValue.end(), pair.second.begin(), pair.second.end());
        }
    }
    return retValue;
}

std::vector<SubGraph> Graph::extend(const std::vector<SubGraph> &subs) {
    std::vector<SubGraph> retValue;
    for (auto sub : subs){
        for(const auto& edge : m_edges){
            if (sub.canConnect(edge)){
                auto new_sub = sub;
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

    // size(previousGroups) >= size(currentGroups) => if currentGroups[j] doesn't cover previousGroups[i] ( j <= i), no currentGroup covers it
    /*
     * Why? -> ideal case: previousGroups[i] is extended in currentGroups[i] (Every preceding i is extended as well)
     *      -> previousGroups[i] is extended in currentGroups[j], j < i (Some preceding i aren't extended / this group has merged with another group that is already extended in a currentGroup)
     * */

    std::vector<std::size_t > to_delete;

    for(auto i = 0; i  < previousGroups.size(); ++i){
        for(auto j = std::min(i, int(currentGroups.size()-1)); j >= 0; --j){
            auto group1 = previousGroups[i];
            auto group2 = currentGroups[j];
            if(group1.second.size() > group2.second.size()) // All clones from the previous iteration need to be in the current iteration for a full cover
                continue;

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

std::vector<std::vector<SubGraph>> Graph::getAllCloneGroups() const {
    std::vector<std::vector<SubGraph>> clones;
    for(const auto& pair : m_cloneGroups){ // all clone groups with same iteration
        for(const auto& group : pair.second){ // a clone group
            clones.push_back(group.second);
        }
    }
    return clones;
}

SubGraph::SubGraph(const edge_ptr &edge) {
    m_edges = {edge};
    m_nodes = {edge->from().first, edge->to().first};

    remap();
}

bool SubGraph::canConnect(const edge_ptr &edge) {
    return (contains(m_nodes, edge->from().first) || contains(m_nodes, edge->to().first)) and not(contains(m_edges, edge));
}

void SubGraph::addEdge(const edge_ptr &edge) {
    m_edges.push_back(edge);
    if (!contains(m_nodes, edge->from().first)) {
        m_nodes.push_back(edge->from().first);
    } else {
        m_nodes.push_back(edge->to().first);
    }
    remap();
}

std::string SubGraph::representation() const {
    std::string representation;
    for (const auto& edge : m_edges){
        representation+= m_mapping.at(edge->from().first) + " -> " + m_mapping.at(edge->to().first) + " ("
                        + std::to_string(edge->from().second) + "/" + std::to_string(edge->to().second) + ")\n";
    }
    return representation;
}

void SubGraph::remap() {
    std::sort(m_edges.begin(), m_edges.end(), compareEdges);

    m_mapping.clear();
    m_counter.clear();
    for(const auto& edge : m_edges) {
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

bool SubGraph::compareEdges(const edge_ptr &e1, const edge_ptr &e2) {
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

bool overlap(const SubGraph &sg1, const SubGraph &sg2) {
    return ! intersection(sg1.edges(), sg2.edges()).empty();
}

bool covers(const SubGraph &sg, const std::vector<SubGraph> &cover) {
    for (const auto& sg2 : cover){
        if (intersection(sg.edges(), sg2.edges()).size() == sg.edges().size()) return true;
    }

    return false;
}

std::vector<std::vector<SubGraph>> getCloneGroups(const std::vector<Graph*> &graphs) {
    Graph total_graph;
    for(const auto& graph: graphs){
        total_graph.m_edges.insert(total_graph.m_edges.begin(), graph->m_edges.begin(), graph->m_edges.end());
        total_graph.m_nodes.insert(total_graph.m_nodes.begin(), graph->m_nodes.begin(), graph->m_nodes.end());
    }
    total_graph.findClones();
    return total_graph.getAllCloneGroups();
}
