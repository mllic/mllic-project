function(add_mllic_library target language)
    foreach (SOURCE ${ARGN})
        string(REGEX REPLACE "[.][^.]*$" ".cir" CIR "${SOURCE}")
        string(REGEX REPLACE "[.][^.]*$" ".mlir" MLIR "${SOURCE}")
        string(REGEX REPLACE "[.][^.]*$" ".ll" LL "${SOURCE}")
        string(REGEX REPLACE "^${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}" CIR "${CIR}")
        string(REGEX REPLACE "^${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}" MLIR "${MLIR}")
        string(REGEX REPLACE "^${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}" LL "${LL}")
        set(CIRS ${CIRS} "${CIR}")
        set(MLIRS ${MLIRS} "${MLIR}")
        set(LLS ${LLS} "${LL}")
    endforeach ()

    set(COMPILER "CMAKE_${language}_COMPILER")
    foreach (arg IN ZIP_LISTS CIRS MLIRS LLS ARGN)

        set(CIR "${arg_0}")
        set(MLIR "${arg_1}")
        set(LL "${arg_2}")
        set(INPUT "${arg_3}")

        add_custom_command(
                OUTPUT ${LL}
                COMMAND ${${COMPILER}}
                -fplugin="${MLLIC_PATH}/mllic-c.so"
                -emit-cir
                $<TARGET_PROPERTY:${target},COMPILE_OPTIONS>
                "${INPUT}" -o "${CIR}"
                COMMAND cir-opt --cir-to-llvm "${CIR}" -o "${MLIR}"
                COMMAND mlir-translate --mlir-to-llvmir -allow-unregistered-dialect "${MLIR}" > "${LL}"
                DEPENDS "${INPUT}"
        )
    endforeach ()

    set(TARGET_PSM_FILE "$<TARGET_PROPERTY:${target},LIBRARY_OUTPUT_DIRECTORY>/$<TARGET_PROPERTY:${target},PREFIX>${target}.psm")
    set(ENSEMBLE_LL "$<TARGET_PROPERTY:${target},BINARY_DIR>/$<TARGET_PROPERTY:${target},PREFIX>${target}.ll")
    add_custom_target(${target}
            COMMAND clang -shared $<TARGET_PROPERTY:${target},LINK_OPTIONS> ${LLS}
            -o "$<TARGET_PROPERTY:${target},LIBRARY_OUTPUT_DIRECTORY>/$<TARGET_PROPERTY:${target},PREFIX>${target}$<TARGET_PROPERTY:${target},SUFFIX>"
            DEPENDS ${LLS}
    )
    set_target_properties(${target} PROPERTIES
            LIBRARY_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
            TARGET_PSM_FILE "${TARGET_PSM_FILE}"
            PREFIX "${CMAKE_SHARED_LIBRARY_PREFIX}"
            SUFFIX "${CMAKE_SHARED_LIBRARY_SUFFIX}"
    )
endfunction()
