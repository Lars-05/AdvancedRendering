#include "CSVTools.hpp"
#include <fstream>
#include <iostream>
#include <string>


void CSVTools::WriteCSV(const std::vector<float>& frameTimes, const std::vector<int>& particleCounts, std::string name)
{
    std::string filename = "../Data/";
    filename += name;
    filename += ".csv";

    std::ofstream file(filename);

    if (!file) {
        std::cerr << "Error: Could not open " << filename << " for writing.\n";
        return;
    }

    // Write header
    file << "Frame;DeltaTime(ms);FPS(1.0 / deltaTime);Particle Count(int)\n";

    // Write average values
    float averageDeltaTime = 0;
    for (int i = 0; i < frameTimes.size(); i++) {
        averageDeltaTime += frameTimes[i];
    }
    averageDeltaTime /= frameTimes.size();
    file << "Average" << ";"<< averageDeltaTime << ";"<< static_cast<int>(std::round(1.0f / averageDeltaTime)) << "\n";


    /// Write Performance Data
    size_t count = std::min(frameTimes.size(), particleCounts.size());

    for (size_t i = 0; i < count; i++)
    {
        float deltaTime = frameTimes[i];
        int fps = (deltaTime > 0.0f)? static_cast<int>(std::round(1.0f / deltaTime)): 0;
        int particleCount = particleCounts[i];

        file << i << ";"<< deltaTime * 1000 << ";"<< fps << ";"<< particleCount<< "\n";
    }

    std::cout << "Saved " << frameTimes.size() << " frames to " << filename << "\n";
}