#ifndef __SERIALIZATION_H__
#define __SERIALIZATION_H__

#include<iostream>
#include<sstream>
#include<fstream>
#include<vector>
#include<list>
#include<set>
#include<map>
#include<string>
#include<type_traits>
#include"tinyxml2.h"

struct UserDefinedType{
  int idx;
  std::string name;
  std::vector<double> data;
  bool operator==( const UserDefinedType & rhs ) const {
    return ( idx == rhs.idx || name == rhs.name || data == rhs.data );
  }
};

namespace binary_Serialization{

    //arithmetic类型
    template<typename T, typename std::enable_if_t<std::is_arithmetic<T>::value, int> = 0>
    void serialize( const T & rhs, std::ostream & fout );

    template<typename T, typename std::enable_if_t<std::is_arithmetic<T>::value, int> = 0>
    void deserialize( T & rhs, std::istream & fin );

    //带迭代器的stl容器的序列化
    template<typename T, typename std::enable_if_t<std::is_same<typename T::iterator, decltype(std::declval<T>().begin())>::value && std::is_same<typename T::iterator, decltype(std::declval<T>().end())>::value, int> = 0>
    void serialize( const T & rhs, std::ostream & fout );

    //除map和set之外的stl容器的反序列化
    template<typename T, typename std::enable_if_t<std::is_same<typename T::iterator, decltype(std::declval<T>().begin())>::value && std::is_same<typename T::iterator, decltype(std::declval<T>().end())>::value, int> = 0>
    void deserialize( T & rhs, std::istream & fin );

    template<typename T>
    void deserialize( std::set<T> & rhs, std::istream & fin );

    template<typename K,typename V>
    void deserialize( std::map<K,V> & rhs, std::istream & fin );

    template<typename F,typename S>
    void serialize( const std::pair<F,S> & rhs, std::ostream & fout );

    template<typename F,typename S>
    void deserialize( std::pair<F,S> & rhs, std::istream & fin );

    template<typename T, typename std::enable_if_t<std::is_arithmetic<T>::value, int> = 0>
    void serialize( const T & rhs, std::ostream & fout ){
        fout.write( (const char*)&rhs, sizeof(T) );//对于算术类型直接写进二进制文件
    }
    template<typename T, typename std::enable_if_t<std::is_arithmetic<T>::value, int> = 0>
    void deserialize( T & rhs, std::istream & fin ){
        fin.read( (char*)&rhs, sizeof(T) );//对于算术类型直接读二进制文件
    }

    template<typename T, typename std::enable_if_t<std::is_same<typename T::iterator, decltype(std::declval<T>().begin())>::value && std::is_same<typename T::iterator, decltype(std::declval<T>().end())>::value, int> = 0>
    void serialize( const T & rhs, std::ostream & fout ){
        //对于stl容器，先序列化元素个数，再递归序列化容器内的元素
        unsigned int size = rhs.size();
        fout.write( (const char*)&size, sizeof(unsigned int) );
        for( auto & x: rhs ) serialize( x, fout );
    }
    template<typename T, typename std::enable_if_t<std::is_same<typename T::iterator, decltype(std::declval<T>().begin())>::value && std::is_same<typename T::iterator, decltype(std::declval<T>().end())>::value, int> = 0>
    void deserialize( T & rhs, std::istream & fin ){
        //先读出容器大小
        //除了set和map其他的stl容器可以直接设置容器大小，并通过引用对其中的元素进行递归反序列化
        unsigned int size = 0;
        fin.read( (char*)&size, sizeof(unsigned int) );
        rhs.resize(size);
        for( auto & x: rhs ) deserialize( x, fin );
    }

    //set 和 map 同样需要先读出容器大小
    //再主意反序列化所有元素插入容器
    template<typename T>
    void deserialize( std::set<T> & rhs, std::istream & fin ){
        unsigned int size = 0;
        fin.read( (char*)&size, sizeof(unsigned int) );
        T temp;
        rhs.clear();
        for( int i = 0; i < size; i++ ){
            deserialize( temp, fin );
            rhs.insert(temp);
        }
    }

    template<typename K,typename V>
    void deserialize( std::map<K,V> & rhs, std::istream & fin ){
        unsigned int size = 0;
        fin.read( (char*)&size, sizeof(unsigned int) );
        std::pair<K,V> temp;
        rhs.clear();
        for( int i = 0; i < size; i++ ){
            deserialize( temp, fin );
            rhs.insert(temp);
        }
    }

