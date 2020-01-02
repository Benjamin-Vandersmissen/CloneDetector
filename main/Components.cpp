//
// Created by benji on 14/11/2019.
//

#include "Components.h"
#include <iostream>

std::map<std::string, std::pair<int, int> > circuit_port_map = {};

Wire::Wire(const std::string &from, const std::string &to) {
    unsigned from_x = std::stoi(from.substr(1, from.find(',') - 1));
    unsigned from_y = std::stoi(from.substr(from.find(',') + 1, from.size() - from.find(',')));

    unsigned to_x = std::stoi(to.substr(1, to.find(',') - 1));
    unsigned to_y = std::stoi(to.substr(to.find(',') + 1, to.size() - to.find(',')));

        m_points = std::vector<Coordinate >{makeCoordinate(from_x, from_y), makeCoordinate(to_x, to_y)};
}

bool Wire::canConnectTo(const Wire &wire)  const{
    return ! intersection(wire.m_points, m_points).empty();
}

bool Wire::canOutputTo(const Component *component) const {
    return ! intersection(component->m_in, m_points).empty();
}

int Wire::connectedPort(const Component *component) const {
    if (contains(component->m_in, m_points[0]))  // TODO: this if statement is not very correct, because m_in is private
        return component->indexOfInPort(m_points[0]);
    else
        return component->indexOfInPort(m_points[1]);
}

Component::Component(int lib, const std::string &name, const std::string &loc) : m_lib(lib), m_name(name){
    // extract the coordinates from the string
    unsigned loc_x = std::stoi(loc.substr(1, loc.find(',') - 1));
    unsigned loc_y = std::stoi(loc.substr(loc.find(',') + 1, loc.size() - loc.find(',')));

    m_loc = makeCoordinate(loc_x, loc_y);
}

void Component::addAttribute(const std::string &name, const std::string &val) {
    m_attributes[name] = val;
}

bool Component::canOutputTo(const Wire &wire, unsigned long outport) const {
    return contains(wire.m_points, m_out[outport]);
}

bool Component::canOutputTo(const Component *component, unsigned outport) const{
    return contains(component->m_in, m_out[outport]);
}

const std::string &Component::name() const {
    return m_name;
}

int Component::connectedInPort(const Component *component, int outport) const {
    auto val = component->indexOfInPort(m_out[outport]);
    if (val != -1) return val;
    return -1;
}

int Component::indexOfInPort(const Coordinate &coordinate) const {
    return index(m_in, coordinate);
}

const std::vector<Coordinate> &Component::getInputPorts() const {
    return m_in;
}

