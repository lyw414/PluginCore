/**
 * @details     通用定义
 *              
 * @brief       通用定义
 * @file        PluginCoreCommDefine.h
 *
 * @version     1.0
 * @author      ywlong
 * @date        2024年10月31日
 */


#ifndef __LYW_PLUGIN_CORE_COMM_DEFINE_FILE_H__
#define __LYW_PLUGIN_CORE_COMM_DEFINE_FILE_H__

#include <stdio.h>

namespace LYW_PLUGIN_CORE
{
#define _VISIBLE_ __attribute__ ((visibility("default")))
    typedef long long int64;
    typedef int int32;
    typedef short int16;
    typedef char int8;
    typedef char byte;
    
    typedef unsigned long long uint64;
    typedef unsigned int uint32;
    typedef unsigned short uint16;
    typedef unsigned char uint8;
    typedef unsigned char ubyte;
    typedef void * pvoid;
    
    
    /**
     *
     * @class           Function
     * @brief           集成 BIND 与 FUNCTION 功能的建议函数指针对象
     *
     */
    template <typename T> 
    class Function;

#define FUNCTION_DEFINE_BEGIN(...) \
    template <typename RES __VA_ARGS__>
#define FUNCTION_DEFINE_CLASS(...) \
    class _VISIBLE_ Function<RES(__VA_ARGS__)> \
    {\
        private: \
            class None {};\
            typedef RES(None::*ClassFunc_f)(__VA_ARGS__); \
            typedef RES(*StaticFunc_f)(__VA_ARGS__); \
            None * m_obj; \
            union {\
                ClassFunc_f classFunc;\
                StaticFunc_f staticFunc;\
            } m_func;\
        public: \
            template <typename OBJ> \
            Function(RES(OBJ::*func)(__VA_ARGS__), OBJ *obj) \
            {\
                m_obj = (None *)obj;\
                m_func.classFunc = (ClassFunc_f)func;\
            }\
            Function(RES(*func)(__VA_ARGS__)) \
            {\
                m_obj = NULL;\
                m_func.staticFunc = (StaticFunc_f)func;\
            }\
            Function() \
            {\
                m_obj = NULL;\
                m_func.staticFunc = NULL;\
            }\
            ~Function()\
            {\
            }\
            Function & operator = (pvoid ptr)\
            {\
                m_obj = NULL;\
                m_func.staticFunc = ptr;\
                return *this;\
            }\
            Function & operator = (StaticFunc_f func)\
            {\
                m_obj = NULL;\
                m_func.staticFunc = func;\
                return *this;\
            }\
            Function & operator = (const Function &obj)\
            {\
                m_obj = obj.m_obj;\
                m_func.classFunc = obj.m_func.classFunc;\
                return *this;\
            }\
            bool operator == (pvoid ptr)\
            {\
                if (NULL != m_obj || NULL != ptr)\
                {\
                    return false;\
                }\
                else\
                {\
                    if (NULL == m_func.staticFunc)\
                    {\
                        return true;\
                    }\
                    return false;\
                }\
            }\
            bool operator == (const Function &obj)\
            {\
                if (m_obj == obj.m_obj)\
                {\
                    if ((m_obj != NULL && m_func.classFunc == obj.m_func.classFunc) || (m_obj == NULL && m_func.staticFunc == obj.m_func.staticFunc))\
                    {\
                        return true;\
                    }\
                }\
                return false;\
            }\
            bool operator != (void *ptr)\
            {\
                return ~(*this == ptr);\
            }\
            bool operator != (const Function &obj)\
            {\
                return ~(*this == obj);\
            }\
            bool IsNull ()\
            {\
                return (*this == NULL);\
            }\

#define FUNCTION_DEFINE_FUNC(...) \
            RES operator()(__VA_ARGS__) \
            {\
                if (m_obj != NULL) \
                { 
#define FUNCTION_DEFINE_CALL(...) \
                    return (m_obj->*(m_func.classFunc))(__VA_ARGS__); \
                } \
                else \
                { \
                    return (m_func.staticFunc)(__VA_ARGS__); \
                } \
            } 
#define FUNCTION_DEFINE_END \
    }; 


//定义参数的函数模板类
FUNCTION_DEFINE_BEGIN()
FUNCTION_DEFINE_CLASS()
FUNCTION_DEFINE_FUNC() 
FUNCTION_DEFINE_CALL() 
FUNCTION_DEFINE_END


//定义一个参数的函数模板类
FUNCTION_DEFINE_BEGIN(,typename ARGS1)
FUNCTION_DEFINE_CLASS(ARGS1)
FUNCTION_DEFINE_FUNC(ARGS1 args1) 
FUNCTION_DEFINE_CALL(args1) 
FUNCTION_DEFINE_END


//定义两个参数的函数模板类
FUNCTION_DEFINE_BEGIN(,typename ARGS1, typename ARGS2)
FUNCTION_DEFINE_CLASS(ARGS1,ARGS2)
FUNCTION_DEFINE_FUNC(ARGS1 args1, ARGS2 args2) 
FUNCTION_DEFINE_CALL(args1, args2) 
FUNCTION_DEFINE_END

//定义三个参数的函数模板类
FUNCTION_DEFINE_BEGIN(,typename ARGS1, typename ARGS2, typename ARGS3)
FUNCTION_DEFINE_CLASS(ARGS1,ARGS2,ARGS3)
FUNCTION_DEFINE_FUNC(ARGS1 args1, ARGS2 args2, ARGS3 args3) 
FUNCTION_DEFINE_CALL(args1, args2, args3) 
FUNCTION_DEFINE_END

//定义四个参数的函数模板类
FUNCTION_DEFINE_BEGIN(,typename ARGS1, typename ARGS2, typename ARGS3, typename ARGS4)
FUNCTION_DEFINE_CLASS(ARGS1,ARGS2,ARGS3, ARGS4)
FUNCTION_DEFINE_FUNC(ARGS1 args1, ARGS2 args2, ARGS3 args3, ARGS4 args4) 
FUNCTION_DEFINE_CALL(args1, args2, args3, args4) 
FUNCTION_DEFINE_END

//定义五个参数的函数模板类
FUNCTION_DEFINE_BEGIN(,typename ARGS1, typename ARGS2, typename ARGS3, typename ARGS4, typename ARGS5)
FUNCTION_DEFINE_CLASS(ARGS1,ARGS2,ARGS3, ARGS4, ARGS5)
FUNCTION_DEFINE_FUNC(ARGS1 args1, ARGS2 args2, ARGS3 args3, ARGS4 args4, ARGS5 args5) 
FUNCTION_DEFINE_CALL(args1, args2, args3, args4, args5) 
FUNCTION_DEFINE_END


    typedef uint32 ID;                                                  ///< 作为索引使用的唯一ID 
    
    typedef Function<int32(const byte *, int32)> LogWriteFunc;          ///< 写日志函数

    typedef int64 TaskHandle;                                           ///< 任务句柄 
                                                                
    typedef Function<void(pvoid, int32, pvoid)> TaskFunc;               ///< 任务执行函数

    typedef Function<void(pvoid, int32, pvoid)> TaskRecycleFunc;        ///< 任务移除回收函数 参数对应请求执行函数参数

    /**
     * @brief           任务状态
     */
    typedef enum tag_TaskState
    {
        TASK_WAIT_TIMEOUT = -1, ///< 等待任务执行完成超时
        TASK_WAITTING = 0, ///< 任务等待执行中
        TASK_EXCUTING = 1, ///< 任务执行中
        TASK_FINISHED = 2  ///< 任务执行完成
    } eTaskState;

}
#endif
