组件如果包含了driver的头文件，需要在CMakeLists.txt中加上driver，否则编译可能会报错文件找不到。
～～～
idf_component_register(SRCS "timer_controller.c"
                    REQUIRES driver
                    INCLUDE_DIRS "include")
～～～