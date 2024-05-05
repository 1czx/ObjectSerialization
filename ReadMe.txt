设计思路：
一、binary
对所有类型的serialize（deserialize）进行模板元函数的重载
给serialize（deserialize）函数提供一个ostream（/istream）类型的参数
对于arithmetic类型直接进行二进制读写
通过ostream（/istream）参数对于其他的容器类型以及pair的serialize（deserialize）通过递归调用相应直到最底层的arithmetic类型而能够在同一个文件中进行读（写）
对于用户自定义的类型只需根据类型内部所定义的类型调用相应serialize（deserialize）即可，例子如下
struct UserDefinedType{
  int idx;
  std::string name;
  std::vector<double> data;
  bool operator==( const UserDefinedType & rhs ) const {
    return ( idx == rhs.idx || name == rhs.name || data == rhs.data );
  }
};

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

最后提供一个统一的接口供用户使用
从而实现了容器以及多层嵌套不同类型的容器的序列化
二、xml
大体思路同上
所提供的tinyxml2中xml文件类的实现方式是一个兄弟儿子表示法的树状结构
那么对于每层嵌套就进入相应的子树
将binary中的ostream（/istream）类型的参数改为父节点的指针再按照上述思路稍作修改即可
用户自定义的类型也是类似的

编译方式：
1. g++ -o test test.cpp tinyxml2.cpp -std=c++17
2. 提供了CMakeLists.txt文件
--mkdir build
--cd build
--cmake ..
--make
--./test
