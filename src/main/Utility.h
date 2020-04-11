//
// Created by benji on 05/11/2019.
//

#ifndef CLONEDETECTOR_UTILITY_H
#define CLONEDETECTOR_UTILITY_H

#endif //CLONEDETECTOR_UTILITY_H

#include <vector>
#include <algorithm>
#include <map>
#include <set>

// Simple override for convoluted one-liner
template<typename T>
bool contains(const std::vector<T>& vector, const T& item){
    return std::find(vector.begin(), vector.end(), item) != vector.end();
}

template<typename T>
bool contains(const std::set<T>& set, const T& item){
    return set.find(item) != set.end();
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
std::set<T> intersection(std::set<T> s1, std::set<T> s2){
    std::set<T> s3;

    std::set_intersection(s1.begin(), s1.end(), s2.begin(), s2.end(), std::inserter(s3, s3.begin()));
    return s3;
}

// Call by value, to not change the original vectors, these are typically low size vectors
template<typename T>
std::vector<T> difference(std::vector<T> v1, std::vector<T> v2){
    std::vector<T> v3;

    std::sort(v1.begin(), v1.end());
    std::sort(v2.begin(), v2.end());

    std::set_difference(v1.begin(), v1.end(), v2.begin(), v2.end(), std::back_inserter(v3));
    return v3;
}

template<typename T>
std::set<T> difference(std::set<T> s1, std::set<T> s2){
    std::set<T> s3;

    std::set_difference(s1.begin(), s1.end(), s2.begin(), s2.end(), std::inserter(s3, s3.begin()));
    return s3;
}

template<typename T>
std::set<T> merge(std::set<T> s1, std::set<T> s2){
    std::set<T> s3;

    std::set_union(s1.begin(), s1.end(), s2.begin(), s2.end(), std::inserter(s3, s3.begin()));
    return s3;
}

struct Coordinate{
    unsigned x;
    unsigned y;

    Coordinate()=default;

    Coordinate(unsigned x, unsigned y){ this->x = x; this->y = y;}

    explicit operator std::string() const{ return "(" + std::to_string(x) + ", " + std::to_string(y) + ")";}

    bool operator<(const Coordinate& coord) const{
        return x < coord.x || (x == coord.x && y < coord.y);
    }

    bool operator==(const Coordinate& coord) const{
        return x == coord.x && y == coord.y;
    }
};

Coordinate makeCoordinate(unsigned int x, unsigned int y);

template<typename T>
std::size_t totalSize(std::vector<std::vector<T>> vec_of_vec) {
    std::size_t total = 0;
    for(auto vec : vec_of_vec)
        total += vec.size();
    return total;
}
