//
// Created by benji on 05/11/2019.
//

#ifndef CLONEDETECTOR_NODE_H
#define CLONEDETECTOR_NODE_H

#include <string>
#include <vector>
#include <map>
#include <exception>
#include <tuple>
#include "Components.h"


class Node {
private:
    component_ptr m_component;

public:
    explicit Node(const component_ptr &component);

    /**
     * \brief Simple getter for the name
     * */
    const std::string &getName() const;

    /**
     * \brief Simple getter for the component
     * */
    component_ptr component() const;

    /**
     * \brief Simple getter for the type
     * */
    const std::string& getType() const;
};

using node_ptr = std::shared_ptr<Node>;

class Edge{
private:
    std::pair<node_ptr, unsigned> m_from;

    std::pair<node_ptr , unsigned> m_to;

    std::string m_parent;

    std::string m_file;

    Coordinate m_from_coord;

    Coordinate m_to_coord;
public:
    Edge(const node_ptr& from, unsigned outport, const node_ptr &to, unsigned inport);

    /**
     * \brief Simple getter for m_from
     * */
    const std::pair<node_ptr, unsigned int> & from() const;

    /**
     * \brief Simple getter for m_to
     * */
    const std::pair<node_ptr, unsigned int> & to() const;

    /**
     * \brief Get representation of the form "type0 -> type1 (port0/port1)"
     *
     * \arg positions -> print the start and end postions of the edge
     * */

    std::string text(bool positions=false) const;

    std::string dot() const;

    const std::string& parent() const;

    void parent(const std::string& parent);

    void file(const std::string& file);

    const std::string& file() const;

    void setCoordinates(const Coordinate& from, const Coordinate& to);
};

using edge_ptr = std::shared_ptr<Edge>;

#endif //CLONEDETECTOR_NODE_H
