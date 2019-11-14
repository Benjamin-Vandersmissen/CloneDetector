//
// Created by benji on 05/11/2019.
//

#ifndef CLONEDETECTOR_UTILITY_H
#define CLONEDETECTOR_UTILITY_H

#endif //CLONEDETECTOR_UTILITY_H

#include <vector>
#include <algorithm>

// Simple override for convoluted one-liner
template<typename T>
bool contains(const std::vector<T>& vector, const T& item){
    return std::find(vector.begin(), vector.end(), item) != vector.end();
}

template<typename T>
int index(const std::vector<T>& vector, const T& item){
    if (contains(vector, item))
        return std::find(vector.begin(), vector.end(), item) - vector.begin();
    return -1;
}

using Coordinate = std::pair<unsigned, unsigned>;
Coordinate makeCoordinate(unsigned int x, unsigned int y);
