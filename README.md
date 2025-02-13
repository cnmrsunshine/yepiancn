# 飞星StarFly：利用硬件断点Hook绕过安全软件用户模式检测的C语言实现

## 免责声明

### 本项目仅供学习和交流使用，请勿用于非法目的。开发者不承担任何责任。

## 理论验证 PoC

*   [飞星StarFly 最新版 - VirusTotal 98.7% 未检出](https://www.virustotal.com/gui/file/5a0a6fc86f76b3e75a998f6b76ec2aa98300754eb247dfe6908485835e749a10)
*   [飞星StarFly 最新版 - 微步云沙箱静态及动态扫描均未检出](https://s.threatbook.com/report/file/5a0a6fc86f76b3e75a998f6b76ec2aa98300754eb247dfe6908485835e749a10)
*   [飞星StarFly 最新版 - 奇安信天穹云沙箱行为检测未检出](https://sandbox.qianxin.com/tq/report/toViewReport.do?rid=a381cd0fc08e011226ff5aa801434822&sk=28483534)
*   [飞星StarFly 最新版 - 360云沙箱 威胁评分0](https://ata.360.net/report/656108077574144/static)

## 使用方式

> ### 接入飞星内核

*   将所有 `Nt*` 函数替换为 `SFNt*` 函数。详情请见注意事项。

> ### 直接使用飞星控制台

*   `ps <进程名(可选)>`: 输出全部或符合筛选条件的进程信息，支持关键字搜索（大小写敏感）。
*   `kill <目标进程PID>`: 终止指定进程。
*   `getsystem`: 本地权限提升至 `SYSTEM` （需要管理员权限）。
*   `steal <目标进程PID>`: 窃取并复制指定进程主令牌和模拟令牌
*   `respawn`: 使用复制的主令牌重启飞星控制台
*   `run <可执行文件绝对路径>`: 使用复制的主启动指定可执行文件。
*   `cmd`: 使用复制的主令牌启动cmd
*   `lang`: 切换语言（英语/简体中文）。
*   `startup <可执行文件绝对路径>`: 设置开机自启动 且添加的项在注册表不可见
*   `fkill <目标进程PID>`: 通过远程线程调度 注入dll 终止目标进程 可突破会话隔离

## 原理详解

> ### 飞星内核

*   **间接系统调用:** 通过两次重定向执行流至目标 `Nt*` 函数，伪造完整调用链，同时覆写调用参数，实现间接系统调用。
*   **系统调用动态解析:** 利用 PEB 寻址，动态解析 `NtDll.dll`，获取 `Nt*` 函数的系统调用地址和系统调用号。
*   **函数调用拦截:** 修改线程上下文以在无调试权限情况下访问调试寄存器，设置硬件断点，对执行流进行无痕 Hook。
*   **函数调用跳转:** 向量异常处理器捕获断点单步异常，劫持执行流并修改 RIP。
*   **函数调用注入:** 遵循 `_FastCall` 调用约定，设置传参相关寄存器和栈，传入实际参数。

> ### 飞星模组

*   `SFListProcess`: 调用 `NtQuerySystemInformation` 枚举并筛选进程信息。
*   `SFKillProcess`: 调用 `NtTerminateProcess` 终止指定进程。
*   `SFLocalPrivilege`: 自动扫描可访问的系统进程，窃取并复制其主令牌和模拟令牌。
*   `SFCreateProcess` `SFRespawn` `SFOpenCMD`: 调用 `NtCreateUserProcess` 创建指定映像的进程。
*   `SFCreateProcess2`: **暂未实现**的启动进程方案。调用 `NtCreateProcessEx` 创建空进程，`NtCreateSection` 和 `NtMapViewOfSection` 将可执行程序映像加载到目标进程。通过 `NtReadVirtualMemory` 解析 DOS 和 NT 文件头，计算找到入口点。**但是在调用 `NtCreateThreadEx` 创建线程时出现了未预期且难以修复的错误。**
*   `SFGetToken`: 窃取并复制指定进程主令牌和模拟令牌。

## 注意事项 & 已知 BUG

### 自定义DLL注入

#### 将你的DLL重命名为StarFly.dll 并放置在与飞星主程序同目录下 在控制台输入`fkill <目标进程PID>` 即可注入自定义DLL

### VS2022 编译命令行

#### 建议使用提供的命令行编译飞星，否则将不保证绕过安全软件。

**C/C++:** [/permissive- /GS /Qpar /W3 /Gy /Zc:wchar_t /Gm- /O1 /sdl /Zc:inline /fp:precise /D "NDEBUG" /D "CONSOLE" /D "_UNICODE" /D "UNICODE" /errorReport:prompt /WX- /Zc:forScope /std:clatest /Gr /Oi /MD /std:c++latest /FC /EHsc /nologo /diagnostics:column]

**链接器:** [/OUT:"你的输出路径" /MANIFEST /NXCOMPAT /DYNAMICBASE "ntdll.lib" "kernel32.lib" "user32.lib" "gdi32.lib" "winspool.lib" "comdlg32.lib" "advapi32.lib" "shell32.lib" "ole32.lib" "oleaut32.lib" "uuid.lib" "odbc32.lib" "odbccp32.lib" /MACHINE:X64 /OPT:REF /SUBSYSTEM:CONSOLE /MANIFESTUAC:"level='requireAdministrator' uiAccess='false'" /OPT:NOICF /ERRORREPORT:PROMPT /NOLOGO /TLBID:1]

### 其他注意事项

*   飞星不会产生联网行为。部分云沙箱检测到的是 Windows 与微软云服务器的正常数据交换。
*   请不要包含 `winterl.h`。飞星提供的 `nt.h` 来自 `ProcessHacker` 库以及 `SysWhisper3`，已经包含所有底层调用所需定义。
*   **BUG:** 当编译器不启用 `/ZI` （应用于编译与生成的调试信息库）时，大于等于 6 个参数的某些 `SFNt*` 函数在内核执行后会触发一次执行访问冲突异常，地址为被函数填充的内存区域。暂未找到解决方法，暂时利用异常处理器忽略该异常。

## 未来规划

### 实现突破会话隔离，终止 Session 0 中的进程。

## 参考引用

*   [SysWhisper3](https://github.com/klezVirus/SysWhispers3)
*   [Capt-Meelo's PoC](https://github.com/capt-meelo/NtCreateUserProcess)
