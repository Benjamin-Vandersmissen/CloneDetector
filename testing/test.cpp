//
// Created by benji on 30/12/2019.
//
#include "gtest/gtest.h"
#include "../main/XMLParser.h"

TEST(CorrectPorts, Large_GateComponents){
    XMLParser parser("./circuits/large_gates.circ");
    parser.parse();
    parser.generateGraphs();
    auto circuit = parser.getCircuits()[0];

    ASSERT_EQ(circuit.getComponents().size(), 14);

    auto components = circuit.getComponents();

    {   //should be an AND Gate with 3 inputs, facing East
        auto component = components[0];
        auto ports = component->getInputPorts();
        std::vector<Coordinate> expected {{150,170}, {150,200}, {150,230}};
        for(auto i = 0; i < ports.size(); ++i){
            ASSERT_EQ(ports[i], expected[i]);
        }
    }

    {   //should be an AND Gate with 4 inputs, facing East
        auto component = components[1];
        auto ports = component->getInputPorts();
        std::vector<Coordinate> expected {{150,270}, {150,290}, {150,310}, {150, 330}};
        for(auto i = 0; i < ports.size(); ++i){
            ASSERT_EQ(ports[i], expected[i]);
        }
    }

    {   //should be an AND Gate with 9 inputs, facing East
        auto component = components[2];
        auto ports = component->getInputPorts();

        auto x = 150;
        auto y = 470;

        for(auto i = 0; i < ports.size(); ++i){
            ASSERT_EQ(ports[i], Coordinate(x, y+10*i));
        }
    }

    {   //should be an OR Gate with 5 inputs, facing East
        auto component = components[3];
        auto ports = component->getInputPorts();

        auto x = 540;
        auto y = 80;

        for(auto i = 0; i < ports.size(); ++i){
            ASSERT_EQ(ports[i], Coordinate(x, y+10*i));
        }
    }

    {   //should be a NAND Gate with 5 inputs, facing East
        auto component = components[4];
        auto ports = component->getInputPorts();

        auto x = 640;
        auto y = 80;

        for(auto i = 0; i < ports.size(); ++i){
            ASSERT_EQ(ports[i], Coordinate(x, y+10*i));
        }
    }

    {   //should be a XNOR Gate with 5 inputs, facing East
        auto component = components[5];
        auto ports = component->getInputPorts();

        auto x = 920;
        auto y = 80;

        for(auto i = 0; i < ports.size(); ++i){
            ASSERT_EQ(ports[i], Coordinate(x, y+10*i));
        }
    }

    {   //should be an AND Gate with 2 inputs, facing East
        auto component = components[6];
        auto ports = component->getInputPorts();
        std::vector<Coordinate> expected {{150,80}, {150,120}};
        for(auto i = 0; i < ports.size(); ++i){
            ASSERT_EQ(ports[i], expected[i]);
        }
    }

    {   //should be an AND Gate with 10 inputs, facing East
        auto component = components[7];
        auto ports = component->getInputPorts();

        auto x = 150;
        auto y = 580;

        for(auto i = 0; i < 5; ++i){
            ASSERT_EQ(ports[i], Coordinate(x, y+10*i));
        }

        for(auto i = 5; i < 10; ++i){
            ASSERT_EQ(ports[i], Coordinate(x, y+10*i + 10));
        }
    }

    {   //should be a XOR Gate with 5 inputs, facing East
        auto component = components[8];
        auto ports = component->getInputPorts();

        auto x = 830;
        auto y = 80;

        for(auto i = 0; i < ports.size(); ++i){
            ASSERT_EQ(ports[i], Coordinate(x, y+10*i));
        }
    }

    {   //should be an AND Gate with 5 inputs, facing North
        auto component = components[9];
        auto ports = component->getInputPorts();

        auto x = 270;
        auto y = 130;

        for(auto i = 0; i < ports.size(); ++i){
            ASSERT_EQ(ports[i], Coordinate(x+10*i, y));
        }
    }

    {   //should be an AND Gate with 5 inputs, facing West
        auto component = components[10];
        auto ports = component->getInputPorts();

        auto x = 500;
        auto y = 80;

        for(auto i = 0; i < ports.size(); ++i){
            ASSERT_EQ(ports[i], Coordinate(x, y+10*i));
        }
    }

    {   //should be an AND Gate with 5 inputs, facing South
        auto component = components[11];
        auto ports = component->getInputPorts();

        auto x = 360;
        auto y = 60;

        for(auto i = 0; i < ports.size(); ++i){
        ASSERT_EQ(ports[i], Coordinate(x+10*i, y));
        }
    }

    {   //should be an AND Gate with 5 inputs, facing East
        auto component = components[12];
        auto ports = component->getInputPorts();

        auto x = 150;
        auto y = 370;

        for(auto i = 0; i < ports.size(); ++i){
            ASSERT_EQ(ports[i], Coordinate(x, y+10*i));
        }
    }

    {   //should be an NOR Gate with 5 inputs, facing East
        auto component = components[13];
        auto ports = component->getInputPorts();

        auto x = 740;
        auto y = 80;

        for(auto i = 0; i < ports.size(); ++i){
            ASSERT_EQ(ports[i], Coordinate(x, y+10*i));
        }
    }
}

