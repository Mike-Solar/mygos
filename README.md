# My Great Operating System

### 简称 MyGOS
````
qemu-system-riscv64 -machine virt -kernel cmake-build-debug/mygos -m 2G -nographic -smp 2 -bios default

````

问题： 所有的链接脚本里定义的符号全是NULL,不知道为啥。