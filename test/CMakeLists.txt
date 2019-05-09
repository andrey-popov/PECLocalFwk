cmake_minimum_required(VERSION 3.11 FATAL_ERROR)
project(mensura-tests VERSION 4 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Directory for produced executables
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")

find_package(mensura)

add_executable(btag-efficiencies src/btag-efficiencies.cpp)
target_link_libraries(btag-efficiencies PRIVATE mensura::mensura)

add_executable(btag-scale-factors src/btag-scale-factors.cpp)
target_link_libraries(btag-scale-factors PRIVATE mensura::mensura)

add_executable(jet-corrections src/jet-corrections.cpp)
target_link_libraries(jet-corrections
    PRIVATE mensura::mensura mensura::mensura-pec
)

add_executable(jet-met-recorrection src/jet-met-recorrection.cpp)
target_link_libraries(jet-met-recorrection
    PRIVATE mensura::mensura mensura::mensura-pec
)

add_executable(jet-met-systematics src/jet-met-systematics.cpp)
target_link_libraries(jet-met-systematics
    PRIVATE mensura::mensura mensura::mensura-pec
)

add_executable(lepton-scale-factors src/lepton-scale-factors.cpp)
target_link_libraries(lepton-scale-factors
    PRIVATE mensura::mensura mensura::mensura-pec
)

add_executable(minimal src/minimal.cpp)
target_link_libraries(minimal
    PRIVATE mensura::mensura mensura::mensura-pec
)

add_executable(multithread src/multithread.cpp)
target_link_libraries(multithread
    PRIVATE mensura::mensura mensura::mensura-pec
)

