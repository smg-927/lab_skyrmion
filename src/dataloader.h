#ifndef __DATALOADER_H__
#define __DATALOADER_H__

#include <vector>
#include <string>
#include <memory>

#include "common.h"

CLASS_PTR(DataLoader);
class DataLoader {
public:
    // 정적 팩토리 메서드로 DataLoader 객체 생성
    static DataLoaderUPtr Create();
    
    // 텍스트 파일에서 데이터를 읽어와 4차원 배열에 저장
    bool ReadFile(const std::string& filename, glm::vec3 datalength, int w);
    bool ReduceAndAverage();

    // 데이터 접근 메서드
    const std::vector<std::vector<std::vector<std::vector<float>>>>& GetData() const;


private:
    DataLoader() {} // 생성자는 private으로 감춤

    // 데이터를 저장할 4차원 배열
    std::vector<std::vector<std::vector<std::vector<float>>>> m_data;
};

#endif // __DATALOADER_H__
