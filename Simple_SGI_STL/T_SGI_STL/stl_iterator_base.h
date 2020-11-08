#ifndef _T_SGI_STL_ITERATOR_BASE_H_
#define _T_SGI_STL_ITERATOR_BASE_H_

#include <cstddef>
namespace mystl
{
    //分类标签
    struct input_iterator_tag{};
    struct output_iterator_tag{};
    struct forward_iterator_tag : public input_iterator_tag{};
    struct bidirectional_iterator_tag : public forward_iterator_tag{};
    struct random_access_iterator_tag : public bidirectional_iterator_tag{};

    //给出五种迭代器具体的结构
    template <class TP,class Distance> 
    struct input_iterator {
        typedef input_iterator_tag    iterator_category;
        typedef TP                    value_type;
        typedef Distance              difference_type;
        typedef TP*                   pointer;
        typedef TP&                   reference; 
    };

    struct output_iterator{
        typedef output_iterator_tag    iterator_category;
        typedef void                   value_type;
        typedef void                   difference_type;
        typedef void                   pointer;
        typedef void                   reference; 
    };  //???为什么是void

    template <class TP,class Distance>
    struct forward_iterator
    {
        typedef forward_iterator_tag    iterator_category;
        typedef TP                      value_type;
        typedef Distance                difference_type;
        typedef TP*                     pointer;
        typedef TP&                     reference; 
    };

    template <class TP,class Distance>
    struct bidirectional_iterator {
        typedef bidirectional_iterator_tag    iterator_category;
        typedef TP                            value_type;
        typedef Distance                      difference_type;
        typedef TP*                           pointer;
        typedef TP&                           reference; 
    };

    template <class TP,class Distance>
    struct random_access_iterator {
        typedef random_access_iterator_tag    iterator_category;
        typedef TP                            value_type;
        typedef Distance                      difference_type;
        typedef TP*                           pointer;
        typedef TP&                           reference; 
    };

    //便于用户自定义的迭代器
    template<class Category,class TP,class Distance=ptrdiff_t,class Pointer=TP*,class Reference=TP&>
    struct iterator {
        typedef Category                      iterator_category;
        typedef TP                            value_type;
        typedef Distance                      difference_type;
        typedef Pointer                       pointer;
        typedef Reference                     reference; 
    };

    //萃取迭代器的特性
    template <class Iterator>
    struct iterator_traits {
        typedef typename Iterator::iterator_category  iterator_category;
        typedef typename Iterator::value_type         value_type;
        typedef typename Iterator::difference_type    difference_type;
        typedef typename Iterator::pointer            pointer;
        typedef typename Iterator::reference          reference;
    };

    //iterator_traits的偏特化
    template <class TP>
    struct iterator_traits <TP*> {
        typedef random_access_iterator_tag  iterator_category;
        typedef TP                          value_type;
        typedef ptrdiff_t                   defference_type;
        typedef TP*                         pointer;
        typedef TP&                         reference;
    };

    template <class TP>
    struct iterator_traits <const TP*> {
        typedef random_access_iterator_tag  iterator_category;
        typedef TP                          value_type;
        typedef ptrdiff_t                   defference_type;
        typedef const TP*                   pointer;
        typedef const TP&                   reference;
    };

    //提取category
    template <class Iter>
    inline typename iterator_traits<Iter>::iterator_category
    __iterator_category(const Iter&)
    {
        typedef typename iterator_traits<Iter>::iterator_category Category;
        return Category();//???
    }
    
    template <class Iter>
    inline typename iterator_traits<Iter>::iterator_category
    iterator_category(const Iter& i)
    {
        return __iterator_category(i);
    }

    //提取difference_type
    template <class Iter>
    inline typename iterator_traits<Iter>::difference_type*
    __distance_type(const Iter &)
    {
        return static_cast<typename iterator_traits<Iter>::difference_type*>(0);
    }//???*

    template <class Iter>
    inline typename iterator_traits<Iter>::difference_type*
    distance_type(const Iter &i)
    {
        return __distance_type(i);
    }

    //提取value_type
    template <class Iter>
    inline typename iterator_traits<Iter>::value_type*
    __value_type(const Iter&)
    {
        
        return static_cast<typename iterator_traits<Iter>::value_type*>(0);
    }

    template <class Iter>
    inline typename iterator_traits<Iter>::value_type*
    value_type(const Iter& i)
    {
        return __value_type(i);
    }

#define VALUE_TYPE(i)         value_type(i)
#define DISTANCE_TYPE(i)      distance_type(i)
#define ITERATOR_CATEGORY(i)  iterator_category(i)


    //计算InputIter类型迭代器之间的距离
    template <class InputIterator,class Distance>
    inline void _distance(InputIterator first, InputIterator last, Distance &n,input_iterator_tag)
    {
        while(first!=last) {++first; ++n;}
    }

    //计算RandomAccessIter类型迭代器之间的距离
    template <class RandomAccessIterator,class Distance>
    inline void _distance(RandomAccessIterator first, RandomAccessIterator last, Distance &n,random_access_iterator_tag)
    {
        n+=last-first;
    }

    //泛化
    template <class Iterator,class Distance>
    inline void distance(Iterator first, Iterator last, Distance &n)
    {
        _distance(first,last,n,iterator_category(first));
    }

//??
#ifdef _STL_CLASS_PARTIAL_SPECIALIZATION
    template <class InputIter>
    inline typename iterator_traits<InputIter>::difference_type
    __distance (InputIter first)
#endif



    //input_iterator只能++
    template <class InputIter,class Distance>
    inline void _advance(InputIter& i,Distance n, input_iterator_tag)
    {
        while(n--) ++i;
    }

    //bidirectional_iterator可以++/--
    template <class BidirectionalIter,class Distance>
    inline void _advance(BidirectionalIter& i,Distance n, bidirectional_iterator_tag)
    {
        if(n>=0)
        {
            while(n--) ++i;
        }
        while(n++) --i;
    }

    template <class RandomAccessIter, class Distance>
    inline void _advance(RandomAccessIter& i,Distance n,random_access_iterator_tag)
    {
        i+=n;
    }

    template <class Iterator,class Distance>
    inline void advance(Iterator& i,Distance n)
    {
        _advance(i,n,iterator_category(i));
    }
}
#endif