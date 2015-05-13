add_subdirectory(common)
add_subdirectory(sens)
add_subdirectory(sim)
add_subdirectory(tests)
add_library(salib $<TARGET_OBJECTS:common> $<TARGET_OBJECTS:sens>
  $<TARGET_OBJECTS:sim>)

target_link_libraries(salib sbml sundials_nvecserial sundials_cvode)

