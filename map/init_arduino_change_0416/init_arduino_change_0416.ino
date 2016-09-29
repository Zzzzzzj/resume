double time_u,lat_u,lon_u,alt_u;
int MS_S , stat;

double Datatransfer(char *data_buf,int num)//数据转换：将字符型数据转换为浮点型数据
  {                                           //*data_buf：要转换的数组；num：小数点的个数
    double temp=0.0;
    unsigned char i,j;
  
    if(data_buf[0]=='-')//负数的情况
    {
      i=1;
      //数组中的字符型数据转换成整数并累加
      while(data_buf[i]!='.')
        temp=temp*10+(data_buf[i++]-0x30);
      for(j=0;j<num;j++)
        temp=temp*10+(data_buf[++i]-0x30);
      //将转换后的整数转换成浮点数
      for(j=0;j<num;j++)
        temp=temp/10;
      //转换成负数
      temp=0-temp;
    }
    else//正数情况
    {
      i=0;
      while(data_buf[i]!='.')
        temp=temp*10+(data_buf[i++]-0x30);
      for(j=0;j<num;j++)
        temp=temp*10+(data_buf[++i]-0x30);
      for(j=0;j<num;j++)
        temp=temp/10 ;
    }
    return temp;
  }
  
  char ID()//接收语句的ID
  { 
    char i=0;
    char value[6]={
      '$','G','P','G','G','A'  };//要接收的GPS语句的ID内容
    char val[6]={
      '0','0','0','0','0','0'  };
  
    while(1)
    {
      if(Serial.available())
      {
        val[i] = Serial.read();//接收串口的数据
        if(val[i]==value[i])//对比是否是正确的ID
        {    
          i++;
          if(i==6)
          {
            i=0;
            return 1;//接收完毕返回1
          }
        }
        else
          i=0;
      }
    } 
  }
  
  void comma(char num)//接收逗号字符
  {   
    char val;
    char count=0;//对接收到的逗号计数
  
    while(1)
    {
      if(Serial.available())
      {
        val = Serial.read();
        if(val==',')
          count++;
      }
      if(count==num)//数目正确结束接收
        return;
    }
  
  }
void UTC()//获取时间信息
  {
    char i;
    char time[9]={
      '0','0','0','0','0','0','0','0','0'
    };
    double t=0.0;
  
    if( ID())//语句正确
    {
      comma(1);//接收1个逗号
      //读取语句第一个逗号后的数据
      while(1)
      {
        if(Serial.available())
        {
          time[i] = Serial.read();
          i++;
        }
        if(i==9)
        {
          i=0;
          t=Datatransfer(time,2);//转换成浮点型数据
          t=t+80000.00;//将时间转换成北京时间
          Serial.println(t);//输出时间数据
          time_u=t; 
          return ;
        }  
      }
    }
  }
 void latitude()//获取纬度信息
  {
    char i;
    char lat[10]={
      '0','0','0','0','0','0','0','0','0','0'
    };
  
    if( ID())
    {
      comma(2);
      //读取语句第二个逗号后的数据
      while(1)
      {
        if(Serial.available())
        {
          lat[i] = Serial.read();
          i++;
        }
        if(i==10)
        {
          i=0;
          lat_u=Datatransfer(lat,6);
          Serial.println(lat_u,6);//输出纬度数据
          return;
        }  
      }
    }
  }
  
void longitude()//获取经度信息
  {
    char i;
    char lon[11]={
      '0','0','0','0','0','0','0','0','0','0','0'
    };
  
    if( ID())
    {
      comma(4);
      //读取语句第四个逗号后的数据
      while(1)
      {
        if(Serial.available())
        {
          lon[i] = Serial.read();
          i++;
        }
        if(i==11)
        {
          i=0;
          lon_u=Datatransfer(lon,6);
          Serial.println(lon_u,6);//输出经度数据
          return;
        }  
      }
    }
  }

