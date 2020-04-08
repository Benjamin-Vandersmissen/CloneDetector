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
#include "Circuit.h"
#include "Graph.h"

class XMLParser {
private:
    tinyxml2::XMLDocument m_document;  // Needs to be kept in memory

    tinyxml2::XMLElement* m_root_elem;

    std::vector<Circuit> m_circuits;

    std::string m_filename;

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
    explicit XMLParser(const std::string& filename);

    void addNewFile(const std::string& filename);

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
     * \brief Output the labeled logisim file to a given filename
     * */
    void writeTrough(const std::string& filename);

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