void GateComponent::calculatePorts() {
    auto width = 50;
    auto length = 50; // default length for most gates

    m_out.push_back(m_loc); //one output port

    if (contains(m_attributes, "size")){
        length = std::stoi(m_attributes["size"]);
        width = length;
    }

    if (this->m_name[0] == 'X')
        length += 10;
    if (this->m_name[0] == 'N')
        length += 10;
    if (this->m_name == "XNOR Gate")
        length += 10;

    auto relative_center = m_loc;

    auto facing = directions::EAST;
    if (contains(m_attributes, "facing")) {
        if (m_attributes["facing"] == "north") {
            facing = directions::NORTH;
        } else if (m_attributes["facing"] == "south") {
            facing = directions::SOUTH;
        } else if (m_attributes["facing"] == "west") {
            facing = directions::WEST;
        }
    }

    auto nr_inputs = 5;
    if (contains(m_attributes, "inputs")){
        nr_inputs = std::stoi(m_attributes["inputs"]);
    }

    switch(facing){
        case directions ::EAST:
            relative_center.first -= length;
            break;
        case directions ::SOUTH:
            relative_center.second -= length;
            break;
        case directions ::WEST:
            relative_center.first += length;
            break;
        case directions ::NORTH:
            relative_center.second += length;
            break;
    }

    // step = room between 2 inputs
    // offset : first port is relative_center - offset

    auto step = (nr_inputs > 3 ? 10 : 20);  // step between ports is 20 if 3 or less ports
    auto offset = (nr_inputs > width / 10 ? nr_inputs / 2 * 10 : (width / 20) * 10);  // offset is nr_inputs/2*10 or length/20 * 10

    // Edge cases for formula
    if (width == 30 and nr_inputs == 3){
        step = 10;
    }

    if (width == 70 and (nr_inputs == 2 or nr_inputs == 4)){
        offset = 20;
        step = 20;  // edge case for large width, 4 inputs
    }
    if (width == 70 and nr_inputs == 3){
        step = 30;
    }

    if (width == 70 and nr_inputs == 5){
        offset = 20;
    }

    if (nr_inputs % 2 == 1){  // uneven ports
        auto input = relative_center;
        if (facing == directions::NORTH or facing == directions::SOUTH) { // ports are on x-axis
            input.first -= offset;
            for (auto i = 0; i < nr_inputs; ++i) {
                m_in.push_back(input);
                input.first += step;
            }
        }
        else { // ports are on y-axis
            input.second -= offset;
            for (auto i = 0; i < nr_inputs; ++i) {
                m_in.push_back(input);
                input.second += step;
            }
        }
    }
    else{  // even ports, skip center
        auto input = relative_center;
        if (facing == directions::NORTH or facing == directions::SOUTH) { // ports are on x-axis
            input.first -= offset;
            for (auto i = 0; i < nr_inputs; ++i) {
                if (input.first == relative_center.first) {
                    if (not(width == 70 and nr_inputs == 4)) { // another edge case
                        i -= 1;
                        input.first += step;
                        continue;
                    }
                }
                m_in.push_back(input);
                input.first += step;
            }
        }
        else{ // ports are on y-axis
            input.second -= offset;
            for (auto i = 0; i < nr_inputs; ++i) {
                if (input.second == relative_center.second) {
                    if (not(width == 70 and nr_inputs == 4)) { // another edge case
                        i -= 1;
                        input.second += step;
                        continue;
                    }
                }
                m_in.push_back(input);
                input.second += step;
            }
        }
    }

    for(auto i = 0; i < m_in.size(); ++ i){
        auto& coord = m_in[i];
        if (contains(m_attributes, "negate" + std::to_string(i))){
            switch(facing){
                case directions ::EAST:
                    coord.first -= 10;
                    break;

                case directions ::WEST:
                    coord.first += 10;
                    break;

                case directions ::NORTH:
                    coord.second -= 10;
                    break;

                case directions ::SOUTH:
                    coord.second += 10;
                    break;
            }
        }
    }
}

GateComponent::GateComponent(int lib, const std::string &name, const std::string &loc) : Component(lib, name, loc) {}

Component *createComponent(int lib, const std::string &name, const std::string &loc) {
    if (lib == 1 and (name == "AND Gate" or name == "OR Gate" or name == "NAND Gate" or name == "NOR Gate" or name == "XOR Gate" or name == "XNOR Gate"))
        return new GateComponent(lib, name, loc);

    if (lib == 1 and (name == "NOT Gate"))
        return new NotComponent(lib, name, loc);

    if (lib == 0 and (name == "Pin"))
        return new PinComponent(lib, name, loc);
    if (lib == -1)
        return new CircuitComponent(lib, name, loc);
    return nullptr;
}

NotComponent::NotComponent(int lib, const std::string &name, const std::string &loc) : Component(lib, name, loc) {}

