#include <iostream>
#include "XMLParser.h"
#include <fstream>

int main() {

    //TODO: - testing!!!
    //TODO: - comments in Node.h
    //TODO: - detect clones in vector of Graphs (easiest way is to convert it to 1 graph and keep track in which

    XMLParser parser("./test/test.circ");
    parser.parse();
    parser.generateGraphs();
    auto graphs = parser.getGraphs();
    std::ofstream file("./test.dot");
    auto names = std::vector<std::string>{};
    for(auto circuit : parser.getCircuits()){
        names.push_back(circuit.getName());
    }
    plot(file, graphs, names);
    file.close();
    graphs[0]->findClones();

    std::ofstream file2("./test2.dot");
    plot_clones(file2, graphs, names);
    return 0;
}