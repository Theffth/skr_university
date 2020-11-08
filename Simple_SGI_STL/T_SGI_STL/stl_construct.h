#ifndef T_SGI_STL_STL_CONSTRUCT_H_
#define T_SGI_STL_STL_CONSTRUCT_H_

#include "type_traits.h"
#include "stl_iterator_base.h"
#include <new>

namespace mystl{
    template <class T1,class T2>
    inline void Construct(T1 *p,const T2& value)
    {
        new((void *)p) T1(value);
    }   //placement new 在p指针指向的内存上调用对象的构造函数T1::T1(value);并赋初值value

    template <class T1>
    inline void Construct(T1 *p)
    {
        new((void *)p) T1();
    }   //同上 但调用对象的默认构造函数T1::T1();

    template <class T1,class T2>
    inline void construct(T1* p,const T2& value)
    {
        Construct(p,value);
    }

    template <class T1>
    inline void construct(T1 *p)
    {
        Construct(p);
    }

    //第一种类型的Destroy
    template <class TP>
    inline void Destroy(TP* pointer)
    {
        pointer->~TP();   //调用pointer所指对象的析构函数
    }

    template <class ForwardIter>
    void destroy_aux(ForwardIter first,ForwardIter last, false_type)
    {
        for(;first!=last;++first)
        {
            destroy(&*first);
        }
    }

    template<class ForwardIter>
    inline void destroy_aux(ForwardIter ,ForwardIter ,true_type){}

    template <class ForwardIter,class TP>
    inline void destroy(ForwardIter first, ForwardIter last, TP*)
    {
        typedef typename type_traits<TP>::has_trivial_destructor Trivial_destructor;
        destroy_aux(first, last ,Trivial_destructor());  //??()是类型实例化吗
    }

    //第二种类型的Destroy
    template <class ForwardIter>
    inline void Destroy(ForwardIter first,ForwardIter last)
    {
        destroy(first,last,VALUE_TYPE(first));  //VALUE_TYPE会返回一个指向TP的指针
    }
    
    //特化版本 以下类型不需要做具体的析构
    inline void Destroy(char*,char*) {}
    inline void Destroy(int *,int *) {}
    inline void Destroy(long*,long*) {}
    inline void Destroy(float*,float*) {}
    inline void Destroy(double*,double*) {}

    template <class TP>
    inline void destroy(TP *p)
    {
        Destroy(p);
    }
    template <class ForwardIter>
    inline void destroy(ForwardIter first,ForwardIter last)
    {
        Destroy(first,last);
    }

}
//C++  析构（构造） 异常没好好处理  多线程 条件竞争

#endif