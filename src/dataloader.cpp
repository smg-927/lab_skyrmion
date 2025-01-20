#include "dataloader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>

DataLoaderUPtr DataLoader::Create() {
    return DataLoaderUPtr(new DataLoader());
}

bool DataLoader::ReadFile(const std::string& filename, int x, int y, int z, int w) {
    // 파일 열기
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return false;
    }

    // 텍스트 파일에서 숫자 읽기
    std::vector<float> allNumbers;
    float number;
    while (file >> number) {
        allNumbers.push_back(number);
    }
    file.close();

    // 예상 데이터 크기 확인
    size_t expectedSize = static_cast<size_t>(x * y * z);
    if (allNumbers.size() < expectedSize) {
        std::cerr << "Warning: Not enough data in file. Expected " << expectedSize
                  << ", but got " << allNumbers.size() << std::endl;
        return false;
    }

    // 4차원 배열의 크기 초기화 (단 한 번만 호출되는 초기화 부분)
    if (m_data.empty()) {
        m_data.resize(x, std::vector<std::vector<std::vector<float>>>(
                            y, std::vector<std::vector<float>>(
                                   z, std::vector<float>(3, 0.0f)))); // 마지막 차원의 크기를 3으로 고정
    }

    // 데이터를 4차원 배열에 저장 (지정된 w 인덱스에 값 삽입)
    size_t idx = 0;
    for (int i = 0; i < x; i++) {
        for (int j = 0; j < y; j++) {
            for (int k = 0; k < z; k++) {
                if (idx < allNumbers.size()) {
                    m_data[i][j][k][w] = allNumbers[idx++];
                }
            }
        }
    }

    return true;
}

const std::vector<std::vector<std::vector<std::vector<float>>>>& DataLoader::GetData() const {
    return m_data;
}