TEST(CorrectPorts, Medium_GateComponents){
    XMLParser parser("./circuits/medium_gates.circ");
    parser.parse();
    parser.generateGraphs();
    auto circuit = parser.getCircuits()[0];

    ASSERT_EQ(circuit.getComponents().size(), 14);

    auto components = circuit.getComponents();

    {   //should be an XOR Gate with 5 inputs, facing East
        auto component = components[0];
        auto ports = component->getInputPorts();

        auto x = 850;
        auto y = 80;

        for(auto i = 0; i < ports.size(); ++i){
            ASSERT_EQ(ports[i], Coordinate(x, y+10*i));
        }
    }

    {   //should be an AND Gate with 5 inputs, facing North
        auto component = components[1];
        auto ports = component->getInputPorts();

        auto x = 270;
        auto y = 110;

        for(auto i = 0; i < ports.size(); ++i){
            ASSERT_EQ(ports[i], Coordinate(x+10*i, y));
        }
    }

    {   //should be an AND Gate with 10 inputs, facing East
        auto component = components[2];
        auto ports = component->getInputPorts();

        auto x = 170;
        auto y = 580;

        for(auto i = 0; i < 5; ++i){
            ASSERT_EQ(ports[i], Coordinate(x, y+10*i));
        }

        for(auto i = 5; i < 10; ++i){
            ASSERT_EQ(ports[i], Coordinate(x, y+10*i + 10));
        }
    }

    {   //should be an AND Gate with 5 inputs, facing East
        auto component = components[3];
        auto ports = component->getInputPorts();

        auto x = 170;
        auto y = 370;

        for(auto i = 0; i < ports.size(); ++i){
            ASSERT_EQ(ports[i], Coordinate(x, y+10*i));
        }
    }

    {   //should be a NAND Gate with 5 inputs, facing East
        auto component = components[4];
        auto ports = component->getInputPorts();

        auto x = 660;
        auto y = 80;

        for(auto i = 0; i < ports.size(); ++i){
            ASSERT_EQ(ports[i], Coordinate(x, y+10*i));
        }
    }

    {   //should be an AND Gate with 2 inputs, facing East
        auto component = components[5];
        auto ports = component->getInputPorts();
        std::vector<Coordinate> expected {{170,80}, {170,120}};
        for(auto i = 0; i < ports.size(); ++i){
            ASSERT_EQ(ports[i], expected[i]);
        }
    }

    {   //should be an AND Gate with 5 inputs, facing West
        auto component = components[6];
        auto ports = component->getInputPorts();

        auto x = 480;
        auto y = 80;

        for(auto i = 0; i < ports.size(); ++i){
            ASSERT_EQ(ports[i], Coordinate(x, y+10*i));
        }
    }

    {   //should be an AND Gate with 5 inputs, facing South
        auto component = components[7];
        auto ports = component->getInputPorts();

        auto x = 360;
        auto y = 80;

        for(auto i = 0; i < ports.size(); ++i){
            ASSERT_EQ(ports[i], Coordinate(x+10*i, y));
        }
    }

    {   //should be a XNOR Gate with 5 inputs, facing East
        auto component = components[8];
        auto ports = component->getInputPorts();

        auto x = 940;
        auto y = 80;

        for(auto i = 0; i < ports.size(); ++i){
            ASSERT_EQ(ports[i], Coordinate(x, y+10*i));
        }
    }

    {   //should be an AND Gate with 3 inputs, facing East
        auto component = components[9];
        auto ports = component->getInputPorts();
        std::vector<Coordinate> expected {{170,180}, {170,200}, {170, 220}};
        for(auto i = 0; i < ports.size(); ++i){
        ASSERT_EQ(ports[i], expected[i]);
        }
    }

    {   //should be an OR Gate with 5 inputs, facing East
        auto component = components[10];
        auto ports = component->getInputPorts();

        auto x = 560;
        auto y = 80;

        for(auto i = 0; i < ports.size(); ++i){
            ASSERT_EQ(ports[i], Coordinate(x, y+10*i));
        }
    }

    {   //should be an AND Gate with 4 inputs, facing East
        auto component = components[11];
        auto ports = component->getInputPorts();
        std::vector<Coordinate> expected {{170,270}, {170,280}, {170, 300}, {170, 310}};
        for(auto i = 0; i < ports.size(); ++i){
            ASSERT_EQ(ports[i], expected[i]);
        }
    }

    {   //should be an AND Gate with 9 inputs, facing East
        auto component = components[12];
        auto ports = component->getInputPorts();

        auto x = 170;
        auto y = 470;

        for(auto i = 0; i < ports.size(); ++i){
            ASSERT_EQ(ports[i], Coordinate(x, y+10*i));
        }
    }

    {   //should be a NOR Gate with 5 inputs, facing East
        auto component = components[13];
        auto ports = component->getInputPorts();

        auto x = 760;
        auto y = 80;

        for(auto i = 0; i < ports.size(); ++i){
            ASSERT_EQ(ports[i], Coordinate(x, y+10*i));
        }
    }
}

