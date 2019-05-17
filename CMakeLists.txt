# The project is compiled into a main shared library and an additional one that
# implements reading of PEC files. The libraries are kept in the source tree
# (i.e. no installation procedure is implemented). The CMake targets for the
# generated libraries are exported.

cmake_minimum_required(VERSION 3.11 FATAL_ERROR)
project(mensura VERSION 4 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Directory for produced shared libraries
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib")


# External dependencies
find_package(Boost 1.69 REQUIRED COMPONENTS filesystem)
find_package(ROOT 6 REQUIRED)


# Embedded external packages are compiled into convenience static libraries
# that are then included in the main shared library

# jsoncpp
add_library(jsoncpp STATIC
    src/external/JsonCpp/jsoncpp.cpp
)
target_include_directories(jsoncpp PUBLIC include)

# Convenience library to read b-tagging scale factors
add_library(btag STATIC
    src/external/BTagCalibration/BTagCalibration.cpp
    src/external/BTagCalibration/BTagCalibrationReader.cpp
    src/external/BTagCalibration/BTagEntry.cpp
)
target_include_directories(btag PUBLIC include)

# Jet calibration convenience library
add_library(jerc STATIC
    src/external/JERC/FactorizedJetCorrector.cpp
    src/external/JERC/JetCorrectionUncertainty.cpp
    src/external/JERC/JetCorrectorParameters.cpp
    src/external/JERC/JetResolution.cpp
    src/external/JERC/JetResolutionObject.cpp
    src/external/JERC/SimpleJetCorrectionUncertainty.cpp
    src/external/JERC/SimpleJetCorrector.cpp
)
target_link_libraries(jerc
    PRIVATE ROOT::GenVector ROOT::Hist
)

# Manually request that the convenience libraries above are compiled with
# option POSITION_INDEPENDENT_CODE so that they can be included in a shared
# library. In newer versions of CMake it should be possible to use object
# libraries instead and link against them, which would be a more elegant
# solution.
set_target_properties(jsoncpp btag jerc
    PROPERTIES POSITION_INDEPENDENT_CODE ON
)


# Main library
add_library(mensura SHARED
    src/AnalysisPlugin.cpp
    src/BasicKinematicsPlugin.cpp
    src/BTagEffHistograms.cpp
    src/BTagEffService.cpp
    src/BTagger.cpp
    src/BTagSFService.cpp
    src/BTagWeight.cpp
    src/BTagWeightCSVShape.cpp
    src/BTagWPService.cpp
    src/Config.cpp
    src/DatasetBuilder.cpp
    src/Dataset.cpp
    src/DatasetSelector.cpp
    src/EventCounter.cpp
    src/EventID.cpp
    src/EventIDFilter.cpp
    src/EventIDReader.cpp
    src/EventIDReminderFilter.cpp
    src/EventWeightPlugin.cpp
    src/FileInPath.cpp
    src/GeneratorReader.cpp
    src/GenJetMETReader.cpp
    src/GenParticle.cpp
    src/GenParticleReader.cpp
    src/GenWeightSyst.cpp
    src/JetCorrectorService.cpp
    src/JetFilter.cpp
    src/JetFunctorFilter.cpp
    src/JetMETReader.cpp
    src/JetMETUpdate.cpp
    src/JetResolution.cpp
    src/LeptonFilter.cpp
    src/LeptonReader.cpp
    src/LeptonSFWeight.cpp
    src/Logger.cpp
    src/LumiMaskFilter.cpp
    src/MetFilter.cpp
    src/PhysicsObjects.cpp
    src/PileUpReader.cpp
    src/PileUpWeight.cpp
    src/Plugin.cpp
    src/Processor.cpp
    src/ReaderPlugin.cpp
    src/ROOTLock.cpp
    src/RunManager.cpp
    src/Service.cpp
    src/SystService.cpp
    src/TFileService.cpp
    src/TriggerRange.cpp
    src/WeightCollector.cpp
)
target_include_directories(mensura PUBLIC include)
target_link_libraries(mensura
    PRIVATE
        jsoncpp btag jerc
    PUBLIC
        stdc++fs
        Boost::boost Boost::filesystem
    PUBLIC
        ROOT::Hist ROOT::MathCore ROOT::Physics ROOT::Tree ROOT::TreePlayer
)


# Library to read PEC files and the corresponding dictionary. The function that
# generates the dictionary relies on variable CMAKE_INSTALL_LIBDIR, which is
# not initialized. Set it manually.
include("${ROOT_USE_FILE}/../modules/RootNewMacros.cmake")
set(CMAKE_INSTALL_LIBDIR "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")
root_generate_dictionary(mensura-pec_dict
    src/PECReader/classes.h LINKDEF src/PECReader/classes_def.xml
    MODULE mensura-pec
)

add_library(mensura-pec SHARED
    src/PECReader/Candidate.cpp
    src/PECReader/CandidateWithID.cpp
    src/PECReader/Electron.cpp
    src/PECReader/EventID.cpp
    src/PECReader/GeneratorInfo.cpp
    src/PECReader/GenJet.cpp
    src/PECReader/GenParticle.cpp
    src/PECReader/Jet.cpp
    src/PECReader/Lepton.cpp
    src/PECReader/Muon.cpp
    src/PECReader/PECGeneratorReader.cpp
    src/PECReader/PECGenJetMETReader.cpp
    src/PECReader/PECGenParticleReader.cpp
    src/PECReader/PECInputData.cpp
    src/PECReader/PECJetMETReader.cpp
    src/PECReader/PECLeptonReader.cpp
    src/PECReader/PECPileUpReader.cpp
    src/PECReader/PECTriggerFilter.cpp
    src/PECReader/PECTriggerObjectReader.cpp
    src/PECReader/PileUpInfo.cpp
    src/PECReader/ShowerParton.cpp
    "${CMAKE_BINARY_DIR}/mensura-pec_dict.cxx"
)
target_include_directories(mensura-pec PUBLIC include)
target_link_libraries(mensura-pec
    PUBLIC mensura
)


# Export targets to be used in dependent projects
export(TARGETS mensura mensura-pec
    NAMESPACE mensura::
    FILE "${CMAKE_SOURCE_DIR}/cmake/mensuraTargets.cmake"
)

