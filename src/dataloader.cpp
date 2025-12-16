#include "dataloader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>

DataLoaderUPtr DataLoader::Create() {
    return DataLoaderUPtr(new DataLoader());
}

bool DataLoader::ReadFile(const std::string& filename, glm::vec3 datalength, int w) {
    // 파일 열기
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return false;
    }

    int x = datalength[0];
    int y = datalength[1];
    int z = datalength[2];

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

bool DataLoader::ReduceAndAverage() {
    // 원래 배열 크기
    int x = static_cast<int>(m_data.size());
    int y = static_cast<int>(m_data[0].size());
    int z = static_cast<int>(m_data[0][0].size());

    // 새로운 배열 크기 계산
    int newX = x / 2;
    int newY = y / 2;
    int newZ = z / 2;

    // 축소된 배열 생성
    std::vector<std::vector<std::vector<std::vector<float>>>> reducedData(
        newX, std::vector<std::vector<std::vector<float>>>(
                  newY, std::vector<std::vector<float>>(
                            newZ, std::vector<float>(3, 0.0f)))); // 마지막 차원의 크기는 3

    // 평균 계산 및 데이터 저장
    for (int i = 0; i < newX; i++) {
        for (int j = 0; j < newY; j++) {
            for (int k = 0; k < newZ; k++) {
                glm::vec3 avgDirection(0.0f);
                int count = 0;

                // 2x2x2 블록 내의 데이터를 평균 계산
                for (int dx = 0; dx < 2; dx++) {
                    for (int dy = 0; dy < 2; dy++) {
                        for (int dz = 0; dz < 2; dz++) {
                            int origX = i * 2 + dx;
                            int origY = j * 2 + dy;
                            int origZ = k * 2 + dz;

                            if (origX < x && origY < y && origZ < z) {
                                avgDirection.x += m_data[origX][origY][origZ][0];
                                avgDirection.y += m_data[origX][origY][origZ][1];
                                avgDirection.z += m_data[origX][origY][origZ][2];
                                count++;
                            }
                        }
                    }
                }

                // 평균값 저장
                if (count > 0) {
                    reducedData[i][j][k][0] = avgDirection.x / count;
                    reducedData[i][j][k][1] = avgDirection.y / count;
                    reducedData[i][j][k][2] = avgDirection.z / count;
                }
            }
        }
    }

    for (int i = 0; i < newX; i++) {
        for (int j = 0; j < newY; j++) {
            for (int k = 0; k < newZ; k++) {
                SPDLOG_INFO("{}", reducedData[i][j][k][0]);
            }
        }
    }
    // 기존 데이터 교체
    m_data = std::move(reducedData);
    return true;
}

const std::vector<std::vector<std::vector<std::vector<float>>>>& DataLoader::GetData() const {
    return m_data;
}

bool DataLoader::ExpandAndInterpolate() {
    // 원래 배열 크기
    int x = static_cast<int>(m_data.size());
    int y = static_cast<int>(m_data[0].size());
    int z = static_cast<int>(m_data[0][0].size());

    // 새로운 배열 크기 계산 (두 배로 확장)
    int newX = x * 2 - 1;  // 3->5, 4->7, 5->9
    int newY = y * 2 - 1;
    int newZ = z * 2 - 1;

    // 확장된 배열 생성
    std::vector<std::vector<std::vector<std::vector<float>>>> expandedData(
        newX, std::vector<std::vector<std::vector<float>>>(
                  newY, std::vector<std::vector<float>>(
                            newZ, std::vector<float>(3, 0.0f))));

    // 기존 데이터 복사 (짝수 인덱스에 배치)
    for (int i = 0; i < x; i++) {
        for (int j = 0; j < y; j++) {
            for (int k = 0; k < z; k++) {
                int newI = i * 2;
                int newJ = j * 2;
                int newK = k * 2;
                
                expandedData[newI][newJ][newK][0] = static_cast<float>(m_data[i][j][k][0]);
                expandedData[newI][newJ][newK][1] = static_cast<float>(m_data[i][j][k][1]);
                expandedData[newI][newJ][newK][2] = static_cast<float>(m_data[i][j][k][2]);
            }
        }
    }

    // 새로운 점들을 보간으로 계산 (홀수 인덱스에 배치)
    for (int i = 0; i < newX; i++) {
        for (int j = 0; j < newY; j++) {
            for (int k = 0; k < newZ; k++) {
                // 이미 기존 데이터가 있는 경우 건너뛰기
                if (i % 2 == 0 && j % 2 == 0 && k % 2 == 0) {
                    continue;
                }

                glm::vec3 interpolatedValue(0.0f);
                int count = 0;

                // 주변 8개 점의 평균 계산 (짝수 인덱스만 참조)
                for (int di = -1; di <= 1; di += 1) {
                    for (int dj = -1; dj <= 1; dj += 1) {
                        for (int dk = -1; dk <= 1; dk += 1) {
                            int neighborI = i + di;
                            int neighborJ = j + dj;
                            int neighborK = k + dk;

                            // 경계 확인
                            if (neighborI >= 0 && neighborI < newX && 
                                neighborJ >= 0 && neighborJ < newY &&
                                neighborK >= 0 && neighborK < newZ) {
                                
                                // 이웃 점이 기존 데이터인지 확인 (짝수 인덱스)
                                if (neighborI % 2 == 0 && neighborJ % 2 == 0 && neighborK % 2 == 0) {
                                    interpolatedValue.x += expandedData[neighborI][neighborJ][neighborK][0];
                                    interpolatedValue.y += expandedData[neighborI][neighborJ][neighborK][1];
                                    interpolatedValue.z += expandedData[neighborI][neighborJ][neighborK][2];
                                    count++;
                                }
                            }
                        }
                    }
                }

                // 평균값 저장
                if (count > 0) {
                    expandedData[i][j][k][0] = interpolatedValue.x / count;
                    expandedData[i][j][k][1] = interpolatedValue.y / count;
                    expandedData[i][j][k][2] = interpolatedValue.z / count;
                }
            }
        }
    }
    
    // 기존 데이터 교체
    m_data = std::move(expandedData);
    return true;
}

