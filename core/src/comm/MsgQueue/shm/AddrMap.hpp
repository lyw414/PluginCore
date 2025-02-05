/**
 * @details     地址map
 *              
 * @brief       地址map
 * @file        AddrMap.hpp
 *
 * @version     1.0
 * @author      lyw
 * @date        2025年2月5日
 */

#ifndef __LYW_PLUGIN_CORE_MSQ_QUEUE_ADDR_MAP_FILE_H__
#define __LYW_PLUGIN_CORE_MSQ_QUEUE_ADDR_MAP_FILE_H__

#include "PluginCoreCommDefine.h"

#include <set>

namespace LYW_PLUGIN_CORE
{
    class AddrMap
    {
        private:
            class Address
            {
                public:
                    Address(pvoid ptr, uint32 size)
                    {
                        m_ptr = ptr;
                        m_size = size;
                    }

                    ~Address()
                    {
                    }

                    bool operator < (const Address & addr) const 
                    {
                        if (0 == addr.m_size)
                        {
                            if ((((uint8 *)(this->m_ptr) + this->m_size) >= addr.m_ptr) && this->m_ptr <= addr.m_ptr)
                            {
                                //地址处于区间 -- 地址相等
                                return false;
                            }
                        }

                        return (this->m_ptr < addr.m_ptr);
                    }

                    bool operator <= (const Address & addr) const 
                    {
                        if (0 == addr.m_size)
                        {
                            if ((((uint8 *)(this->m_ptr) + this->m_size) >= addr.m_ptr) && this->m_ptr <= addr.m_ptr)
                            {
                                //地址处于区间 -- 地址相等
                                return true;
                            }
                        }

                        return (this->m_ptr < addr.m_ptr);
                    }

                    bool operator > (const Address & addr) const 
                    {
                        if (0 == addr.m_size)
                        {
                            if ((((uint8 *)(this->m_ptr) + this->m_size) >= addr.m_ptr) && this->m_ptr <= addr.m_ptr)
                            {
                                //地址处于区间 -- 地址相等
                                return false;
                            }
                        }

                        return (this->m_ptr > addr.m_ptr);
                    }

                    bool operator >= (const Address & addr) const 
                    {
                        if (0 == addr.m_size)
                        {
                            if ((((uint8 *)(this->m_ptr) + this->m_size) >= addr.m_ptr) && this->m_ptr <= addr.m_ptr)
                            {
                                //地址处于区间 -- 地址相等
                                return true;
                            }
                        }

                        return (this->m_ptr > addr.m_ptr);
                    }


                    bool operator == (const Address & addr) const 
                    {
                        if (0 == addr.m_size)
                        {
                            if ((((uint8 *)(this->m_ptr) + this->m_size) >= addr.m_ptr) && this->m_ptr <= addr.m_ptr)
                            {
                                //地址处于区间 -- 地址相等
                                return true;
                            }
                        }

                        return (this->m_ptr == addr.m_ptr);
                    }


                    bool operator != (const Address & addr) const 
                    {
                        if (0 == addr.m_size)
                        {
                            if ((((uint8 *)(this->m_ptr) + this->m_size) >= addr.m_ptr) && this->m_ptr <= addr.m_ptr)
                            {
                                //地址处于区间 -- 地址相等
                                return false;
                            }
                        }

                        return (this->m_ptr != addr.m_ptr);
                    }

                    pvoid m_ptr;
                    uint32 m_size;
            };

        public:

            /**
             * @brief               插入一个地址
             *
             * @param ptr           起始地址
             * @param size          空间大小
             *
             * @return 错误码
             */
            eErrCode Insert(pvoid ptr, uint32 size)
            {
                if (NULL == ptr || 0 == size)
                {
                    return ERR_INVALID_PARAM;
                }

                //查询地址
                Address addr(ptr, 0);

                std::set <Address>::iterator it;

                it = m_set.find(addr);

                if (it != m_set.end())
                {
                    m_set.erase(it);
                }

                m_set.insert(Address(ptr, size));

                return SUC_SUCCESS;
            }


            /**
             * @brief               移除一个地址
             *
             * @param ptr           起始地址
             *
             * @return 错误码
             */
            eErrCode Remove(pvoid ptr)
            {
                if (NULL == ptr)
                {
                    return ERR_INVALID_PARAM;
                }

                //查询地址
                Address addr(ptr, 0);

                std::set <Address>::iterator it;

                it = m_set.find(addr);

                if (it != m_set.end())
                {
                    m_set.erase(it);
                }

                return SUC_SUCCESS;
            }


            /**
             * @brief               检索一个地址
             *
             * @param ptr           地址区间值
             * @param size          空间大小
             *
             * @return 起始地址 NULL 失败
             */
            pvoid Find(pvoid ptr, uint32 *size = NULL)
            {
                if (NULL == ptr)
                {
                    return NULL;
                }

                //查询地址
                Address addr(ptr, 0);

                std::set <Address>::iterator it;

                it = m_set.find(addr);

                if (it != m_set.end())
                {
                    if (NULL != size)
                    {
                        *size = it->m_size;
                    }
                }
                else
                {
                    return NULL;
                }

                return it->m_ptr;
            }

        private:
            std::set <Address> m_set;
    };
}

#endif
