#ifndef T_SGI_STL_ALGOBASE_H_
#define T_SGI_STL_ALGOBASE_H_

#include <string.h>
#include "stl_iterator_base.h"
#include "stl_iterator.h"
#include "type_traits.h"

namespace mystl{

    //交换两个迭代器的值
    template <class ForwardIter1,class ForwardIter2,class TP>
    inline void __iter_swap(ForwardIter1 a , ForwardIter2 b,TP *){
        TP tmp=*a;
        *a=*b;
        *b=tmp;
    }
    template <class ForwardIter1,class ForwardIter2>
    inline void iter_swap(ForwardIter1 a, ForwardIter2 b)
    {
        __iter_swap(a,b,VALUE_TYPE(a));//默认接口迭代器类型相同
    }

    //swap->引用传参
    template <class TP>
    inline void swap(TP& a, TP& b)
    {
        TP tmp=a;
        a=b;
        b=tmp;
    }

    //min & max
    template <class TP>
    inline const TP& min(const TP& a, const TP& b)
    {
        return b < a ? b : a ;
    }

    template <class TP>
    inline const TP& max(const TP& a, const TP& b)
    {
        return a < b ?  b : a ; //默认都是通过operator<进行比较
    }

    template <class TP,class Compare>
    inline const TP& min(const TP& a, const TP& b,Compare comp)
    {
        return comp( a , b ) ? a : b ;
    }

    template <class TP,class Compare>
    inline const TP& max(const TP& a, const TP& b,Compare comp)
    {
        return comp( a , b ) ? b : a ; 
    }

    //copy
    //InputIter的copy 
    template <class InputIter, class OutputIter, class Distance>
    inline OutputIter __copy(InputIter first, InputIter last, OutputIter result,
                             input_iterator_tag , Distance* )
    {
        for( ; first != last ; ++result , ++first ) {
            *result=*last;
            return result;
        }
    }
    //RandomAccessIter的iter
    template <class RandomAccessIter,class OutputIter,class Distance>
    inline OutputIter __copy(RandomAccessIter first, RandomAccessIter last, OutputIter result,
                             random_access_iterator_tag , Distance* )
    {
        for(Distance n = last - first ; n>0 ; --n)
        {
            *result = *first;
            ++first;
            ++result;
        }
        return result;
    }
    //移动复制
    template <class TP>
    inline TP* __copy_trivial(const TP* first, const TP* last, TP* result)
    {
        memmove(result, first , sizeof(TP)*(last-first)); //与memcpy相似 但更安全 可处理重叠的部分 //?既然是trivial 那肯定是连续的内存??
        return result + (last - first);
    }
    template <class InputIter, class OutputIter>
    inline OutputIter __copy_aux2( InputIter first, InputIter last, OutputIter result, false_type)
    {
        return __copy(first,last,result,ITERATOR_CATEGORY(first),DISTANCE_TYPE(first));
    }
    template <class InputIter ,class OutputIter>
    inline OutputIter __copy_aux2(InputIter first, InputIter last,OutputIter result,true_type)
    {
        return __copy(first,last,result,ITERATOR_CATEGORY(first),DISTANCE_TYPE(first));
    }
    
    template <class TP>
    inline TP* __copy_aux2(TP* first,  TP* last ,TP* result, true_type)
    {
        return __copy_trivial(first,last,result);
    }

    template <class TP>
    inline TP* __copy_aux2(const TP* first, const TP* last ,TP* result, true_type)
    {
        return __copy_trivial(first,last,result);
    }
    template <class InputIter,class OutputIter ,class TP>
    inline OutputIter copy_aux(InputIter first,InputIter last, OutputIter result,TP*)
    {
        typedef typename type_traits<TP>::has_trivial_assignment_operator Trivial;
        return __copy_aux2(first,last,result,Trivial());
    }
    template <class InputIter,class OutputIter>
    inline OutputIter copy(InputIter first, InputIter last,OutputIter result){
        return __copy_aux(first,last,result,VALUE_TYPE(first));
    }
    //一层辅助判类型是否trivial复制  二层辅助使用memmove或者逐个复制的方式复制迭代器所指的对象

    //copy_backward
    //双向迭代器版本
    template <class BidirectionalIter1, class BidirectionalIter2, class Distance>
    inline BidirectionalIter2 __copy_backward(BidirectionalIter1 first,BidirectionalIter1 last, 
    BidirectionalIter2 result, bidirectional_iterator_tag, Distance*)
    {
        while(first!=last) *--result=*--last; //反向复制?
        return result;
    }
    