    //直接对first和second序列化即可
    template<typename F,typename S>
    void serialize( const std::pair<F,S> & rhs, std::ostream & fout ){
        serialize( rhs.first, fout );
        serialize( rhs.second, fout); 
    }
    //直接对first和second反序列化即可
    template<typename F,typename S>
    void deserialize( std::pair<F,S> & rhs, std::istream & fin ){
        deserialize( rhs.first, fin );
        deserialize( rhs.second, fin );
    }

    //对于用户自己定义的类型
    //只需根据class内部的类型依次调用相关的序列化接口即可，反序列化也相同
    void serialize( const UserDefinedType & rhs, std::ostream & fout ){
        serialize( rhs.idx, fout );
        serialize( rhs.name, fout );
        serialize( rhs.data, fout );
    }

    void deserialize( UserDefinedType & rhs, std::istream & fin ){
        deserialize( rhs.idx, fin );
        deserialize( rhs.name, fin );
        deserialize( rhs.data, fin );
    }

    //为所有类型的序列化提供统一的接口
    template<typename T>
    void serialize( const T & rhs, const std::string & filename ){
        std::ofstream fout( filename, std::ios::binary );
        serialize( rhs, fout );
        fout.close();
    }
    //为所有类型的反序列化提供统一的接口
    template<typename T>
    void deserialize( T & rhs, const std::string & filename ){
        std::ifstream fin( filename, std::ios::binary );
        deserialize( rhs, fin );
        fin.close();
    }
}

//和二进制序列化大差不多，都是利用递归调用实现嵌套序列化和反序列化，只不过多了对xml文件的操作
namespace xml_Serialization{

    template<typename T, typename std::enable_if_t<std::is_arithmetic<T>::value, int> = 0>
    void serialize( const T & rhs, const std::string & name, tinyxml2::XMLElement* & pE, tinyxml2::XMLDocument & doc );

    template<typename T, typename std::enable_if_t<std::is_same<typename T::iterator, decltype(std::declval<T>().begin())>::value && std::is_same<typename T::iterator, decltype(std::declval<T>().end())>::value, int> = 0>
    void serialize( const T & rhs, const std::string & name, tinyxml2::XMLElement* & pE, tinyxml2::XMLDocument & doc);

    template<typename F,typename S>
    void serialize( const std::pair<F,S> & rhs, const std::string & name, tinyxml2::XMLElement* & pE, tinyxml2::XMLDocument & doc );

    template<typename T, typename std::enable_if_t<std::is_arithmetic<T>::value, int> = 0>
    void deserialize( T & rhs, tinyxml2::XMLElement* pE );

    template<typename T, typename std::enable_if_t<std::is_same<typename T::iterator, decltype(std::declval<T>().begin())>::value && std::is_same<typename T::iterator, decltype(std::declval<T>().end())>::value, int> = 0>
    void deserialize( T & rhs, tinyxml2::XMLElement* pE );

    template<typename F,typename S>
    void deserialize( std::pair<F,S> & rhs, tinyxml2::XMLElement* pE );
    
    template<typename T>
    void deserialize( std::set<T> & rhs, tinyxml2::XMLElement* pE );

    template<typename K,typename V>
    void deserialize( std::map<K,V> & rhs, tinyxml2::XMLElement* pE );

    template<>
    void deserialize<std::string>( std::string & rhs, tinyxml2::XMLElement* pE );

    template<typename T, typename std::enable_if_t<std::is_arithmetic<T>::value, int> = 0>
    void serialize( const T & rhs, const std::string & name, tinyxml2::XMLElement* & pE, tinyxml2::XMLDocument & doc ){
        tinyxml2::XMLElement* element = doc.NewElement(name.c_str());
        pE->InsertEndChild( element );
        element->SetAttribute( "val", rhs );
    }

    template<typename F,typename S>
    void serialize( const std::pair<F,S> & rhs, const std::string & name, tinyxml2::XMLElement* & pE, tinyxml2::XMLDocument & doc ){
        tinyxml2::XMLElement* element = doc.NewElement(name.c_str());
        pE->InsertEndChild( element );
        serialize( rhs.first, "first", element, doc );
        serialize( rhs.second, "second", element, doc );
    }

