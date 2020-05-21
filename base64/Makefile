#++++++++++++++++++++++++++++++++++++++++++#
#平台定义
ifeq ($(PLATFORM),)
PLATFORM=x64-linux
endif

#++++++++++++++++++++++++++++++++++++++++++#
#得到顶层目录【rules目录的父目录】绝对路径
TOPDIR=$(shell sh -c '\
        dir=`pwd|sed s/[/]$$//`; \
        while test $$dir; do \
                if test -d $$dir/rules; then \
                        echo $$dir; \
                        exit; \
                fi; \
                dir=`echo $$dir|sed s/[/][^/]*$$//`; \
        done \
')

# 用户设定

#++++++++++++++++++++++++++++++++++++++++++#
#EXECUTABLE 是最终目标文件名，自己设定
#eg : EXECUTABLE := mushroom.exe

MAKE_STATIC_LIB := 1
#MAKE_SHARED_LIB := 1
EXECUTABLE := base64.a

#++++++++++++++++++++++++++++++++++++++++++#

#++++++++++++++++++++++++++++++++++++++++++#
#INCLUDE_PATH 是用户创建include 文件的搜索附加路径
INCLUDE_PATH := $(SYS_INCLUDE_PATH) \
								-I. \
								-I./include
	
#++++++++++++++++++++++++++++++++++++++++++#

#++++++++++++++++++++++++++++++++++++++++++#
#DEFINES 用户自定义宏
DEFINES := -DXP_UNIX -DOS_LINUX
#++++++++++++++++++++++++++++++++++++++++++#

#{{{{连接库包含路径设定++++++++++++++++++++#
#LIBRARY_PATH是项目在此路径下搜索包含的动态库(.so)，如：-L./ -L./source
#SYS_LIBRARY_PATH为系统lib文件路径的环境变量
LIBRARY_PATH := $(SYS_LIBRARY_PATH)

#需要对DEBUG和RELEASE版本link不同的动态库(.so)的情况
ifeq ($(RELEASE),1)
LIBRARY_PATH +=
else
LIBRARY_PATH +=
endif
#}}}}连接库包含设定++++++++++++++++++++++++#

#++++++++++++++++++++++++++++++++++++++++++#
#LIBS 是本目标程序需要连接的动态库(.so)列表（例如 alleg, stdcx, iostr 等等），
#带-l前缀，只写该动态库的link名，例如stdc++表示会link在LIBRARY_PATH路径下的 libstdc++.so
#eg : LIBS := alleg
LIBS := -lstdc++ -lpthread -lrt -ldl

#USERLIBS本目标程序需要连接的静态库(.a)，
#不带-l前缀，且需要写出该静态库的全路径，例如 ../../smart/debug.$(PLATFORM)/smart.a
USERLIBS :=

#需要对DEBUG和RELEASE版本link不同的静态库(.a)的情况
ifeq ($(RELEASE),1)
USERLIBS +=
else
USERLIBS +=
endif
#++++++++++++++++++++++++++++++++++++++++++#

#++++++++++++++++++++++++++++++++++++++++++#
# 默认SOURCE为makefile目录的./source下的所有C/C++源文件
SOURCE := $(wildcard ./source/*.c) $(wildcard ./source/*.cpp)
_SOURCE := 

# 源文件最终集合 = [SOURCE集] - [_SOURCE集]
SOURCE := $(filter-out $(_SOURCE),$(SOURCE))

#++++++++++++++++++++++++++++++++++++++++++#

#++++++++++++++++++++++++++++++++++++++++++#
# C++预编译设置，C语言不支持预编译
#PRECOMPILE_HEADER := ./StdAfx.h

#++++++++++++++++++++++++++++++++++++++++++#

-include local.$(PLATFORM).defs
include $(TOPDIR)/rules/rules.$(PLATFORM)
