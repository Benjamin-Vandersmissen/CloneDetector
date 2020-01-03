#include <iostream>
#include "XMLParser.h"
#include <fstream>

int main() {

    //TODO: use smart pointers everywhere

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
    return 0;
}