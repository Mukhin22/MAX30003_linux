cmake_minimum_required (VERSION 2.8.1)

######## Project settings ########
PROJECT(yocto_try)
SET(LICENSE "TBD")

######## Build and include settings ########
include_directories(
	inc
)

link_directories(
	${LINK_DIRECTORIES}
)


file(GLOB SOURCES
	"src/*.c"
)

add_executable(
	yocto_try

	${SOURCES}
)

TARGET_LINK_LIBRARIES(
	yocto_try
)

######## Install targets ########
INSTALL(TARGETS yocto_try
	RUNTIME DESTINATION usr/bin
)
