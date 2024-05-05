#include"serialization.h"
#include<iostream>

using namespace std;

int main(){
    cout << "Binary Serialization:" << endl;
    int n0 = 256, n1 = 0;
    binary_Serialization::serialize( n0, "n.data" );
    binary_Serialization::deserialize( n1, "n.data" );
    cout << "int: " << (n0==n1) << endl;
    
    list<double> l0{1,2,3,4},l1;
    binary_Serialization::serialize( l0, "l.data" );
    binary_Serialization::deserialize( l1, "l.data" );
    cout << "std::list<double>: " << (l0==l1) << endl;
    
    //string的实现是调用char的序列化，所以不再测试char的序列化
    string str{"hello"}, str1;
    binary_Serialization::serialize( str, "str.data" );
    binary_Serialization::deserialize( str1, "str.data" );
    cout << "std::string: " <<(str==str1) << endl;
    
    pair<int,double> p0{114,0.514},p1;
    binary_Serialization::serialize( p0, "p.data" );
    binary_Serialization::deserialize( p1, "p.data" );
    cout << "std::pair<int,double>: " << (p0==p1) << endl;
    
    map<int,string> m0{{2,"222"},{1,"1111"},{3,"33"}},m1;
    binary_Serialization::serialize( m0, "m.data" );
    binary_Serialization::deserialize( m1, "m.data" );
    cout << "std::map<int,std::string>: " << (m0==m1) << endl;

    set<char> s0{4,2,3,1,6,5},s1;
    binary_Serialization::serialize( s0, "s.data" );
    binary_Serialization::deserialize( s1, "s.data" );
    cout << "std::set<char>: " <<(s0==s1) << endl;

    //容器嵌套
    vector<vector<int>> v0{{1,1,1},{2,2,2,2},{3,3,3,3,3},{4}}, v1;
    binary_Serialization::serialize( v0, "v.data" );
    binary_Serialization::deserialize( v1, "v.data" );
    cout << "std::vector<std::vector<int>>: " << (v0==v1) << endl;
    
    UserDefinedType u0{114514,"1919810",{{1},{2},{3}}},u1;
    binary_Serialization::serialize( u0, "u.data" );
    binary_Serialization::deserialize( u1, "u.data" );
    cout << "UserDefinedType: " << (u0==u1) << endl << endl;

    cout << "xml_Serialization:" << endl;

    double xn0 = 114.514, xn1 = 0;
    xml_Serialization::serialize_xml( xn0, "int", "n.xml" );
    xml_Serialization::deserialize_xml( xn1, "int", "n.xml" );
    cout << "double: " << (xn0==xn1) << endl;

    list<double> xl0{1,2,3,4},xl1;
    xml_Serialization::serialize_xml( xl0, "std_list", "l.xml" );
    xml_Serialization::deserialize_xml( xl1, "std_list", "l.xml" );
    cout << "std::list<double>: " << (xl0==xl1) << endl;
    
    pair<int,double> xp0{114,0.514},xp1;
    xml_Serialization::serialize_xml( xp0, "std_pair", "p.xml" );
    xml_Serialization::deserialize_xml( xp1, "std_pair", "p.xml" );
    cout << "std::pair<int,double>: " << (xp0==xp1) << endl;
    
    string xstr{"hello"}, xstr1;
    xml_Serialization::serialize_xml( xstr, "std_string", "str.xml" );
    xml_Serialization::deserialize_xml( xstr1, "std_string", "str.xml" );
    cout << "std::string: " << (xstr==xstr1) << endl;
    
    vector<vector<int>> xv0{{1,1,1},{2,2,2,2},{3,3,3,3,3},{4}}, xv1;
    xml_Serialization::serialize_xml( xv0, "std_vector", "v.xml" );
    xml_Serialization::deserialize_xml( xv1, "std_vector", "v.xml" );
    cout << "std::vector<std::vector<int>>: " <<(xv0==xv1) << endl;

    set<char> xs0{4,2,3,1,6,5},xs1;
    xml_Serialization::serialize_xml( xs0, "std::set", "s.xml" );
    xml_Serialization::deserialize_xml( xs1, "std::set","s.xml" );
    cout << "std::set<char>: " <<(s0==s1) << endl;

    map<int,string> xm0{{2,"222"},{1,"1111"},{3,"33"}}, xm1;
    xml_Serialization::serialize_xml( xm0, "std_map", "m.xml");
    xml_Serialization::deserialize_xml( xm1, "std_map", "m.xml");
    cout << "std::map<int,string>: " << (xm0==xm1) << endl;

    UserDefinedType xu0{114514,"1919810",{{1},{2},{3}}},xu1;
    xml_Serialization::serialize_xml( xu0, "UserDefined", "u.xml" );
    xml_Serialization::deserialize_xml( xu1, "UserDefined", "u.xml" );
    cout << "UserDefinedType: " << (xu0==xu1) << endl;

}