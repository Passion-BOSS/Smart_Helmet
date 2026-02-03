#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
#===============================================================================

# 无条件添加 serial1（因为你的main.c需要它）
add_subdirectory_if_exist(serial1)

    add_subdirectory_if_exist(button)

    add_subdirectory_if_exist(led_pwm)
# 传递给父目录
set(SOURCES "${SOURCES}" PARENT_SCOPE)