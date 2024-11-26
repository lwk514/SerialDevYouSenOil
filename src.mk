
#该文件的编译规则和makefile一样，src.mk所在目录深度不能大于2，例如storage/src.mk、storage/xx/src.mk、storage/xx/xx/src.mk合法，storage/xx/xx/xx/src.mk不合法

#PRODUCT_TYPE(设备类型)

#使用gcc进行编译。有些在开源库基础上开发的代码使用g++进行编译会出现一大堆警告，默认使用g++进行编译,比如const char*和char*相互赋值
USE_GCC=n


#1.定义当前路径
LOCAL_PATH=$(ROOT_DIR)
RELEASER_DIR_PATH=$(ROOT_DIR)/../../../..
$(warning LOCAL_PATH=${LOCAL_PATH})


#2.定义需要包含的头文件
LOCAL_INCLUDES+=-I${LOCAL_PATH}/
#LOCAL_INCLUDES+=-I${LOCAL_PATH}/../inc


LOCAL_INCLUDES+=-I${RELEASER_DIR_PATH}/streamaxsdk/include
LOCAL_INCLUDES+=-I${RELEASER_DIR_PATH}/truck_main/ModuleMsg
LOCAL_INCLUDES+=-I${RELEASER_DIR_PATH}/include
LOCAL_INCLUDES+=-I${RELEASER_DIR_PATH}/truck_TruckCBB/GlobalEvent/inc
LOCAL_INCLUDES+=-I${RELEASER_DIR_PATH}/truck_TruckCBB/GlobalEvent/pri_inc
LOCAL_INCLUDES+=-I${RELEASER_DIR_PATH}/truck_TruckCBB/pubfunction
LOCAL_INCLUDES+=-I${RELEASER_DIR_PATH}/truck_TruckCBB/SCM/inc
LOCAL_INCLUDES+=-I${RELEASER_DIR_PATH}/truck_mainbusiness/peripheralmodule
LOCAL_INCLUDES+=-I${RELEASER_DIR_PATH}/truck_TruckCBB/CM/inc


#3.定义需要包含的库文件路径
LOCAL_LDFLAGS+=-L${RELEASER_DIR_PATH}/streamaxsdk/linux/${PRODUCT_TYPE}/lib
#LOCAL_LDFLAGS+=-L${RELEASER_DIR_PATH}/streamaxsdk/interface
LOCAL_LDFLAGS+=-L${RELEASER_DIR_PATH}/build/lib/release/libcommon/lib

#4.定义需要包含的静态库
LOCAL_STATIC_LIBRARIES+=

#5.定义需要包含的动态库
LOCAL_DYNAMIC_LIBRARIES+=-lpthread 
# LOCAL_DYNAMIC_LIBRARIES+=-lrmbasic-streamaxsdk
LOCAL_DYNAMIC_LIBRARIES+=-lrmbasicapi-streamaxsdk
LOCAL_DYNAMIC_LIBRARIES+=-lstreamaxcbb
LOCAL_DYNAMIC_LIBRARIES+=-ltinyxml
LOCAL_DYNAMIC_LIBRARIES+=-ltinyxml2
LOCAL_DYNAMIC_LIBRARIES+=-lz
LOCAL_DYNAMIC_LIBRARIES+=-lrmfs5.0
LOCAL_DYNAMIC_LIBRARIES+=-lpthread 
#LOCAL_DYNAMIC_LIBRARIES+=-lappcommon


#####芯片定义


#6.定义需要宏定义
LOCAL_CFLAGS+=

#7.定义需要的源文件路径，makefile会扫描以下路径的.c和.cpp文件进行编译
#此处包含各个业务的配置项，待改进，自动搜索.config加入进来
LOCAL_SRC_DIR+=${LOCAL_PATH}
#LOCAL_SRC_DIR+=${LOCAL_PATH}/../src

#8.如果不期望自动扫描目录，上面的路径可以不设，通过以下直接定义需要编译的文件
	
#9.本模块编译后的文件名称,当后缀为.a时编译为静态库,后缀为.so时编译为动态库，不加后缀为可执行文件
LOCAL_MODULE=SerialDevYouSenOil.so
