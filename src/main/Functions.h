//
// Created by benji on 29/03/2020.
//

#ifndef CLONEDETECTOR_FUNCTIONS_H
#define CLONEDETECTOR_FUNCTIONS_H

#include <string>
#include <filesystem>
#include <fstream>
#include "Graph.h"

void drawCloneGroups(const std::vector<std::vector<CandidateClone>> &clone_groups, const std::string& directory);

void writeCloneGroups(const std::vector<std::vector<CandidateClone>> &clonegroups, const std::string& string);

void calculate_and_plot(const std::string& filename);

void calculate_and_plot_directory(const std::filesystem::path& directory);
#endif //CLONEDETECTOR_FUNCTIONS_H
