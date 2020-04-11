//
// Created by benji on 29/03/2020.
//

#include "Functions.h"
#include "XMLParser.h"

namespace fs = std::filesystem;

void calculate_and_plot(const std::string &filename) {
    fs::path path(filename);
    std::ifstream ifile(path);

    XMLParser parser(filename);
    parser.parse();
    parser.generateGraphs();
    fs::path output_directory("output"); //TODO: make argument
    fs::path out_prefix = output_directory / path.stem();
    parser.writeTrough(out_prefix.string() + "_annotated.circ");
//    auto clone_groups = getSelectCloneGroups(parser.getGraphs());
//    writeCloneGroups(clone_groups, out_prefix.string());
//
//    drawCloneGroups(clone_groups, "output/clones");
}

void calculate_and_plot_directory(const fs::path &directory) {
    auto it = fs::directory_iterator(directory);
    auto entry = *it;
    std::ifstream ifile(entry.path());

    XMLParser parser(entry.path().string());
    parser.parse();


    ++it;
    for(;it != fs::end(it); ++it){
        entry = *it;
        parser.addNewFile(entry.path().string());
        parser.parse();
        auto new_path = entry.path();
        new_path.replace_filename(entry.path().stem() / "_annotated.circ");
        parser.writeTrough(new_path.string());
    }

    parser.generateGraphs();
    fs::path output_directory("output"); //TODO: make argument
    fs::path out_prefix = output_directory / "circuits";

    generateClones(parser.getGraphs());

//    auto clone_groups = getSelectCloneGroups(parser.getGraphs());
//    writeCloneGroups(clone_groups, out_prefix.string());
//    drawCloneGroups(clone_groups, "output/clones");
}

void writeCloneGroups(const std::vector<std::vector<CandidateClone>> &clone_groups, const std::string &filename) {
    std::ofstream ofile(filename);
    auto group_counter = 0;
    for (const auto & group : clone_groups){
        ofile << "Clone Group " << group_counter << "\n----------------------------------\n\n";
        for(const auto& subGraph : group){
            ofile << "{ circuit: \"" << subGraph.lastEdge()->parent() << "\", file: \"" << subGraph.lastEdge()->file() <<"\"\n";
            for (const auto& edge : subGraph.edges())
                ofile << edge->text(true)+ "\n";
            ofile << "}\n\n";
        }
        group_counter++;
    }
    ofile.close();
}

void drawCloneGroups(const std::vector<std::vector<CandidateClone>> &clone_groups, const std::string &directory) {
    auto group_counter = 0;
    for(const auto & group : clone_groups){
        std::filesystem::path path = std::filesystem::path(directory) / ("group_" + std::to_string(group_counter) + ".dot");
        auto clone = group[0];
        std::ofstream  ofile(path);
        ofile << "digraph clone_" << group_counter << "{\n";
        auto nodes = clone.nodes();
        //TODO: generate dot file for clone, first all nodes + labels, then all edges
        for(const auto& node : nodes){
            ofile <<"\t\"" << node->getName() << "\"[label=\"" << node->getType() << "\"];\n";
        }

        for(const auto& edge : clone.edges()){
            ofile << "\t" << edge->dot() << "[label=\"(" << edge->from().second << "/" << edge->to().second <<")\"];\n";
        }
        ofile << "}";
        group_counter++;
    }
}
