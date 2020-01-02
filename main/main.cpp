#include <iostream>
#include "XMLParser.h"
#include <fstream>

int main() {

    XMLParser parser("./test/test.circ");
    parser.parse();
    parser.generateGraphs();
    auto forests = parser.getForests();
    std::ofstream file("./test.dot");
    auto names = std::vector<std::string>{};
    for(auto circuit : parser.getCircuits()){
        names.push_back(circuit.getName());
    }
    plot(file, forests, names);
    file.close();
    return 0;
}