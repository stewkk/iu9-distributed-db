add_executable(iu9-db main.cpp)
target_compile_features(iu9-db PRIVATE cxx_std_20)
set_target_properties(iu9-db PROPERTIES
  CXX_STANDART 20
  CXX_STANDART_REQUIRED YES
  CXX_EXTENSIONS YES
)
target_compile_options(iu9-db PRIVATE ${BASE_COMPILE_FLAGS})
target_link_libraries(iu9-db PRIVATE)
