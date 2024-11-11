/**
 * @details     节点链表
 *
 * @brief       节点链表
 * @file        NodeList.h
 *
 * @version     1.0
 * @author      ywlong
 * @date        2023年11月05日
 */

#ifndef __LYW_PLUGIN_CORE_NODE_LIST_FILE_H__
#define __LYW_PLUGIN_CORE_NODE_LIST_FILE_H__

#include "PluginCoreCommDefine.h"

namespace LYW_PLUGIN_CORE
{

    /**
     * @brief               节点链表 节点资源不维护
     */
    class _VISIBLE_ NodeList
    {
        private:
            /**
             * @brief       节点插入方式<br>
             *              插入至 锚节点前
             *              插入至 锚节点后
             */
            typedef enum tag_InsertMode
            {
                INSERT_AFTER,       ///< 插入至锚节点后
                INSERT_BEFORE       ///< 插入至锚节点前
            } eInsertMode;
 
        public:
            class NodeBasic
            {
                friend class NodeList;
                private:
                    NodeList *owner;              ///< 节点所属
                    NodeBasic * pre;                   ///< 前指针
                    NodeBasic * next;                  ///< 后指针
                public:

                    /**
                     * @brief           构造
                     */
                    NodeBasic();

                    /**
                     * @brief           析构
                     */     
                    ~NodeBasic();
            };

            /**
             * @brief       节点 -- 模板
             *
             * @tparam T    节点数据
             */
            template <typename T>
            class Node: public NodeBasic
            {
                public:
                    T data;
            };

        public:
            /**
             * @brief                       构造
             */
            NodeList();

            /**
             * @brief                       析构
             */
            ~NodeList();
 
            /**
             * @brief                       判空
             *
             * @return                      true 链表为空 false 链表非空
             */
            bool Empty();

            /**
             * @brief                       链表大小
             *
             * @return  >= 0                链表节点数<br>
             *          <  0                异常
             */
            int32 Size();

            /**
             * @brief                       获取头节点
             *
             * @return  != NULL             头节点<br>
             *          == NULL             链表空
             */
            NodeBasic * Front();

            /**
             * @brief                       弹出头节点
             *
             * @return  != NULL             头节点<br>
             *          == NULL             链表空
             */
            NodeBasic * PopFront();

            /**
             * @brief                       获取尾节点
             *
             * @return  != NULL             尾节点<br>
             *          == NULL             链表空
             */
            NodeBasic * Back();

            /**
             * @brief                       弹出尾节点
             *
             * @return  != NULL             尾节点<br>
             *          == NULL             链表空
             */
            NodeBasic * PopBack();
            

            /**
             * @brief                       获取下一个节点
             *
             * @param[in] node              当前节点
             *
             * @return  != NULL             下一个节点<br>
             *          == NULL             链表结束
             */
            NodeBasic * Next(NodeBasic * node);

            /**
             * @brief                       获取前一个节点
             *
             * @param[in] node              当前节点
             *
             * @return  != NULL             前节点<br>
             *          == NULL             链表结束
             */
            NodeBasic * Pre(NodeBasic * node);

            /**
             * @brief                       链表中移除节点
             *
             * @param[in] node              待移除的节点
             *
             * @return  >= 0                移除成功<br>
             *          <  0                错误码：-1：非当前链表节点操作 
             */
            int32 Remove(NodeBasic *node);
            
            /**
             * @brief                       插入节点
             *
             * @param node                  锚点节点
             * @param insertNode            待插入的节点
             *
             * @return  >= 0                插入成功
             *          <  0                错误码：-1：待插入的节点被其他链表持有
             *                                      -2: 锚点节点非当前链表节点
             */
            int32 Insert(NodeBasic *node, NodeBasic *insertNode, eInsertMode mode = INSERT_AFTER);

            /**
             * @brief                       头部插入
             *
             * @param[in] node              待插入的节点
             *
             * @return  >= 0                插入成功
             *          <  0                错误码：-1：非当前链表节点操作 
             */
            int32 PushFront(NodeBasic * node);

            /**
             * @brief                       尾部插入
             *
             * @param[in] node              待插入的节点
             *
             * @return  >= 0                成功
             *          <  0                错误码：-1：非当前链表节点操作 
             */
            int32 PushBack(NodeBasic *node);


            /**
             * @brief                       清空所有节点
             *
             * @return  >= 0                成功 <br>
             *          <  0                失败
             */
            int32 Clear();

            /**
             * @brief                       获取节点持有链表
             *
             * @param[in] node              检测节点
             *
             * @return  != NULL             节点持有链表
             *          == NULL             节点无持有者 
             */
            static NodeList * Owner(NodeBasic * node);


        private:
            NodeBasic * m_headNode;     ///< 链表头

            NodeBasic * m_tailNode;     ///< 链表尾

            uint32 m_size;              ///< 节点数
    };
}
#endif
