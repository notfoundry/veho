set(BOOST_ROOT third_party)
set(BOOST_INCLUDEDIR  third_party/boost)

SET(Boost_USE_STATIC_LIBS ON)

find_package(Boost 1.66 REQUIRED)
if (Boost_FOUND)
    message(STATUS "Found Boost")

    target_include_directories(veho INTERFACE ${Boost_INCLUDE_DIRS})
else()
    message(ERROR "Did not find Boost")
endif()