TEST(CorrectPorts, Narrow_GateComponents){
    XMLParser parser("./circuits/narrow_gates.circ");
    parser.parse();
    parser.generateGraphs();
    auto circuit = parser.getCircuits()[0];

    ASSERT_EQ(circuit.getComponents().size(), 12);

    auto components = circuit.getComponents();

    {   //should be an AND Gate with 5 inputs, facing North
        auto component = components[0];
        auto ports = component->getInputPorts();

        auto x = 260;
        auto y = 110;

        for(auto i = 0; i < ports.size(); ++i){
            ASSERT_EQ(ports[i], Coordinate(x+10*i, y));
        }
    }

    {   //should be an AND Gate with 5 inputs, facing West
        auto component = components[1];
        auto ports = component->getInputPorts();

        auto x = 430;
        auto y = 80;

        for(auto i = 0; i < ports.size(); ++i){
            ASSERT_EQ(ports[i], Coordinate(x, y+10*i));
        }
    }

    {   //should be an AND Gate with 5 inputs, facing East
        auto component = components[2];
        auto ports = component->getInputPorts();

        auto x = 190;
        auto y = 370;

        for(auto i = 0; i < ports.size(); ++i){
            ASSERT_EQ(ports[i], Coordinate(x, y+10*i));
        }
    }

    {   //should be a XOR Gate with 5 inputs, facing East
        auto component = components[3];
        auto ports = component->getInputPorts();

        auto x = 650;
        auto y = 80;

        for(auto i = 0; i < ports.size(); ++i){
            ASSERT_EQ(ports[i], Coordinate(x, y+10*i));
        }
    }

    {   //should be an AND Gate with 2 inputs, facing East
        auto component = components[4];
        auto ports = component->getInputPorts();
        std::vector<Coordinate> expected {{190,90}, {190,110}};
        for(auto i = 0; i < ports.size(); ++i){
            ASSERT_EQ(ports[i], expected[i]);
        }
    }

    {   //should be a NAND Gate with 5 inputs, facing East
        auto component = components[5];
        auto ports = component->getInputPorts();

        auto x = 530;
        auto y = 80;

        for(auto i = 0; i < ports.size(); ++i){
            ASSERT_EQ(ports[i], Coordinate(x, y+10*i));
        }
    }

    {   //should be a XNOR Gate with 5 inputs, facing East
        auto component = components[6];
        auto ports = component->getInputPorts();

        auto x = 720;
        auto y = 80;

        for(auto i = 0; i < ports.size(); ++i){
            ASSERT_EQ(ports[i], Coordinate(x, y+10*i));
        }
    }

    {   //should be an AND Gate with 3 inputs, facing East
        auto component = components[7];
        auto ports = component->getInputPorts();
        std::vector<Coordinate> expected {{190,190}, {190,200}, {190, 210}};
        for(auto i = 0; i < ports.size(); ++i){
            ASSERT_EQ(ports[i], expected[i]);
        }
    }

    {   //should be an AND Gate with 4 inputs, facing East
        auto component = components[8];
        auto ports = component->getInputPorts();
        std::vector<Coordinate> expected {{190,270}, {190,280}, {190, 300}, {190, 310}};
        for(auto i = 0; i < ports.size(); ++i){
            ASSERT_EQ(ports[i], expected[i]);
        }
    }

    {   //should be an AND Gate with 5 inputs, facing South
        auto component = components[9];
        auto ports = component->getInputPorts();

        auto x = 330;
        auto y = 80;

        for(auto i = 0; i < ports.size(); ++i){
            ASSERT_EQ(ports[i], Coordinate(x+10*i, y));
        }
    }

    {   //should be an OR Gate with 5 inputs, facing west
        auto component = components[10];
        auto ports = component->getInputPorts();

        auto x = 480;
        auto y = 80;

        for(auto i = 0; i < ports.size(); ++i){
            ASSERT_EQ(ports[i], Coordinate(x, y+10*i));
        }
    }

    {   //should be a NOR Gate with 5 inputs, facing East
        auto component = components[11];
        auto ports = component->getInputPorts();

        auto x = 590;
        auto y = 80;

        for(auto i = 0; i < ports.size(); ++i){
            ASSERT_EQ(ports[i], Coordinate(x, y+10*i));
        }
    }
}

