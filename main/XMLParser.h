//
// Created by benji on 04/11/2019.
//

#ifndef CLONEDETECTOR_XMLPARSER_H
#define CLONEDETECTOR_XMLPARSER_H


#include <string>
#include <map>
#include <iostream>
#include <vector>
#include <algorithm>
#include "tinyxml2.h"
#include "Node.h"


/**
 * \brief A Circuit, made out of wires linking components together. Equivalent to a circuit in a logisim file
 * */
class Circuit{
private:
    std::vector<Wire> m_wires;

    std::vector<Component*> m_components;

    std::string m_name;

    /**
     * Maps all the components in the circuit to a node for the graph representation
     * */
    std::map<Component*, Node*> m_component_map;

    Graph* m_graph = nullptr;

    /**
     * If the circuit is used as a subcircuit, it is useful to know which Pin corresponds with which input / output
     *
     * These vectors are ordered, entry 0 is the first in/ output, entry i is the (i+1)th in/ output
     * */
    std::vector<Component*> m_inputs;

    std::vector<Component*> m_outputs;

public:
    Circuit(std::string  name);

    /**
     * \brief Add a wire, inputs are the strings read from the XML file
     * */
    void addWire(const std::string& from, const std::string& to);

    /**
     * \brief Add a component, inputs are the values read from the XML file
     * */
    void addComponent(int lib, const std::string& name, const std::string& loc);

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
    const std::vector<Component *> &getComponents() const;

    /**
     * \brief This calculates the number of in/out ports for a circuit in case it will be used as a subcircuit later
     * */
    void calculatePorts();

    /**
     * \brief Simple getter for m_name
     * */
    const std::string &getName() const;
};

class XMLParser {
private:
    tinyxml2::XMLDocument m_document;  // Needs to be kept in memory

    tinyxml2::XMLElement* m_root_elem;

    std::vector<Circuit> m_circuits;

    /**
     * \brief parses the libraries in a logisim file, this has no real use, other than being informative on the output stream
     * */
    void parseLibraries();

    /**
     * \brief parses the circuits, using parseWires and parseComponents for the specific circuit XML root
     * */
    void parseCircuits();

    /**
     * \brief parse a wire and add it to the correct circuit
     * */
    void parseWires(tinyxml2::XMLElement* circuit_root_elem);

    /**
     * \brief parse a component, read all it's attributes using parseAttributes and add it to the correct circuit
     * */
    void parseComponents(tinyxml2::XMLElement* circuit_root_elem);

    /**
     * \brief parse the additional attributes associated with a component
     * */
    void parseAttributes(tinyxml2::XMLElement* comp_root_elem);

    bool print_while_parsing = false;
public:
    XMLParser(const std::string& filename);

    /**
     * \brief parse the whole file, when finished, m_circuits should correspond with the circuits from the XML minus some additional (useless) data
     *
     * Should only be called once (TODO? enforce this via a state?)
     * */
    void parse();

    /**
     * \brief Generate a graph for each circuit in the XML file
     *
     * Should only be called after parse is called (TODO? enforce this via a state?)
     * */
    void generateGraphs();

    /**
     * \brief Get the graphs for all circuits
     * */
    std::vector<Graph*> getGraphs() const;

    /**
     * \brief Simple getter for the circuits
     * */
    const std::vector<Circuit> &getCircuits() const;
};


#endif //CLONEDETECTOR_XMLPARSER_H