void altitude()//获取海拔信息
  {
    char i,flag=0;
    char alt[8]={
      '0','0','0','0','0','0','0','0'
    };
  
    if( ID())
    {
      comma(9);
      //读取语句第九个逗号后的数据
      while(1)
      {
        if(Serial.available())
        {
          alt[i] = Serial.read();
          if(alt[i]==',')
            flag=1;
          else
            i++;
        }
        if(flag)
        {
          i=0;
          alt_u=Datatransfer(alt,1);
          Serial.println(alt_u,1);//输出海拔数据
          return;
        }  
      }
    }
  }

int update_net(double lat_n,double lon_n,double alt_n){
     Serial.println("AT");   
     delay(2000);
     Serial.println("AT+CSTT");
     delay(2000);
     Serial.println("AT+CIICR");
     delay(2000);
     Serial.println("AT+CIFSR");
     delay(2000);
     Serial.println("AT+CIPSTART=\"TCP\",\"tcp.lewei50.com\",9960");
     delay(5000);
     Serial.println("AT+CIPSEND");
     delay(2000);
     Serial.print("{\"method\": \"update\",\"gatewayNo\": \"02\",\"userkey\": \"a449390aebc645c09ee7bdf4be8852e1\"}&^!");
     Serial.write(26); 
     delay(3000);
     Serial.println("AT+CIPSEND");
     delay(2000);
     Serial.print("{\"method\": \"upload\",\"data\":[{\"Name\":\"lat\",\"Value\":\"");
     Serial.print(lat_n,5);
     Serial.print("\"},{\"Name\":\"lon\",\"Value\":\"");  
     Serial.print(lon_n,5);
     Serial.print("\"},{\"Name\":\"alt\",\"Value\":\"");  
     Serial.print(alt_n);             
     Serial.print("\"}]}&^!");
     Serial.write(26); 
     delay(2000);
     Serial.println("AT+CIPCLOSE");
     delay(1000);
     return 1;

  }
  
  void setup()
  {
    //设置引脚为输出模式
    pinMode(3,OUTPUT);
    pinMode(4,OUTPUT);
    pinMode(5,OUTPUT);
    //GSM开机时序 
    digitalWrite(5,HIGH);
    delay(1500);
    digitalWrite(5,LOW);
     
    //使能GSM串口
     digitalWrite(3,LOW);
     digitalWrite(4,HIGH);
     delay(2000);
     //设置波特率
     Serial.begin(9600); 
     //等待GPS ready
     delay(5000);
     
     //发送AT命令同步
     Serial.println("AT");   
     delay(2000);
     //turn on GPS power supply
     Serial.println("AT+CGPSPWR=1");
     delay(1000);
     //reset GPS in autonomy mode
     Serial.println("AT+CGPSRST=1");
     delay(1000);
     
     //使能GPS串口
     digitalWrite(4,LOW);
     digitalWrite(3,LOW);
     delay(2000);
   
     MS_S = 0;

  }
  void loop()
  {
      Serial.print("UTC:");
      UTC();
      Serial.print("Lat:");
      latitude();
      Serial.print("Lon:");
      longitude();
      Serial.print("Alt:");
      altitude();
    //  Serial.println(' ');
    // Serial.println(' ');

     if(1.0/lat_u!=0.0  && 1.0/lon_u!=0.0 && 1.0/alt_u!=0.0){
        stat = update_net(lat_u,lon_u,alt_u);
        MS_S+=1;
        
        if(stat == 1 && MS_S == 1){
     // Serial.println("AT");
     // delay(1000);
      Serial.println("AT+CMGF=1");
      delay(1000);
      Serial.println("AT+CMGS=\"18309231012\"");//xxx为电话号码
      delay(1000);
      Serial.print("time:");
       Serial.println(time_u);
      Serial.print("lat:");
      Serial.println(lat_u,5);
      Serial.print("lon:");
      Serial.println(lon_u,5);
      Serial.print("alt:");
      Serial.println(alt_u);      
      delay(1000);
      Serial.write(26);
      if(MS_S==20){
        MS_S=0;
        }
       }
       stat=0;
     }
  }
  
