#ifndef T_SGI_STL_STL_VECTOR_H_
#define T_SGI_STL_STL_VECTOR_H_

#include "stl_alloc.h"
#include "stl_iterator.h"
#include "type_traits.h"
#include "stl_uninitialized.h"
#include "stl_construct.h"
#include "stl_algobase.h"
#include <cstddef>

namespace mystl
{
    template <class TP,class Alloc>
    class Vector_base{
        protected:
            TP* M_start;   //迭代器头 vector的迭代器就是原生指针
            TP* M_finish;  //迭代器尾
            TP* M_end_of_storage;  //可用空间尾
            typedef simple_alloc<TP,Alloc> M_data_allocator;
            TP* M_allocate(size_t n) {
                return M_data_allocator::allocate(n);
            }
            void M_deallocate(TP* p,size_t n){
                M_data_allocator::deallocate(p,n);
            }
        public:
            typedef Alloc allocator_type;
            allocator_type get_allocator() const {
                return allocator_type();
            }
            //构造函数
            Vector_base(const Alloc&): M_start(0),M_finish(0),M_end_of_storage(0){}
            Vector_base(size_t n, const Alloc&): M_start(0),M_finish(0),M_end_of_storage(0){
                M_start = M_allocate(n);
                M_finish = M_start;
                M_end_of_storage = M_start + n;
            }
            //析构函数
            ~Vector_base(){ M_deallocate( M_start , M_end_of_storage - M_start); }
    };

    //vector类
    template <class TP, class Alloc=alloc>
    class vector : protected Vector_base<TP,Alloc>
    {
        private:
            typedef Vector_base<TP,Alloc> Base;
        public:
            typedef TP value_type;
            typedef value_type* pointer;
            typedef const value_type* const_pointer;
            typedef value_type* iterator;
            typedef const value_type* const_iterator;
            typedef value_type& reference;
            typedef const value_type& const_reference;
            typedef size_t size_type;  //size_t => long long unsigned int 
            typedef ptrdiff_t difference_type;
    
            typedef typename Base::allocator_type allocator_type;
            allocator_type get_allocator() const {
                return Base::get_allocator();
            }
            typedef reverse_iterator<const_iterator> const_reverse_iterator;
            typedef reverse_iterator<iterator> reverse_iterator;
        protected:
            using Base::M_allocate;
            using Base::M_deallocate;
            using Base::M_start;
            using Base::M_finish;
            using Base::M_end_of_storage;
            void M_insert_aux(iterator position,const TP& x);
            void M_insert_aux(iterator position);
        public:
            iterator begin() { return M_start; }
            const_iterator begin() const { return M_start; }  //const函数只读
            iterator end() { return M_finish; }
            const_iterator end() const { return M_finish; }
            reverse_iterator rbegin() {
                return reverse_iterator(end());
            }
            const_reverse_iterator rbegin() const{
                return const_reverse_iterator(end());
            }
            reverse_iterator rend(){
                return reverse_iterator(begin());
            }
            const_reverse_iterator rend() const{
                return const_reverse_iterator(begin());
            }

            size_type size() const{
                return size_type(end()-begin());
            }
            size_type max_size() const{
                return size_type(-1)/sizeof(TP);
            }
            size_type capacity() const{
                return size_type(M_end_of_storage-begin());
            }
            bool empty() const{
                return begin()==end();
            }
            reference operator[](size_type n){
                return *(begin()+n);
            }
            const_reference operator[](size_type n) const{
                return *(begin()+n);
            }

            //异常检测 是否越界
            void M_range_check(size_type n) const {
               // if(n>=this->size()){throw range_error("vector");}
               if(n>=this->size()){throw;}
            }
            //at函数
            reference at(size_type n){
                M_range_check(n);
                return (*this)[n];
            } 
            const_reference at(size_type n) const{
                M_range_check(n);
                return (*this)[n];
            }

            //vector的构造函数
            //默认构造函数
            explicit vector(const allocator_type& a=allocator_type()) : Base(a) {}
            //n个初始值为value的构造函数
            vector(size_type n,const TP& value,const allocator_type& a=allocator_type()):Base(n,a)
            {
                M_finish=uninitialized_fill_n(M_start,n,value);
            }
            //只有容器包含元素个数n的构造函数
            explicit vector(size_type n) : Base(n,allocator_type()){
                M_finish=uninitialized_fill_n(M_start,n,TP());
            }
            //拷贝构造函数
            vector(const vector<TP,Alloc>& x):Base(x.size(),x.get_allocator()){
                M_finish=uninitialized_copy(x.begin(),x.end(),M_start);
            }

