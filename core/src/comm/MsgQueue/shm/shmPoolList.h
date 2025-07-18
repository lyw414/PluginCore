/**
 * @details     共享缓存池链表
 *              
 * @brief       共享缓存池链表
 * @file        shmPoolList.h
 *
 * @version     1.0
 * @author      lyw
 * @date        2026年7月2日
 */

#ifndef __LYW_PLUGIN_CORE_MSQ_QUEUE_SHM_LIST_FILE_H__
#define __LYW_PLUGIN_CORE_MSQ_QUEUE_SHM_LIST_FILE_H__

#include "MsgQueue/shm/shmPoolCommDef.h"

namespace LYW_PLUGIN_CORE
{
    class ShmPool;
    class ShmPoolList : public ShmPoolCommDef
    {
        public:
            /**
             * @brief               操作
             */
            typedef enum tag_Opt
            {
                BEFORE, ///< 前插
                AFTER, ///< 后插
                REPLACE, ///< 替换
            } eOpt;

            /**
             * @brief               节点
             */
            typedef struct __attribute__((aligned(4))) tag_Node
            {
                ShmAddr_t pre; ///< 前节点
                ShmAddr_t next; ///< 后节点
                ShmAddr_t current; ///< 当前节点
            } Node_t;

            /**
             * @brief               链表
             */
            typedef struct __attribute__((aligned(4))) tag_List
            {
                Node_t head; ///< 头节点
                Node_t tail; ///< 尾巴节点
                int32 size; ///< 节点数
            } List_t;

        public:

            /**
             * @brief                   构造
             *
             * @param shmPool           共享缓存池
             */
            ShmPoolList(List_t *list, ShmPool &shmPool);

            /**
             * @brief                   析构
             */
            ~ShmPoolList();

            /**
             * @brief                   弹出节点
             *
             * @return 节点指针
             */
            Node_t * Pop_front();

            /**
             * @brief                   获取前节点
             *
             * @return 节点指针
             */
            Node_t * Front();

            /**
             * @brief           获取后节点
             *
             * @param node      当前节点
             *
             * @return 节点指针
             */
            Node_t * Next(Node_t *node);

            /**
             * @brief           获取前节点
             *
             * @param node      当前节点
             *
             * @return 节点指针
             */
            Node_t * Pre(Node_t *node);

            /**
             * @brief           节点数
             *
             * @return 节点数量
             */
            int32 Size(void);

            /**
             * @brief               插入节点
             *
             * @param node          节点
             */
            void Push_Back(Node_t *node);

            /**
             * @brief               插入节点
             *
             * @param node          节点
             */
            void Push_Front(Node_t *node);

        private:
            List_t *m_list; ///< 链表
            ShmPool &m_shmPool; ///< 共享缓存池
    };
}
#endif
