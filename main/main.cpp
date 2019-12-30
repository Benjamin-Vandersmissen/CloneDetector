#include <iostream>
#include "XMLParser.h"
#include <fstream>

int main() {

    XMLParser parser("./test/test.circ");
    parser.parse();
    parser.generateGraphs();
    auto forests = parser.getForests();
    std::ofstream file("./test.dot");
    plot(file, forests);
    file.close();
    return 0;
}