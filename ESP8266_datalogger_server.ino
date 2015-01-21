/*
Arduino code for remote power monitor using ESP2866 - Tom Dixon Jan 2015.
We set up a server which remote nodes can connect to by making http requests on port 80
The code then scans the http request for the phrases "voltage=" and "current=" which it converts into integers
These are displayed on a local LCD then, when internet access is available, the unit connects to a thingspeak server and posts the values

TODO
battery state of charge calculations
lcd stuff
wind speed

*/


    #include <SoftwareSerial.h>
    #define DEBUG true
    SoftwareSerial esp8266(2,3); // espTX==>arduinoD2, espRX==>D3
    String GET = "GET /update?key=[ThingspeakAPIkey]&field1="; //Thingspeak API key for remote logging

    void setup()
    {
    Serial.begin(9600);
    esp8266.begin(9600); 
    
    //setting up the esp using AT commands
    sendData("AT+RST\r\n",2000,DEBUG); // reset module
    sendData("AT+CWMODE=3\r\n",1000,DEBUG); // configure as client AND access point
    sendData("AT+CWJAP=\"[SSID]\",\"[Password]\"\r\n",5000,DEBUG); //connect to local network if available
    sendData("AT+CIFSR\r\n",1000,DEBUG); // get ip address(es)
    sendData("AT+CIPMUX=1\r\n",1000,DEBUG); // configure for multiple connections
    sendData("AT+CIPSERVER=1,80\r\n",1000,DEBUG); // turn on server on port 80
    
  }
     
    void loop()
    {
    if(esp8266.available()) // check if the esp is sending a message
    {
    if(esp8266.find("+IPD,"))
    {
    delay(1000); // wait for the serial buffer to fill up (read all the serial data)
    // get the connection id so that we can then disconnect
    int connectionId = esp8266.read()-48; // subtract 48 because the read() function returns
    // the ASCII decimal value and 0 (the first decimal number) starts at 48
    esp8266.find("voltage="); // advance cursor to "voltage="
    int voltage = (esp8266.read()-48)*1000; // voltage readings are 4 digits so multiply first digit by 1000
    voltage += (esp8266.read()-48)*100; //then multiply next digit by 100
    voltage += (esp8266.read()-48)*10;
    voltage += (esp8266.read()-48);
    
    esp8266.find("current="); // advance cursor to "current="
    int current = (esp8266.read()-48)*1000; // 
    current += (esp8266.read()-48)*100;
    current += (esp8266.read()-48)*10;
    current += (esp8266.read()-48);
    
    String closeCommand = "AT+CIPCLOSE=";
    closeCommand+=connectionId; // append connection id
    closeCommand+="\r\n";
    sendData(closeCommand,1000,DEBUG); // close connection
  thingSpeak(voltage, current);  
  }
    }
     
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

    //repeat data to thingspeak server
    
    int thingSpeak(int volt, int amp)
    {
      sendData("AT+CWJAP=\"[SSID]\",\"[password]\"\r\n",5000,DEBUG); //connect to local network if available
      delay(1000);
      sendData("AT+CIPMUX=0\r\n",2000,DEBUG); 
      delay(1000);
      sendData("AT+CIPSTART=\"TCP\",\"184.106.153.149\",80\r\n",5000,DEBUG);
      
  String cmd = GET;
  cmd += volt;
  cmd += "&field2=";
  cmd += amp;
  cmd += "\r\n";
  esp8266.print("AT+CIPSEND=");
  esp8266.println(cmd.length());
  if(esp8266.find(">"))
  {
    esp8266.print(cmd);
  }
  delay(4000);
  sendData("AT+CIPCLOSE\r\n",3000,DEBUG);
  sendData("AT+CIPMUX=1\r\n",1000,DEBUG); 
  
  
  
}
