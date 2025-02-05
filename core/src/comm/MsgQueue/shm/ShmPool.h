/**
 * @details     共享缓存池
 *              
 * @brief       共享缓存池
 * @file        
 *
 * @version     1.0
 * @author      lyw
 * @date        2025年2月5日
 */

#ifndef __LYW_PLUGIN_CORE_MSQ_QUEUE_SHM_POOL_FILE_H__
#define __LYW_PLUGIN_CORE_MSQ_QUEUE_SHM_POOL_FILE_H__

#include "PluginCoreCommDefine.h"
#include "MsgQueue/shm/AddrMap.hpp"

#include <pthread.h>
#include <sys/shm.h>

#include <map>

namespace LYW_PLUGIN_CORE
{
    /**
     * @brief                   
     *      整体构体
     *
     *                          --> OccupyList :: Block_t <-> Block_t <-> Block_t <-> Block_t ...
     *          ShmHeadInfo_t
     *                          --> FreeList :: Block_t <-> Block_t <-> Block_t <-> Block_t ...
     *
     *      块结构
     *          Block_t | dataField 
     *          /     \
     *      listNode | state | reCount | leftSize | id | totalSize
     *
     *
     *      分配策略：
     *          1. 使用 b_FreeBlock 分配，若 b_FreeBlock::leftSize 不足时 弹出该block 并添加至 占用链表 
     *          2. b_FreeBlock 为 0 时 表示无空闲块 重新申请Block 添加至空闲链表
     *          3. b_FreeBlock::leftSize 不足时 
     *              (1) 若需要大小超过 普通块 则按需要块大小创建块，分配后直接放入占用链表 空闲链表不变
     *              (2) 若需要大小不超过 普通块 则看使用后剩余大小 保留剩余长度大的作为 b_FreeBlock
     *
     *      释放策略：
     *          1. Block_t::reCount 归零后 从占用链表中弹出 并添加至 空连链表
     *          2. 若使用内存已超过 最大内存上限 则会进行释放 -- 由计数归零者进行释放
     *
     *      守护策略：
     *          1. 唯一守护进程 检查进程锁状态、防止进程崩溃导致的死锁等情况
     *          2. 单例守护线程 检查需要断开的共享缓存块
     *
     */
    class _VISIBLE_ ShmPool 
    {
        private:
            typedef struct tag_ShmAddr
            {
                key_t key;          ///< 共享内存块键值
                uint32 offset;      ///< 偏移量
            } ShmAddr_t;

            typedef enum tag_BlockState
            {
                BLK_VALID,      ///< 块可以
                BLK_INVALID,    ///< 块不可用 - 需要进程自行断开
            } eBlockState;

            typedef struct __attribute__((aligned(4))) tag_ShmHeadInfo 
            {
                key_t b_OccupyBlock;    ///< 非空闲块 链表头节点
                key_t e_OccupyBlock;    ///< 非空闲块 链表尾节点

                key_t b_FreeBlock;      ///< 空闲块 链表头节点
                key_t e_FreeBlock;      ///< 空闲块 链表尾节点

                pthread_mutex_t lock;
                pid_t lockPid;
                pid_t daemonPid;   
            } ShmHeadInfo_t;

            typedef struct __attribute__((aligned(4))) tag_Block
            {
                key_t pre;              ///< 链接前节点
                key_t next;             ///< 链表后节点

                eBlockState state;      ///< 块状态 
                uint32 size;            ///< 块大小 - 不包含头 Block_t 通常为 512K
                uint32 reCount;         ///< 引用计数 
                uint32 leftSize;        ///< 剩余可分配大小 不足时 申请新的块
                key_t id;               ///< 块id - 共享缓存id
                uint8 data[0];          ///< 数据开始区域
            } Block_t;

        private:

            /**
             * @brief           构造
             */
            ShmPool();

            /**
             * @brief           析构
             */
            ~ShmPool();

        public:

            /**
             * @brief           单例
             *
             * @return 共享缓存池实例
             */
            static ShmPool * GetInstance();

            /**
             * @brief           申请内存
             *
             * @param size      申请大小
             *
             * @return  虚拟内存地址 NULL 失败
             */
            pvoid Malloc(int32 size);

            /**
             * @brief           释放内存
             *
             * @param ptr       虚拟内存地址
             */
            void Free(pvoid ptr);

            /**
             * @brief           虚拟地址转换为共享缓存地址
             *
             * @param ptr       虚拟地址
             *
             * @return 错误码
             */
            eErrCode TransToShmAddr (pvoid ptr,  ShmAddr_t &shmAddr);

            /**
             * @brief           共享缓存地址转换为虚拟地址
             *
             * @param shmAddr   共享缓存地址
             *
             * @return 虚拟地址 NULL 失败
             */
            pvoid TransToMmAddr(const ShmAddr_t *shmAddr);

        private:

            /**
             * @brief           链接至共享缓存头
             *
             * @return 错误码
             */
            eErrCode ConnectShmHead();

            /**
             * @brief           断开共享缓存头链接
             *
             * @return 错误码
             */
            eErrCode DisConnectShmHead();

            /**
             * @brief           创建一个块
             *
             * @param size      块大小
             *
             * @return 错误码
             */
            Block_t * CreateBlock(int32 size);

            /**
             * @brief           销毁一个块
             *
             * @param id        共享内存键值
             *
             * @return 错误码
             */
            eErrCode DestroyBlock(key_t id);

            /**
             * @brief           链接至一个块
             *
             * @param id        共享内存键值
             *
             * @return 错误码
             */
            eErrCode ConnectBlock(key_t id);

            /**
             * @brief           断开共享内存块链接
             *
             * @param id        共享内存键值
             *
             * @return 错误码
             */
            eErrCode DisConnectBlock(key_t id);

            /**
             * @brief           守护
             */
            void Daemon();
        private:
            /**
             * @brief           锁
             */
            void ShmLock();

            /**
             * @brief           解锁
             */
            void ShmUnLock();

        private:
            key_t m_headKey;    ///< head 键值

            ShmHeadInfo_t *m_shmHead;   ///< 共享缓存头

            std::map<key_t, Block_t *> m_blockMap; ///< 共享缓存块表

            AddrMap m_addrMap;  ///< 地址map 起始地址为 Block_t *

            pthread_mutex_t m_lock; ///< 资源锁
    };
}
#endif
