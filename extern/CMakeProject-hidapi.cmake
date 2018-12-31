set(HIDAPI_DIR "${SM_EXTERN_DIR}/hidapi/")
set(HIDAPI_INCLUDE_DIR "${HIDAPI_DIR}hidapi/")

if(APPLE)
  set(HIDAPI_SRC "${HIDAPI_DIR}mac/hid.c")
elseif(MSVC)
  set(HIDAPI_SRC "${HIDAPI_DIR}windows/hid.c")
elseif(LINUX)
  set(HIDAPI_SRC "${HIDAPI_DIR}linux/hid.c")
endif()

set(HIDAPI_HPP "${HIDAPI_INCLUDE_DIR}hidapi.h")

source_group("Source Files" FILES ${HIDAPI_SRC})
source_group("Header Files" FILES ${HIDAPI_HPP})

add_library("hidapi" STATIC ${HIDAPI_SRC} ${HIDAPI_HPP})

set_property(TARGET "hidapi" PROPERTY FOLDER "External Libraries")

target_include_directories("hidapi" PUBLIC "${HIDAPI_INCLUDE_DIR}")