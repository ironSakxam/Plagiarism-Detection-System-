 #include<iostream>  
 using namespace std;    
 class Time   
{ 
       int hour, minute;
          public:  void setTime(int h, int m)   
{    
    hour = h;   minute = m;   
}    
 void add(Time t)   
 {  
     hour = hour + t.hour;   
           minute = minute + t.minute;   
 if(minute >= 60)   
 {   
             hour++;   
  minute = minute - 60;   
 }   
 }   
 void show()   
 {   
 cout << "Time = " << hour << " hour " << minute << " minute";   
 }   };   
 int main()   
 {   
 Time t1, t2;   
 t1.setTime(3, 40);   
t2.setTime(2, 50);   
 t1.add(t2); 
  t1.show(); 
    return 0;
  }  
  
