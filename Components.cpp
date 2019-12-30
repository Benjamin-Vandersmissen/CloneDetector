//
// Created by benji on 14/11/2019.
//

#include "Components.h"
#include <iostream>

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

bool Wire::canOutputTo(const Component *component) const {
    bool result = false;
    result |= contains(component->m_in, m_to);
    result |= contains(component->m_in, m_from);
    return result;
}

int Wire::connectedPort(const Component *component) const {
    if (contains(component->m_in, m_to))  // TODO: this if statement is not very correct, because m_in is private
        return component->indexOfPort(m_to);
    else
        return component->indexOfPort(m_from);
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

bool Component::canOutputTo(const Wire &wire) const {
    return m_out == wire.m_from || m_out == wire.m_to;
}

bool Component::canOutputTo(const Component *component) const{
    return contains(component->m_in, m_out);
}

const std::string &Component::name() const {
    return m_name;
}

int Component::connectedPort(const Component *component) const {
    return component->indexOfPort(m_out);
}

int Component::indexOfPort(const Coordinate &coordinate) const {
    return index(m_in, coordinate);
}

void GateComponent::calculatePorts() {
    // TODO: inverted ports implementeren
    auto size = 50; // default size for most gates
    if (m_attributes.find("size") != m_attributes.end()){
        size = std::stoi(m_attributes["size"]);
    }

    if (this->m_name[0] == 'X')
        size += 10;
    if (this->m_name[0] == 'N')
        size += 10;
    if (this->m_name == "XNOR")
        size += 10;

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

    auto nr_inputs = 5;
    if (m_attributes.find("inputs") != m_attributes.end()){
        nr_inputs = std::stoi(m_attributes["inputs"]);
    }

    switch(facing){
        case directions ::EAST:
            relative_center.first -= size;
            break;
        case directions ::SOUTH:
            relative_center.second -= size;
            break;
        case directions ::WEST:
            relative_center.first += size;
            break;
        case directions ::NORTH:
            relative_center.second += size;
            break;
    }

    // step = room between 2 inputs
    // offset : first port is relative_center - offset

    auto step = (nr_inputs > 3 ? 10 : 20);  // step between ports is 20 if 3 or less ports
    auto offset = (nr_inputs > size/10? nr_inputs/2*10 : (size/20)*10);  // offset is nr_inputs/2*10 or size/20 * 10

    // Edge cases for formula
    if (size >= 70 and (nr_inputs == 2 or nr_inputs == 4)){
        offset = 20;
        step = 20;  // edge case for large size, 4 inputs
    }
    if (size >= 70 and nr_inputs == 3){
        step = 30;
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
                    if (not(size >= 70 and nr_inputs == 4)) { // another edge case
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
                    if (not(size >= 70 and nr_inputs == 4)) { // another edge case
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
}

GateComponent::GateComponent(int lib, const std::string &name, const std::string &loc) : Component(lib, name, loc) {}

Component *createComponent(int lib, const std::string &name, const std::string &loc) {
    if (lib == 1 and (name == "AND Gate" or name == "OR Gate" or name == "NAND Gate" or name == "NOR Gate" or name == "XOR Gate" or name == "XNOR Gate"))
        return new GateComponent(lib, name, loc);

    return nullptr;
}