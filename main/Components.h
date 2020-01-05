//
// Created by benji on 14/11/2019.
//

#ifndef CLONEDETECTOR_COMPONENTS_H
#define CLONEDETECTOR_COMPONENTS_H

#include "Utility.h"
#include <map>
#include <memory>
// circuit name : nr inputs, nr outputs

extern std::map<std::string, std::pair<int, int> > circuit_port_map;

class Component;
using component_ptr = std::shared_ptr<Component>;

class GateComponent;

class Wire{
private:
    //a vector of points the wire connects, should only ever be two, but implemented as a vector for easy iterating and intersecting
    std::vector<Coordinate > m_points;

public:
    friend class Component;

    /**
     * \brief create a wire, inputs are the string representation of coordinates
     * */
    Wire(const std::string &from, const std::string &to);

    /**
     * \brief Test if a wire can connect to another wire
     *
     * Do this by intersecting the point vectors, a non-empty intersection means they can connect
     * */
    bool canConnectTo(const Wire& wire) const;

    /**
     * \brief Test if a wire can connect to an inport of a given Component
     *
     * Do this by intersecting the point vector of the wire with the inports vector of the Component
     * */
    bool canOutputTo(const component_ptr &component) const;

    /**
     * \brief Find the input port this wire is connected to.
     *
     * Returns the index instead of the position
     * */
    int connectedPort(const component_ptr &component) const;
};

class Component{
protected:
    std::string m_name;

    int m_lib;

    Coordinate m_loc;

    // attribute name : attribute value
    std::map<std::string, std::string> m_attributes;

    // outgoing ports
    std::vector<Coordinate > m_out;

    // incoming ports
    std::vector<Coordinate > m_in;

    // the directions a component could be facing
    enum directions{
        EAST,  // default value
        SOUTH,
        WEST,
        NORTH
    };

    std::string m_unique_id;

public:
    // counts the occurences for each type of Component
    static std::map<std::string, unsigned int> counter;

    friend class Wire;

    friend class Node;

    friend class Circuit;

    /**
     * \brief Create a component
     * */
    Component(int lib, std::string name, const std::string &loc);

    /**
     * \brief add an attribute and a value, overwrites the attribute if already defined
     *
     * Should only ever be called from XML parser
     * */
    void addAttribute(const std::string& name, const std::string& val);

    /**
     * \brief calculates the positions each in / outport should have
     *
     * Differs for different classes of components
     * */
    virtual void calculatePorts() = 0;

    /**
     * \brief Test if a connection can be made with a wire on the given outport
     * */
    bool canOutputTo(const Wire &wire, unsigned long outport=0) const;

    /**
     * \brief Test if a connection can be made with a component on the given outport
     * */
    bool canOutputTo(const component_ptr &component, unsigned outport= 0) const;

    /**
     * \brief Simple getter for m_name
     * */
    const std::string& name() const;

    /**
     * \brief Find the index of the inport the given outport of the component is connected to
     * */
    int connectedInPort(const component_ptr &component, int outport) const;

    /**
     * \brief converts a coordinate of an inport to its index
     * */
    int indexOfInPort(const Coordinate& coordinate) const;

    /**
     * \brief Simple getter for m_in
     * */
    const std::vector<Coordinate> &getInputPorts() const;

    friend bool sortPorts(const component_ptr &comp1, const component_ptr &comp2);

    /**
     * \brief simple getter for m_unique_id
     * */
    const std::string &uniqueName() const;

    const Coordinate &loc() const;
};

/**
 * \brief Sorts 2 coordinates, first descending on y-value, then in case of same y-value, on x-value
 *
 * Used for calculating which port of a subCircuit corresponds to which pin
 * */
bool sortPorts(const component_ptr &comp1, const component_ptr &comp2);

// AND, OR, NAND, NOR, XOR, XNOR
class GateComponent : public Component{
public:
    GateComponent(int lib, const std::string& name, const std::string& loc);

    /**
     * \brief Calculates the in / outports for GateComponents
     *
     * GateComponents always have one output, 2+ inputs, an input can be negated and 3 different possible sizes
     * */

    void calculatePorts() override ;
};

// NOT
class NotComponent : public Component{
public:
    NotComponent(int lib, const std::string &name, const std::string &loc);

    /**
     * \brief Calculates the in / outports for NotComponents
     *
     * NotComponents always have one input, one output and 2 different possible sizes
     * */
    void calculatePorts() override ;
};

// Pin
class PinComponent : public Component{
public:
    PinComponent(int lib, const std::string &name, const std::string &loc);

    /**
     * \brief Calculates the in / outports for PinComponent
     *
     * PinComponents have one input OR one output and a fixed size
     * */
    void calculatePorts() override ;
};

// Circuit
class CircuitComponent : public Component{
public:
    CircuitComponent(int lib, const std::string &name, const std::string &loc);

    /**
     * \brief Calculates the in / outports for CircuitComponent
     *
     * CircuitComponents are userdefined, so they can have as many in / outports as they want,
     * This implementation is based on the default appearance of SubCircuits, it won't work otherwise
     * */
    void calculatePorts() override ;
};

/**
 * \brief Creates a component based on some parameter, Uses the correct SubClass for the component
 * */
component_ptr createComponent(int lib, const std::string &name, const std::string &loc);
#endif //CLONEDETECTOR_COMPONENTS_H
