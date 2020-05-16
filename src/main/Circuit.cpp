//
// Created by benji on 29/03/2020.
//

#include "Circuit.h"

Circuit::Circuit(std::string name, std::string file) : m_name(std::move(name)), m_file(std::move(file)){}

void Circuit::addWire(const std::string &from, const std::string &to) {
    m_wires.emplace_back(from, to);
}

bool Circuit::addComponent(int lib, const std::string &name, const std::string &loc) {
    m_components.emplace_back(createComponent(lib, name, loc));
    auto success = true;
    if (m_components.back() == nullptr){
        m_components.pop_back();
        success = false;
    }
    return success;
}

Component &Circuit::lastComponent() const{
    return *m_components.back();
}

void Circuit::generateGraph() {
    m_graph = new Graph(m_name, m_file);
    for(auto& component : m_components){
        component->calculatePorts();
        m_component_map[component] = std::make_shared<Node>(component);
        m_graph->addNode(m_component_map[component]);
    }

    for (auto & component : m_components){
        // for each component

        for(auto outport = 0; outport < component->getOutPorts().size(); ++outport) {
            // for each output port of the component

            Coordinate start_coord = component->getOutPorts()[outport];
            std::vector<Wire *> wires;
            for (auto &wire : m_wires) {
                if (component->canOutputTo(wire, outport)) {
                    wires.push_back(&wire);
                }
            }
            auto previous_size = 0;
            while (wires.size() > previous_size) {  // find all wires directly connected to this output
                previous_size = wires.size();
                for (auto &wire1 : m_wires) {
                    auto size = wires.size();
                    for (auto i = 0; i < size; ++i) {
                        auto wire2 = wires[i];
                        if (wire2->canConnectTo(wire1) and ! contains(wires, &wire1)) { // Add a new wire
                            wires.push_back(&wire1);
                        }
                    }
                }
            }
            if (wires.empty()) { // A component could be directly connected
                for (auto & other_component : m_components) {
                    if (component->canOutputTo(other_component, outport)) {
                        auto inPort = component->connectedInPort(other_component, outport);
                        auto negated = other_component->negated(inPort);
                        Coordinate end_coord = other_component->getInputPorts()[inPort];
                        auto e = std::make_shared<Edge>(m_component_map[component], outport, m_component_map[other_component], inPort, negated);
                        e->setCoordinates(start_coord, end_coord);
                        m_graph->addEdge(e);
                    }
                }
            } else {  // find all components whose inputs are connected to this component's output
                for (auto & other_component : m_components) {
                    for (auto &wire : wires) {
                        if (wire->canOutputTo(other_component)) {
                            auto inPort = wire->connectedPort(other_component);
                            auto negated = other_component->negated(inPort);
                            Coordinate end_coord = other_component->getInputPorts()[inPort];
                            auto e = std::make_shared<Edge>(m_component_map[component], outport, m_component_map[other_component], inPort, negated);
                            e->setCoordinates(start_coord, end_coord);
                            m_graph->addEdge(e);
                        }
                    }
                }
            }
        }
    }
}

Graph * Circuit::getGraphRepresentation() const {
    return m_graph;
}

const std::vector<component_ptr> &Circuit::getComponents() const {
    return m_components;
}

void Circuit::calculatePorts() {
    for(const auto& component : m_components){
        if (component->name() == "Pin"  and component->lib() == 0){
            if(component->getAttributes().find("output") != component->getAttributes().end())
                m_outputs.push_back(component);
            else
                m_inputs.push_back(component);

        }
    }
    std::sort(m_inputs.begin(), m_inputs.end(), sortPorts);
    std::sort(m_outputs.begin(), m_outputs.end(), sortPorts);

    circuit_port_map[m_name] = {m_inputs.size(), m_outputs.size()};
}

const std::string &Circuit::getName() const {
    return m_name;
}

const std::vector<component_ptr> &Circuit::inputs() const {
    return m_inputs;
}

const std::vector<component_ptr> &Circuit::outputs() const {
    return m_outputs;
}