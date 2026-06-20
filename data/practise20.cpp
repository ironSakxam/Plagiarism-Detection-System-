#include<iostream>
using namespace std ;
class Item {
    int Number ;
    public :
friend istream & operator >>(istream & , Item & );
friend ostream & operator <<(ostream & , Item & );

};

istream & operator >>(istream & input , Item &C) {
    input>>C.Number ;
    return input ;
}
ostream & operator <<(ostream & output ,Item &C) {
    output<<C.Number ;
    return output ;
}
int main(){
    Item I ;
    cin>>I;
    cout<<I;
    return 0 ;

}