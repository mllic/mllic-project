
include_directories(${CMAKE_CURRENT_LIST_DIR})

file(GLOB DIRS LIST_DIRECTORIES true "mllic*")
foreach (DIR IN LISTS DIRS)
    add_subdirectory(${DIR})
endforeach ()
