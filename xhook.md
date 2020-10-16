#
```
xhook:爱奇艺 Android native PLT hook 方案
xhook 支持 armeabi, armeabi-v7a 和 arm64-v8a。支持 Android 4.0 (含) 以上版本 (API level >= 14)。

https://zhuanlan.zhihu.com/p/36426206

```

```
最新版中文readme
https://github.com/iqiyi/xHook/blob/master/docs/overview/android_plt_hook_overview.zh-CN.md

示例代码
https://github.com/iqiyi/xHook/tree/master/docs/overview/code

xhook项目地址
https://github.com/iqiyi/xHook
```

```
总结一下 xhook 中执行 PLT hook 的流程：

1、读 maps，获取 ELF 的内存首地址（start address）。
2、验证 ELF 头信息。
3、从 PHT 中找到类型为 PT_LOAD 且 offset 为 0 的 segment。计算 ELF 基地址。
4、从 PHT 中找到类型为 PT_DYNAMIC 的 segment，从中获取到 .dynamic section，从 .dynamic section中获取其他各项 section 对应的内存地址。
5、在 .dynstr section 中找到需要 hook 的 symbol 对应的 index 值。
6、遍历所有的 .relxxx section（重定位 section），查找 symbol index 和 symbol type 都匹配的项，对于这项重定位项，执行 hook 操作。hook 流程如下：
	读 maps，确认当前 hook 地址的内存访问权限。
	如果权限不是可读也可写，则用 mprotect 修改访问权限为可读也可写。
	如果调用方需要，就保留 hook 地址当前的值，用于返回。
	将 hook 地址的值替换为新的值。（执行 hook）
	如果之前用 mprotect 修改过内存访问权限，现在还原到之前的权限。
	清除 hook 地址所在内存页的处理器指令缓存。
```

# 可以直接从文件中读取 ELF 信息吗？
```
可以。而且对于格式解析来说，读文件是最稳妥的方式，因为 ELF 在运行时，原理上有很多 section 不需要一直保留在内存中，可以在加载完之后就从内存中丢弃，这样可以节省少量的内存。但是从实践的角度出发，各种平台的动态链接器和加载器，都不会这么做，可能它们认为增加的复杂度得不偿失。所以我们从内存中读取各种 ELF 信息就可以了，读文件反而增加了性能损耗。另外，某些系统库 ELF 文件，APP 也不一定有访问权限。
```

# 计算基地址的精确方法是什么？
```
正如你已经注意到的，前面介绍 libtest.so 基地址获取时，为了简化概念和编码方便，用了“绝大多数情况下”这种不应该出现的描述方式。对于 hook 来说，精确的基地址计算流程是：

在 maps 中找到找到 offset 为 0，且 pathname 为目标 ELF 的行。保存该行的 start address 为 p0。
找出 ELF 的 PHT 中第一个类型为 PT_LOAD 且 offset 为 0 的 segment，保存该 segment 的虚拟内存相对地址（p_vaddr）为 p1。
p0 - p1 即为该 ELF 当前的基地址。
绝大多数的 ELF 第一个 PT_LOAD segment 的 p_vaddr 都是 0。

另外，之所以要在 maps 里找 offset 为 0 的行，是因为我们在执行 hook 之前，希望对内存中的 ELF 文件头进行校验，确保当前操作的是一个有效的 ELF，而这种 ELF 文件头只能出现在 offset 为 0 的 mmap 区域。

可以在 Android linker 的源码中搜索“load_bias”，可以找到很多详细的注释说明，也可以参考 linker 中对 load_bias_ 变量的赋值程序逻辑。
```
