idf_component_register(SRCS "web_encoder_motor.c"
                    INCLUDE_DIRS ".")
set(WEB_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../front/motor/dist")
if(EXISTS ${WEB_SRC_DIR})
    spiffs_create_partition_image(www ${WEB_SRC_DIR} FLASH_IN_PROJECT)
else()
    message(FATAL_ERROR "${WEB_SRC_DIR} doesn't exit.")
endif()