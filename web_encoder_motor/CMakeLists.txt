# For more information about build system see
# https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/build-system.html
# The following five lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)
list(APPEND EXTRA_COMPONENT_DIRS 
                                "components"
                                "components/pcnt_controller"  
                                "components/uart_sender" 
                                "components/pwm_controller" 
                                "components/timer_controller" 
                                "components/PID_controller"
                                "components/wifi_connect"
                                "components/simple_server"
                                )
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(web_encoder_motor)
