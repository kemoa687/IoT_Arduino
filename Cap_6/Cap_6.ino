#include <WiFi.h>               // library of wifi
#include <FirebaseESP32.h>      //library of firebase
#include <OneWire.h>            //library of tempreaure sensor
#include <DallasTemperature.h>  //library of tempreaure sensor

#define WIFI_SSID "OMEN"         //name of wifi that esp will connect to
#define WIFI_PASSWORD "12345678" //password of wifi that esp will connect to

#define FIREBASE_HOST "https://cap6-4240d-default-rtdb.firebaseio.com" //database url => we get it from our firebase account
#define FIREBASE_AUTH "NlKhKb6spNOajbj1fu5qb6Fxea86j55jqtiYxwdq"       //databse secret => we get it from our firebase account

FirebaseData fbdo;  //we create object fbdo form FirebaseData class to store our data
String sensor = "sensor"; //we created variable and its name sensor

unsigned long time1; //created variable its name is time1 and its type unsigned long

const int dry =3070 ; // we created this two variables to callibrate the sensor
const int wet = 2189;

#define CO2SensorInput     34       //Co2 sensor(MQ135) connected to pin 34
#define co2Zero     55  //from datasheet

#define MethaneSensor  36  //Methan sensor(MQ5) connected to pin 36(vp)
float gas_value; //created variable its type float and its name gas_value

#define ONE_WIRE_PIN 15  //temp sensor is connected to pin 15
OneWire oneWire(ONE_WIRE_PIN);    //read the readings from pin 15
DallasTemperature sensors(&oneWire);//read the readings from pin 15


void setup() {
  // put your setup code here, to run once:
  pinMode(CO2SensorInput,INPUT);
  Serial.begin(115200); //make serial monitor on
  Serial.println();

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD); //make wifi on and it take the predefined wifi name and wifi password
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) //check the connection of esp32 with the wifi
  {
    Serial.print(".");
    delay(300);

  }
  Serial.println();
  Serial.print("Connected with IP: "); //print the IP of the connected wifi
  Serial.println(WiFi.localIP()); //print the IP of the connected wifi
  Serial.println();

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH); //make the firebase on
  Firebase.reconnectWiFi(true); //check the connection of firebase
  time1=millis(); // store time in time1 variable
 
delay(20000); // delay 20 seconds for gas sensors
}

void loop() {

int humiditySensorVal = analogRead(33); //taking readings from pin 33 that sensor send and store it in humiditySensorVal
int percentageHumididy = map(humiditySensorVal, wet, dry, 100, 0); //converting sensor reading to percent of humidity

int co2now[10];       // variable to store 10 readings from the co2 sensor                        
  int co2raw = 0;                               
  int co2ppm = 0;                               
  int zzz = 0;                                


  for (int x = 0;x<10;x++)  //taking 10 readings from pin 34
  {                   
    co2now[x]=analogRead(34);
    delay(200);
  }

  for (int x = 0;x<10;x++)  //add samples together
  {                     
    zzz=zzz + co2now[x];  
  }
  
  co2raw = zzz/10;                           //taking average of 10 readings
  co2ppm = co2raw - co2Zero;                 //get calculated ppm or final concentration
  delay(50);  

gas_value=analogRead(MethaneSensor);  //store readings from methane analog pin
float gas_value_ppm = map(gas_value, 0, 4095, 0, 10000); //converting the readings of methane gas sensor to ppm in its range 

sensors.requestTemperatures(); // calling or requesting the temp sensor readings

           if(millis()-time1>=1000){ //send the readings of the sensor every second
           Firebase.setInt(fbdo, sensor + "/moisture", percentageHumididy ); // sending soil moisture reading to firebase
           Firebase.setFloat(fbdo, sensor + "/methaneConc", gas_value_ppm ); //sending methane gas sensor readings
           Firebase.setInt(fbdo, sensor + "/co2Conc", co2ppm ); //sending co2 sensor gas readings to firebase
           Firebase.setFloat(fbdo, sensor + "/temp", sensors.getTempCByIndex(0) ); //sending temp sensor readings to firebase

            time1=millis();
          
           }



}
