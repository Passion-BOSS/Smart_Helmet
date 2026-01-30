# 注意事项

## 1.项目存放路径
本项目文件夹为Smart_Helmet，需要放在application文件夹下

## 2.application/Kconfig,application/CMakeLists.txt,build/config/target_config/ws63/config.py配置
**需要在application/Kconfig中添加如下代码：**
```python
config ENABLE_SMART_HELMET
    bool
    prompt "Enable Smart Helmet."
    default n
    help
        This option means support Smart Helmet.
```
**需要在application/CMakeLists.txt中添加如下代码：**
```C
add_subdirectory(Smart_Helmet)
```
**需要在build/config/target_config/ws63/config.py的"ram_component"的参数添加"Smart_Helmet"**
注意写的时候补一个逗号
这个参数完整版长这样：
```python
        'ram_component': [
            'ws63_liteos_app',
            'ws63_liteos_app_lds',
            'liteos_port',
            'irmalloc',
            'non_os',
            'arch_port',
            'board_config', # 被 pinctrl 组件依赖
            'lpm', # 被 pwm 组件依赖
            'chip_ws63', 'pmp_cfg_ws63',
            'reboot', 'hal_reboot', 'reboot_port', 'cpu_utils', 'hal_cpu_core',
            'gpio','hal_gpio_v150','gpio_port',
            "dfx_port_ws63", "algorithm", "cmn_header", "lwip", "lwip_tcm", "wifi_service", "mbedtls", "wpa_supplicant",
            "at", "wifi_driver_hmac", "wifi_driver_dmac", "wifi_driver_tcm", "wifi_at", "wifi_csa", "wifi_frag", "wifi_alg_txbf", "wifi_alg_temp_protect", "wifi_tx_amsdu",
            "wifi_auto_adjust_freq", "wifi_alg_anti_interference", "wifi_alg_edca_opt", "wifi_alg_cca_opt", "wifi_radar_sensor",
            'wifi_btcoex', "wifi_uapsd_ap", 'sio_port', 'i2s', 'hal_sio',
            'liteos_208_5_0',
            'nv', 'nv_ws63', 'nv_zdiag_ws63', 'plt_at', 'dfx_printer',
            'update_common', 'update_common_ws63', 'update_ab_ws63', 'factory_ws63', 'update_storage', 'update_storage_ws63',
            'pm_port_ws63',
            'bt_at',
            "bt_host",
            'bg_common',
            'bth_gle',
            'bth_sdk',
            'samples',
            'bts_header',
            'bt_app',
            'mips',
            'hal_mips',
            "bgtp",
            'soc_port',
            'radar_sensing',
            'radar_at',
            'radar_ai',
            "cjson",
            'xo_trim_port',
            "coap",
            "sle_netdev", "chba_at",
            "tiot_driver",
            "mqtt",
            "Smart_Helmet" # 加一下这行，别忘了上一行的逗号
        ],
```

## 3.Git相关

### Git 分支管理
咱们有必要为项目创建两个分支，一人一个分支，由于一个人管的模块很多，所以分支以自己姓名首字母命名即可
所以正常情况一共三个分支：
- main：主分支，用于存放最终的代码
- xtl
- jlh
当每个人的一个功能开发完成后，需要及时将自己的分支合并到main和另一个人的分支，优先同步到main分支
当软硬件协同测试时，需要再新建一个test分支，用于记录用于测试的代码，之后要将测试的代码合并到main分支或者直接丢弃

### Git Commit 提交说明
虽然咱们对是合作项目，但其实对于两人来说，commit不应该有什么要求，只要随时能保证沟通即可
对于咱们的项目来说，commit仅仅为记录项目的变化，想回档时能及时回档
所以，commit时，只要能清楚地描述自己的改动即可
不过我还是建议重点标注一下**操作类型**、**操作范围**、**操作描述**这三个部分

举几个例子：

比如增加了一个关于GNSS定位模块的头文件，但是其中abc函数有待测试
可以写：（记得换行）
>Add gnss.h: 仍不完善
>TODO: 测试abc函数

比如今天为了和硬件对接，测试硬件有没有虚焊，于是加了一个完全没必要合并到main分支的分支，名字为test_xuhan，把主函数的代码都注释掉了，要记得删掉这个没用的分支
可以写：
>Test: 测试是否虚焊
>TODO: 记得删掉test_xuhan分支

## 4.其余说明
main文件夹下的代码为所有任务的代码，文件夹底下的CMakeLists.txt我没写东西，等咱创建了文件再补充
drivers文件夹下的代码为咱们自己写的驱动代码

我看官方给的东西还挺全的，open_source文件夹下都是咱们用得着的东西，尤其是cjson和7-zip，捡到宝了属于是