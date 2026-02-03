#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
#===============================================================================

# 无条件添加你的传感器目录
add_subdirectory_if_exist(gassensor)
add_subdirectory_if_exist(hallsensor)
add_subdirectory_if_exist(flashlight)
# 传递给父目录
set(SOURCES "${SOURCES}" PARENT_SCOPE)