void NotComponent::calculatePorts() {
    auto length = 30; // default length for NOT gates
    if (contains(m_attributes, "size")){
        length = std::stoi(m_attributes["size"]);
    }

    m_out.push_back(m_loc);

    auto relative_center = m_loc;

    auto facing = directions::EAST;
    if (contains(m_attributes, "facing")) {
        if (m_attributes["facing"] == "north") {
            facing = directions::NORTH;
        } else if (m_attributes["facing"] == "south") {
            facing = directions::SOUTH;
        } else if (m_attributes["facing"] == "west") {
            facing = directions::WEST;
        }
    }

    switch(facing){
        case directions ::EAST:
            relative_center.first -= length;
            break;
        case directions ::SOUTH:
            relative_center.second -= length;
            break;
        case directions ::WEST:
            relative_center.first += length;
            break;
        case directions ::NORTH:
            relative_center.second += length;
            break;
    }
    m_in.push_back(relative_center);
}

bool sortPorts(const Component* comp1, const Component* comp2){
    if(comp1->m_loc.second < comp2->m_loc.second)
        return true;
    else if (comp1->m_loc.first < comp2->m_loc.first and comp1->m_loc.second == comp2->m_loc.second)
        return true;
    return false;
}

PinComponent::PinComponent(int lib, const std::string &name, const std::string &loc) : Component(lib, name, loc) {}

void PinComponent::calculatePorts() {
    if (not(contains(m_attributes, "output"))) {  // Input pin has no inputs, output pin has one
        m_out.push_back(m_loc);
        return;
    }

    auto length = 20;
    auto relative_center = m_loc;

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

    switch(facing){
        case directions ::EAST:
            relative_center.first -= length;
            break;
        case directions ::SOUTH:
            relative_center.second -= length;
            break;
        case directions ::WEST:
            relative_center.first += length;
            break;
        case directions ::NORTH:
            relative_center.second += length;
            break;
    }
    m_in.push_back(relative_center);
}

CircuitComponent::CircuitComponent(int lib, const std::string &name, const std::string &loc) : Component(lib, name,
                                                                                                         loc) {}
/**
 *  _____________
 * /             \
 * \             /
 *  \           /
 *   \         /    This works only with the default appearance of Circuits, if one changes the appearance of a circuit, this no longer works
 *    \       /
 *     \     /
 *      \   /
 *       \ /
 *
 *      (  )
 * */
void CircuitComponent::calculatePorts() {
    auto ports = circuit_port_map[m_name];
    auto width = std::max(20, (std::max(ports.first, ports.second)-1)*10);
    auto length = 30;

    auto first_in = m_loc;

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

    // If no outputs, m_loc is the position of the first input, otherwise it's the position of the first output
    if(ports.second == 0){
        for(auto i = 0; i < ports.first; ++i){
            if (facing == directions::EAST or facing == directions::WEST)
                m_in.push_back(makeCoordinate(m_loc.first, m_loc.second + 10*i));
            else
                m_in.push_back(makeCoordinate(m_loc.first + 10*i, m_loc.second));
        }
        return;
    }
    for(auto i = 0; i < ports.second; ++i){
        if (facing == directions::EAST or facing == directions::WEST)
            m_out.push_back(makeCoordinate(m_loc.first, m_loc.second + 10*i));
        else
            m_out.push_back(makeCoordinate(m_loc.first + 10*i, m_loc.second));
    }

    switch(facing){
        case directions ::EAST:
            first_in.first -= length;
            first_in.second += ((ports.second - ports.first) / 2) * 10;
            break;
        case directions ::SOUTH:
            first_in.second -= length;
            first_in.first += ((ports.second - ports.first) / 2) * 10;
            break;
        case directions ::WEST:
            first_in.first += length;
            first_in.second += ((ports.second - ports.first) / 2) * 10;
            break;
        case directions ::NORTH:
            first_in.second += length;
            first_in.first += ((ports.second - ports.first) / 2) * 10;
            break;
    }
    for(auto i = 0; i < ports.first; ++i){
        if (facing == directions::EAST or facing == directions::WEST)
            m_in.push_back(makeCoordinate(first_in.first, first_in.second + 10*i));
        else
            m_in.push_back(makeCoordinate(first_in.first + 10*i, first_in.second));
    }
}
