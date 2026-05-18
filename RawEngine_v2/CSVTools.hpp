#pragma once
#include <string>
#include <vector>


static class CSVTools {
public:
    static void WriteCSV(const std::vector<float>& frameTimes, const std::vector<int>& particleCounts, std::string name);
};