    template<typename T, typename std::enable_if_t<std::is_same<typename T::iterator, decltype(std::declval<T>().begin())>::value && std::is_same<typename T::iterator, decltype(std::declval<T>().end())>::value, int> = 0>
    void serialize( const T & rhs, const std::string & name, tinyxml2::XMLElement* & pE, tinyxml2::XMLDocument & doc){
        tinyxml2::XMLElement* element = doc.NewElement(name.c_str());
        pE->InsertEndChild( element );
        for( const auto & x: rhs ) serialize( x, "item", element, doc );
    }

    template<typename T, typename std::enable_if_t<std::is_arithmetic<T>::value, int> = 0>
    void deserialize( T & rhs, tinyxml2::XMLElement* pE ){
        const char* valueStr = pE->Attribute("val");
        double temp;
        std::istringstream iss(valueStr);
        iss >> temp;
        rhs = temp;
    }

    template<typename F,typename S>
    void deserialize( std::pair<F,S> & rhs, tinyxml2::XMLElement* pE ){
        tinyxml2::XMLElement* element = pE->FirstChildElement();
        deserialize( rhs.first, element );
        element = element->NextSiblingElement();
        deserialize( rhs.second, element );
    }

    template<typename T, typename std::enable_if_t<std::is_same<typename T::iterator, decltype(std::declval<T>().begin())>::value && std::is_same<typename T::iterator, decltype(std::declval<T>().end())>::value, int> = 0>
    void deserialize( T & rhs, tinyxml2::XMLElement* pE ){
        rhs.clear();
        tinyxml2::XMLElement* element = pE->FirstChildElement();
        while( element != nullptr ){
            typename T::value_type item;
            deserialize( item, element );
            rhs.push_back( item );
            element = element->NextSiblingElement();
        }
    }

    template<typename T>
    void deserialize( std::set<T> & rhs, tinyxml2::XMLElement* pE ){
        rhs.clear();
        tinyxml2::XMLElement* element = pE->FirstChildElement();
        while( element != nullptr ){
            T item;
            deserialize( item, element );
            rhs.insert( item );
            element = element->NextSiblingElement();
        }
    }

    template<typename K,typename V>
    void deserialize( std::map<K,V> & rhs, tinyxml2::XMLElement* pE ){
        rhs.clear();
        tinyxml2::XMLElement* element = pE->FirstChildElement();
        while( element != nullptr ){
            std::pair<K,V> item;
            deserialize( item, element );
            rhs.insert( item );
            element = element->NextSiblingElement();
        }
    }

    template<>
    void deserialize<std::string>( std::string & rhs, tinyxml2::XMLElement* pE ){
        rhs.clear();
        tinyxml2::XMLElement* element = pE->FirstChildElement();
        while( element != nullptr ){
            char item;
            deserialize( item, element );
            rhs += item;
            element = element->NextSiblingElement();
        }
    }

    void serialize( const UserDefinedType & rhs, const std::string & name, tinyxml2::XMLElement* & pE, tinyxml2::XMLDocument & doc ){
        tinyxml2::XMLElement* element = doc.NewElement(name.c_str());
        pE->InsertEndChild( element );
        serialize( rhs.idx, "int", element, doc );
        serialize( rhs.name, "std_string", element, doc );
        serialize( rhs.data,  "std_vector", element, doc );
    }
 
    void deserialize( UserDefinedType & rhs, tinyxml2::XMLElement* pE ){
        tinyxml2::XMLElement* element = pE->FirstChildElement();
        deserialize( rhs.idx, element );
        element = element->NextSiblingElement();
        deserialize( rhs.name, element );
        element = element->NextSiblingElement();
        deserialize( rhs.data, element );
    }

    template<typename T>
    void serialize_xml( const T & rhs, const std::string & name, const std::string & filename ){
        tinyxml2::XMLDocument doc;
        tinyxml2::XMLElement* root( doc.NewElement("serialization") );
        doc.InsertEndChild( root );
        serialize( rhs, name, root, doc );
        doc.SaveFile( filename.c_str() );
    }

    template<typename T>
    void deserialize_xml( T& rhs, const std::string & name, const std::string & filename ){
        tinyxml2::XMLDocument doc;
        doc.LoadFile( filename.c_str() );
        tinyxml2::XMLElement* root = doc.FirstChildElement();
        deserialize( rhs, root->FirstChildElement() );
    }
    
}


#endif