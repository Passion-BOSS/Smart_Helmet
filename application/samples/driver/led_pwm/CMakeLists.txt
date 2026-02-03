#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
#===============================================================================

# 直接添加源文件（海思标准写法2）
set(SOURCES "${SOURCES}" "${CMAKE_CURRENT_SOURCE_DIR}/led_pwm.c" PARENT_SCOPE)