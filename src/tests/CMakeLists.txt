add_executable(test_sens testsens.cpp TestFrameworks.cpp)
target_link_libraries(test_sens salib)

add_executable(test_sim sim.cpp)
target_link_libraries(test_sim salib)