            //针对某个区间的构造函数
            template <class InputIter>
            vector(InputIter first,InputIter last,const allocator_type& a=allocator_type()):Base(a){
                typedef typename Is_Integer<InputIter>::Integral Integral;
                M_initialize_aux(first,last,Integral());
            }
            template <class Integer>
            void M_initialize_aux(Integer n,Integer value,true_type){
                M_start = M_allocate(n);
                M_end_of_storage = M_start + n;
                M_finish = uninitialized_fill_n(M_start,n,value);
            }
            template <class InputIter>
            void M_initialize_aux(InputIter first, InputIter last,false_type){
                M_range_initialize(first,last,ITERATOR_CATEGORY(first));
            }

        //析构函数
            ~vector() { destroy(M_start , M_finish); }
            vector<TP,Alloc>& operator=(const vector<TP,Alloc>& x);
            //调整可用空间的大小为n
            void reserve(size_type n){
                if(capacity() < n){
                    const size_type old_size=size();
                    //分配更大的内存空间并把原先的内容拷贝过来
                    iterator tmp=M_allocate_and_copy(n,M_start,M_finish);  
                    destroy(M_start,M_finish);
                    M_deallocate(M_start, M_end_of_storage-M_start);
                    M_start=tmp;
                    M_finish=tmp+old_size;
                    M_end_of_storage=M_start+n;
                }
            }

            void assign(size_type n,const TP& val){
                M_fill_assign(n,val);
            }
            void M_fill_assign(size_type n,const TP& val);

            template <class InputIter>
            void assign(InputIter first, InputIter last){
                typedef typename Is_Integer<InputIter>::Integral Integral;
                M_assign_dispatch(first, last, Integral());
            }
            template <class Integer>
            void M_assign_dispatch(Integer n, Integer val, true_type){
                M_fill_assign((size_type) n, (TP) val);
            }

            template <class InputIter>
            void M_assign_dispatch(InputIter first, InputIter last, false_type){
                M_assign_aux(first,last,ITERATOR_CATEGORY(first));
            }
            template <class InputIter>
            void M_assign_aux(InputIter first, InputIter last, input_iterator_tag);
            template <class ForwardIter>
            void M_assign_aux(ForwardIter first, ForwardIter last, forward_iterator_tag);

            reference front() {return *begin();}
            const_reference front() const { return *begin(); }
            reference back() { return *(end()-1); }
            const_reference back() const { return *(end()-1); }
            
            void push_back(const TP& x){
                if(M_finish != M_end_of_storage){
                    construct(M_finish,x);
                    ++M_finish;
                }
                else{
                    M_insert_aux(end(),x);
                }
            }
            //交换容器 -> 交换迭代器所指的内容
            void swap(vector<TP,Alloc>& x){
                mystl::swap(M_start,x.M_start);
                mystl::swap(M_finish,x.M_finish);
                mystl::swap(M_end_of_storage,x.M_end_of_storage);
            }

            // 插入x 分为在最后插入和其他情况
            iterator insert(iterator position, const TP& x){
                size_type n = position - begin();
                if(M_finish!=M_end_of_storage && position == end() ){
                    construct(M_finish,x);
                    ++M_finish;
                }
                else M_insert_aux(position, x);
                return begin() +n;
            }

            iterator insert (iterator position ){
                size_type n=position - begin();
                if(M_finish!=M_end_of_storage && position == end()){
                    construct(M_finish);
                    ++M_finish;
                }
                else M_insert_aux(position);
                return begin() + n ;
            }

            // 判断是否是Integer整型类型
            template <class InputIter>
            void insert(iterator pos,InputIter first, InputIter last){
                typedef typename Is_Integer<InputIter>::Integral Integral;
                M_insert_dispatch(pos, first, last, Integral());
            }
            template <class Integer>
            void M_insert_dispatch(iterator pos,Integer n,Integer val,true_type){
                M_fill_insert(pos, (size_type) n ,(TP) val);
            }
            template <class InputIter>
            void M_insert_dispatch(iterator pos,InputIter first, InputIter last,false_type){
                M_range_insert(pos,first, last, ITERATOR_CATEGORY(first)); }
            void insert(iterator pos, size_type n, const TP& x){
                M_fill_insert(pos,n,x);
            }
            void M_fill_insert (iterator pos, size_type n, const TP& x);
            
            // 弹出vector最后一个元素
            void pop_back(){
                --M_finish;
                destroy(M_finish);
            }
            
            iterator erase(iterator position){
                //排除pop_back的情况  从被erase的元素依次向前赋值(浅拷贝) 最后析构最后一个元素
                if(position+1 != end()) copy( position+1, M_finish, position);
                --M_finish;
                destroy(M_finish);
                return position;
            }
            //擦除一个区间  (浅拷贝)
            iterator erase(iterator first, iterator last){
                iterator i= copy(last, M_finish, first);
                destroy(i,M_finish);
                M_finish= M_finish -(last-first);
                return first;
            }

