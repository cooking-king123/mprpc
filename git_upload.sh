#!/bin/bash

# 显示当前状态
echo "当前git状态："
git status

# 确保.gitignore文件存在并合理
echo "\n检查.gitignore文件..."
if [ -f ".gitignore" ]; then
    echo ".gitignore已存在，内容如下："
    cat .gitignore
else
    echo "创建新的.gitignore文件..."
    cat > .gitignore << EOF
# CMake
CMakeFiles/
cmake_install.cmake
Makefile
CMakeCache.txt
*.cmake

# Build outputs
build/
bin/
lib/
*.o
*.a
*.so
*.dylib
*.exe

# IDE files
.idea/
.vscode/
*.swp
*.swo
*~
.DS_Store

# Protobuf generated files
*.pb.cc
*.pb.h

# Temporary files
*.tmp
*.temp
.cache/

# Logs
*.log
EOF
fi

# 清理不需要的文件
echo "\n清理不需要的文件..."
find . -name "*.o" -delete
find . -name "*.a" -delete
find . -name "*.so" -delete
find . -name "*.pb.cc" -delete
find . -name "*.pb.h" -delete
find . -name "*.log" -delete

# 清理CMake生成的文件
rm -rf CMakeFiles/ build/

# 重新添加文件
echo "\n重新添加文件..."
git add .
git status

# 提交更改
echo "\n提交更改..."
git commit -m "Update mprpc project with proper gitignore"

# 推送更改
echo "\n推送更改到远程仓库..."
git push origin master

echo "\n完成！"