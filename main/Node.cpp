//
// Created by benji on 05/11/2019.
//

#include "Node.h"
#include "XMLParser.h"

#include <utility>

Node::Node(const component_ptr &component) {
    m_component = component;
}

const std::string &Node::getName() const {
    return m_component->uniqueName();
}

component_ptr Node::component() const {
    return m_component;
}

const std::string &Node::getType() const {
    return m_component->name();
}


void plot(std::ostream &stream, const std::vector<Graph*> &graphs, std::vector<std::string> names) {
    stream << "digraph Circuit{\n";
    auto name_it = names.begin();
    for (const auto& graph : graphs){
        stream << "subgraph \"cluster_" << *name_it << "\" {\n";
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
        stream << "subgraph \"cluster_" << *name_it << "\" {\n";
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

std::string Edge::text() const {
    std::string retValue = m_from.first->getType() + " -> " + m_to.first->getType() + " (" + std::to_string(m_from.second) + "/";
    if (!m_to.first->component()->interchangeable_inputs())
        retValue += std::to_string(m_to.second);
    return retValue +")";
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

std::vector<SubGraph> Graph::prune(const std::vector<SubGraph>& subs, unsigned iteration) {
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
        for(auto j = static_cast<long>(currentGroups.size())-1; j >= 0; --j){
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

std::vector<std::vector<SubGraph>> Graph::getAllCloneGroups() const {
    std::vector<std::vector<SubGraph>> clones;
    for(const auto& pair : m_cloneGroups){ // all clone groups with same iteration
        for(const auto& group : pair.second){ // a clone group
            clones.push_back(group.second);
        }
    }
    return clones;
}

//TODO: cleanup constructor, maybe also AddEdge?
SubGraph::SubGraph(const edge_ptr &edge) {
    auto from = edge->from().first;
    auto to = edge->to().first;
    if (m_graph.find(from) == m_graph.end()) {
        m_graph[from] = {edge};
    }
    else{
        auto& vec = m_graph[from];
        bool inserted = false;
        for(auto it = vec.begin(); it != vec.end(); ++it) {
            const auto& edge1 = *it;
            if (edge1->text() > edge->text()) {
                vec.insert(it, edge);
                inserted = true;
                break;
            }
        }
        if(!inserted) vec.push_back(edge);
    }
    if (!contains(m_outgoing_nodes, from) and !contains(m_incoming_nodes, from)) {
        m_outgoing_nodes.push_back(from);
    }
    if (contains(m_outgoing_nodes, to)) {
        m_outgoing_nodes.erase(std::find(m_outgoing_nodes.begin(), m_outgoing_nodes.end(), to));
    }
    m_incoming_nodes.push_back(to);

    if (m_graph.find(to) == m_graph.end())
        m_graph[to] = {};
    m_edges.push_back(edge);
    if (!contains(m_nodes, from)) {
        m_nodes.push_back(from);
    }
    if (!contains(m_nodes, to)) {
        m_nodes.push_back(to);
    }

}

bool SubGraph::canConnect(const edge_ptr &edge) const{
    return (contains(m_nodes, edge->from().first) || contains(m_nodes, edge->to().first)) and not(contains(m_edges, edge));
}

void SubGraph::addEdge(const edge_ptr &edge) {
    auto from = edge->from().first;
    auto to = edge->to().first;
    if (m_graph.find(from) == m_graph.end()) {
        m_graph[from] = {edge};
    }
    else{
        auto& vec = m_graph[from];
        bool inserted = false;
        for(auto it = vec.begin(); it != vec.end(); ++it) {
            const auto& edge1 = *it;
            if (edge1->text() > edge->text()) {
                vec.insert(it, edge);
                inserted = true;
                break;
            }
        }
        if(!inserted) vec.push_back(edge);
    }
    if (!contains(m_outgoing_nodes, from) and !contains(m_incoming_nodes, from)) {
        m_outgoing_nodes.push_back(from);
    }
    if (contains(m_outgoing_nodes, to)) {
        m_outgoing_nodes.erase(std::find(m_outgoing_nodes.begin(), m_outgoing_nodes.end(), to));
    }
    m_incoming_nodes.push_back(to);

    if (m_graph.find(to) == m_graph.end())
        m_graph[to] = {};
    m_edges.push_back(edge);
    if (!contains(m_nodes, from)) {
        m_nodes.push_back(from);
    }
    if (!contains(m_nodes, to)) {
        m_nodes.push_back(to);
    }
}

// Assume no loops in the graph
std::string SubGraph::representation_dfs(const node_ptr &node, std::map<node_ptr, bool> &visited) const{
    std::string representation;
    std::vector<std::string> representations;
    for(const auto& edge : m_graph.at(node)){
        representations.push_back(edge->text() + "  " + representation_dfs(edge->to().first, visited));
    }
    std::sort(representations.begin(), representations.end());
    for(const auto& s : representations){
        representation += s + "  ";
    }
    return representation + ">";
}


void SubGraph::longest_path_dfs(const node_ptr& node, std::map<node_ptr, unsigned int> &paths) const{
    for(const auto& edge : m_graph.at(node)){
        longest_path_dfs(edge->to().first, paths);
    }
    if (m_graph.at(node).empty())
        paths[node] = 0;
    else{
        unsigned max=0;
        for(const auto& edge : m_graph.at(node))
            max = std::max(max, paths[edge->to().first]);
        paths[node] = max+1;
    };
}

std::string SubGraph::representation() const {
    std::map<node_ptr, unsigned> longest_paths;
    for(const auto& node : m_outgoing_nodes){
        if (longest_paths.find(node) == longest_paths.end()) // longest path not yet calculated for this node
            longest_path_dfs(node, longest_paths);
    }
    std::map<node_ptr, bool> visited;

    std::vector<std::pair<node_ptr, unsigned > > longest_paths_sorted;
    longest_paths_sorted.reserve(longest_paths.size());
    for(auto & longest_path : longest_paths) longest_paths_sorted.emplace_back(longest_path);

    std::sort(longest_paths_sorted.begin(), longest_paths_sorted.end(), [this](auto& left, auto& right){ //https://stackoverflow.com/questions/279854/how-do-i-sort-a-vector-of-pairs-based-on-the-second-element-of-the-pair
        return left.second > right.second;
    });

    std::vector<std::string> representations;
    for(const auto& pair : longest_paths_sorted){
        if (contains(m_outgoing_nodes, pair.first))
            representations.push_back(representation_dfs(pair.first, visited));
    }
    std::string representation;
    std::sort(representations.begin(), representations.end());
    for(const auto& s : representations){
        representation += s + "  ";
    }
    return representation ;
}

bool SubGraph::canMerge(const SubGraph &sg) const{
    if (this->edges().size() == 1 and sg.edges().size() == 1)
        return canConnect(sg.edges()[0]);
    return this->edges().size() == sg.edges().size() and intersection(this->edges(), sg.edges()).size() == this->edges().size()-1;
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

std::vector<std::vector<SubGraph>> getSelectCloneGroups(const std::vector<Graph *> &graphs) {
    auto clone_groups = getCloneGroups(graphs);
    std::vector<std::vector<SubGraph>> retValue;
    for (auto group : clone_groups){
        if(coveredNodes(group.front()) > 7) // test only front of group, because all other clones in group are identical in form
            retValue.push_back(group);
    }
    return retValue;
}

void Graph::print() {
    for (const auto& edge : m_edges){
        std::cout << edge->from().first->getName() << " " << edge->from().second << " => " << edge->to().first->getName() << " " << edge->to().second << std::endl;
    }
}

unsigned coveredNodes(const SubGraph &sg) {
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
