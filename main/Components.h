//
// Created by benji on 14/11/2019.
//

#ifndef CLONEDETECTOR_COMPONENTS_H
#define CLONEDETECTOR_COMPONENTS_H

#include "Utility.h"
#include <map>

extern std::map<std::string, std::pair<int, int> > circuit_port_map;

class Component;

class GateComponent;

class Wire{
private:
    std::vector<Coordinate > m_points;

public:
    friend class Component;

    Wire(const std::string &from, const std::string &to);

    bool canConnectTo(const Wire& wire) const;

    bool canOutputTo(const Component *component) const;

    int connectedPort(const Component *component) const;
};

class Component{
protected:
    std::string m_name;

    int m_lib;

    Coordinate m_loc;

    std::map<std::string, std::string> m_attributes;

    std::vector<Coordinate > m_out;

    std::vector<Coordinate > m_in;

    enum directions{
        EAST,  // default value
        SOUTH,
        WEST,
        NORTH
    };

public:
    friend class Wire;

    friend class Node;

    friend class Circuit;

    Component(int lib, const std::string &name, const std::string &loc);

    void addAttribute(const std::string& name, const std::string& val);

    virtual void calculatePorts() = 0;

    bool canOutputTo(const Wire &wire, unsigned long outport=0) const;

    bool canOutputTo(const Component *component, unsigned outport=0) const;

    const std::string& name() const;

    int connectedInPort(const Component *component) const;

    int indexOfInPort(const Coordinate& coordinate) const;

    const std::vector<Coordinate> &getInputPorts() const;

    friend bool sortPorts(const Component* comp1, const Component* comp2);
};

bool sortPorts(const Component* comp1, const Component* comp2);

// AND, OR, NAND, NOR, XOR, XNOR
class GateComponent : public Component{
public:
    GateComponent(int lib, const std::string& name, const std::string& loc);

    void calculatePorts() override ;
};

// NOT
class NotComponent : public Component{
public:
    NotComponent(int lib, const std::string &name, const std::string &loc);

    void calculatePorts() override ;
};

// Pin
class PinComponent : public Component{
public:
    PinComponent(int lib, const std::string &name, const std::string &loc);

    void calculatePorts() override ;
};

// Circuit
class CircuitComponent : public Component{
public:
    CircuitComponent(int lib, const std::string &name, const std::string &loc);

    void calculatePorts() override ;
};
Component* createComponent(int lib, const std::string &name, const std::string &loc);
#endif //CLONEDETECTOR_COMPONENTS_H
