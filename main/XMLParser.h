//
// Created by benji on 04/11/2019.
//

#ifndef CLONEDETECTOR_XMLPARSER_H
#define CLONEDETECTOR_XMLPARSER_H


#include <string>
#include <map>
#include <iostream>
#include <vector>
#include <algorithm>
#include "tinyxml2.h"
#include "Node.h"

class Circuit{
private:
    std::vector<Wire> m_wires;

    std::vector<Component*> m_components;

    std::string m_name;

    std::map<Component*, Node*> m_component_map;

    std::vector<Component*> m_inputs;

    std::vector<Component*> m_outputs;

public:
    Circuit(const std::string& name);

    void addWire(const std::string& from, const std::string& to);

    void addComponent(int lib, const std::string& name, const std::string& loc);

    Component& lastComponent();

    void generateGraph();

    std::vector<Node*> getForestRepresentation() const;

    const std::vector<Component *> &getComponents() const;

    void calculatePorts();
};

class XMLParser {
private:
    tinyxml2::XMLDocument m_document;  // Needs to be kept in memory

    tinyxml2::XMLElement* m_root_elem;

    std::vector<Circuit> m_circuits;

    void parseLibraries();

    void parseCircuits();

    void parseWires(tinyxml2::XMLElement* circuit_root_elem);

    void parseComponents(tinyxml2::XMLElement* circuit_root_elem);

    void parseAttributes(tinyxml2::XMLElement* comp_root_elem);
public:
    XMLParser(const std::string& filename);

    void parse();

    void generateGraphs();

    std::vector<std::vector<Node*>> getForests() const;

    const std::vector<Circuit> &getCircuits() const;
};


#endif //CLONEDETECTOR_XMLPARSER_H
