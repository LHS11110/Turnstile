# Turnstile

Turnstile은 현대적인 C++로 구현된 **System LK**(고전 시퀀트 계산법, Classical Sequent Calculus) 기반의 증명 도우미(Proof Assistant)입니다.

## 프로젝트 구조

- `src/`: 소스 파일 (.cpp)
- `include/`: 헤더 파일 (.hpp)
- `tests/`: 유닛 테스트
- `build/`: 빌드 디렉터리 (CMake에 의해 생성됨)

## 사전 요구 사항

- C++20을 지원하는 컴파일러 (GCC, Clang 또는 MSVC)
- CMake 3.14 이상

## 프로젝트 빌드 방법

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

## 실행 방법

```bash
./turnstile
```
