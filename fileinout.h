#ifndef FILEINOUT_H
#define	FILEINOUT_H

#include <iostream>
#include <fstream>
#include <sstream>
std::vector<double> readDataFromFile(const std::string& filename) {
    std::vector<double> data;
    std::ifstream file(filename);

    if (file.is_open()) {
        std::string line;

        while (std::getline(file, line)) {
            // 将一行数据分割成多个数字
            std::istringstream iss(line);
            double value;

            while (iss >> value) {
                data.push_back(value);
            }
        }

        file.close();
    } else {
        std::cout << "Failed to open the file: " << filename << std::endl;
    }

    return data;
}

void writeDataToFile(const std::vector<double>& data, const std::string& filename) {
    std::ofstream file(filename);

    if (file.is_open()) {
        for (double value : data) {
            file << value << std::endl;
        }

        file.close();
        std::cout << "Data has been written to the file: " << filename << std::endl;
    } else {
        std::cout << "Failed to open the file: " << filename << std::endl;
    }
}

#endif