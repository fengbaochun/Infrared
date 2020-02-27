SET PATH=D:\Keil_v5\ARM\ARMCC\Bin;D:\Ai-Thinker\AiThinkerIDE_V1.0\msys32\opt\arm-nono-eabi-gcc\bin;D:\Ai-Thinker\AiThinkerIDE_V1.0\msys32\opt\xtensa-lx106-elf\bin;D:\Ai-Thinker\AiThinkerIDE_V1.0\msys32\opt\xtensa-esp32-elf\bin;D:\Ai-Thinker\AiThinkerIDE_V1.0\msys32\mingw32\bin;D:\Ai-Thinker\AiThinkerIDE_V1.0\msys32\usr\bin;D:\Ai-Thinker\AiThinkerIDE_V1.0\eclipse\eclipse.exe;C:\Program Files (x86)\Common Files\Oracle\Java\javapath;C:\Windows\system32;C:\Windows;C:\Windows\System32\Wbem;C:\Windows\System32\WindowsPowerShell\v1.0\;D:\TortoiseGit\bin;D:\Python36;D:\Python36\Scripts;D:\pip-20.0.2\pip-20.0.2;C:\Users\Administrator\AppData\Roaming\pip\pip.ini;D:\Altium Designer Summer 09\System;C:\Program Files (x86)\NVIDIA Corporation\PhysX\Common;C:\Program Files\NVIDIA Corporation\NVIDIA NvDLISR;D:\MinGW\bin;D:\VS Code\bin;D:\Ai-Thinker\AiThinkerIDE_V1.0\msys32\home\aithinker\ESP8266_RTOS_SDK;D:\Ai-Thinker\AiThinkerIDE_V1.0\msys32\home\aithinker\ESP8266_RTOS_SDK\tools;D:\VS Code\bin
SET CPU_TYPE=STM32F407ZGTx
SET CPU_VENDOR=STMicroelectronics
SET UV2_TARGET=LED
SET CPU_CLOCK=0x00B71B00
"D:\Keil_v5\ARM\ARMCC\Bin\ArmAsm" --Via "..\output\startup_stm32f407xx._ia"
"D:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via "..\output\system_stm32f4xx.__i"
"D:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via "..\output\stm32f4xx_hal.__i"
"D:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via "..\output\stm32f4xx_hal_adc.__i"
"D:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via "..\output\stm32f4xx_hal_adc_ex.__i"
"D:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via "..\output\stm32f4xx_hal_can.__i"
"D:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via "..\output\stm32f4xx_hal_cec.__i"
"D:\Keil_v5\ARM\ARMCC\Bin\ArmLink" --Via "..\Output\YH-F407.lnp"
"D:\Keil_v5\ARM\ARMCC\Bin\fromelf.exe" "..\Output\YH-F407.axf" --i32combined --output "..\Output\YH-F407.hex"
