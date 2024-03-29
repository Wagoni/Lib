#ifndef DUALCIRCLELIST_H
#define DUALCIRCLELIST_H
#include "DualLinkList.h"
#include "LinuxList.h"

namespace Lib
{

template <typename T>
class DualCircleList : public DualLinkList<T>
{
protected:
    struct Node : public Object
    {
        list_head head;
        T value;
    };

    list_head m_header;

    list_head* m_current;

    int m_step;

    list_head* position(int i) const
    {
        //在const函数内部不允许直接对成员取地址
        list_head* ret = const_cast<list_head*>(&m_header);

        for(int p = 0; p < i; p++)
        {
            ret = ret->next;
        }

        return ret;

    }


    int mod(int i) const
    {
        return (this->m_length == 0) ? 0 : (i%this->m_length);
    }

public:
    DualCircleList()
    {
        this->m_length = 0;
        this->m_step = 1;

        m_current = NULL;

        INIT_LIST_HEAD(&m_header);
    }

    bool insert(const T& e)
    {
        return insert(this->m_length, e);
    }

    bool insert(int i, const T& e)
    {
        bool ret = true;

        Node* node = new Node();

        i = i % (this->m_length + 1);

        if(node != NULL)
        {
            node->value = e;

            list_add_tail(&node->head, position(i)->next);

            this->m_length++;
        }
        else
        {
            THROW_EXCEPTION(NoEnoughMemoryException, "No memory to insert new element...");
        }

        return ret;
    }

    bool remove(int i)
    {
        bool ret = true;

        i = mod(i);

        ret = ((0 <= i) && (i < this->m_length));

        if(ret)
        {
            list_head* toDel = position(i)->next;

            if(m_current == toDel)
            {
                m_current = toDel->next;
            }

            //调用LINUX内核中的删除函数进行删除
            list_del(toDel);

            this->m_length--;

            //将toDel转换成真正的结点再删除
            delete list_entry(toDel, Node, head);
        }

        return ret;
    }

    //O(n)
    bool set(int i, const T& e)
    {
        bool ret = true;

        i = mod(i);

        ret = ((0 <= i) && (i < this->m_length));

        if(ret)
        {

            list_entry(position(i)->next, Node, head)->value = e;
        }

        return ret;
    }

    //为了便于打印结果重载了一个get函数
    //O(n)
    T get(int i)
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
        bool ret = true;

        i = mod(i);

        ret = ((0 <= i) && (i < this->m_length));


        if(ret)
        {
            e = list_entry(position(i)->next, Node, head)->value;
        }

        return ret;
    }

    //O(n)
    int find(const T& e) const
    {
        int ret = -1;
        int i = 0;

        list_head* slider = NULL;

        list_for_each(slider, &m_header)
        {
            if(list_entry(slider, Node, head)->value == e)
            {
                ret = i;
                break;
            }

            i++;
        }

        return ret;
    }

    int length() const
    {
        return this->m_length;

    }

    void clear()
    {
        while(this->m_length > 0)
        {
            remove(0);
        }
    }

    bool move(int i, int step = 1)
    {
        bool ret = (step > 0);

        i = mod(i);

        ret = ret && ((0 <= i) && (i < this->m_length));

        if(ret)
        {
            m_current = position(i)->next;

            this->m_step = step;
        }

        return ret;
    }

    bool end()
    {
        return (m_current == NULL) || (this->m_length == 0);
    }

    //换回当前游标所指向的当前数据元素的值
    T current()
    {
        if(!end())
        {
            return list_entry(m_current, Node, head)->value;
        }
        else
        {
            THROW_EXCEPTION(InvalidOperationException, "No value at current position...");
        }
    }

    //游标后移
    bool next()
    {
        int i = 0;

        while((i < m_step) && !end())
        {
            //如果
            if(m_current != &m_header)
            {
                m_current = m_current->next;
                i++;
            }
            //如果是头结点则直接跳过
            else
            {
                m_current = m_current->next;
            }
        }

        if(m_current == &m_header)
        {
            m_current = m_current->next;
        }

        //移动的节点数和当前m_step的值是否相等的
        return (i == m_step);
    }

    bool pre()
    {
        int i = 0;

        while((i < m_step) && !end())
        {
            //如果
            if(m_current != &m_header)
            {
                m_current = m_current->prev;
                i++;
            }
            //如果是头结点则直接跳过
            else
            {
                m_current = m_current->prev;
            }
        }

        if(m_current == &m_header)
        {
            m_current = m_current->prev;
        }

        //移动的节点数和当前m_step的值是否相等的
        return (i == m_step);
    }

    ~DualCircleList()
    {
        clear();
    }

};

}

#endif // DUALCIRCLELIST_H
