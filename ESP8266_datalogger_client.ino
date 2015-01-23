/*
client to send energy data from sensor board to server board using esp8266 - Tom Dixon Jan 2015
*/

    #include <SoftwareSerial.h>
    #define DEBUG true
    SoftwareSerial esp8266(2,3); // espTX==>arduinoD2, espRX==>D3
    int voltage = 4800;
    int current = 1000;
    void setup()
    {
    Serial.begin(9600);
    esp8266.begin(9600); 
    
    //setting up the esp using AT commands
    sendData("AT+RST\r\n",2000,DEBUG); // reset module
    sendData("AT+CWMODE=1\r\n",1000,DEBUG); // configure as client
    sendData("AT+CWJAP=\"[SSID]\",\"[Password]\"\r\n",5000,DEBUG); //connect to local network if available
    sendData("AT+CIFSR\r\n",1000,DEBUG); // get ip address(es)
    sendData("AT+CIPMUX=0\r\n",1000,DEBUG); // configure for single connection
    sendData("AT+CIPSTART=\"TCP\",\"192.168.4.1\",80\r\n",5000,DEBUG); //connect to server esp
    
    }
     void loop()
    {
    
  String cmd = "GET";
  cmd += "voltalge=";
  cmd += voltage;
  cmd += "current=";
  cmd += current;
  cmd += "\r\n";
  
  esp8266.print("AT+CIPSEND=");
  esp8266.println(cmd.length());
  if(esp8266.find(">"))
  {
    esp8266.print(cmd);
  }
  delay(4000);
  sendData("AT+CIPCLOSE\r\n",3000,DEBUG);
   
  }
     
    /*
    * Name: sendData
    * Description: Function used to send data to ESP8266.
    * Params: command - the data/command to send; timeout - the time to wait for a response; debug - print to Serial window?(true = yes, false = no)
    * Returns: The response from the esp8266 (if there is a reponse)
    */
    String sendData(String command, const int timeout, boolean debug)
    {
    String response = "";
    esp8266.print(command); // send the read character to the esp8266
    long int time = millis();
    while( (time+timeout) > millis())
    {
    while(esp8266.available())
    {
    // The esp has data so display its output to the serial window
    char c = esp8266.read(); // read the next character.
    response+=c;
    }
    }
    if(debug)
    {
    Serial.print(response);
    }
    return response;
    }
