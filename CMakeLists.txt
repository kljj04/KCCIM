cmake_minimum_required(VERSION 3.20)
project(KCCIM C)

set(CMAKE_C_STANDARD 99)

# 윈도우 환경에서 .dll 빌드할 때 매크로 딲 주입!
add_definitions(-DKCCIM_EXPORTS)

# 헤더 파일 경로 지정
include_directories(include internal)

# C 코어 동적 라이브러리(kccim.dll) 빌드 타겟 지정
add_library(kccim SHARED
        src/automata.c
        src/table.c
        src/kccim.c
)

# 윈도우 환경일 경우 .dll 확장자 보장
set_target_properties(kccim PROPERTIES PREFIX "")