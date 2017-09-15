#include <LWiFi.h>

char ssid[] = "ssid";     
char pass[] = "password";

int status = WL_IDLE_STATUS;
char server[] = "server";  
WiFiClient client;

unsigned long lastConnectionTime = 0;            // last time you connected to the server, in milliseconds
const unsigned long POSTING_INTERVAL = 2L * 1000L; // delay between updates, in milliseconds

const char SEPARATOR_LAB_START='!';
const char SEPARATOR_LAB_END='>';
const char IDENTIFIER_PROF_NON_EXIST='+';
const char IDENTIFIER_STUDENT_INSERTED='(';
const char IDENTIFIER_STUDENT_NON_EXIST='%';
const char IDENTIFIER_STUDENT_NON_INSERT='#';

String barcodeToSendProf[]={"3141591010","3141591017"};
int counterBarcodeProf=0;

String barcodeToSendSt[]={"3141591010","3141591017"};
int counterBarcodeSt=0;

int isTeacher=1;
String infoLab="";
int counterOfArray=0;
String stringCreator="";
String item[]={"","","","",""}; 
int iteratorOfCells=0;
String chosenItemOnArray="";


void setup()
{ 
   Serial.begin(9600);
    while (!Serial) {
        ; // wait for serial port to connect. Needed for native USB port only
    }
    // attempt to connect to Wifi network:
    while (status != WL_CONNECTED) {
       // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
        status = WiFi.begin(ssid, pass);
    }    
}
void loop()
{ 
  while(1==isTeacher){
       if (millis() - lastConnectionTime > POSTING_INTERVAL) {
         Serial.println("Start");
         Serial.println("Retrieving professor ("+barcodeToSendProf[counterBarcodeProf]+") labs");
         connectionToServer(barcodeToSendProf[counterBarcodeProf],isTeacher,infoLab);
       }
     while (client.available()){
          char incoomingChar = client.read();
          if(SEPARATOR_LAB_START==incoomingChar){
            isTeacher=0;
            stringCreator="";
            incoomingChar = client.read();
            counterOfArray=counterOfArray;
          }
          else if(IDENTIFIER_PROF_NON_EXIST==incoomingChar){       
                 Serial.println("non-exist prof..");  
                 stringCreator="";
                 counterBarcodeProf++;
          }  
          if(SEPARATOR_LAB_END==incoomingChar){
            item[counterOfArray]=stringCreator;
            incoomingChar = client.read();
            counterOfArray=counterOfArray+1;             
            stringCreator="";       
            incoomingChar = client.read();
          }  
          stringCreator.concat(incoomingChar);
     }
     while((!client.connected()) && 0==isTeacher){
          printLaboratories(iteratorOfCells);
          break;   
     }
  }  
  
  if(Serial.available()){
    char choic = Serial.read();
    chosenItemOnArray=chooseLab(choic);
  }
   
  while(0==isTeacher && chosenItemOnArray!=""){
       if (millis() - lastConnectionTime > POSTING_INTERVAL) {
       Serial.println("Scan students");
       Serial.println("=============");
       infoLab=chosenItemOnArray;
       connectionToServer(barcodeToSendSt[counterBarcodeSt],isTeacher,infoLab);
       }    
       stringCreator="";
       if(client.available()){
         char incoomingChar = client.read();
         readPrintMsgForStudent(incoomingChar);
       }
       if(barcodeToSendSt[counterBarcodeSt]=="3141591017"){
         printMsgsAndWaitForEnding(); 
       }      
  }
}

//-------------FUCTIONS------------------------------------

void connectionToServer(String b,int isTeacher,String info){
  
    String path="GET /barcode/arController.php?barcode=";
    String teacherPart="&isTeacher=";
    String infoLab="&infoLab=";
    String endOfPath=" HTTP/1.0";

    boolean isConnected=client.connect(server, 80);
  
    if (isConnected){
      path.concat(b);
      path.concat(teacherPart);
      path.concat(isTeacher);
      path.concat(infoLab);
      path.concat(info);
      path.concat(endOfPath);    
      client.println(path);
      client.println("Host:  server");
      client.println("Accept: */*");
      client.println("Connection: close");
      client.println();
      delay(1000);
      lastConnectionTime = millis();
    }
    else{
        Serial.println("Trying to connect again...");   
    }
}

String chooseLab(char choic){
       
      char choice= (choic - '0');
      if(choice >= 0 && choice < 5){
        Serial.println("");
        Serial.println("Your choice is :");
        Serial.println("================");
        Serial.println(item[choice]);
        Serial.println(""); 
        return item[choice];
      }  
}

void readPrintMsgForStudent(char incoomingChar){
    if(IDENTIFIER_STUDENT_INSERTED==incoomingChar){
         Serial.println("inserted student");
         stringCreator="";
         counterBarcodeSt++;
         }
         else if(IDENTIFIER_STUDENT_NON_EXIST==incoomingChar){
                Serial.println("non-exist ");
                Serial.println("student scan...");
                stringCreator="";
                counterBarcodeSt++;
         }
         else if(IDENTIFIER_STUDENT_NON_INSERT==incoomingChar){
                Serial.println("non-inserted ");
                Serial.println("student scan...");
                stringCreator="";
         }
         stringCreator.concat(incoomingChar);
 
}

void printMsgsAndWaitForEnding(){
    Serial.println("");
    Serial.println("Professor scanned");
    Serial.println("RESET the device");
    Serial.println("for another laboratory");
    while(true){         
    }
}

void printLaboratories(int iteratorOfCells){
     client.stop(); 
     Serial.println("");
     Serial.println("Lesson_Day_Time");
     Serial.println("===============");
     for(iteratorOfCells;iteratorOfCells<5;iteratorOfCells++){
        Serial.print(iteratorOfCells);
        Serial.println(" )"+item[iteratorOfCells]);        
     }    
}

