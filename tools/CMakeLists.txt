cmake_minimum_required (VERSION 3.1)
project(tools VERSION 1.0)

add_custom_target(bin2cpp ALL
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/bin2cpp/bin2cpp.py
)

set_property(TARGET bin2cpp PROPERTY EXEC ${CMAKE_CURRENT_SOURCE_DIR}/bin2cpp/bin2cpp.py)

add_custom_target(hashgen ALL
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/hashgen/hashgen.py
)

set_property(TARGET hashgen PROPERTY EXEC ${CMAKE_CURRENT_SOURCE_DIR}/hashgen/hashgen.py)

add_custom_target(psppack ALL
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/psppack/psppack.py
)

set_property(TARGET psppack PROPERTY EXEC ${CMAKE_CURRENT_SOURCE_DIR}/psppack/psppack.py)

add_custom_target(pack_user_fw_module ALL
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/psptools/pack_user_fw_module.py
)

set_property(TARGET pack_user_fw_module PROPERTY EXEC ${CMAKE_CURRENT_SOURCE_DIR}/psptools/pack_user_fw_module.py)


add_custom_target(pack_kernel_module ALL
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/psptools/pack_kernel_module.py
)

set_property(TARGET pack_kernel_module PROPERTY EXEC ${CMAKE_CURRENT_SOURCE_DIR}/psptools/pack_kernel_module.py)

add_custom_target(pack_updater ALL
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/psptools/pack_updater.py
)

set_property(TARGET pack_updater PROPERTY EXEC ${CMAKE_CURRENT_SOURCE_DIR}/psptools/pack_updater.py)
