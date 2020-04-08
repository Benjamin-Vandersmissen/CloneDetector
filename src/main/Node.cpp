//
// Created by benji on 05/11/2019.
//

#include "Node.h"
#include "XMLParser.h"
#include "Graph.h"
#include "Clone.h"

#include <utility>

Node::Node(const component_ptr &component) {
    m_component = component;
}

const std::string &Node::getName() const {
    return m_component->uniqueName();
}

component_ptr Node::component() const {
    return m_component;
}

const std::string &Node::getType() const {
    return m_component->name();
}


Edge::Edge(const node_ptr& from, unsigned outport, const node_ptr &to, unsigned inport): m_from{from, outport}, m_to{to, inport} {}

const std::pair<node_ptr, unsigned int> & Edge::from() const {
    return m_from;
}

const std::pair<node_ptr, unsigned int> & Edge::to() const {
    return m_to;
}

std::string Edge::text(bool positions) const {
    std::string retValue = m_from.first->getType() + " -> " + m_to.first->getType() + " [" + std::to_string(m_from.second) + "/";
    if (!m_to.first->component()->interchangeable_inputs())
        retValue += std::to_string(m_to.second);
    retValue += "]";
    if (positions)
        retValue += "    (" + static_cast<std::string>(m_from_coord) + " -> " + static_cast<std::string>(m_to_coord) + ")";
    return retValue;
}

const std::string &Edge::parent() const {
    return m_parent;
}

void Edge::parent(const std::string &parent) {
    m_parent = parent;
}

void Edge::file(const std::string &file) {
    m_file = file;
}

const std::string &Edge::file() const {
    return m_file;
}

void Edge::setCoordinates(const Coordinate &from, const Coordinate &to) {
    m_from_coord = from;
    m_to_coord = to;
}

std::string Edge::dot() const {
    std::string retValue = "\"" + this->from().first->getName() + "\" -> \"" + this->to().first->getName() + "\"";
    return retValue;
}
