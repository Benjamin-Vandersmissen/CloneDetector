#include <iostream>
#include "XMLParser.h"
#include <fstream>

int main() {

    //TODO: - testing!!!

    XMLParser parser("./testing/circuits/canonical.circ");
    parser.parse();
    parser.generateGraphs();
    auto graphs = parser.getGraphs();
    std::ofstream file("./test.dot");
    auto names = std::vector<std::string>{};
    for(const auto& circuit : parser.getCircuits()){
        names.push_back(circuit.getName());
    }
    plot(file, graphs, names);
    file.close();
    graphs[0]->findClones();

    std::ofstream file2("./test2.dot");
    plot_clones(file2, graphs, names, false);

    parser.getAnnotatedClones("./output/test.circ", "./output/clones.txt");
    return 0;
}