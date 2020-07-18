#ifndef T_SGI_STL_STL_UNINITIALIZED_H_
#define T_SGI_STL_STL_UNINITIALIZED_H_

#include "type_traits.h"
#include "stl_algobase.h"
namespace mystl {
    template <class InputIter, class ForwardIter>
    inline ForwardIter __uninitialized_copy_aux(InputIter first,
                                                InputIter last, ForwardIter result, true_type){
        return copy(first,last,result);  //如果是POD 则直接拷贝
    }

    template <class InputIter, class ForwardIter>
    ForwardIter __uninitialized_copy_aux(InputIter   first, InputIter last,
                                         ForwardIter result,false_type){
        ForwardIter cur=result;
        try{
            for ( ; first!=last;++first,++cur)
            Construct(&*cur,*first);    //如果不是POD 则先构造再拷贝
            return cur;
        }
        catch(...) 
        {
            Destroy(result,cur); throw;
        }
    }   //??对构造过程中出现的异常进行处理  为什么destroy result???
    template <class InputIter, class ForwardIter, class TP>
    inline ForwardIter __uninitialized_copy(InputIter first, InputIter last, ForwardIter result, TP*){
        typedef typename type_traits<TP>::is_POD_type IS_POD;
        return __uninitialized_copy_aux(first,last,result,IS_POD());
    }

    template <class InputIter,class ForwardIter>
    inline ForwardIter uninitialized_copy(InputIter first,InputIter last,ForwardIter result){
        return __uninitialized_copy(first,last,result,VALUE_TYPE(result));
    }

    inline char* uninitialized_copy(const char* first,const char* last,char* result){
        memmove(result,first,last-first);
        return result + (last-first);
    }

    template <class ForwardIter, class TP>
    inline void __uninitialized_fill_aux(ForwardIter first, ForwardIter last,
                                         const TP& x,true_type){
        fill(first,last,x);
    }
    template <class ForwardIter,class TP>
    void __uninitialized_fill_aux(ForwardIter first,ForwardIter last,const TP& x,false_type){
        ForwardIter cur=first;
        try{
            for( ; cur!=last;++cur)
            Construct(&*cur,x);
        }
        catch(...)
        {
            Destroy(first,cur); throw;  //??throw
        }
    }
    template <class ForwardIter,class TP,class TP1>
    inline void __uninitialized_fill(ForwardIter first,ForwardIter last,const TP& x, TP1*)
    {
        typedef typename type_traits<TP1>::is_POD_type IS_POD;
        __uninitialized_fill_aux(first,last,x,IS_POD()); 
    }
    template <class ForwardIter,class TP>
    inline void uninitialized_fill(ForwardIter first,ForwardIter last,const TP& x){
        __uninitialized_fill(first,last,x,VALUE_TYPE(first));
    }

    //拷贝构造等同于赋值 析构是平凡的
    template <class ForwardIter, class Size, class TP>
    inline ForwardIter __uninitialized_fill_n_aux(ForwardIter first,Size n,const TP& x,true_type)
    {
        return fill_n(first,n,x);
    }
    template <class ForwardIter, class Size, class TP>
    inline ForwardIter __uninitialized_fill_n_aux(ForwardIter first,Size n,const TP& x,false_type)
    {
        ForwardIter cur=first;
        try{
            for( ; n>0 ; --n, ++cur){
                Construct(&*cur,first);
                return cur;
            }
        }
        catch(...){
            Destroy(first,cur);
        }
    }
    template <class ForwardIter, class Size,class TP,class TP1>
    inline ForwardIter __uninitialized_fill_n(ForwardIter first, Size n,const TP& x,TP1*){
        typedef typename type_traits<TP1>::is_POD_type IS_POD;
        return __uninitialized_fill_n_aux(first,n,x,IS_POD());
    }
    template <class ForwardIter,class Size, class TP>
    inline ForwardIter uninitialized_fill_n(ForwardIter first,Size n,const TP& x)
    {
        return __uninitialized_fill_n(first,n,x,VALUE_TYPE(first));
    }
    
}

#endif 