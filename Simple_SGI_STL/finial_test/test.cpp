#include <iostream>
#include <cstdlib>
#include <cstring>
using namespace std;
class Element {
private:
    int number;
public:
    //默认构造函数
	Element() :number(0) {
	    cout << "ctor" << endl;
	}
    //初始值为num
  	Element(int num):number(num) {
  		cout << "ctor" << endl;
  	}
    //拷贝构造函数
  	Element(const Element& e):number(e.number) {
  		cout << "copy ctor" << endl;
  	}
    //右值拷贝函数
  	Element(Element&& e):number(e.number) {
  		cout << "right value ctor" << endl;
  	}
    //析构函数
  	~Element() {
  		cout << "dtor" << endl;
  	}
  	void operator=(const Element& item) {
  		number = item.number;
  	}
  	bool operator==(const Element& item) {
  		return (number == item.number);
  	}
  	void operator()() {
  		cout << number ;
  	}
  	int GetNumber() {
  		return number;
  	}
};
template<typename T>
class Vector {
private:
  	T* items;  //vector占用的堆内存
  	int count; //元素的数量
public:
    //默认构造函数
  	Vector() :count{ 0 }, items{nullptr} {

  	}
    //拷贝构造函数
  	Vector(const Vector& vector) :count{vector.count} {
  		items = static_cast<T*>(malloc(sizeof(T) * count));
  		memcpy(items, vector.items, sizeof(T) * count);
  	}
    //右值引用构造  接管右值引用的资源并把其置空
  	Vector(Vector&& vector) :count{ vector.count }, items{ vector.items } {
        items = static_cast<T*>(malloc(sizeof(T) * count));
        memcpy(items,vector.items,sizeof(T) * count);
        //for(int i=0; i<count; ++i){
        //    new(items+i) T(move(vector.items[i])); //对应于Elements接受的右值引用构造&&
        //}
  		vector.items=nullptr;
        vector.count=0;
        //TODO
  	}
  	~Vector() {
        Clear();
  		//TODO
  	}
    T& operator[](int index){
    	if (index<0||index>=count) {
    		cout<<"invalid index"<<endl;
    		return items[0];
    	}
    	return items[index];
    }
    int returnCount(){
    	return count;
    }
  	void Clear() {
        for(int i=0;i<count;++i){
            (items+i)->~T();
        }
        if(items!=nullptr) free(items);
        items=nullptr;
        count=0;
  		//TODO
  	}

  	void Add(const T& item) {
        int i;
        int old_count=count;
        T* new_items=static_cast<T*>(malloc(sizeof(T) * (old_count+1)));
        for(i=0;i<count;++i){
            new(new_items+i) T(move(items[i]));
        }
        new(new_items+i) T(move(item));
        Clear();
        items=new_items;
        count=old_count+1;
  		//TODO
  	}
  	bool Insert(const T& item,int index) {
        if (index<0||index>=count) {
    		//cout<<"invalid index"<<endl;
    		return false;
    	}
        int i;
        int old_count=count;
        T* new_items=static_cast<T*>(malloc(sizeof(T) * (old_count+1)));
        for(i=0;i<index;++i){
            new(new_items+i) T(move(items[i]));
        }
        new(new_items+i) T(item);
        for(i=index+1;i<old_count+1;++i){
            new(new_items+i) T(move(items[i-1]));
        }
        Clear();
        items=new_items;
        count=old_count+1;
        return true;
  		//TODO
  	}
  	bool Remove(int index) {
        if (index<0||index>=count) {
    		//cout<<"invalid index"<<endl;
    		return false;
    	}
        int i;
        int old_count=count;
        T* new_items=static_cast<T*>(malloc(sizeof(T) * (old_count-1)));
        for(i=0;i<index;++i){
            new(new_items+i) T(move(items[i]));
        }
        for(i=index+1;i<count;++i){
            new(new_items+i-1) T(move(items[i]));
        }
        Clear();
        items=new_items;
        count=old_count-1;
        return true;
  		//TODO
  	}
  	int Contains(const T& item) {
        for(int i=0;i<count;i++){
            if(items[i]==item) return i;
        }
        return -1;
  		//TODO
  	}
};
template<typename T>
void PrintVector(Vector<T>& v){
	  int count=v.returnCount();
	  for (int i = 0; i < count; i++)
	  {
		  v[i]();
		  cout << " ";
	  }
	  cout << endl;
}
int main() {
  	Vector<Element> v;
  	for (int i = 0; i < 4; i++) {
  		Element e(i);
  		v.Add(e);
  	}
  	PrintVector(v);
  	Element e2(4);
  	if (!v.Insert(e2, 10))
  	{
  		v.Insert(e2, 2);
  	}
  	PrintVector(v);
  	if (!v.Remove(10))
  	{
  		v.Remove(2);
  	}
  	PrintVector(v);
  	Element e3(1), e4(10);
  	cout << v.Contains(e3) << endl;
  	cout << v.Contains(e4) << endl;
  	Vector<Element> v2(v);
  	Vector<Element> v3(move(v2));
  	PrintVector(v3);
  	v2.Add(e3);
  	PrintVector(v2);
  	return 0;
}