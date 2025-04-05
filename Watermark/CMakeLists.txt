set(ENV_ECK_INCLUDE $ENV{ECK_INCLUDE})
set(ENV_ECK_LIB $ENV{ECK_INCLUDE})

add_executable(Watermark WIN32 
	"Watermark.cpp"
	"pch.cpp"
	"MyEck/MyEck.cpp"
	"CWndMain.cpp"
	"1.manifest"
	"CWndOptions.h"
	"CWndOptions.cpp"
 "GlobalOptions.h" "GlobalOptions.cpp")

set_property(TARGET Watermark PROPERTY
	LINK_FLAGS "/MANIFESTUAC:level='requireAdministrator'"
)

target_precompile_headers(Watermark PRIVATE "pch.h")

include_directories(
	${ENV_ECK_INCLUDE}
)

target_link_directories(Watermark PRIVATE
	${ENV_ECK_LIB}
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
	set_property(TARGET Watermark PROPERTY CXX_STANDARD 20)
endif()