            // resize 调整容器大小 
            void resize(size_type new_size,const TP& x) {
                if(new_size<size())  erase(begin()+new_size,end());  //若size变小 则进行擦除操作 
                else                 insert(end(),new_size-size(),x);//若size变大 则进行插入操作
            }
            void resize(size_type new_size) { resize(new_size,TP()); }
            void clear()  { erase(begin(),end()); }  //清空容器
        
        protected:
            template <class ForwardIter>
            iterator M_allocate_and_copy(size_type n,ForwardIter first, ForwardIter last)
            {
                iterator result = M_allocate(n);
                try{
                    uninitialized_copy(first, last, result);
                    return result;
                }
                catch(...){
                    M_deallocate(result,n); throw;
                }
            }
            
            template <class InputIter>
            void M_range_initialize(InputIter first, InputIter last, input_iterator_tag){
                for(; first!=last; ++first) push_back(*first);
            }
            template <class ForwardIter>
            void M_range_initialize(ForwardIter first, ForwardIter last,forward_iterator_tag){
                size_type n=0;
                distance(first,last,n);
                M_start=M_allocate(n);
                M_end_of_storage=M_start+n;
                M_finish=uninitialized_copy(first,last,M_start);
            }
            template <class InputIter>
            void M_range_insert(iterator pos,InputIter first,InputIter last,input_iterator_tag);
            template <class ForwardIter>
            void M_range_insert(iterator pos,ForwardIter first, ForwardIter last, forward_iterator_tag);
    };
        template <class TP,class Alloc>
        inline bool operator==(const vector<TP,Alloc>& x,const vector<TP,Alloc>& y){
            return x.size()==y.size() && equal(x.begin(),x.end(),y.begin());
        }
        template <class TP,class Alloc>
        inline bool operator<(const vector<TP,Alloc>& x,const vector<TP,Alloc>& y){
            return lexicographical_compare(x.begin(),x.end(),y.begin(),y.end());
        }
        template <class TP, class Alloc>
        vector<TP,Alloc>& vector<TP,Alloc>::operator=(const vector<TP,Alloc>& x){
            if(&x!=this){
                const size_type xlen=x.size();
                if( xlen>capacity() ) {
                    iterator tmp=M_allocate_and_copy(xlen,x.begin(),x.end());
                    destroy(M_start,M_finish);
                    M_deallocate(M_start,M_end_of_storage-M_start);
                    M_start=tmp;
                    M_end_of_storage=M_start+xlen;
                }
                else if(size() >= xlen) {
                    iterator i= copy(x.begin(),x.end(),begin());
                    destroy(i,M_finish);
                }
                else{
                    copy(x.begin(),x.begin()+size(),M_start);
                    uninitialized_copy(x.begin()+size(),x.end(),M_finish);
                }
                M_finish = M_start + xlen;
                return *this;
            }
        }
        template <class TP,class Alloc>
        void vector<TP,Alloc>::M_fill_assign(size_t n,const value_type& val){
            if( n>capacity() ){
                vector<TP,Alloc> tmp(n,val,get_allocator());
                tmp.swap(*this);
            }
            else if( n>size() ){
                fill(begin(),end(),val);
                M_finish=uninitialized_fill_n(M_finish,n-size(),val);
            }
            else erase(fill_n(begin(),n,val),end());
        }

        //插入操作
        template <class TP, class Alloc>
        void vector<TP,Alloc>::M_insert_aux(iterator position,const TP& x){
            if(M_finish != M_end_of_storage){
                construct(M_finish,*(M_finish-1));
                ++M_finish;
                TP x_copy=x;
                copy_backward(position,M_finish-2,M_finish-1);
                *position=x_copy;
            }
            else{
                const size_type old_size = size();
                const size_type len = old_size != 0 ? 2 * old_size : 1;  //二倍
                iterator new_start = M_allocate(len);
                iterator new_finish = new_start;
                try{
                    new_finish=uninitialized_copy(M_start,position,new_start);
                    construct(new_finish,x);
                    ++new_finish;
                    new_finish=uninitialized_copy(position,M_finish,new_finish);
                }
                catch(...){
                    destroy(new_start,new_finish);
                    M_deallocate(new_start,len);
                    throw;
                }
                destroy(begin(),end());
                M_deallocate(M_start, M_end_of_storage-M_start);
                M_start = new_start;
                M_finish = new_finish;
                M_end_of_storage = new_start + len;
            }
        }

