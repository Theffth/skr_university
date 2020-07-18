#ifndef _T_SGI_STL_STL_ITERATOR_H_
#define _T_SGI_STL_STL_ITERATOR_H_

#include "stl_iterator_base.h"

//几种常见的迭代器适配器adaptor
namespace mystl
{
    //向后插入迭代器 使用it=t会调用c.push_back(t) *it/it++/++it不会做任何事情只返回it
    template <class Container>
    class back_insert_iterator{
    protected://类内/派生类内可以访问
        Container* container;
    public:
        typedef Container               container_type;
        typedef output_iterator_tag     iterator_category;
        typedef void                    value_type;
        typedef void                    difference_type;
        typedef void                    pointer;
        typedef void                    reference;
        //explicit表示定义的函数不能隐式转换
        explicit back_insert_iterator(Container& x) : container(&x){} //???
        back_insert_iterator<Container>& operator=(const typename Container::value_type& value)
        {
            container->push_back(value);
            return *this;
            //在成员函数中this用来指被调用对象
        }
        back_insert_iterator<Container>& operator*() {return *this;}
        back_insert_iterator<Container>& operator++() {return *this;}
        back_insert_iterator<Container>& operator++(int)  {return *this;}
        //int类型的形参区分前置或后置版本的函数
    };
    #ifndef STL_CLASS_PARTIAL_SPECIALIZATION//??
    template <class Container>
    inline output_iterator_tag iterator_category(const back_insert_iterator<Container>&)
    {
        return output_iterator_tag();
    }//返回后插入迭代器的标签类型
    #endif
    
    template <class Container>
    inline back_insert_iterator<Container> back_inserter(Container& x){
        return back_insert_iterator<Container>(x);
    }
    
    template <class Container>
    class front_insert_iterator{
    protected:
        Container* container;
    public:
        typedef Container               container_type;
        typedef output_iterator_tag     iterator_category;
        typedef void                    value_type;
        typedef void                    difference_type;
        typedef void                    pointer;
        typedef void                    reference;

    //构造函数???
    explicit front_insert_iterator(Container& x) : container(&x) {}
    front_insert_iterator<Container>& operator=(const typename Container::value_type& value)
    {
        container->push_front(value);
        return *this;
    }
    front_insert_iterator<Container>& operator*() {return *this;}
    front_insert_iterator<Container>& operator++() {return *this; }
    front_insert_iterator<Container>& operator++(int) {return *this ;}
    };

    #ifndef STL_CLASS_PARTIAL_SPECIALIZATION//??
    template <class Container>
    inline output_iterator_tag iterator_category(const front_insert_iterator<Container>&)
    {
        return output_iterator_tag();
    }//返回后插入迭代器的标签类型
    #endif
    
    template <class Container>
    inline front_insert_iterator<Container> front_inserter(Container& x){
        return front_insert_iterator<Container>(x);
    }

    template <class Container>
    class insert_iterator{
    protected:
        Container* container;
        typename Container::iterator iter;
    public:
        typedef Container               container_type;
        typedef output_iterator_tag     iterator_category;
        typedef void                    value_type;
        typedef void                    difference_type;
        typedef void                    pointer;
        typedef void                    reference;     
    
    //?构造函数 两个成员变量
    insert_iterator(Container& x,typename Container::iterator i) : container(&x),iter(i){}
    insert_iterator <Container>& 
    operator=(const typename Container::value_type& value)
    {
        iter = container->insert(iter,value);
        ++iter;//指回原来的it
        return *this;
    }
    insert_iterator<Container>& operator*()     {return *this;}
    insert_iterator<Container>& operator++()    {return *this;}
    insert_iterator<Container>& operator++(int) {return *this;}
};
    #ifndef STL_CLASS_PARTIAL_SPECIALIZATION//??
    template <class Container>
    inline output_iterator_tag iterator_category(const insert_iterator<Container>&)
    {
        return output_iterator_tag();
    }//返回插入迭代器的标签类型
    #endif

