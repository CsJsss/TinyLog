

## 日志格式

参考`python`的日志记录格式, 设计日志中一行的记录格式如下:

```
[日期]      [时间]    [毫秒]  [线程ID] [源文件[行号]]     - [日志级别] : [正文]
2022-05-01  16:12:46  233     tid=123456   test.py[line:200] - INFO: just test, hhh.
```

### 优化

1. 编译期计算字符串长度


2. 编译期将__LINE__转化成字符串

    [ref-Link](https://blog.csdn.net/tcmprogrammer/article/details/41014811)


## 遇到的问题和解决方法

1. `thread_local`的初始化过程发送在其声明位置, 并且在该`thread`中只初始化一次, 其生命周期和生命的声明周期一致. 因此必须将``thread_local Buffer buffer`的声明放置在`append`函数中. 否则会出现如下错误:

    ```bash
    /usr/bin/ld: main.cpp:(.text._ZTWN7TinyLog6Logger6bufferE[_ZTWN7TinyLog6Logger6bufferE]+0x19): undefined reference to `TinyLog::Logger::buffer'
    ```

    解决方法就是在`Logger::append`函数中进行线程局部变量的声明. 或者将其作为非局部的`thread_local`, 这样该变量的初始化会在线程启动前完成.


2. 格式化输出`this_thread::get_id()`的问题

    [stack overflow的解决方法](https://stackoverflow.com/questions/61203655/how-to-printf-stdthis-threadget-id-in-c)
    可以考虑将其进行封装, 提供友好的接口进行格式化的输出.

3. 可变参数模板的声明和实现分离, 导致其他文件必须include其实现文件.

    解决的方式是不使用可变参数模板接收参数, 而是使用`va_list`, `va_args`, `va_start`, `va_end`和`vsnprintf`函数处理可变参数.


4. 把`TinyLog::Logger::getLogLevel()`声明为inline后, `LOG_TRACE`和`LOG_DEBUG`无法使用, 编译会报`undefined reference to TinyLog::Logger::getLogLevel()`.

    [stack overflow解决方法](https://stackoverflow.com/questions/16245521/c99-inline-function-in-c-file/16245669#16245669)

## tips

1. `make_unique`是通过`new`在堆上构造对象.