    //随机迭代器版本
    template <class RandomAccessIter, class BidirectionalIter, class Distance>
    inline BidirectionalIter __copy_backward(RandomAccessIter first,RandomAccessIter last,
    BidirectionalIter result, random_access_iterator_tag, Distance*)
    {
        for(Distance n = last - first;n>0;--n)
        {
            *--result=*--last;
            return result;
        }
    }
    //二级对外接口
    template <class BidirectionalIter1,class BidirectionalIter2,class BoolType>
    struct __copy_backward_dispatch{
        typedef typename iterator_traits<BidirectionalIter1>::iterator_category Cat;
        typedef typename iterator_traits<BidirectionalIter1>::difference_type   Distance;
        static BidirectionalIter2 copy(BidirectionalIter1 first,BidirectionalIter1 last,
        BidirectionalIter2 result)
        {
            return __copy_backward(first,last,result, Cat(),(Distance*)(0));
        }
    };
    //偏特化
    template <class TP>
    struct __copy_backward_dispatch<TP*,TP*,true_type>{
        static TP* copy(const TP* first,const TP* last,TP* result)
        {
            const ptrdiff_t num=last-first;
            memmove(result-num,first,sizeof(TP)*num);
            return result-num;
        }
    };
    //偏特化
    template <class TP>
    struct __copy_backward_dispatch<const TP*,TP*,true_type>
    {
        static TP* copy(const TP* first, const TP* last,TP* result){
            return __copy_backward_dispatch<TP*,TP*,true_type>::copy(first,last,result);
        }
    };
    //一级对外接口
    template <class BI1,class BI2>
    inline BI2 copy_backward(BI1 first,BI1 last,BI2 result){
        typedef typename type_traits<typename iterator_traits<BI2>::value_type>
        ::has_trivial_assignment_operator Trivial;
        return __copy_backward_dispatch<BI1,BI2,Trivial>::copy(first,last,result);
    }

    //fill
    template <class ForwardIter , class TP>
    void fill(ForwardIter first,ForwardIter last ,const TP& value){
        for( ; first!=last ; ++first)
            *first=value;
    }
    //fill_n
    template <class OutputIter,class Size, class TP>
    OutputIter fill_n(OutputIter first,Size n, const TP& value){
        for(; n>0;--n,++first){
            *first=value;
        }
        return first;
    }
    
    //lexicographical_compare 字母序比较
    template <class InputIter1,class InputIter2>
    bool lexicographical_compare(InputIter1 first1,InputIter1 last1,
                                 InputIter2 first2,InputIter2 last2){
        for( ; first1 != last1 && first2 != last2 ; ++first1, ++first2) {
            if( *first1 < *first2 ){
                return true;
            }
            if(*first1 > *first2){
                return false;
            }
        }
        return first1 == last1 && first2 != last2;
    }
    template <class InputIter1,class InputIter2, class Compare>
    bool lexicographical_compare(InputIter1 first1, InputIter1 last1,
                                 InputIter2 first2, InputIter2 last2, Compare comp){
        for( ; first1 != last1 && first2 != last2 ; ++first1, ++first2)
        {
            if (comp(*first1,*first2)) return true;
            if (comp(*first2,*first1)) return false;
        }
        return first1 == last1 && first2 != last2 ;
    }
    inline bool lexicographical_compare(const unsigned char* first1,
                                        const unsigned char* last1,
                                        const unsigned char* first2,
                                        const unsigned char* last2){
        const size_t len1 = last1 - first1;
        const size_t len2 = last2 - first2;
        const int    result = memcmp(first1,first2,min(len1,len2));
        return result !=0 ? result < 0 : len1 < len2;  
    }

    //equal 
    template <class InputIter1, class InputIter2,class BinaryPredicate>
    inline bool equal(InputIter1 first1, InputIter1 last1, InputIter2 first2, BinaryPredicate binary_pred){
        for( ; first1 != last1 ; ++first1, ++first2)
            if(!binary_pred(*first1,*first2)) return false;
        return true;
    }
    template <class InputIter1,class InputIter2>
    inline bool equal(InputIter1 first1,InputIter1 last1,InputIter2 first2){
        for( ; first1!=last1; ++first1,++first2) 
            if(*first1!=*first2)  return false;
        return true;
    }
    /*
    inline bool lexicographical_compare(const char* first1,const char* last1,const char* first2, const char* last2){
        if CHAR_MAX == SCHAR_MAX
            return lexicographical_compare((const signed char*) first1,
                                           (const signed char*) last1,
                                           (const signed char*) first2,
                                           (const signed char*) last2);
        
        #else
            return lexicographical_compare((const unsigned char*) first1,
                                           (const unsigned char*) last1,
                                           (const unsigned char*) first2,
                                           (const unsigned char*) last2);*/
}

#endif 