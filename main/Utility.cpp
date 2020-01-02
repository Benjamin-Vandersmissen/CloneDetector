//
// Created by benji on 05/11/2019.
//

#include "Utility.h"


Coordinate makeCoordinate(unsigned int x, unsigned int y) {
    return std::make_pair<unsigned, unsigned>(std::move(x),std::move(y));
}

bool contains(const std::map<std::string, std::string>& map, const std::string& item){
    return map.find(item) != map.end();
}