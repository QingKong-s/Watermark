set(ENV_ECK_INCLUDE $ENV{ECK_INCLUDE})
set(ENV_ECK_LIB $ENV{ECK_INCLUDE})

add_executable(Watermark WIN32 
	"Watermark.cpp"
	"pch.cpp"
	"MyEck/MyEck.cpp"
	"CWndMain.cpp"
	"CWndOptions.cpp"
	"GlobalOptions.cpp"
	"1.manifest"
	"Res/Resource.rc"
)

target_compile_definitions(Watermark PRIVATE
	ECK_OPT_NO_DX=1
	ECK_OPT_NO_YYJSON=1
	ECK_OPT_NO_PUGIXML=1
)

set_property(TARGET Watermark PROPERTY
	LINK_FLAGS "/MANIFESTUAC:level='requireAdministrator'"
)
list(APPEND CMAKE_RC_STANDARD_INCLUDE_DIRECTORIES 
	CMAKE_C_STANDARD_INCLUDE_DIRECTORIES
	CMAKE_C_STANDARD_INCLUDE_DIRECTORIES
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