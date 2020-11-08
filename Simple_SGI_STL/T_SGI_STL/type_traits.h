#ifndef __T_SGI_STL_TYPE_TRAITS_H_
#define __T_SGI_STL_TYPE_TRAITS_H_

namespace mystl
{
    struct true_type{};
    struct false_type{};
    
    template <class TP>
    struct type_traits
    {
        typedef true_type    this_dummy_member_must_be_first;
        typedef false_type   has_trivial_default_constructor;//默认构造
        typedef false_type   has_trivial_copy_constructor;//拷贝构造
        typedef false_type   has_trivial_assignment_operator;//拷贝赋值
        typedef false_type   has_trivial_destructor;//析构函数
        typedef false_type   is_POD_type;
    };
    template<> struct type_traits<bool>
    {
        typedef true_type   has_trivial_default_constructor;
        typedef true_type   has_trivial_copy_constructor;
        typedef true_type   has_trivial_assignment_operator;
        typedef true_type   has_trivial_destructor;
        typedef true_type   is_POD_type;
    };
    template<> struct type_traits<char>
    {
        typedef true_type   has_trivial_default_constructor;
        typedef true_type   has_trivial_copy_constructor;
        typedef true_type   has_trivial_assignment_operator;
        typedef true_type   has_trivial_destructor;
        typedef true_type   is_POD_type;
    };
    template<> struct type_traits<signed char>
    {
        typedef true_type   has_trivial_default_constructor;
        typedef true_type   has_trivial_copy_constructor;
        typedef true_type   has_trivial_assignment_operator;
        typedef true_type   has_trivial_destructor;
        typedef true_type   is_POD_type;
    };
    template<> struct type_traits<unsigned char>
    {
        typedef true_type   has_trivial_default_constructor;
        typedef true_type   has_trivial_copy_constructor;
        typedef true_type   has_trivial_assignment_operator;
        typedef true_type   has_trivial_destructor;
        typedef true_type   is_POD_type;
    };
    template<> struct type_traits<short>
    {
        typedef true_type   has_trivial_default_constructor;
        typedef true_type   has_trivial_copy_constructor;
        typedef true_type   has_trivial_assignment_operator;
        typedef true_type   has_trivial_destructor;
        typedef true_type   is_POD_type;
    };
    template<> struct type_traits<unsigned short>
    {
        typedef true_type   has_trivial_default_constructor;
        typedef true_type   has_trivial_copy_constructor;
        typedef true_type   has_trivial_assignment_operator;
        typedef true_type   has_trivial_destructor;
        typedef true_type   is_POD_type;
    };
    template<> struct type_traits<int>
    {
        typedef true_type   has_trivial_default_constructor;
        typedef true_type   has_trivial_copy_constructor;
        typedef true_type   has_trivial_assignment_operator;
        typedef true_type   has_trivial_destructor;
        typedef true_type   is_POD_type;
    };
    template<> struct type_traits<unsigned int>
    {
        typedef true_type   has_trivial_default_constructor;
        typedef true_type   has_trivial_copy_constructor;
        typedef true_type   has_trivial_assignment_operator;
        typedef true_type   has_trivial_destructor;
        typedef true_type   is_POD_type;
    };
    template<> struct type_traits<long>
    {
        typedef true_type   has_trivial_default_constructor;
        typedef true_type   has_trivial_copy_constructor;
        typedef true_type   has_trivial_assignment_operator;
        typedef true_type   has_trivial_destructor;
        typedef true_type   is_POD_type;
    };
    template<> struct type_traits<unsigned long>
    {
        typedef true_type   has_trivial_default_constructor;
        typedef true_type   has_trivial_copy_constructor;
        typedef true_type   has_trivial_assignment_operator;
        typedef true_type   has_trivial_destructor;
        typedef true_type   is_POD_type;
    };
    template<> struct type_traits<long long>
    {
        typedef true_type   has_trivial_default_constructor;
        typedef true_type   has_trivial_copy_constructor;
        typedef true_type   has_trivial_assignment_operator;
        typedef true_type   has_trivial_destructor;
        typedef true_type   is_POD_type;
    };
    template<> struct type_traits<unsigned long long>
    {
        typedef true_type   has_trivial_default_constructor;
        typedef true_type   has_trivial_copy_constructor;
        typedef true_type   has_trivial_assignment_operator;
        typedef true_type   has_trivial_destructor;
        typedef true_type   is_POD_type;
    };
    template<> struct type_traits<float>
    {
        typedef true_type   has_trivial_default_constructor;
        typedef true_type   has_trivial_copy_constructor;
        typedef true_type   has_trivial_assignment_operator;
        typedef true_type   has_trivial_destructor;
        typedef true_type   is_POD_type;
    };
    template<> struct type_traits<double>
    {
        typedef true_type   has_trivial_default_constructor;
        typedef true_type   has_trivial_copy_constructor;
        typedef true_type   has_trivial_assignment_operator;
        typedef true_type   has_trivial_destructor;
        typedef true_type   is_POD_type;
    };
    template<> struct type_traits<long double>
    {
        typedef true_type   has_trivial_default_constructor;
        typedef true_type   has_trivial_copy_constructor;
        typedef true_type   has_trivial_assignment_operator;
        typedef true_type   has_trivial_destructor;
        typedef true_type   is_POD_type;
    };
    //??线性数组 不是动态内存??
    template<> struct type_traits<char*>
    {
        typedef true_type   has_trivial_default_constructor;
        typedef true_type   has_trivial_copy_constructor;
        typedef true_type   has_trivial_assignment_operator;
        typedef true_type   has_trivial_destructor;
        typedef true_type   is_POD_type;
    };
    template<> struct type_traits<signed char*>
    {
        typedef true_type   has_trivial_default_constructor;
        typedef true_type   has_trivial_copy_constructor;
        typedef true_type   has_trivial_assignment_operator;
        typedef true_type   has_trivial_destructor;
        typedef true_type   is_POD_type;
    };
    template<> struct type_traits<unsigned char*>
    {
        typedef true_type   has_trivial_default_constructor;
        typedef true_type   has_trivial_copy_constructor;
        typedef true_type   has_trivial_assignment_operator;
        typedef true_type   has_trivial_destructor;
        typedef true_type   is_POD_type;
    };
    template<> struct type_traits<const char*>
    {
        typedef true_type   has_trivial_default_constructor;
        typedef true_type   has_trivial_copy_constructor;
        typedef true_type   has_trivial_assignment_operator;
        typedef true_type   has_trivial_destructor;
        typedef true_type   is_POD_type;
    };
    template<> struct type_traits<const signed char*>
    {
        typedef true_type   has_trivial_default_constructor;
        typedef true_type   has_trivial_copy_constructor;
        typedef true_type   has_trivial_assignment_operator;
        typedef true_type   has_trivial_destructor;
        typedef true_type   is_POD_type;
    };
    template<> struct type_traits<const unsigned char*>
    {
        typedef true_type   has_trivial_default_constructor;
        typedef true_type   has_trivial_copy_constructor;
        typedef true_type   has_trivial_assignment_operator;
        typedef true_type   has_trivial_destructor;
        typedef true_type   is_POD_type;
    };

    template <class TP>
    struct Is_Integer
    {
        typedef false_type  Integral;
    };
    template<> struct Is_Integer<bool>
    {
        typedef true_type   Integral;
    };
    template<> struct Is_Integer<char>
    {
        typedef true_type   Integral;
    };
    template<> struct Is_Integer<signed char>
    {
        typedef true_type   Integral;
    };
    template<> struct Is_Integer<unsigned char>
    {
        typedef true_type   Integral;
    };
    template<> struct Is_Integer<short>
    {
        typedef true_type   Integral;
    };
    template<> struct Is_Integer<unsigned short>
    {
        typedef true_type   Integral;
    };
    template<> struct Is_Integer<int>
    {
        typedef true_type   Integral;
    };
    template<> struct Is_Integer<unsigned int>
    {
        typedef true_type   Integral;
    };
    template<> struct Is_Integer<long>
    {
        typedef true_type   Integral;
    };
    template<> struct Is_Integer<unsigned long>
    {
        typedef true_type   Integral;
    };
    template<> struct Is_Integer<long long>
    {
        typedef true_type   Integral;
    };
    template<> struct Is_Integer<unsigned long long>
    {
        typedef true_type   Integral;
    };
}

#endif