file(GLOB_RECURSE SOURCES "${MoxfieldExporter_SOURCE_DIR}/src/*.cpp")


add_executable(MoxfieldExporter ${SOURCES})
target_include_directories(MoxfieldExporter PUBLIC "${MoxfieldExporter_SOURCE_DIR}/src")
target_compile_features(MoxfieldExporter PUBLIC cxx_std_17)
target_link_libraries(MoxfieldExporter PRIVATE nlohmann_json cpr::cpr)