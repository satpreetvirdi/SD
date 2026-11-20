#include<iostream>


// Thread Safe 
class Singleton{
    private: 
    static Singleton *obj; 
    Singleton(){}
    public :
    static Singleton* getInstance(){
        if(obj==nullptr){
            obj = new Singleton();
        }
        return obj;
    }
}

Singleton* Singleton::obj = nullptr;


// Eager initialisation 

class Singleton{
    private:
    static Singleton *obj;
    Singleton(){};
    public :
    static Singleton* getinstance(){
        return obj;
    }
}

Singleton* Singleton::obj = new Singleton();
