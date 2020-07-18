#ifndef T_SGI_STL_STL_ALLOC_H_
#define T_SGI_STL_STL_ALLOC_H_

#ifndef THROW_BAD_ALLOC
#  include <stdio.h>
#  include <stdlib.h>
#define THROW_BAD_ALLOC fprintf(stderr,"out of memory!\n");  exit(1)
#endif

#include <stddef.h>
#include <stdlib.h>

namespace mystl
{
    //对malloc free realloc 函数的简单一级封装
    template <int __inst>  //inst是编译器常量 用于多个定义相同类型不同的实现
    class __malloc_alloc_template{
        public:
            static void* allocate(size_t n)
            {
                void* result=malloc(n);
                if (result == 0) THROW_BAD_ALLOC;
                return result;
            }
            static void deallocate(void* ptr,size_t /*n*/)  //??size_t指定
            {
                free(ptr);
            }
            static void* reallocate(void *ptr,size_t /*old_sz*/,size_t new_sz)
            {
                void* result=realloc(ptr,new_sz);
                if(result==0) THROW_BAD_ALLOC;
                return result;
            }
    };
    
    
    template <class TP,class Alloc>//Alloc???
    class simple_alloc{
    public:
        static TP* allocate(size_t n)
        {
           return 0==n?0:(TP*)Alloc::allocate(n*sizeof(TP));
        }
        //函数重载
        static TP* allocate(void)
        {
            return (TP*)Alloc::allocate(sizeof(TP));
        }
        static void deallocate(TP* p,size_t n)
        {
            if(n!=0) Alloc::deallocate(p,n*sizeof(TP));
        }
        static void deallocate(TP* p)
        {
            Alloc::deallocate(p,sizeof(TP));  //p==nullptr??
        }
    };

    //模板特化
    typedef __malloc_alloc_template<0> malloc_alloc;

    typedef malloc_alloc alloc;
    //???
    //提供对外接口
    /*template <class TP>
    class allocator{
        typedef malloc_alloc Alloc;
    public:
        typedef size_t       size_type;
        typedef ptrdiff_t    differnce_type;
        typedef TP*          pointer;
        typedef const TP*    const_pointer;
        typedef TP&          reference;
        typedef const TP&    const_reference;
        typedef TP           value_type;
        
        //?嵌套一个模板
        template <class TP1> rebind
    };*/
        
}

#endif