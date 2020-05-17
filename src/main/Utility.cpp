//
// Created by benji on 05/11/2019.
//

#include "Utility.h"


Coordinate makeCoordinate(unsigned int x, unsigned int y) {
    return Coordinate(x,y);
}

bool contains(const std::map<std::string, std::string>& map, const std::string& item){
    return map.find(item) != map.end();
}

bool contains(const std::string &string, const std::string &item) {
    return string.find(item) != std::string::npos;
}


