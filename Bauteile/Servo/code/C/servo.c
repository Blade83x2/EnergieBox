#include<stdio.h>  
#include<wiringPi.h>  
  
void init();  
int main()  
{  
        init();  
        int angle=180;  
      
  
        int i=0;  
        int x=0;  
  
        int k=180;
        
while(1)
{
while(k--)  
        {  
                x=11.11*i;  
                digitalWrite(1,HIGH);  
                delayMicroseconds(500+x);  
                digitalWrite(1,LOW);  
                delayMicroseconds(19500-x);  
                if(i==angle)  
                    break;  
  
                i++;  
         }  
delay(10);

i=0;
while(k--)  
        {  
                x=11.11*i;  
                digitalWrite(1,HIGH);  
                delayMicroseconds(2500-x);  
                digitalWrite(1,LOW);  
                delayMicroseconds(17500+x);
                if(i==angle)  
                    break;  
  
                i++;
         }
delay(10);
i=0;

}  
          
        return 0;  
}  
  
  
void init()  
{  
        wiringPiSetup();  
        pinMode(1,OUTPUT);
}  