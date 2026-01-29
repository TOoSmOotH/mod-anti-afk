# mod-anti-afk-logout CMake configuration

set(MOD_ANTI_AFK_LOGOUT_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)

message(STATUS "WORLD: Loading mod-anti-afk-logout")

# Collect all source files
CollectSourceFiles(
  ${MOD_ANTI_AFK_LOGOUT_SOURCE_DIR}
  PRIVATE_SOURCES
)

# Check if we're building for worldserver
if(SERVERS)
  list(APPEND PRIVATE_SOURCES_LIST ${PRIVATE_SOURCES})
endif()

AddCXXFiles("${PRIVATE_SOURCES}")

# Copy configuration file
CopyModuleConfig("mod_anti_afk_logout.conf.dist")

message(STATUS "WORLD: mod-anti-afk-logout loaded")
