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
#include "DynamicArray/DynamicArray.hpp"

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
            /**
             * @brief               共享内存块地址
             */
            typedef struct __attribute__((aligned(4))) tag_ShmBlockAddr
            {
                key_t key;          ///< 共享内存块键值 
                uint32 offset;      ///< 偏移量
                uint32 uuid;        ///< 块id 键值存在复用的情况
                int32 index;        ///< 块索引号
                int32 isValid;      ///< 是否有效 0 无效 其他有效
            } ShmBlockAddr_t;

            /**
             * @brief               共享内存块链表
             */
            typedef struct __attribute__((aligned(4))) tag_ShmBlockList
            {
                ShmBlockAddr_t head;    ///< 首节点
                ShmBlockAddr_t tail;    ///< 尾节点
            } ShmBlockList_t;

            /**
             * @brief               共享内存块链表节点
             */
            typedef struct __attribute__((aligned(4))) tag_ShmBlockListNode
            {
                ShmBlockAddr_t pre;     ///< 前节点
                ShmBlockAddr_t next;    ///< 后节点
                ShmBlockAddr_t current; ///< 当前节点
            } ShmBlockListNode_t;

            /**
             * @brief               共享内存锁
             */
            typedef struct __attribute__((aligned(4))) tag_ShmLock
            {
                pthread_mutex_t lock;   ///< 进程互斥量
                pid_t pid;              ///< 持有进程ID -- 防止进程崩溃出现锁无法释放的情况
            } ShmLock_t;

            /**
             * @brief               共享内存池
             */
            typedef struct __attribute__((aligned(4))) tag_ShmPool
            {
               ShmBlockList_t blockList;            ///< 共享内存块链表
               ShmBlockList_t freeNodeList;         ///< 可分配节点链表
               ShmLock_t lock;                      ///< 操作锁
               key_t key;                           ///< 键值
               int32 index;                         ///< 块键值索引 新增 Block 键值为 key + index
            } ShmPool_t;

            /**
             * @brief               共享内存块状态
             */
            typedef enum tag_BlockState
            {
                BLK_VALID,      ///< 块可以
                BLK_INVALID,    ///< 块不可用 
            } eBlockState;

            /**
             * @brief               共享内存块
             */
            typedef struct __attribute__((aligned(4))) tag_ShmBlock
            {
                ShmBlockAddr_t shmBlockAddr;            ///< 块地址
                ShmBlockListNode_t block_ListNode;      ///< 块链表节点 -> ShmPool::blockList
                uint32 size;                            ///< 块大小 -- 通常为 1M
                uint32 leftSize;                        ///< 剩余可分配大小 块会被拆解为多个 BlockNode
                uint8 shmBlockNode[0];                  ///< ShmBlockNode_t 数组 注意 结构为变长 使用 ShmBlockNode_t::size 进行偏移
            } ShmBlock_t;

            /**
             * @brief               共享内存块节点
             */
            typedef struct __attribute__((aligned(4))) tag_ShmBlockNode
            {
                ShmBlockAddr_t shmBlockAddr;                ///< 块地址
                ShmBlockListNode_t blockNode_ListNode;      ///< 块节点链表节点 -> ShmPool::blockList
                uint32 size;                                ///< 块大小 -- 通常为 64K
                uint32 leftSize;                            ///< 剩余可分配大小
                uint8 address[0];                           ///< 开始地址
            } ShmBlockNode_t;

            /**
             * @brief               地址映射表节点
             */
            class AddrArrayNode
            { 
                public:
                    ShmBlockAddr_t shmBlockAddr;                ///< 块地址
                    ShmBlock_t *address;                              ///< 虚拟地址

                    /**
                     * @brief       构造
                     */
                    AddrArrayNode()
                    {
                        shmBlockAddr.isValid = 0;
                        shmBlockAddr.index = -1;
                        shmBlockAddr.uuid = 0;
                        shmBlockAddr.key = 0;
                        shmBlockAddr.offset = 0;
                        address = NULL;
                    }
            };


            typedef enum tag_ListOpt
            {
                INSERT_FRONT,   ///< 前插
                INSERT_AFTER,   ///< 后插
            } eListOpt;

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
            eErrCode TransToShmAddr(pvoid ptr,  ShmBlockAddr_t &shmAddr);

            /**
             * @brief           共享缓存地址转换为虚拟地址
             *
             * @param shmAddr   共享缓存地址
             *
             * @return 虚拟地址 NULL 失败
             */
            pvoid TransToMmAddr(const ShmBlockAddr_t &shmAddr);

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
             * @return 索引
             */
             int32 CreateBlock(int32 size);

            /**
             * @brief           销毁一个块
             *
             * @param id        共享内存键值
             *
             * @return 错误码
             */
            eErrCode DestroyBlock(key_t id);

            /**
             * @brief           守护
             */
            void Daemon();

        private:

            /**
             * @brief               锁
             *
             * @param lock          锁结构
             *
             * @return 错误码
             */
            eErrCode ShmLock(ShmLock_t &lock);

            /**
             * @brief               解锁
             *
             * @param lock          锁结构
             *
             * @return 错误码
             */
            eErrCode ShmUnLock(ShmLock_t &lock);

        private:    ///< 链表操作
            /**
             * @brief               插入链表 -- 线程安全
             *
             * @param list          链表
             * @param basicNode     基准节点
             * @param inSertNode    待插入的节点
             * @param opt           操作 默认后插
             *
             * @return 错误码
             */
            eErrCode InsertList(ShmBlockList_t &list, ShmBlockListNode_t &basicNode, ShmBlockListNode_t &inSertNode, eListOpt opt = INSERT_AFTER);


            /**
             * @brief               移除节点 -- 线程安全
             *
             * @param list          链表
             * @param node          节点
             *
             * @return 错误码
             */
            eErrCode RemoveList(ShmBlockList_t &list, ShmBlockListNode_t &node);



        private:    ///< 地址操作

            /**
             * @brief           是否已经链接
             *
             * @param addr      共享缓存地址
             *
             * @return true 已连接 false 未链接
             */
            bool IsConnected(const ShmBlockAddr_t &addr);

            /**
             * @brief           链接至一个块
             *
             * @param addr      共享缓存地址
             *
             * @return 错误码
             */
            eErrCode ConnectBlock(const ShmBlockAddr_t &addr);

            /**
             * @brief           断开共享内存块链接
             *
             * @param addr      共享内存地址
             *
             * @return 错误码
             */
            eErrCode DisConnectBlock(const ShmBlockAddr_t &addr);



        private:
            pid_t m_pid;        ///< 当前进程pid

            key_t m_headKey;    ///< head 键值

            ShmPool_t *m_shmPool;   ///< 共享缓存头

            DynamicArray <AddrArrayNode> m_blockAddr;   ///< 块地址
            AddrMap m_addrMap;  ///< 地址map 起始地址为 Block_t *
            pthread_rwlock_t m_lock;    ///< 资源锁
    };
}
#endif
