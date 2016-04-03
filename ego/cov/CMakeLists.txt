cmake_minimum_required(VERSION 2.8)

set(SOURCES
    cov.cpp
#    matern.cpp
#    maternard.cpp
#    materniso.cpp
#    seard.cpp
#    seiso.cpp
#    gaborard.cpp
#    gaboriso.cpp
	square_dist.cpp
    stationary_iso.cpp
    stationary_ard.cpp
    stationary_kernels.cpp
)

set(DIR_SRCS)
foreach(file ${SOURCES})
  set(DIR_SRCS ${DIR_SRCS} ${CMAKE_CURRENT_SOURCE_DIR}/${file})
endforeach()
set(EGO_SRCS ${EGO_SRCS} ${DIR_SRCS} PARENT_SCOPE)


