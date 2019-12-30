//
// Created by benji on 14/11/2019.
//

#ifndef CLONEDETECTOR_COMPONENTS_H
#define CLONEDETECTOR_COMPONENTS_H

#include "Utility.h"
#include <map>

class Component;

class GateComponent;

class Wire{
private:
    Coordinate m_from;

    Coordinate m_to;

public:
    friend class Component;

    Wire(const std::string &from, const std::string &to);

    bool canConnectTo(const Wire& wire) const;

    bool canOutputTo(const Component *component) const;

    int connectedPort(const Component *component) const;
};

//TODO: create subclasses for easier manipulation?


class Component{
protected:
    std::string m_name;

    int m_lib;

    Coordinate m_loc;

    std::map<std::string, std::string> m_attributes;

    Coordinate m_out;

    std::vector<Coordinate > m_out_temp;

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

    Component(int lib, const std::string &name, const std::string &loc);

    void addAttribute(const std::string& name, const std::string& val);

    virtual void calculatePorts() = 0;

    bool canOutputTo(const Wire& wire) const;

    bool canOutputTo(const Component *component) const;

    const std::string& name() const;

    int connectedPort(const Component *component) const;

    int indexOfPort(const Coordinate& coordinate) const;

    const std::vector<Coordinate> &getInputPorts() const;
};

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

Component* createComponent(int lib, const std::string &name, const std::string &loc);
#endif //CLONEDETECTOR_COMPONENTS_H
