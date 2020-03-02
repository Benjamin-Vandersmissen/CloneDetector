//
// Created by benji on 05/11/2019.
//

#ifndef CLONEDETECTOR_UTILITY_H
#define CLONEDETECTOR_UTILITY_H

#endif //CLONEDETECTOR_UTILITY_H

#include <vector>
#include <algorithm>
#include <map>

// Simple override for convoluted one-liner
template<typename T>
bool contains(const std::vector<T>& vector, const T& item){
    return std::find(vector.begin(), vector.end(), item) != vector.end();
}

bool contains(const std::map<std::string, std::string>& map, const std::string& item);

template<typename T>
int index(const std::vector<T>& vector, const T& item){
    if (contains(vector, item))
        return std::find(vector.begin(), vector.end(), item) - vector.begin();
    return -1;
}


// Call by value, to not change the original vectors, these are typically low size vectors\
// based on https://stackoverflow.com/questions/19483663/vector-intersection-in-c
template<typename T>
std::vector<T> intersection(std::vector<T> v1, std::vector<T> v2){
    std::vector<T> v3;

    std::sort(v1.begin(), v1.end());
    std::sort(v2.begin(), v2.end());

    std::set_intersection(v1.begin(), v1.end(), v2.begin(), v2.end(), std::back_inserter(v3));
    return v3;
}

template<typename T>
std::vector<T> difference(std::vector<T> v1, std::vector<T> v2){
    std::vector<T> v3;

    std::sort(v1.begin(), v1.end());
    std::sort(v2.begin(), v2.end());

    std::set_difference(v1.begin(), v1.end(), v2.begin(), v2.end(), std::back_inserter(v3));
    return v3;
}
using Coordinate = std::pair<unsigned, unsigned>;
Coordinate makeCoordinate(unsigned int x, unsigned int y);
