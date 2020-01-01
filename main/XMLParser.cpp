//
// Created by benji on 04/11/2019.
//

#include "XMLParser.h"

XMLParser::XMLParser(const std::string &filename) {
    m_document.LoadFile(filename.c_str());
    m_root_elem = m_document.FirstChildElement("project");
}

void XMLParser::parse() {
    parseLibraries();
    parseCircuits();
}

void XMLParser::parseLibraries() {
    auto lib_elem = m_root_elem->FirstChildElement("lib");
    while(lib_elem != NULL){
        auto library_name = lib_elem->Attribute("desc");
        auto library_id = lib_elem->IntAttribute("name");
        std::cout << library_name << "  " << library_id << std::endl;
        lib_elem = lib_elem->NextSiblingElement("lib");
    }
}

void XMLParser::parseCircuits() {
    auto circuit_elem = m_root_elem->FirstChildElement("circuit");
    while(circuit_elem != NULL){
        auto circuit_name = circuit_elem->Attribute("name");
        std::cout << "CIRCUIT: " << circuit_name << std::endl;
        m_circuits.emplace_back(circuit_name);
        parseWires(circuit_elem);
        parseComponents(circuit_elem);
        circuit_elem = circuit_elem->NextSiblingElement("circuit");
        m_circuits.back().calculatePorts();
    }
}

void XMLParser::parseWires(tinyxml2::XMLElement *circuit_root_elem) {
    auto wire_elem = circuit_root_elem->FirstChildElement("wire");
    while(wire_elem != NULL){
        auto from = wire_elem->Attribute("from");
        auto to = wire_elem->Attribute("to");
        m_circuits.back().addWire(from, to);
        std::cout << "\t wire from " << from << " to " << to << std::endl;
        wire_elem = wire_elem->NextSiblingElement("wire");
    }
}

void XMLParser::parseComponents(tinyxml2::XMLElement *circuit_root_elem) {
    auto comp_elem = circuit_root_elem->FirstChildElement("comp");
    while(comp_elem != NULL){
        auto loc = comp_elem->Attribute("loc");
        int lib = comp_elem->IntAttribute("lib", -1);
        auto name = comp_elem->Attribute("name");
        m_circuits.back().addComponent(lib, name, loc);
        std::cout << "\t component of library " << lib << " named " << name << " at " << loc << std::endl;
        parseAttributes(comp_elem);
        comp_elem = comp_elem->NextSiblingElement("comp");
    }
}

void XMLParser::parseAttributes(tinyxml2::XMLElement *comp_root_elem) {
    auto attr_elem = comp_root_elem->FirstChildElement("a");
    while(attr_elem != NULL){
        auto name = attr_elem->Attribute("name");
        auto value = attr_elem->Attribute("val");
        std::cout << "\t \t " << name << " : " << value << std::endl;
        m_circuits.back().lastComponent().addAttribute(name, value);
        attr_elem = attr_elem->NextSiblingElement("a");
    }
}

void XMLParser::generateGraphs() {
    for (auto& circuit : this->m_circuits){
        circuit.generateGraph();
    }
}

std::vector<std::vector<Node *>> XMLParser::getForests() const {
    std::vector<std::vector<Node*>> forests;
    for(auto& circuit : m_circuits){
        forests.push_back(circuit.getForestRepresentation());
    }
    return forests;
}

const std::vector<Circuit> &XMLParser::getCircuits() const {
    return m_circuits;
}

Circuit::Circuit(const std::string &name) : m_name(name){

}

void Circuit::addWire(const std::string &from, const std::string &to) {
    m_wires.emplace_back(from, to);
}

void Circuit::addComponent(int lib, const std::string &name, const std::string &loc) {
    m_components.emplace_back(createComponent(lib, name, loc));
}

Component &Circuit::lastComponent() {
    return *m_components.back();
}

void Circuit::generateGraph() {
    for(auto& component : m_components){
        component->calculatePorts();
        m_component_map[component] = new Node(component->name());
    }
    for (size_t i = 0; i < m_components.size(); ++i){
        auto& component = m_components[i];
        std::vector<Wire*> wires;
        for(auto& wire : m_wires){
            if (component->canOutputTo(wire)){
                wires.push_back(&wire);
            }
        }
        auto previous_size = 0;
        while(wires.size() > previous_size){  // find all wires directly connected to this output
            previous_size = wires.size();
            for(auto& wire1 : m_wires){
                for(auto wire2 : wires){
                    if (wire2->canConnectTo(wire1) and std::find(wires.begin(), wires.end(), &wire1) == wires.end()){ // Add a new wire
                        wires.push_back(&wire1);
                    }
                }
            }
        }
        if (wires.empty()){ // A component could be directly connected
            for(size_t j = 0; j < m_components.size(); ++j){
                auto& other_component = m_components[j];
                if (component->canOutputTo(other_component)){
                    auto port = component->connectedPort(other_component);
                    m_component_map[component]->addOutGoingConnection(m_component_map[other_component], port);
                }
            }
        }
        else{  // find all components whose inputs are connected to this component's output
            for(size_t j = 0; j < m_components.size(); ++j){
                auto& other_component = m_components[j];
                for(auto& wire : wires){
                    if (wire->canOutputTo(other_component)){
                        auto port = wire->connectedPort(other_component);
                        m_component_map[component]->addOutGoingConnection(m_component_map[other_component], port);
                    }
                }
            }
        }
    }
}

std::vector<Node *> Circuit::getForestRepresentation() const {
    std::vector<Node*> forest;
    for (auto& pair : this->m_component_map){
        forest.push_back(pair.second);
    }
    return forest;
}

const std::vector<Component *> &Circuit::getComponents() const {
    return m_components;
}

void Circuit::calculatePorts() {
    for(auto component : m_components){
        if (component->name() == "Pin"  and component->m_lib == 0){
            if(component->m_attributes.find("output") != component->m_attributes.end())
                m_outputs.push_back(component);
            else
                m_inputs.push_back(component);

        }
    }
    std::sort(m_inputs.begin(), m_inputs.end(), sortPorts);
    std::sort(m_outputs.begin(), m_outputs.end(), sortPorts);
}
