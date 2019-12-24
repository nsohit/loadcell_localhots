#include "HX711.h"
const int SCALE_DOUT_PIN = D6;
const int SCALE_SCK_PIN = D7;

HX711 scale(SCALE_DOUT_PIN, SCALE_SCK_PIN);
#include <ESP8266WiFi.h>           // Use this for WiFi instead of Ethernet.h
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>

float calibration_factor = -7050; // this calibration factor is adjusted according to my load cell
float units;
float ounces;

IPAddress server_addr(192,168,43,245);  // IP of the MySQL server here
char user[] = "admin";              // MySQL user login username
char password[] = "admin";        // MySQL user login password

// Sample query
char INSERT_SQL[] = "INSERT INTO panen.data_panenikan (hasil) VALUES (%f)";
char query [100];
// WiFi card example
char ssid[] = "Toy";         // your SSID
char pass[] = "balivio16";     // your SSID Password

WiFiClient client;                 // Use this for WiFi instead of EthernetClient
MySQL_Connection conn(&client);
MySQL_Cursor* cursor;

void setup()
{
  Serial.begin(115200);
  while (!Serial); // wait for serial port to connect. Needed for Leonardo only

  scale.set_scale();
  scale.tare();  //Reset the scale to 0
  long zero_factor = scale.read_average(); //Get a baseline reading
  
  // Begin WiFi section
  Serial.printf("\nConnecting to %s", ssid);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // print out info about the connection:
  Serial.println("\nConnected to network");
  Serial.print("My IP address is: ");
  Serial.println(WiFi.localIP());

  Serial.print("Connecting to SQL...  ");
  if (conn.connect(server_addr, 3306, user, password))
    Serial.println("OK.");
  else
    Serial.println("FAILED.");
  
  // create MySQL cursor object
  cursor = new MySQL_Cursor(&conn);
}

void loop()
{
 
  scale.set_scale(calibration_factor); //Adjust to this calibration factor
  units = scale.get_units(),10;
  //units = 0.00; 
  ounces = units * 35.274;
  if (units > 1){
 
  delay(1000); //10 sec
  sprintf(query,INSERT_SQL, units);
  Serial.println("Recording data.");
  Serial.println(query);
  cursor->execute(query);
  }
   if(Serial.available())
  {
    char temp = Serial.read();
    if(temp == '+' || temp == 'a')
      calibration_factor += 100;
    else if(temp == '-' || temp == 'z')
      calibration_factor -= 100;
   delay(1000);
  
  }
}
