# lab_skyrmion
 
> **OpenGL-based 3D Interactive Visualizer for Skyrmion Simulation Data**  
> 숭실대학교 물리학과 X-ray Resonant Scattering Lab — 학부연구생 프로젝트
 
---
 
## Overview
 
물질 내부에 전기장을 인가했을 때 나타나는 **Skyrmion 구조**(자기 소용돌이 위상 구조)의 시뮬레이션 결과를 3D로 시각화하는 인터랙티브 뷰어입니다.
 
기존의 Python 기반 시각화는 X/Y/Z 단면(슬라이스)만 표시할 수 있어 3차원 전체 구조 파악이 어렵다는 한계가 있었습니다.  
이 프로젝트는 OpenGL을 활용해 분극 벡터 필드를 3D 화살표로 렌더링하고, 실시간으로 시점·범위·스케일을 조작할 수 있는 연구용 시각화 도구를 구현했습니다.
 
| 기존 방식 (Python) | 본 프로젝트 (OpenGL) |
|---|---|
| X/Y/Z 축 슬라이스 단면만 표시 | 3D 벡터 필드 전체 렌더링 |
| 정적 이미지 출력 | 실시간 인터랙티브 조작 |
| 구조 전체 파악 불가 | 시점·범위·스케일 자유 조작 |
 
---
 
## Demo
 
| 벡터 최적화 OFF | 벡터 최적화 ON |
|---|---|
| ![off](image/off.png) | ![on](image/on.png) |
 
> *최적화 ON 시 2×2×2 블록 평균화로 렌더링 부하를 대폭 감소*
 
---
 
## Tech Stack
 
| Category | Stack |
|---|---|
| Rendering | OpenGL, GLSL (Vertex / Fragment Shader) |
| Math | GLM (행렬·벡터 연산) |
| UI | Dear ImGui |
| Windowing | GLFW |
| Build | CMake (Windows / Linux 크로스플랫폼) |
| Data preprocessing | Python, Jupyter Notebook |
| Version control | Git, GitHub |
 
---
 
## Key Features
 
### 1. 데이터 파싱 파이프라인
Jupyter Notebook을 활용해 시뮬레이션 원본 txt 파일(약 **300만 개** 수치)에서 X/Y/Z 방향벡터만 추출·저장하여 로딩 속도를 최적화했습니다.
 
### 2. 직교좌표계 → 구면좌표계 변환 (`normalizeandrot`)
분극 벡터를 3D 화살표 모델에 올바르게 렌더링하기 위해 방향벡터를 회전 행렬로 변환하는 알고리즘을 직접 구현했습니다.
 
```
θ_raw = √(x²+z²) / √(x²+y²+z²)
φ     = acos(x / √(x²+z²)) × 180/π
 
y ≥ 0  →  θ = +acos(θ_raw) × 180/π
y < 0  →  θ = -acos(θ_raw) × 180/π
z < 0  →  φ = -φ
 
R_phi   = glm::rotate(I, -radians(φ),    Y축)
R_theta = glm::rotate(I,  radians(θ)×1.5, Z축)
return R_phi × R_theta
```
 
### 3. 렌더링 성능 최적화 (`ReduceAndAverage`)
데이터 인덱스 크기가 커질수록 발생하는 프레임 드롭을 해결하기 위해, **2×2×2 블록 단위로 벡터를 평균화**하여 데이터 포인트 수를 자동으로 줄이는 알고리즘을 구현했습니다.
 
```cpp
// 원본 배열 크기 (x, y, z) → 축소 배열 (x/2, y/2, z/2)
// 각 2×2×2 블록 내 8개 벡터의 방향 평균 → 대표 벡터 1개로 압축
newX = x / 2;  newY = y / 2;  newZ = z / 2;
// 블록별 avgDirection 누적 후 count로 나누어 저장
```
 
### 4. ImGui 실시간 제어 패널
실행 중 아래 파라미터를 즉시 수정할 수 있습니다.
 
| 항목 | 설명 |
|---|---|
| `clear color` | 배경 RGBA |
| `camera pos / yaw / pitch` | 3D 시점 제어 |
| `Range Control (X/Y/Z)` | 렌더링할 인덱스 범위 슬라이싱 |
| `Arrow Scale` | 화살표 크기 |
| `light` | 조명 position / direction / cutoff / distance / ambient / diffuse / specular |
| `flash light / animation` | 특수 조명·애니메이션 토글 |
| `Color Mode` | XY axis / Y plane / Z axis 색상 모드 전환 |
 
### 5. 크로스플랫폼 빌드 환경 (CMake)
시뮬레이션 서버(Linux)와 개발 환경(Windows) 모두에서 빌드·실행 가능하도록 `CMake ExternalProject_Add`를 사용해 의존성을 자동으로 설치·구성했습니다.
 
---
 
## Project Structure
 
```
lab_skyrmion/
├── src/
│   ├── main.cpp
│   ├── context.cpp / context.h   # 렌더링 컨텍스트, normalizeandrot, ReduceAndAverage
│   ├── buffer.cpp / buffer.h     # VAO / VBO 관리
│   ├── dataloader.cpp            # 시뮬레이션 데이터 파싱
│   ├── framebuffer.cpp
│   └── common.cpp
├── shader/                       # GLSL vertex / fragment shaders
├── imgui/                        # Dear ImGui 소스
├── model/                        # 화살표 3D 모델
├── Data/                         # 전처리된 시뮬레이션 데이터
├── CMakeLists.txt
└── Dependency.cmake              # 외부 라이브러리 빌드 설정
```
 
---
 
## Build & Run
 
### Requirements
- CMake ≥ 3.14
- C++17 호환 컴파일러 (MSVC / GCC / Clang)
- OpenGL 3.3+
### Windows
 
```bash
mkdir build && cd build
cmake ..
cmake --build . --config Debug
./Debug/opengl_example.exe
```
 
### Linux
 
```bash
mkdir build && cd build
cmake ..
make -j$(nproc)
./opengl_example
```
 
> 외부 라이브러리(GLFW, GLAD, GLM, assimp 등)는 `Dependency.cmake`를 통해 빌드 시 자동으로 설치됩니다.
 
---
 
## Background
 
**숭실대학교 물리학과 X-ray Resonant Scattering Lab** ([Dong Ryeol Lee Group](https://drleegroup.site123.me/))에서 학부연구생으로 참여한 프로젝트입니다.
 
Skyrmion은 자성 물질 내에서 전기장 등 외부 자극에 의해 형성되는 위상학적으로 안정된 스핀 소용돌이 구조입니다. 본 시각화 도구는 수치 시뮬레이션 결과물을 연구자가 직관적으로 탐색할 수 있도록 제작되었습니다.