TEST(CorrectConnections, simpleConnection){
    XMLParser parser("./circuits/simpleConnection.circ");
    parser.parse();
    parser.generateGraphs();
    auto circuit = parser.getCircuits()[0];
    auto forest = circuit.getForestRepresentation();

    { //output pin has incoming connection from AND Gate
        auto node = forest[0];
        ASSERT_EQ(node->getName(), "Pin_0");
        auto incoming = node->getIncomingNodes();
        ASSERT_EQ(incoming.size(), 1);
        auto tuple = incoming[0];
        ASSERT_EQ(std::get<0>(tuple), 0);
        ASSERT_EQ(std::get<1>(tuple)->getName(), "AND Gate_0");
        ASSERT_EQ(std::get<2>(tuple), 0);
    }

    { //input pin 1 has no incoming connections
        auto node = forest[1];
        ASSERT_EQ(node->getName(), "Pin_1");
        auto incoming = node->getIncomingNodes();
        ASSERT_EQ(incoming.size(), 0);
    }

    { //AND gate has 2 incoming connections
        auto node = forest[2];
        ASSERT_EQ(node->getName(), "AND Gate_0");
        auto incoming = node->getIncomingNodes();
        ASSERT_EQ(incoming.size(), 2);

        auto tuple = incoming[0];
        ASSERT_EQ(std::get<0>(tuple), 0);
        ASSERT_EQ(std::get<1>(tuple)->getName(), "Pin_1");
        ASSERT_EQ(std::get<2>(tuple), 0);

        tuple = incoming[1];
        ASSERT_EQ(std::get<0>(tuple), 1);
        ASSERT_EQ(std::get<1>(tuple)->getName(), "Pin_2");
        ASSERT_EQ(std::get<2>(tuple), 0);
    }

    { //input pin 2 has no incoming connections
        auto node = forest[3];
        ASSERT_EQ(node->getName(), "Pin_2");
        auto incoming = node->getIncomingNodes();
        ASSERT_EQ(incoming.size(), 0);
    }
}