        template <class TP, class Alloc>
        void vector<TP,Alloc>::M_insert_aux(iterator position){
            if(M_finish != M_end_of_storage){
                construct(M_finish,*(M_finish-1));
                ++M_finish;
                copy_backward(position, M_finish-2 , M_finish-1 );
                *position = TP();
            }
            else {
                const size_type old_size=size();
                const size_type len = old_size != 0 ? 2*old_size : 1;
                iterator new_start = M_allocate(len);
                iterator new_finish = new_start;
                try {
                    new_finish = uninitialized_copy(M_start, position, new_start);
                    construct(new_finish);
                    ++new_finish;
                    new_finish = uninitialized_copy(position, M_finish, new_finish);
                }
                catch(...){
                    destroy(new_start,new_finish);
                    M_deallocate(new_start,len);
                    throw;
                }
                destroy(begin(),end());
                M_deallocate(M_start,M_end_of_storage-M_start);
                M_start = new_start;
                M_finish = new_finish;
                M_end_of_storage = new_start + len;
            }
        }

        //在position的位置上插入连续 n个x
        template <class TP,class Alloc>
        void vector<TP,Alloc>::M_fill_insert(iterator position,size_type n,const TP& x){
            if( n != 0) {  
                if(size_type(M_end_of_storage - M_finish) >= n){
                    TP x_copy=x;
                    const size_type elems_after = M_finish - position;
                    iterator old_finish = M_finish;
                    if( elems_after > n){
                        uninitialized_copy(M_finish-n, M_finish,M_finish);
                        M_finish += n;
                        copy_backward(position,old_finish-n,old_finish);
                        fill(position, position+n , x_copy);
                    }
                    else{
                        uninitialized_fill_n(M_finish,n-elems_after,x_copy);
                        M_finish += n-elems_after;
                        uninitialized_copy(position,old_finish,M_finish);
                        M_finish += elems_after;
                        fill(position,old_finish,x_copy);
                    }
                }
                else{
                    const size_type old_size=size();
                    const size_type len= old_size+max(old_size,n);
                    iterator new_start=M_allocate(len);
                    iterator new_finish=new_start;
                    try{
                        new_finish = uninitialized_copy(M_start,position,new_start);
                        new_finish = uninitialized_fill_n(new_finish,n,x);
                        new_finish = uninitialized_copy(position, M_finish, new_finish);
                    }
                    catch(...){
                        destroy(new_start,new_finish);
                        M_deallocate(new_start,len);
                    }
                    destroy(M_start,M_finish);
                    M_deallocate(M_start,M_end_of_storage-M_start);
                    M_start = new_start;
                    M_finish = new_finish;
                    M_end_of_storage = new_start + len;
                }
            }
        }

        template <class TP,class Alloc>
        template <class InputIter>
        void vector<TP,Alloc>::M_range_insert(iterator pos,InputIter first, InputIter last, input_iterator_tag)
        {
            for(; first != last; ++first){
                pos = insert ( pos ,*first);
                ++pos;
            }
        }
        template <class TP,class Alloc>
        template <class ForwardIter>
        void vector<TP,Alloc>::M_range_insert(iterator position,ForwardIter first,ForwardIter last,forward_iterator_tag){
            if( first != last){
                size_type n=0;
                distance(first ,last , n);
                if(size_type(M_end_of_storage - M_finish) >= n){
                    const size_type elems_after = M_finish - position;
                    iterator old_finish = M_finish;
                    if(elems_after > n){
                        uninitialized_copy(M_finish-n , M_finish,M_finish);
                        M_finish +=n;
                        copy_backward(position,old_finish-n,old_finish);
                        copy(first,last,position);
                    }
                    else {
                        ForwardIter mid= first;
                        advance(mid,elems_after);
                        uninitialized_copy(mid,last,M_finish);
                        M_finish+=n-elems_after;
                        uninitialized_copy(position,old_finish,M_finish);
                        M_finish+=elems_after;
                        copy(first,mid,position);
                    }
                }
                else {
                    const size_type old_size=size();
                    const size_type len= old_size+max(old_size,n);
                    iterator new_start=M_allocate(len);
                    iterator new_finish=new_start;
                    try{
                        new_finish=uninitialized_copy(M_start,position,new_start);
                        new_finish=uninitialized_copy(first,last,new_finish);
                        new_finish=uninitialized_copy(position,M_finish,new_finish);
                    }
                    catch(...){
                        destroy(new_start,new_finish);
                        M_deallocate(new_start,len);
                    }
                    destroy(M_start,M_finish);
                    M_deallocate(M_start,M_end_of_storage-M_start);
                    M_start = new_start;
                    M_finish = new_finish;
                    M_end_of_storage = new_start +len;
                }
            }
        }



} // namespace mystl


#endif