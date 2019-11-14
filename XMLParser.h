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
#include "tinyXML/tinyxml2.h"
#include "Node.h"
#include "Utility.h"

class Component;

class Wire{
private:
    Coordinate m_from;

    Coordinate m_to;

public:
    friend class Component;

    Wire(const std::string &from, const std::string &to);

    bool canConnectTo(const Wire& wire) const;

    bool canOutputTo(const Component& component) const;

    int connectedPort(const Component& component) const;
};


//TODO: create subclasses for easier manipulation?
class Component{
private:
    std::string m_name;

    int m_lib;

    Coordinate m_loc;

    std::map<std::string, std::string> m_attributes;

    Coordinate m_out;

    std::vector<Coordinate > m_in;
public:
    friend class Wire;

    friend class Node;

    Component(int lib, const std::string &name, const std::string &loc);

    void addAttribute(const std::string& name, const std::string& val);

    void calculatePorts();

    bool canOutputTo(const Wire& wire) const;

    bool canOutputTo(const Component& component) const;

    const std::string& name() const;

    int connectedPort(const Component& component) const;

    int indexOfPort(const Coordinate& coordinate) const;
};

class Circuit{
private:
    std::vector<Wire> m_wires;

    std::vector<Component> m_components;

    std::string m_name;

    std::map<Component*, Node*> m_component_map;

public:
    Circuit(const std::string& name);

    void addWire(const std::string& from, const std::string& to);

    void addComponent(int lib, const std::string& name, const std::string& loc);

    Component& lastComponent();

    void generateGraph();

    std::vector<Node*> getForestRepresentation() const;
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
};


#endif //CLONEDETECTOR_XMLPARSER_H
