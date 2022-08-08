cmake_minimum_required(VERSION 3.19)
# 生成项目（默认即可）
project(LearnOpenGL)
# 配置C++版本（默认即可）
set(CMAKE_CXX_STANDARD 17)

# 头文件路径配置
include_directories(/Users/tyq/CODE/learnOpenGL/Include) # 这里的路径指向项目目录下的 include 文件夹
include_directories(/usr/local/include) # 此处的路径指向的是系统安装GLFW头文件的位置，通常为通过 make install 安装的头文件为此路径

# 设置库文件所在目录
link_directories(/usr/local/lib) # 安装GLFW时默认安装库文件位置
# 链接编译需要的系统框架
link_libraries("-framework Cocoa -framework OpenGL -framework IOKit -framework CoreVideo")
# 添加源文件（这里的 main.cpp 是你的C++源码文件）
add_executable(LearnOpenGL glad.c main.cpp) # glad.c 一定要填进去！
# 链接 GLFW 库
target_link_libraries(LearnOpenGL glfw3)