cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(Muon)

option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
  find_package(Geant4 REQUIRED ui_all vis_all)
else()
  find_package(Geant4 REQUIRED)
endif()

include(${Geant4_USE_FILE})

find_package(ROOT REQUIRED)

include_directories(${PROJECT_SOURCE_DIR}/include
                    ${ROOT_INCLUDE_DIRS})


file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)

add_executable(MuonMain MuonMain.cc ${sources} ${headers})
target_link_libraries(MuonMain ${Geant4_LIBRARIES})
add_executable(ProcessTrajectories ProcessTrajectories.cc)
target_link_libraries(ProcessTrajectories ${ROOT_LIBRARIES})
add_executable(MuonTests MuonTests.cc)
target_link_libraries(MuonTests ${Geant4_LIBRARIES})

set(MUON_SCRIPTS
  init_vis.mac
  vis.mac
  muminus.mac
  muplus.mac
  )

foreach(_script ${MUON_SCRIPTS})
  configure_file(
    ${PROJECT_SOURCE_DIR}/${_script}
    ${PROJECT_BINARY_DIR}/${_script}
    COPYONLY
    )
endforeach()

add_custom_target(Muon DEPENDS MuonMain ProcessTrajectories MuonTests)

install(TARGETS MuonMain DESTINATION bin)
install(TARGETS ProcessTrajectories DESTINATION bin)
install(TARGETS MuonTests DESTINATION bin)


