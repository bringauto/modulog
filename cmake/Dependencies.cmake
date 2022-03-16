INCLUDE(FetchContent)
FIND_PACKAGE(CMLIB REQUIRED)

LIST(APPEND CMAKE_FIND_ROOT_PATH ${CMAKE_BINARY_DIR}) #TODO if all with cmlib/system dep -> append depending to it
LIST(APPEND CMAKE_FIND_ROOT_PATH ${CMLIB_REQUIRED_ENV_TMP_PATH})


FUNCTION(ADD_DEP_BRINGAUTO_LOGGER)
    # TODO cmlib not working with crosscompile - wait for fix and then integrate + in the future integrate with fetchContent
    SET(BALOGGER_ZIP none)
    IF(${CMAKE_SYSTEM_PROCESSOR} STREQUAL "aarch64")
        message("Architecture: aarch ${CMAKE_SYSTEM_PROCESSOR}")
        SET(BALOGGER_ZIP ${CMAKE_SOURCE_DIR}/lib/ba-logger/libbringauto_logger-dev_v1.1.0_aarch64-ubuntu-1804.zip)
    ELSE()
        message("Architecture: else ${CMAKE_SYSTEM_PROCESSOR}")
        SET(BALOGGER_ZIP ${CMAKE_SOURCE_DIR}/lib/ba-logger/libbringauto_logger-dev_v1.1.0_x86-64-ubuntu-2004.zip)
    ENDIF()
    EXECUTE_PROCESS(
            COMMAND ${CMAKE_COMMAND} -E tar xzf  ${BALOGGER_ZIP}
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )
    FIND_PACKAGE(libbringauto_logger PATHS ${CMAKE_BINARY_DIR})

    IF(BRINGAUTO_INSTALL)
        INSTALL(IMPORTED_RUNTIME_ARTIFACTS bringauto_logger::bringauto_logger_spdlog DESTINATION lib)
    ENDIF()
ENDFUNCTION()

FUNCTION(ADD_DEP_NLOHMANN_JSON)
    # ----------- nlohmann json library
    IF(BRINGAUTO_SYSTEM_DEP)
        FetchContent_Declare(json
                URL https://github.com/nlohmann/json/archive/refs/tags/v3.9.1.tar.gz
                )

        FetchContent_GetProperties(json)
        IF (NOT json_POPULATED)
            FetchContent_Populate(json)
            ADD_SUBDIRECTORY(${json_SOURCE_DIR} ${json_BINARY_DIR} EXCLUDE_FROM_ALL)
        ENDIF ()
    ELSE()
        CMLIB_DEPENDENCY(
                URI "https://github.com/bringauto/nlohmann_json-package.git"
                URI_TYPE GIT
                GIT_TAG v3.9.1
                TYPE MODULE
        )
        FIND_PACKAGE(nlohmann_json_package REQUIRED)
        FIND_PACKAGE(nlohmann_json)
    ENDIF()
ENDFUNCTION()


FUNCTION(ADD_DEP_ASIO)
    FetchContent_Declare(asio
            GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
            GIT_TAG asio-1-20-0
            CONFIGURE_COMMAND ""
            BUILD_COMMAND ""
            )
    FetchContent_GetProperties(asio)
    IF (NOT asio_POPULATED)
        FetchContent_Populate(asio)
    ENDIF ()
    ADD_LIBRARY(asio INTERFACE)
    TARGET_INCLUDE_DIRECTORIES(asio INTERFACE ${asio_SOURCE_DIR}/asio/include)
    FIND_PACKAGE(Threads)
    TARGET_LINK_LIBRARIES(asio INTERFACE Threads::Threads)
ENDFUNCTION()

FUNCTION(ADD_DEP_REPROC)
    SET(REPROC++ ON)
    FetchContent_Declare(
            reproc++
            GIT_REPOSITORY https://github.com/DaanDeMeyer/reproc.git #https://github.com/hofin34/reproc.git
            GIT_TAG v14.2.4 #  0bce0977bc3fa9ae10193607f3e27ad04e8aab94
    )
    FetchContent_MakeAvailable(reproc++)
ENDFUNCTION()

FUNCTION(ADD_DEP_STATE_SMURF)
    # used just when testing enabled
    FetchContent_Declare(
            statesmurf
            GIT_REPOSITORY git@github.com:Melky-Phoe/StateSmurf.git # TODO https
            GIT_TAG v0.1.0
    )
ENDFUNCTION()



FUNCTION(ADD_DEP_CXXOPTS)
    IF(BRINGAUTO_SYSTEM_DEP)
        FetchContent_Declare(
                cxxopts
                GIT_REPOSITORY https://github.com/jarro2783/cxxopts.git
                GIT_TAG        v3.0.0
                GIT_SHALLOW    TRUE
        )
        SET(CXXOPTS_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
        SET(CXXOPTS_BUILD_TESTS OFF CACHE BOOL "" FORCE)
        SET(CXXOPTS_ENABLE_INSTALL OFF CACHE BOOL "" FORCE)
        SET(CXXOPTS_ENABLE_WARNINGS OFF CACHE BOOL "" FORCE)
        FetchContent_MakeAvailable(cxxopts)
    ELSE()
        CMLIB_DEPENDENCY(
                URI "https://github.com/bringauto/cxxopts-package.git"
                URI_TYPE GIT
                GIT_TAG v3.0.0
                TYPE MODULE
        )
        FIND_PACKAGE(cxxopts_package 3.0.0 REQUIRED)
        FIND_PACKAGE(cxxopts)
    ENDIF()
ENDFUNCTION()

ADD_DEP_BRINGAUTO_LOGGER()
ADD_DEP_NLOHMANN_JSON()
ADD_DEP_ASIO()
ADD_DEP_REPROC()
ADD_DEP_STATE_SMURF()
ADD_DEP_CXXOPTS()