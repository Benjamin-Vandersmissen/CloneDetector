//
// Created by benji on 14/11/2019.
//

#include "Components.h"
#include <iostream>
#include <utility>

std::map<std::string, unsigned int> Component::counter = {};

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

bool Wire::canOutputTo(const component_ptr &component) const {
    return ! intersection(component->getInputPorts(), m_points).empty();
}

int Wire::connectedPort(const component_ptr &component) const {
    auto index = component->indexOfInPort(m_points[0]);
    if (index != -1) return index;
    return component->indexOfInPort(m_points[1]);
}

const std::vector<Coordinate> &Wire::points() const {
    return m_points;
}

Component::Component(int lib, std::string name, const std::string &loc) : m_lib(lib), m_name(std::move(name)){
    // extract the coordinates from the string
    unsigned loc_x = std::stoi(loc.substr(1, loc.find(',') - 1));
    unsigned loc_y = std::stoi(loc.substr(loc.find(',') + 1, loc.size() - loc.find(',')));

    m_unique_id = m_name + "_" + std::to_string(counter[m_name]);
    counter[m_name]++;

    m_loc = makeCoordinate(loc_x, loc_y);
}

void Component::addAttribute(const std::string &name, const std::string &val) {
    m_attributes[name] = val;
}

bool Component::canOutputTo(const Wire &wire, unsigned long outport) const {
    return contains(wire.points(), m_out[outport]);
}

bool Component::canOutputTo(const component_ptr &component, unsigned outport) const{
    return contains(component->m_in, m_out[outport]);
}

const std::string &Component::name() const {
    return m_name;
}

int Component::connectedInPort(const component_ptr &component, int outport) const {
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
            relative_center.x -= length;
            break;
        case directions ::SOUTH:
            relative_center.y -= length;
            break;
        case directions ::WEST:
            relative_center.x += length;
            break;
        case directions ::NORTH:
            relative_center.y += length;
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
            input.x -= offset;
            for (auto i = 0; i < nr_inputs; ++i) {
                m_in.push_back(input);
                input.x += step;
            }
        }
        else { // ports are on y-axis
            input.y -= offset;
            for (auto i = 0; i < nr_inputs; ++i) {
                m_in.push_back(input);
                input.y += step;
            }
        }
    }
    else{  // even ports, skip center
        auto input = relative_center;
        if (facing == directions::NORTH or facing == directions::SOUTH) { // ports are on x-axis
            input.x -= offset;
            for (auto i = 0; i < nr_inputs; ++i) {
                if (input.x == relative_center.x) {
                    if (not(width == 70 and nr_inputs == 4)) { // another edge case
                        i -= 1;
                        input.x += step;
                        continue;
                    }
                }
                m_in.push_back(input);
                input.x += step;
            }
        }
        else{ // ports are on y-axis
            input.y -= offset;
            for (auto i = 0; i < nr_inputs; ++i) {
                if (input.y == relative_center.y) {
                    if (not(width == 70 and nr_inputs == 4)) { // another edge case
                        i -= 1;
                        input.y += step;
                        continue;
                    }
                }
                m_in.push_back(input);
                input.y += step;
            }
        }
    }

    for(auto i = 0; i < m_in.size(); ++ i){
        auto& coord = m_in[i];
        if (contains(m_attributes, "negate" + std::to_string(i))){
            switch(facing){
                case directions ::EAST:
                    coord.x -= 10;
                    break;

                case directions ::WEST:
                    coord.x += 10;
                    break;

                case directions ::NORTH:
                    coord.y -= 10;
                    break;

                case directions ::SOUTH:
                    coord.y += 10;
                    break;
            }
        }
    }
}

GateComponent::GateComponent(int lib, const std::string &name, const std::string &loc) : Component(lib, name, loc) {}

component_ptr createComponent(int lib, const std::string &name, const std::string &loc) {
    if (lib == 1 and (name == "AND Gate" or name == "OR Gate" or name == "NAND Gate" or name == "NOR Gate" or name == "XOR Gate" or name == "XNOR Gate"))
        return std::make_shared<GateComponent>(lib, name, loc);

    if (lib == 1 and (name == "NOT Gate"))
        return std::make_shared<NotComponent>(lib, name, loc);

    if (lib == 0 and (name == "Pin"))
        return std::make_shared<PinComponent>(lib, name, loc);
    if (lib == -1)
        return std::make_shared<CircuitComponent>(lib, name, loc);
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
            relative_center.x -= length;
            break;
        case directions ::SOUTH:
            relative_center.y -= length;
            break;
        case directions ::WEST:
            relative_center.x += length;
            break;
        case directions ::NORTH:
            relative_center.y += length;
            break;
    }
    m_in.push_back(relative_center);
}

bool sortPorts(const component_ptr &comp1, const component_ptr &comp2){
    if(comp1->m_loc.y < comp2->m_loc.y)
        return true;
    return comp1->m_loc.x < comp2->m_loc.x and comp1->m_loc.y == comp2->m_loc.y;
}

const std::string &Component::uniqueName() const {
    return m_unique_id;
}

const Coordinate &Component::loc() const {
    return m_loc;
}

const std::vector<Coordinate> &Component::getOutPorts() const {
    return m_out;
}

int Component::lib() const {
    return m_lib;
}

const std::map<std::string, std::string> &Component::getAttributes() const {
    return m_attributes;
}

bool Component::negated(int index) {
    return contains(m_attributes, "negate"+std::to_string(index));
}

PinComponent::PinComponent(int lib, const std::string &name, const std::string &loc) : Component(lib, name, loc) {}

void PinComponent::calculatePorts() {
    if (not(contains(m_attributes, "output")))   // Input pin has no inputs, output pin has one
        m_out.push_back(m_loc);
    else
        m_in.push_back(m_loc);
}

// Give a circuit component a unique prefix, to avoid conflicts with normal components
CircuitComponent::CircuitComponent(int lib, const std::string &name, const std::string &loc) : Component(lib, "user_" + name,
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
    if(ports.first == 0){
        for(auto i = 0; i < ports.second; ++i){
            if (facing == directions::EAST or facing == directions::WEST)
                m_in.push_back(makeCoordinate(m_loc.x, m_loc.y + 10*i));
            else
                m_in.push_back(makeCoordinate(m_loc.x + 10*i, m_loc.y));
        }
        return;
    }
    for(auto i = 0; i < ports.second; ++i){
        if (facing == directions::EAST or facing == directions::WEST)
            m_out.push_back(makeCoordinate(m_loc.x, m_loc.y + 10*i));
        else
            m_out.push_back(makeCoordinate(m_loc.x + 10*i, m_loc.y));
    }

    switch(facing){
        case directions ::EAST:
            first_in.x -= length;
            first_in.y += ((ports.first - ports.second) / 2) * 10;
            break;
        case directions ::SOUTH:
            first_in.y -= length;
            first_in.x += ((ports.second - ports.first) / 2) * 10;
            break;
        case directions ::WEST:
            first_in.x += length;
            first_in.y += ((ports.second - ports.first) / 2) * 10;
            break;
        case directions ::NORTH:
            first_in.y += length;
            first_in.x += ((ports.second - ports.first) / 2) * 10;
            break;
    }
    for(auto i = 0; i < ports.first; ++i){
        if (facing == directions::EAST or facing == directions::WEST)
            m_in.push_back(makeCoordinate(first_in.x, first_in.y + 10*i));
        else
            m_in.push_back(makeCoordinate(first_in.x + 10*i, first_in.y));
    }
}
