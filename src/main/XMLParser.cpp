//
// Created by benji on 04/11/2019.
//

#include "XMLParser.h"
#include "Circuit.h"
#include "Graph.h"

#include <utility>
#include <fstream>

XMLParser::XMLParser(const std::string &filename){
    Component::counter = {};
    addNewFile(filename);
}

void XMLParser::parse() {
    parseLibraries();
    parseCircuits();
}

void XMLParser::parseLibraries() {
    auto lib_elem = m_root_elem->FirstChildElement("lib");
    while(lib_elem != nullptr){
        auto library_name = lib_elem->Attribute("desc");
        auto library_id = lib_elem->IntAttribute("name");
        if (print_while_parsing) std::cout << library_name << "  " << library_id << std::endl;
        lib_elem = lib_elem->NextSiblingElement("lib");
    }
}

void XMLParser::parseCircuits() {
    auto circuit_elem = m_root_elem->FirstChildElement("circuit");
    while(circuit_elem != nullptr){
        auto circuit_name = circuit_elem->Attribute("name");
        if (print_while_parsing) std::cout << "CIRCUIT: " << circuit_name << std::endl;
        m_circuits.emplace_back(circuit_name, m_filename);
        parseWires(circuit_elem);
        parseComponents(circuit_elem);
        circuit_elem = circuit_elem->NextSiblingElement("circuit");
        m_circuits.back().calculatePorts();
    }
}

void XMLParser::parseWires(tinyxml2::XMLElement *circuit_root_elem) {
    auto wire_elem = circuit_root_elem->FirstChildElement("wire");
    while(wire_elem != nullptr){
        auto from = wire_elem->Attribute("from");
        auto to = wire_elem->Attribute("to");
        m_circuits.back().addWire(from, to);
        if (print_while_parsing) std::cout << "\t wire from " << from << " to " << to << std::endl;
        wire_elem = wire_elem->NextSiblingElement("wire");
    }
}

void XMLParser::parseComponents(tinyxml2::XMLElement *circuit_root_elem) {
    auto comp_elem = circuit_root_elem->FirstChildElement("comp");
    while(comp_elem != nullptr){
        auto loc = comp_elem->Attribute("loc");
        int lib = comp_elem->IntAttribute("lib", -1);
        auto name = comp_elem->Attribute("name");
        auto success = m_circuits.back().addComponent(lib, name, loc);

        if (success){  // Add the unique label to the XML, used for getAnnotatedClones
            auto unique_name = m_circuits.back().lastComponent().uniqueName();
            auto new_element = m_document.NewElement("a");
            new_element->SetAttribute("name", "label");
            new_element->SetAttribute("val", unique_name.c_str());
            comp_elem->InsertEndChild(new_element);

            if (print_while_parsing) std::cout << "\t component of library " << lib << " named " << name << " at " << loc << std::endl;
            parseAttributes(comp_elem);
        }

        comp_elem = comp_elem->NextSiblingElement("comp");
    }
}

void XMLParser::parseAttributes(tinyxml2::XMLElement *comp_root_elem) {
    auto attr_elem = comp_root_elem->FirstChildElement("a");
    while(attr_elem != nullptr){
        auto name = attr_elem->Attribute("name");
        auto value = attr_elem->Attribute("val");
        if (print_while_parsing) std::cout << "\t \t " << name << " : " << value << std::endl;
        m_circuits.back().lastComponent().addAttribute(name, value);
        attr_elem = attr_elem->NextSiblingElement("a");
    }
}

void XMLParser::generateGraphs() {
    for (auto& circuit : this->m_circuits){
        circuit.generateGraph();
    }
}

std::vector<Graph*> XMLParser::getGraphs() const {
    std::vector<Graph*> graphs;
    for(const auto& circuit: m_circuits)
        graphs.push_back(circuit.getGraphRepresentation());
    return graphs;
}

const std::vector<Circuit> &XMLParser::getCircuits() const {
    return m_circuits;
}

void XMLParser::writeTrough(const std::string &filename){
    m_document.SaveFile(filename.c_str());
}


void XMLParser::addNewFile(const std::string &filename) {
    m_filename = filename;
    m_document.LoadFile(filename.c_str());
    m_root_elem = m_document.FirstChildElement("project");
}