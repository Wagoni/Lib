#ifndef DUALLINKLIST_H
#define DUALLINKLIST_H
#include "Exception.h"
#include "List.h"
#include "Object.h"

namespace Lib
{

template <typename T>
class DualLinkList : public List<T>
{
protected:
    //数据结点
    struct Node : public Object
    {
        T value;
        Node* pre;
        Node* next;
    };

    //头结点
    mutable struct : public Object
    {
        char reserved[sizeof(T)];
        Node* pre;
        Node* next;
    }m_header;

    //单链表长度
    int m_length;

    //保存游标每次移动的结点的数目
    int m_step;
    //游标     move,next,current,end函数一起使用提高遍历效率，思想:遍历一个输出一个
    Node* m_current;


    //O(n)
    Node* position(int i) const
    {
        Node* ret = reinterpret_cast<Node*>(&m_header);

        for(int p = 0; p < i; p++)
        {
            ret = ret->next;
        }

        return ret;
    }

    //封装结点的申请和删除操作更有利于增强扩展性
    //在堆空间内部创建Node对象
    virtual Node* create()
    {
        return new Node();
    }

    //释放堆空间中的node对象
    virtual void destroy(Node* pn)
    {
        delete pn;
    }


public:
    DualLinkList()
    {
        m_header.pre = NULL;
        m_header.next = NULL;
        m_length = 0;
        m_step = 1;
        m_current = NULL;
    }


    //O(n)
    bool insert(int i, const T& e)
    {

        bool ret = ((0 <= i) && (i <= m_length));

        if(ret)
        {
            Node* node = create();

            if(node != NULL)
            {
                //即将插入结点的前驱结点
                Node* current = position(i);
                //即将插入结点的后继结点
                Node* next = current->next;

                node->value = e;

                node->next = next;
                current->next = node;

                //判断要插入的前驱结点是否为头结点
                if(current != reinterpret_cast<Node*>(&m_header))
                {
                    node->pre = current;
                }
                else
                {
                    node->pre = NULL;
                }

                //判断要插入结点的后继结点是否为空
                if(next != NULL)
                {
                    next->pre = node;
                }

                m_length++;

            }
            else
            {
                THROW_EXCEPTION(NoEnoughMemoryException, "No enough memory to insert element...");
            }
        }

        return ret;
    }

    //尾插法
    bool insert(const T& e)
    {
        return insert(m_length, e);
    }


    //O(n)
    bool remove(int i)
    {
        bool ret = ((0 <= i) && (i < m_length));

        if(ret)
        {
            Node* current = position(i);

            Node* toDel = current->next;
            Node* next = toDel->next;

            if(m_current == toDel)
            {
                m_current = next;
            }

            current->next = next;

            if(next != NULL)
            {
                next->pre = toDel->pre;
            }

            m_length--;

            destroy(toDel);
        }

        return ret;
    }

    //O(n)
    bool set(int i, const T& e)
    {
        bool ret = ((0 <= i) && (i < m_length));

        if(ret)
        {

            position(i)->next->value = e;
        }

        return ret;
    }

    //为了便于打印结果重载了一个get函数
    //O(n)
    virtual T get(int i)
    {
        T ret;

        if(get(i, ret))
        {
            return ret;
        }
        else
        {
            THROW_EXCEPTION(IndexOutBoundsException, "Invalid Parameter i is to get element...");
        }

        return ret;
    }

    bool get(int i, T& e) const
    {
        bool ret = ((0 <= i) && (i < m_length));

        if(ret)
        {
            e = position(i)->next->value;
        }

        return ret;
    }

    //O(n)
    int find(const T& e) const
    {
        int ret = -1;
        int i = 0;

        Node* node = m_header.next;

        while( node )
        {
            if(node->value == e)
            {
                ret = i;
                break;
            }
            else
            {
                node = node->next;
                i++;
            }
        }

        return ret;
    }

    //O(1)
    int length() const
    {
        return m_length;
    }

    //O(n)
    void clear()
    {
        while(m_length > 0)
        {
            remove(0);
        }
    }

    //将游标定位到目标位置i的位置，并且可以用step参数控制一次移动的节点的个数
    virtual bool move(int i, int step = 1)
    {
        bool ret = ((0 <= i) && (i < m_length) && (step > 0));

        if(ret)
        {
            //定位到目标位置处的节点
            m_current = position(i)->next;
            //游标每次移动的节点数
            m_step = step;
        }

        return ret;
    }

    virtual bool end()
    {
        return (m_current == NULL);
    }

    //换回当前游标所指向的当前数据元素的值
    virtual T current()
    {
        if(!end())
        {
            return m_current->value;
        }
        else
        {
            THROW_EXCEPTION(InvalidOperationException, "No value at current position...");
        }
    }

    //游标后移
    virtual bool next()
    {
        int i = 0;

        while((i < m_step) && !end())
        {
            m_current = m_current->next;
            i++;
        }

        //移动的节点数和当前m_step的值是否相等的
        return (i == m_step);
    }

    virtual bool pre()
    {
        int i = 0;

        while((i < m_step) && !end())
        {
            m_current = m_current->pre;
            i++;
        }

        //移动的节点数和当前m_step的值是否相等的
        return (i == m_step);
    }

    //O(n)
    ~DualLinkList()
    {
        clear();
    }

};

}


#endif // DUALLINKLIST_H
