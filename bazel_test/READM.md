# bazel学习

>a fast, scalable, multi-language and extensible build system

bazel就是一个编译打包工具，类似于make、cmake等

## 安装

> ⚠️：Centos7系统安装bazel4
>
> 参考：https://docs.bazel.build/versions/5.0.0/install.html

![image-20221121155314062](https://markdown-1259209976.cos.ap-beijing.myqcloud.com/uPic/2022/11/21/image-20221121155314062.png)

## 学习

如何使用bazel这个工具呢？

**1、建立工作空间**
在构建项目之前，您需要设置其工作空间，工作空间是一个包含项目源文件和 Bazel 构建后文件输出的目录， 它还包含 Bazel 认为特殊的文件：**WORKSPACE 文件**，它将目录及其中的内容标记为 <u>Bazel 工作区</u>，并位于项目目录结构的根目录中，可以使用一个或多个 BUILD 文件，它们告诉 Bazel 如何构建项目的不同部分。（工作空间中包含 BUILD 文件的目录称为一个包（package））

**2、理解Build文件**
一个 BUILD 文件包含几种不同类型的 Bazel 指令。 最重要的类型是构建规则，它告诉 Bazel 如何构建所需的输出，例如可执行的二进制文件或库。 BUILD 文件中构建规则的每个实例称为目标，一个目标可以指向一组特定的源文件和依赖项， 也可以指向其他目标。

看一下cpp-tutorial/stage1/main目录下的BUILD文件：

```shell
cc_binary(
    name = "hello-world",
    srcs = ["hello-world.cc"],
)
```

hello-world 目标实例化了 Bazel 的内置 **cc_binary 规则**，该规则告诉 Bazel 从 hello-world.cc 源文件构建一个独立的，没有依赖关系的可执行二进制文件。

目标中的属性明确声明其依赖项和选项，虽然 **name 属性**是强制性的，但许多是可选的。 例如，在 hello-world 目标中，name 是必需的且不言自明，而 **srcs 是可选的**，意味着指定 Bazel 从中构建目标的源文件。

3、构建工程

为了构建您的示例项目，请跳转到 cpp-tutorial/stage1 目录并运行：

```shell
cd examples/cpp-tutorial/stage1/
bazel build //main:hello-world
```

在目标标签中，**//main**: 是 BUILD 文件在工作空间根目录的位置，**hello-world** 是BUILD 文件中的目标名称(name)。 

Bazel 产生类似于以下内容的输出：

```shell
Starting local Bazel server and connecting to it...
INFO: Analyzed target //main:hello-world (37 packages loaded, 161 targets configured).
INFO: Found 1 target...
Target //main:hello-world up-to-date:
  bazel-bin/main/hello-world
INFO: Elapsed time: 7.820s, Critical Path: 0.59s
INFO: 6 processes: 4 internal, 2 linux-sandbox.
INFO: Build completed successfully, 6 total actions
```

 Bazel 将构建的输出文件放在工作空间根目录的 **bazel-bin** 目录中。 

现在测试新构建的二进制文件：

```shell
bazel-bin/main/hello-world
```

> 学习`bazel_test/examples/cpp-tutorial`下的三个项目：[参考](https://blog.csdn.net/weixin_36354875/article/details/124822985)

![](https://markdown-1259209976.cos.ap-beijing.myqcloud.com/uPic/2022/11/15/3pF6Z9.png)

共有三组文件，每组代表本教程中的一个阶段.：

+ 在第一阶段，将构建单个包中的单个目标。
+ 在第二阶段，将项目拆分为多个目标，但将其保存在一个包中。
+ 在第三个阶段，将项目拆分为多个包并使用多个目标构建它。

### stage1

![image-20221121161753275](https://markdown-1259209976.cos.ap-beijing.myqcloud.com/uPic/2022/11/21/image-20221121161753275.png)

```shell
load("@rules_cc//cc:defs.bzl", "cc_binary")

cc_binary(
    name = "hello-world",
    srcs = ["hello-world.cc"],
)
```

第一阶段，只有一个单一的目标，它构建一个没有额外依赖项的单一源文件：

![图片](https://markdown-1259209976.cos.ap-beijing.myqcloud.com/uPic/2022/11/21/1fdf45352f8e500fa783c2701bf977d7.png)

### stage2

![image-20221121164005430](https://markdown-1259209976.cos.ap-beijing.myqcloud.com/uPic/2022/11/21/image-20221121164005430.png)

如果希望将较大的项目拆分为**多个目标和包**，以允许快速**增量构建**（即仅重建已更改的内容）并通过构建项目的多个部分来加速构建。

```shell
load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")

cc_library(
    name = "hello-greet",
    srcs = ["hello-greet.cc"],
    hdrs = ["hello-greet.h"],
)

cc_binary(
    name = "hello-world",
    srcs = ["hello-world.cc"],
    deps = [
        ":hello-greet",
    ],
)
```

使用这个 BUILD 文件，Bazel 首先构建 **hello-greet** 库（使用 Bazel 的内置 **cc_library 规则**），然后是 hello-world 二进制文件。 hello-world 目标中的 **deps 属性**告诉 Bazel，构建 hello-world 二进制文件需要 hello-greet 库。

![图片](https://markdown-1259209976.cos.ap-beijing.myqcloud.com/uPic/2022/11/21/de6922583bd989f7ad7c8b90eb25fd38.png)

该阶段使用两个目标构建了项目。 **hello-world** 目标构建一个源文件并依赖于另一个目标 (**//main:hello-greet**)，该库是由两个额外的源文件构建的。

### stage3

![image-20221121164342218](https://markdown-1259209976.cos.ap-beijing.myqcloud.com/uPic/2022/11/21/image-20221121164342218.png)

这里将其中一个功能放在另外一个文件夹**//lib**下：

```shell
//main:
load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")

cc_library(
    name = "hello-greet",
    srcs = ["hello-greet.cc"],
    hdrs = ["hello-greet.h"],
)

cc_binary(
    name = "hello-world",
    srcs = ["hello-world.cc"],
    deps = [
        ":hello-greet",
        "//lib:hello-time",
    ],
)

//lib:
load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "hello-time",
    srcs = ["hello-time.cc"],
    hdrs = ["hello-time.h"],
    visibility = ["//main:__pkg__"],
)
```

可以看出主包中的 **hello-world 目标**依赖于 lib 包中的 **hello-time 目标**（因此目标标签为 //lib:hello-time） - Bazel 通过 deps 属性知道这一点。

![图片](https://markdown-1259209976.cos.ap-beijing.myqcloud.com/uPic/2022/11/21/77fe277cf8a49ec94f5a90041651bffb.png)

第三阶段师将项目构建为具有三个目标的两个包，并了解它们之间的依赖关系。

## 使用

> bazel_test

```shell
cc_library(
    name = "lib",
    srcs = ["lib/lib.cc"],
    hdrs = ["lib/lib.h"],
)

cc_binary(
    name = "main.out",
    srcs = ["main.cc"],
    deps = [
        ":lib",
    ],
)
```



### 编译

```shell
bazel build //main:main.out
```

### 运行

```shell
./bazel-bin/main/main.out
```

### 更多

1、https://bazel.build/tutorials/cpp-use-cases

## 参考

1、https://blog.csdn.net/weixin_36354875/article/details/124822985
2、https://blog.csdn.net/butterfly5211314/article/details/125349431