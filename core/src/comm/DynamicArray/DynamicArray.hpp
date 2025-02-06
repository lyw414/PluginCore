/**
 * @details     动态数组
 *
 * @brief       动态数组 -- 保持索引不变 提供数组动态调整
 * @file        DynamicArray.hpp
 *
 * @version     1.0
 * @author      lyw
 * @date        2025年2月05日
 */
#ifndef __RM_SDK_CODE_DYMANICARRAY_FILE_H__
#define __RM_SDK_CODE_DYMANICARRAY_FILE_H__

#include "PluginCoreCommDefine.h"
#include <string.h>
#include <stdlib.h>

namespace LYW_PLUGIN_CORE
{

    /**
     * @brief                   可调整数组 调整后保持索引值不变
     *
     * @tparam T                数组成员类型 暂不支持迭代器
     */
    template <typename T>
    class _VISIBLE_ DynamicArray
    {
        public:
            /**
             * @brief               构造
             */
            DynamicArray()
            {
                m_array = NULL;
                m_size = 0;
            }

            /**
             * @brief               析构
             */ 
            ~DynamicArray()
            {
                for (int32 iLoop = 0; iLoop < m_size; iLoop++)
                {
                    if (m_array[iLoop] != NULL)
                    {
                        delete m_array[iLoop];
                    }
                }
                
                if (m_array != NULL)
                {
                    free(m_array);
                }
            }

            /**
             * @brief               调整数组大小
             *
             * @param size          大小
             *
             * @return              成功
             */
            int32 Resize(int32 size)
            {
                int32 newSize = 0;
                int32 minSize = 0;

                T ** newArray = NULL;

                if (size < m_size)
                {
                    //缩减数组
                    for (int32 iLoop = m_size; iLoop > 0; iLoop--)
                    {
                        if (m_array[iLoop - 1] != NULL)
                        {
                            break;
                        }
                        newSize++;
                    }
                    
                    //缩减后的大小 
                    newSize = m_size - newSize;
                }
                else 
                {
                    //扩大数组
                    newSize = size;
                }

                if (newSize == m_size)
                {
                    return m_size;
                }
                
                if (newSize > 0)
                {
                    newArray = (T **)malloc(sizeof(T *) * newSize);
                    memset(newArray, 0x00, sizeof(T *) * newSize);
                }

                minSize = newSize < m_size ? newSize : m_size;
                
                //拷贝旧值
                if (m_array != NULL)
                {
                    memcpy(newArray, m_array, sizeof(T *) * minSize);
                    free(m_array);
                }

                //数组信息维护
                m_array = newArray;
                m_size = newSize;

                return m_size;
            }

            int32 Erase(int32 index)
            {
                if (index >= 0 && index < m_size && m_array[index] != NULL)
                {
                    delete m_array[index];
                    m_array[index] = NULL;
                }

                return 0;
            }

            int32 Size()
            {
                return m_size;
            }

            T & operator [] (int32 index)
            {
                if (index >= m_size)
                {
                    Resize(index + 1);
                }

                if (NULL == m_array[index])
                {
                    m_array[index] = new T;
                }

                return (*m_array[index]);
            }

            bool IsIndexValid(int32 index)
            {
                if (index >= 0 && index < m_size && NULL != m_array[index])
                {
                    return true;
                }

                return false;
            }

        private:
            int32 m_size;

            T ** m_array;
    };
}
#endif
