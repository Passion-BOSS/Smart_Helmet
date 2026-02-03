#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
#===============================================================================

# 无条件添加你的任务目录
add_subdirectory_if_exist(task1_led)
add_subdirectory_if_exist(task2_gassensor)
add_subdirectory_if_exist(task3_hallsensor)

# 传递给父目录
set(SOURCES "${SOURCES}" PARENT_SCOPE)