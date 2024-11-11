#include "NodeList.h"
#include "Log/Log.h"
namespace LYW_PLUGIN_CORE
{
    NodeList::NodeBasic::NodeBasic()
    {
        pre = NULL;
        next = NULL;
        owner = NULL;
    }

    NodeList::NodeBasic::~NodeBasic()
    {
        pre = NULL;
        next = NULL;
        owner = NULL;
    }

    NodeList::NodeList()
    {
        m_headNode = NULL;
        m_tailNode = NULL;
        m_size = 0;
    }


    NodeList::~NodeList()
    {
        //不做任何操作 节点生命周期不由链表结构维护 防止节点提前释放导致崩溃
        //Clear();
    }

    bool NodeList::Empty()
    {
        if (0 == m_size)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    int32 NodeList::Size()
    {
        return m_size;
    }

    NodeList::NodeBasic * NodeList::Front()
    {
        return m_headNode;
    }

    NodeList::NodeBasic * NodeList::PopFront()
    {
        NodeBasic * head = m_headNode;
        Remove(head);
        return head;
    }

    NodeList::NodeBasic * NodeList::Back()
    {
        return m_tailNode;
    }

    NodeList::NodeBasic * NodeList::PopBack()
    {
        NodeBasic * tail = m_tailNode;
        Remove(tail);
        return tail;
    }

    NodeList::NodeBasic * NodeList::Next(NodeBasic * node)
    {
        if (node != NULL)
        {
            return node->next;
        } 
        else
        {
            return NULL;
        }
    }

    NodeList::NodeBasic * NodeList::Pre(NodeBasic * node)
    {
        if (node != NULL)
        {
            return node->pre;
        } 
        else
        {
            return NULL;
        }
    }

    int32 NodeList::Remove(NodeBasic *node)
    {
        NodeBasic * tmp = NULL;

        if (NULL == node || NULL == node->owner)
        {
            return 0;
        }

        if (node->owner != this)
        {
            LOG_ERROR("Node Not Owner by current list");
            return -1;
        }

        m_size--;
        
        //前节点
        tmp = node->pre;
        if (tmp != NULL)
        {
            //非首节点
            tmp->next = node->next;
        }
        else
        {
            //首节点
            m_headNode = node->next;

            if (m_headNode != NULL)
            {
                m_headNode->pre = NULL;
            }
        }
        
        //后节点
        tmp = node->next;
        if (tmp != NULL)
        {
            //非尾节点
            tmp->pre = node->pre;
        }
        else
        {
            //尾节点
            m_tailNode = node->pre;
            if (m_tailNode != NULL)
            {
                m_tailNode->next = NULL;
            }
        }

        //节点指针清理
        node->pre = NULL;
        node->next = NULL;
        node->owner = NULL;

        return 0;
    }

    int32 NodeList::PushFront(NodeBasic *node)
    {
        return Insert(m_headNode, node, INSERT_BEFORE);
    }

    int32 NodeList::PushBack(NodeBasic *node)
    {
        return Insert(m_tailNode, node, INSERT_AFTER);
    }

    int32 NodeList::Insert(NodeBasic *node, NodeBasic *insertNode, eInsertMode mode)
    {
        if (NULL == insertNode)
        {
            return -3;
        }
        
        //待插入节点检测
        if (insertNode->owner != NULL && insertNode->owner != this)
        {
            //节点被其他链表持有
            LOG_ERROR("Node Not Owner By Current List");
            return -1;
        }

        if (node == insertNode)
        {
            //相同节点位置不用操作
            return 0;
        }

        if (insertNode->owner == this)
        {
            //节点已存在当前链表中
            if (Remove(insertNode) < 0)
            {
                //节点移除异常
                return -7;
            }
        }

        m_size++;
        
        //锚节点检测
        if (node != NULL && node->owner != this)
        {
            LOG_ERROR("Node Not Owner by current list");
            return -2;
        }

        if (NULL == node)
        {
            //空队列插入操作
            if (m_headNode == NULL)
            {
                m_headNode = m_tailNode = insertNode;
                insertNode->owner = this;
                insertNode->pre = NULL;
                insertNode->next = NULL;
                return 0;
            }
            else
            {
                //锚节点为空
                LOG_ERROR("Insert Node unavailable");
                return -4;
            }
        }

        switch (mode)
        {
            case INSERT_BEFORE:
            {
                //node->pre后插
                insertNode->pre = node->pre;
                if (NULL != node->pre)
                {
                    node->pre->next = insertNode;
                }
                else
                {
                    //首节点
                    m_headNode = insertNode;
                }

                //node前插
                node->pre = insertNode;
                insertNode->next = node;
                break;
                
            }
            case INSERT_AFTER:
            {
                //node->next 前插 
                insertNode->next = node->next;
                if (node->next != NULL)
                {
                    //非尾节点
                    node->next->pre = insertNode;
                }
                else
                {
                    //尾节点
                    m_tailNode = insertNode;
                }

                //node 后插
                node->next = insertNode;
                insertNode->pre = node;
                break;
            }
            default:
            {
                LOG_ERROR("operator mode unkown [%d]", mode);
                return -4;
            }
        }

        insertNode->owner = this;

        return 0;
    }

    int32 NodeList::Clear()
    {
        while (m_headNode != NULL)
        {
            m_headNode->owner = NULL;
            m_headNode = m_headNode->next;
        }

        m_tailNode = NULL;

        m_size = 0;

        return 0;
    }

    NodeList * NodeList::Owner(NodeBasic * node)
    {
        if (NULL != node)
        {
            return node->owner;
        }
        return NULL;
    }
}