TEST(CorrectConnections, complicatedConnection){
    XMLParser parser("./circuits/complicatedConnection.circ");
    parser.parse();
    parser.generateGraphs();
    auto circuit = parser.getCircuits()[0];
    auto forest = circuit.getForestRepresentation();

    { //output pin has incoming connection from AND Gate
        auto node = forest[0];
        ASSERT_EQ(node->getName(), "Pin_0");
        auto incoming = node->getIncomingNodes();
        ASSERT_EQ(incoming.size(), 1);
        auto tuple = incoming[0];
        ASSERT_EQ(std::get<0>(tuple), 0);
        ASSERT_EQ(std::get<1>(tuple)->getName(), "AND Gate_0");
        ASSERT_EQ(std::get<2>(tuple), 0);
    }

    { //input pin 2 has no incoming connections
        auto node = forest[1];
        ASSERT_EQ(node->getName(), "Pin_1");
        auto incoming = node->getIncomingNodes();
        ASSERT_EQ(incoming.size(), 0);
    }

    { //input pin 1 has no incoming connections
        auto node = forest[2];
        ASSERT_EQ(node->getName(), "Pin_2");
        auto incoming = node->getIncomingNodes();
        ASSERT_EQ(incoming.size(), 0);
    }

    { //AND gate has 2 incoming connections
        auto node = forest[3];
        ASSERT_EQ(node->getName(), "AND Gate_0");
        auto incoming = node->getIncomingNodes();
        ASSERT_EQ(incoming.size(), 2);

        auto tuple = incoming[0];
        ASSERT_EQ(std::get<0>(tuple), 0);
        ASSERT_EQ(std::get<1>(tuple)->getName(), "Pin_2");
        ASSERT_EQ(std::get<2>(tuple), 0);

        tuple = incoming[1];
        ASSERT_EQ(std::get<0>(tuple), 1);
        ASSERT_EQ(std::get<1>(tuple)->getName(), "Pin_1");
        ASSERT_EQ(std::get<2>(tuple), 0);
    }
}
TEST(CorrectConnections, directConnection) {
    XMLParser parser("./circuits/directConnection.circ");
    parser.parse();
    parser.generateGraphs();
    auto circuit = parser.getCircuits()[0];
    auto forest = circuit.getForestRepresentation();

    { //output pin has incoming connection from AND Gate
        auto node = forest[0];
        ASSERT_EQ(node->getName(), "Pin_0");
        auto incoming = node->getIncomingNodes();
        ASSERT_EQ(incoming.size(), 1);
        auto tuple = incoming[0];
        ASSERT_EQ(std::get<0>(tuple), 0);
        ASSERT_EQ(std::get<1>(tuple)->getName(), "AND Gate_0");
        ASSERT_EQ(std::get<2>(tuple), 0);
    }

    { //AND gate has 2 incoming connections
        auto node = forest[1];
        ASSERT_EQ(node->getName(), "AND Gate_0");
        auto incoming = node->getIncomingNodes();
        ASSERT_EQ(incoming.size(), 2);

        auto tuple = incoming[0];
        ASSERT_EQ(std::get<0>(tuple), 0);
        ASSERT_EQ(std::get<1>(tuple)->getName(), "Pin_2");
        ASSERT_EQ(std::get<2>(tuple), 0);

        tuple = incoming[1];
        ASSERT_EQ(std::get<0>(tuple), 1);
        ASSERT_EQ(std::get<1>(tuple)->getName(), "Pin_1");
        ASSERT_EQ(std::get<2>(tuple), 0);
    }

    { //input pin 2 has no incoming connections
        auto node = forest[2];
        ASSERT_EQ(node->getName(), "Pin_1");
        auto incoming = node->getIncomingNodes();
        ASSERT_EQ(incoming.size(), 0);
    }

    { //input pin 1 has no incoming connections
        auto node = forest[3];
        ASSERT_EQ(node->getName(), "Pin_2");
        auto incoming = node->getIncomingNodes();
        ASSERT_EQ(incoming.size(), 0);
    }
}

int main(){
    ::testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}