    template <class Container,class Iterator>
    inline insert_iterator<Container> inserter(Container& x,Iterator i)//容器的对象 对象的迭代器 返回值是插入迭代器类型
    {
        typedef typename Container::iterator iter;
        return insert_iterator<Container>(x,iter(i));
    }

    template <class Iterator>
    class reverse_iterator
    {
        protected:
            Iterator current; //便于用户得到正序迭代器
        public:
            typedef typename iterator_traits<Iterator>::iterator_category iterator_category;
            typedef typename iterator_traits<Iterator>::value_type        value_type;
            typedef typename iterator_traits<Iterator>::difference_type   difference_type;
            typedef typename iterator_traits<Iterator>::pointer           pointer;
            typedef typename iterator_traits<Iterator>::reference         reference;
            typedef Iterator iterator_type;
            typedef reverse_iterator<Iterator> Self;
        public:
            //可以接受四种构造形式
            reverse_iterator() {}
            explicit reverse_iterator(iterator_type x) : current(x) {}
            reverse_iterator(const Self& x) : current(x.current) {}
            //template <class Iter>
            //reverse_iterator(const reverse_iterator<Iter>& x) : current(x.base){} //???
            iterator_type base()  const { return current; }
            reference operator*() const{
                Iterator tmp=current;
                return *--tmp;       //反向迭代器对应的是正向迭代器的前一个
            }
            pointer operator->() const { return &(operator*());} //注意pointer ?即迭代器
            Self& operator++(){
                 --current;
                 return *this;
            }
            Self operator++(int){
                Self tmp=*this;
                --current;
                return tmp;
            }
            Self& operator--(){
                ++current;
                return *this;
            }
            Self  operator--(int){
                Self tmp=*this;
                ++current;
                return tmp;//已经被改变
            }
            Self operator+(difference_type n) const{  //const函数代表数据成员不能改变
                return Self(current-n);  //???
            }
            Self& operator+=(difference_type n){
                current-=n;
                return *this;
            }
            Self operator-(difference_type n) const {
                return Self(current+n);
            }
            Self& operator-=(difference_type n) {
                current+=n;
                return *this;
            }
            reference operator[](difference_type n) const {
                return *(*this+n);     //base[-n-1]??
            }
    };
    
    //内联函数编译时展开
    template <class Iterator>
    inline bool operator< (const reverse_iterator<Iterator>& x,
                            const reverse_iterator<Iterator>& y){
        return y.base() < x.base();    //原序
    }
    template <class Iterator>
    inline bool operator> (const reverse_iterator<Iterator>&x,
    const reverse_iterator<Iterator>& y){
        return x > y;
    }

    template <class Iterator>
    inline bool operator==(const reverse_iterator<Iterator>& x,
                            const reverse_iterator<Iterator>& y){
        return x.base() == y.base(); //x==y?
    }
    template <class Iterator>
    inline bool operator!=(const reverse_iterator<Iterator>& x,
    const reverse_iterator<Iterator>& y){
        return !(x==y);
    }
    template <class Iterator>
    inline bool operator<=(const reverse_iterator<Iterator>& x,
                            const reverse_iterator<Iterator>& y){
        return !(x>y);
    }
    template <class Iterator>
    inline bool operator>=(const reverse_iterator<Iterator>& x,
                            const reverse_iterator<Iterator>& y){
        return !(x<y);
    }
    
    template <class Iterator>
    inline typename reverse_iterator<Iterator>::difference_type
    operator-(const reverse_iterator<Iterator>& x,
                const reverse_iterator<Iterator>& y){
        return y.base() - x.base();
    }
    template <class Iterator>
    inline reverse_iterator<Iterator>
    operator+(typename reverse_iterator<Iterator>::difference_type n,
                const reverse_iterator<Iterator>& x){
        return reverse_iterator<Iterator>(x.base()-n);   //????
    }
    
}
#endif