#include <AltSoftSerial.h>
AltSoftSerial A7board;

int8_t answer;
char* latitude;
char* longitude;
char* fields[15];
/************************************************************************************************/
void setup() {
  String cont;
  Serial.begin(19200);
  A7board.begin(19200);

  Serial.println("Start");
  A7board.listen();

  Serial.print("Swiching to 19200 baudrait -> ");
  while (answer == 0) {   // Send AT every two seconds and wait for the answer
    answer = sendATcommand("AT", "OK", 2000, cont);
    Serial.print("*");
  }
  Serial.println("");

  sendCommand("AT+CREG=0", "OK", 2000, 1000, cont);
  sendCommand("AT+CIPMUX=0", "OK", 2000, 1000, cont);
  sendCommand("AT+CGATT=1", "OK", 4000, 1000, cont);
  sendCommand("AT+CSTT=""wap"",""0000"",""0000""", "OK", 2000, 0, cont);
  sendCommand("AT+CIICR", "OK", 5000, 1000, cont);
  sendCommand("AT+CIFSR", "OK", 2000, 1000, cont);
  sendCommand("AT+GPS=1", "OK", 2000, 0, cont);
}

/************************************************************************************************/
void loop() {
  uploadGPS();
  delay(45000);
}

/************************************************************************************************/
void uploadGPS() {
  String ans;
  int i;
  if (sendCommand("AT+GPSRD", "OK", 10000, 0, ans)) {
    String latt = (char*)latitude;
    String lont = (char*)longitude;
    if ((latt.toInt() != 0) && (lont.toInt() != 0)) {      
      sendCommand("AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",80", "CONNECT OK", 5000, 1000, ans);
      sendCommand("AT+CIPSEND=44", ">", 2000, 500, ans);
      A7board.println("GET /update?api_key=*****enter your API key******&field1=" + latt + "&field2=" + lont + "&field3=10");
      delay(5000);
      print_result();
    }
  }
}

/************************************************************************************************/
int8_t sendATcommand(String ATcommand, String expected_answer, unsigned int timeout, String &content) {
  uint8_t answer = 0;
  unsigned long previous;

  delay(100); // Delay to be sure no passed commands interfere
  while ( A7board.available() > 0) A7board.read();   // Wait for clean input buffer
  A7board.println(ATcommand);
  previous = millis();
  content = "";
  // this loop waits for the answer
  do {
    // if there are data in the UART input buffer, reads it and checks for the asnwer
    if (A7board.available() != 0) {
      String part = String(char(A7board.read()));
      content += part;

      // check if the desired answer is in the response of the module
      if (content.indexOf(expected_answer) > 0) {
        answer = 1;
      }
    }
  } while ((answer == 0) && ((millis() - previous) < timeout));
  return answer;
}

/************************************************************************************************/
boolean sendCommand(String cmd, String expAnsw, unsigned int timeout, unsigned int freez, String &cont) {
  //String cont;
  cont = "";
  boolean stat = false;
  if (sendATcommand(cmd, expAnsw, timeout, cont) == 1) {
    Serial.print("OK: ");
    stat = true;
  } else {
    Serial.print("ERROR: ");
    stat = false;
  }
  Serial.println("*********** Content Begin *************");
  if(cmd.equals("AT+GPSRD")){
    char data[92];
    cont.toCharArray(data, 92) ;
    char* value;
    value = strtok (data, ",");
    int i = 0;
    while (value != NULL) {
      fields[i] = value;
      value = strtok (NULL, ",");
      i++;
    }
    Serial.print("Latitude: ");
    latitude = fields[2];
    Serial.print(latitude);
    Serial.println(fields[3]);
    Serial.print("Longitude: ");
    longitude = fields[4];
    Serial.print(longitude);
    Serial.println(fields[5]);

  }
  Serial.println(replace4VIEW(cont));
  Serial.println("*********** Content End *************");
  Serial.println();
  delay(freez);
  return stat;
}
String replace4URL(String txt) {
  txt.replace("\r", "%0D");
  txt.replace("\n", "%0A");
  return (txt);
}
String replace4VIEW(String txt) {
  txt.replace("\r", "\\r");
  txt.replace("\n", "\\n");
  return (txt);
}

/************************************************************************************************/
int8_t convert2Degrees(char* input) {
  float deg;
  float minutes;
  boolean neg = false;

  //auxiliar variable
  char aux[10];

  if (input[0] == '-') {
    neg = true;
    strcpy(aux, strtok(input + 1, "."));
  } else {
    strcpy(aux, strtok(input, "."));
  }

  // convert string to integer and add it to final float variable
  deg = atof(aux);

  strcpy(aux, strtok(NULL, '\0'));
  minutes = atof(aux);
  minutes /= 1000000;
  if (deg < 100) {
    minutes += deg;
    deg = 0;
  } else {
    minutes += int(deg) % 100;
    deg = int(deg) / 100;
  }

  // add minutes to degrees
  deg = deg + minutes / 60;


  if (neg == true) {
    deg *= -1.0;
  }

  neg = false;

  if ( deg < 0 ) {
    neg = true;
    deg *= -1;
  }

  float numberFloat = deg;
  int intPart[10];
  int digit;
  long newNumber = (long)numberFloat;
  int size = 0;

  while (1) {
    size = size + 1;
    digit = newNumber % 10;
    newNumber = newNumber / 10;
    intPart[size - 1] = digit;
    if (newNumber == 0) {
      break;
    }
  }

  int index = 0;
  if ( neg ) {
    index++;
    input[0] = '-';
  }
  for (int i = size - 1; i >= 0; i--) {
    input[index] = intPart[i] + '0';
    index++;
  }

  input[index] = '.';
  index++;

  numberFloat = (numberFloat - (int)numberFloat);
  for (int i = 1; i <= 6 ; i++) {
    numberFloat = numberFloat * 10;
    digit = (long)numberFloat;
    numberFloat = numberFloat - digit;
    input[index] = char(digit) + 48;
    index++;
  }
  input[index] = '\0';
}

/************************************************************************************************/
void print_result() {
  while ( A7board.available() != 0)
    Serial.write( A7board.read());
  Serial.println();
}

