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

Wire::Wire(const std::string &from, const std::string &to) {
    unsigned from_x = std::stoi(from.substr(1, from.find(',') - 1));
    unsigned from_y = std::stoi(from.substr(from.find(',') + 1, from.size() - from.find(',')));

    unsigned to_x = std::stoi(to.substr(1, to.find(',') - 1));
    unsigned to_y = std::stoi(to.substr(to.find(',') + 1, to.size() - to.find(',')));

    m_from = makeCoordinate(from_x, from_y);
    m_to = makeCoordinate(to_x, to_y);
}

bool Wire::canConnectTo(const Wire &wire)  const{
    return wire.m_from == this->m_from or wire.m_from == this->m_to or wire.m_to == this->m_from or wire.m_to == this->m_to;
}

bool Wire::canOutputTo(const Component &component) const {
    bool result = false;
    result |= contains(component.m_in, m_to);
    result |= contains(component.m_in, m_from);
    return result;
}

int Wire::connectedPort(const Component &component) const {
    if (contains(component.m_in, m_to))  // TODO: this if statement is not very correct, because m_in is private
        return component.indexOfPort(m_to);
    else
        return component.indexOfPort(m_from);
}

Component::Component(int lib, const std::string &name, const std::string &loc) : m_lib(lib), m_name(name){
    // extract the coordinates from the string
    unsigned loc_x = std::stoi(loc.substr(1, loc.find(',') - 1));
    unsigned loc_y = std::stoi(loc.substr(loc.find(',') + 1, loc.size() - loc.find(',')));

    m_loc = makeCoordinate(loc_x, loc_y);
    m_out = m_loc;  // alias for convenience

}

void Component::addAttribute(const std::string &name, const std::string &val) {
    m_attributes[name] = val;
}

void Component::calculatePorts() {  //TODO: rework maybe and remove some more edge cases e.d.
    if (m_lib == 0 && m_name == "Constant")
        return; // No incoming ports

    auto size = 50; // default size for most gates

    if(m_lib == 1 && m_name == "NOT Gate")
        size = 30; // default size for not gate

    if (m_attributes.find("size") != m_attributes.end()){
        size = std::stoi(m_attributes["size"]);
    }

    enum directions{
        EAST,  // default value
        SOUTH,
        WEST,
        NORTH
    };
    auto facing = directions::EAST;
    if (m_attributes.find("facing") != m_attributes.end()) {
        if (m_attributes["facing"] == "north") {
            facing = directions::NORTH;
        } else if (m_attributes["facing"] == "south") {
            facing = directions::SOUTH;
        } else if (m_attributes["facing"] == "west") {
            facing = directions::WEST;
        }
    }
    auto nr_inputs = 5;
    if(m_lib == 1 && m_name == "NOT Gate")
        nr_inputs = 1; // not gate has always one input
    if (m_attributes.find("inputs") != m_attributes.end()){
        nr_inputs = std::stoi(m_attributes["inputs"]);
    }

    auto relative_center = m_out;  // the relative center around which the inputs are located

    bool inconsistent_size = false;
    if (m_lib == 1 && (m_name == "NAND Gate" or m_name == "NOR Gate")) // actual size is 10 higher than given size
        inconsistent_size = true;

    switch(facing){
        case directions ::EAST:
            relative_center.first -= size + 10 * inconsistent_size;
            break;
        case directions ::SOUTH:
            relative_center.second -= size + 10 * inconsistent_size;
            break;
        case directions ::WEST:
            relative_center.first += size + 10 * inconsistent_size;
            break;
        case directions ::NORTH:
            relative_center.second += size + 10* inconsistent_size;
            break;
    }
    if(nr_inputs == 1){  //exactly one input, in it's relative center
        m_in.push_back(relative_center);
        return;
    }
    auto step = (nr_inputs > 3 ? 10 : 20);  // step between ports is 20 if 3 or less ports
    auto offset = (nr_inputs > size/10? nr_inputs/2*10 : (size/20)*10);  // offset is nr_inputs/2*10 or size/20 * 10

    if (nr_inputs % 2 == 1){  // uneven ports
        auto input = relative_center;
        if (facing == directions::NORTH or facing == directions::SOUTH) {
            input.first -= offset;
            for (auto i = 0; i < nr_inputs; ++i) {
                m_in.push_back(input);
                input.first += step;
            }
        }
        else {
            input.second -= offset;
            for (auto i = 0; i < nr_inputs; ++i) {
                m_in.push_back(input);
                input.second += step;
            }
        }
    }
    else{  // even ports, skip center
        auto input = relative_center;
        if (facing == directions::NORTH or facing == directions::SOUTH) {
            input.first -= offset;
            for (auto i = 0; i <= nr_inputs; ++i) {
                if (input.first == relative_center.first) {
                    input.first += step;
                    continue;
                }
                m_in.push_back(input);
                input.first += step;
            }
        }
        else{
            input.second -= offset;
            for (auto i = 0; i <= nr_inputs; ++i) {
                if (input.second == relative_center.second) {
                    input.second += step;
                    continue;
                }
                m_in.push_back(input);
                input.second += step;
            }
        }
    }
    std::cout << m_name << std::endl;
    for (auto i = 0; i < m_in.size(); ++i){
        auto port = m_in[i];
        std::cout << "\tPort " << i << " : (" << port.first << ", " << port.second << ")" << std::endl;
    }
}

bool Component::canOutputTo(const Wire &wire) const {
    return m_out == wire.m_from || m_out == wire.m_to;
}

bool Component::canOutputTo(const Component &component) const{
    return contains(component.m_in, m_out);
}

const std::string &Component::name() const {
    return m_name;
}

int Component::connectedPort(const Component &component) const {
    return component.indexOfPort(m_out);
}

int Component::indexOfPort(const Coordinate &coordinate) const {
    return index(m_in, coordinate);
}

Circuit::Circuit(const std::string &name) : m_name(name){

}

void Circuit::addWire(const std::string &from, const std::string &to) {
    m_wires.emplace_back(from, to);
}

void Circuit::addComponent(int lib, const std::string &name, const std::string &loc) {
    m_components.emplace_back(lib, name, loc);
}

Component &Circuit::lastComponent() {
    return m_components.back();
}

void Circuit::generateGraph() {
    for(auto& component : m_components){
        component.calculatePorts();
        m_component_map[&component] = new Node(component.name());
    }
    for (size_t i = 0; i < m_components.size(); ++i){
        auto& component = m_components[i];
        std::vector<Wire*> wires;
        for(auto& wire : m_wires){
            if (component.canOutputTo(wire)){
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
                if (component.canOutputTo(other_component)){
                    auto port = component.connectedPort(other_component);
                    m_component_map[&component]->addOutGoingConnection(m_component_map[&other_component], port);
                }
            }
        }
        else{  // find all components whose inputs are connected to this component's output
            for(size_t j = 0; j < m_components.size(); ++j){
                auto& other_component = m_components[j];
                for(auto& wire : wires){
                    if (wire->canOutputTo(other_component)){
                        auto port = wire->connectedPort(other_component);
                        m_component_map[&component]->addOutGoingConnection(m_component_map[&other_component], port);
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
