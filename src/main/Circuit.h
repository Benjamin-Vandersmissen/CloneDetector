//
// Created by benji on 29/03/2020.
//

#ifndef CLONEDETECTOR_CIRCUIT_H
#define CLONEDETECTOR_CIRCUIT_H

#include "Components.h"
#include "Graph.h"

/**
 * \brief A Circuit, made out of wires linking components together. Equivalent to a circuit in a logisim file
 * */
class Circuit{
private:
    std::vector<Wire> m_wires;

    std::vector<component_ptr> m_components;

    std::string m_name;

    std::string m_file;

    /**
     * Maps all the components in the circuit to a node for the graph representation
     * */
    std::map<component_ptr , node_ptr> m_component_map;

    Graph* m_graph = nullptr;

    /**
     * If the circuit is used as a subcircuit, it is useful to know which Pin corresponds with which input / output
     *
     * These vectors are ordered, entry 0 is the first in/ output, entry i is the (i+1)th in/ output
     * */
    std::vector<component_ptr> m_inputs;

    std::vector<component_ptr> m_outputs;

public:
    Circuit(std::string name, std::string file="");

    /**
     * \brief Add a wire, inputs are the strings read from the XML file
     * */
    void addWire(const std::string& from, const std::string& to);

    /**
     * \brief Add a component, inputs are the values read from the XML file
     * */
    bool addComponent(int lib, const std::string& name, const std::string& loc);

    /**
     * \brief shortcut to get a reference to the last added component
     * */
    Component& lastComponent() const;

    /**
     * \brief generate a graph representation for this circuit; glueing the wires and components together
     *
     * This also populates the component map
     * */
    void generateGraph();

    /**
     * \brief Return the forest representation for this circuit
     * */
    Graph * getGraphRepresentation() const;

    /**
     * \brief Simple getter for the components
     * */
    const std::vector<component_ptr> &getComponents() const;

    /**
     * \brief This calculates the number of in/out ports for a circuit in case it will be used as a subcircuit later
     * */
    void calculatePorts();

    /**
     * \brief Simple getter for m_name
     * */
    const std::string &getName() const;

    const std::vector<component_ptr> &inputs() const;

    const std::vector<component_ptr> &outputs() const;
};

#include <string>
#include <map>
#include <iostream>
#include <vector>
#include <algorithm>
#include "tinyxml2.h"
#include "Node.h"
#include "Graph.h"

#endif //CLONEDETECTOR_CIRCUIT_H
