# PAJ7620软件包

## 1、介绍

PAJ7620 软件包是对PAJ7620手势识别芯片的模块的支持包

### 1.1 许可证

PAJ7620 package 遵循 Apache-2.0 许可，详见 `LICENSE` 文件。

### 1.2 依赖

- RT-Thread 4.0+

## 2、如何打开 PAJ7620
使用 PAJ7620 package 需要先使用 `pkgs --upgrade` 更新包列表

然后在 RT-Thread 的包管理器中选择它，具体路径如下：

```
RT-Thread online packages
    peripheral libraries and drivers --->
        [*] PAJ7620: a gesture detection module --->
            [*]   enable samples
                  Version
```

然后让 RT-Thread 的包管理器自动更新，或者使用 `pkgs --update` 命令更新包到 BSP 中。

## 3、联系方式 & 感谢

* 维护：orange2348
* 主页：<https://github.com/orange2348>
* 邮箱：<orange2348@163.com>