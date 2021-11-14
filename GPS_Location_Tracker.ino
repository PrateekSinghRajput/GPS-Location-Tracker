//Prateek
//wwww.prateeks.in
//https://www.youtube.com/c/JustDoElectronics/videos

#include<LiquidCrystal.h>
LiquidCrystal lcd(A0, A1, A2, A3, A4, A5);

#include <SoftwareSerial.h>
SoftwareSerial gps(10,11); // RX, TX

String str="";
char str[70];
String gpsString="";

char *test="$GPGGA";

String latitude="No Range      ";
String longitude="No Range     ";

int temp=0,i;
boolean gps_status=0;

void setup() 
{
  lcd.begin(16,2);
  Serial.begin(9600);
  gps.begin(9600);

  lcd.print("Vehicle Tracking");
  lcd.setCursor(0,1);
  lcd.print("    System      ");
  delay(2000);
  gsm_init();
  lcd.clear();
  Serial.println("AT+CNMI=2,2,0,0,0");
  lcd.print("GPS Initializing");
  lcd.setCursor(0,1);
  lcd.print("  No GPS Range  ");
  get_gps();
  delay(2000);
  lcd.clear();
  lcd.print("GPS Range Found");
  lcd.setCursor(0,1);
  lcd.print("GPS is Ready");
  delay(2000);
  lcd.clear();
  lcd.print("System Ready");
  temp=0;
}

void loop()
{
  serialEvent();
  if(temp)
  {
    get_gps();
    tracking();
  }
}

void serialEvent()
{
  while(Serial.available())
  {
    if(Serial.find("Track Vehicle"))
    {
      temp=1;
      break;
    }
    else
    temp=0;
  }
}

void gpsEvent()
{
  gpsString="";
  while(1)
  {
   while (gps.available()>0)            //checking serial data from GPS
   {
    char inChar = (char)gps.read();
     gpsString+= inChar;                    //store data from GPS into gpsString
     i++;
     if (i < 7)                      
     {
      if(gpsString[i-1] != test[i-1])         //checking for $GPGGA sentence
      {
        i=0;
        gpsString="";
      }
     }
    if(inChar=='\r')
    {
     if(i>65)
     {
       gps_status=1;
       break;
     }
     else
     {
       i=0;
     }
    }
  }
   if(gps_status)
    break;
  }
}

void gsm_init()
{
  lcd.clear();
  lcd.print("Finding Module..");
  boolean at_flag=1;
  while(at_flag)
  {
    Serial.println("AT");
    while(Serial.available()>0)
    {
      if(Serial.find("OK"))
      at_flag=0;
    }
    
    delay(1000);
  }

  lcd.clear();
  lcd.print("Module Connected..");
  delay(1000);
  lcd.clear();
  lcd.print("Disabling ECHO");
  boolean echo_flag=1;
  while(echo_flag)
  {
    Serial.println("ATE0");
    while(Serial.available()>0)
    {
      if(Serial.find("OK"))
      echo_flag=0;
    }
    delay(1000);
  }

  lcd.clear();
  lcd.print("Echo OFF");
  delay(1000);
  lcd.clear();
  lcd.print("Finding Network..");
  boolean net_flag=1;
  while(net_flag)
  {
    Serial.println("AT+CPIN?");
    while(Serial.available()>0)
    {
      if(Serial.find("+CPIN: READY"))
      net_flag=0;
    }
    delay(1000);
  }
  lcd.clear();
  lcd.print("Network Found..");
  delay(1000);
  lcd.clear();
}

void get_gps()
{
   gps_status=0;
   int x=0;
   while(gps_status==0)
   {
    gpsEvent();
    int str_lenth=i;
    latitude="";
    longitude="";
    int comma=0;
    while(x<str_lenth)
    {
      if(gpsString[x]==',')
      comma++;
      if(comma==2)       
      latitude+=gpsString[x+1];     
      else if(comma==4)       
      longitude+=gpsString[x+1];
      x++;
    }
    int l1=latitude.length();
    latitude[l1-1]=' ';
    l1=longitude.length();
    longitude[l1-1]=' ';
    lcd.clear();
    lcd.print("Lat:");
    lcd.print(latitude);
    lcd.setCursor(0,1);
    lcd.print("Long:");
    lcd.print(longitude);
    i=0;x=0;
    str_lenth=0;
    delay(2000);
   }
}

void init_sms()
{
  Serial.println("AT+CMGF=1");
  delay(400);
  Serial.println("AT+CMGS=\"+918830584864\"");  
  delay(400);
}

void send_data(String message)
{
  Serial.println(message);
  delay(200);
}

void send_sms()
{
  Serial.write(26);
}

void lcd_status()
{
  lcd.clear();
  lcd.print("Message Sent");
  delay(2000);
  lcd.clear();
  lcd.print("System Ready");
  return;
}

void tracking()
{
    init_sms();
    send_data("Vehicle Tracking");
    send_data("Your Current Location of Vehicle :- ");
    Serial.print("Latitude:");
    send_data(latitude);
    Serial.print("Longitude:");
    send_data(longitude);
    send_data("Please Find Out The Person ");
    send_sms();
    delay(2000);
    lcd_status();
}
