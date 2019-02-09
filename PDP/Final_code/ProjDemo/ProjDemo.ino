#include <Fuzzy.h>
#include <FuzzyComposition.h>
#include <FuzzyInput.h>
#include <FuzzyIO.h>
#include <FuzzyOutput.h>
#include <FuzzyRule.h>
#include <FuzzyRuleAntecedent.h>
#include <FuzzyRuleConsequent.h>
#include <FuzzySet.h>

/*--- wireless stuff--------*/
#include <SoftwareSerial.h>
#define RX 10
#define TX 11
String AP = "nokiaplus";       // CHANGE ME
String PASS = "harnok123"; // CHANGE ME
String API = "DELYXM0LLCCZIMP3";   // CHANGE ME
String HOST = "api.thingspeak.com";
String PORT = "80";
String field1 = "field11";
//String field1 = "field2";
String field2 = "field2";
String field3 = "field3";
int countTrueCommand;
int countTimeCommand; 
boolean found = false; 
int Serial_Number = 111;
int Device_Name = 222;
int Location = 3;
SoftwareSerial esp8266(RX,TX); 
SoftwareSerial mySerial(3,4);//tx rx

/*--------------*/

// Step 1 -  Instantiating an object library
Fuzzy* fuzzy = new Fuzzy();

//const int buzzer=8;
const int buzzer=5;
//const int redLed=7;
const int redLed=8;
//const int blueLed = 6 ;
//const int greenLed = 2;
const int trigPin = 9;
const int echoPin = 12;
const int minimum= 50;
const int trigPin_back = 7;
const int echoPin_back = 6; 
long duration;
int distance;

int duration_back;
int distance_back;

int x,y,n;

void setup() {

  // put your setup code here, to run once:

  //--------------------MOTOR--------------------
  // put your setup code here, to run once:
  
  // for motor
  Serial.begin(9600);
  mySerial.begin(9600);
  /*----- wifi stuff again */
  esp8266.begin(115200);
  sendCommand("AT",5,"OK");
  sendCommand("AT+CWMODE=1",5,"OK");
  sendCommand("AT+CWJAP=\""+ AP +"\",\""+ PASS +"\"",20,"OK");
 /*----------------------------*/
  pinMode(3,OUTPUT);
  pinMode(5,OUTPUT);
  pinMode(6,OUTPUT);
  pinMode(11,OUTPUT);

  //------------BUZZER-------------------------
  pinMode(buzzer,OUTPUT);
  //acc
  pinMode(A0,INPUT);
  pinMode(A1,INPUT);
   Serial.begin(9600);

//----------------REDLED------------
 pinMode(redLed,OUTPUT);

// //----------------BLUELED----------
// pinMode(blueLed,OUTPUT);
//
// //----------------GREENLED----------
// pinMode(greenLed,OUTPUT);

   //--------------------ULTRASONIC SENSOR---------
pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
pinMode(echoPin, INPUT); // Sets the echoPin as an Input

//FOR SENSOR BACK
pinMode(trigPin_back, OUTPUT); // Sets the trigPin as an Output
pinMode(echoPin_back, INPUT); // Sets the echoPin as an Input

//--------------------------FUZZY RULES------------------
  
 FuzzyInput* distance = new FuzzyInput(1);// With its ID in param
 
  // Creating the FuzzySet to compond FuzzyInput distance
 FuzzySet* small = new FuzzySet(0, 0, 20, 40); // Small distance
 distance->addFuzzySet(small); // Add FuzzySet small to distance
 FuzzySet* moderate = new FuzzySet(30, 50, 50, 70); // Safe distance
 distance->addFuzzySet(moderate); // Add FuzzySet safe to distance
 FuzzySet* big = new FuzzySet(60, 80, 80, 80); // Big distance
 distance->addFuzzySet(big); // Add FuzzySet big to distance

 fuzzy->addFuzzyInput(distance); // Add FuzzyInput to Fuzzy object

// DISTANCE BACK setup
  FuzzyInput* distance_back = new FuzzyInput(3);// With its ID in param
 
  // Creating the FuzzySet to compond FuzzyInput distance
 FuzzySet* small_back = new FuzzySet(0, 0, 20, 40); // Small distance
 distance->addFuzzySet(small_back); // Add FuzzySet small to distance
 FuzzySet* moderate_back = new FuzzySet(30, 50, 50, 70); // Safe distance
 distance->addFuzzySet(moderate_back); // Add FuzzySet safe to distance
 FuzzySet* big_back = new FuzzySet(60, 80, 80, 80); // Big distance
 distance->addFuzzySet(big_back); // Add FuzzySet big to distance

 fuzzy->addFuzzyInput(distance_back); // Add FuzzyInput to Fuzzy object



// SPEED
// Step 3 - Creating a FuzzyInput speed
 FuzzyInput* carspeed = new FuzzyInput(2);// With its ID in param
 
  // Creating the FuzzySet to compond FuzzyInput carspeed
 FuzzySet* slow = new FuzzySet(0, 0, 60, 120); // Small carspeed
 carspeed->addFuzzySet(slow); // Add FuzzySet small to carspeed
 FuzzySet* average = new FuzzySet(90, 130, 130, 180); // Safe carspeed
 carspeed->addFuzzySet(average); // Add FuzzySet safe to carspeed
 FuzzySet* fast = new FuzzySet(160, 200, 200, 255); // Big carspeed
 carspeed->addFuzzySet(fast); // Add FuzzySet big to carspeed

 fuzzy->addFuzzyInput(carspeed); // Add FuzzyInput to Fuzzy object



 
 // OUTPUT

 // Passo 3 - Creating FuzzyOutput situation
 FuzzyOutput* situation = new FuzzyOutput(1);// With its ID in param

 // Creating FuzzySet to compond FuzzyOutput situation
 FuzzySet* safe = new FuzzySet(0, 0, 2, 3); // Slow situation
 situation->addFuzzySet(safe); // Add FuzzySet slow to situation
 FuzzySet* caution = new FuzzySet(2, 3.5, 3.5, 5); // Average situation
 situation->addFuzzySet(caution); // Add FuzzySet average to situation
 FuzzySet* threat = new FuzzySet(4, 6, 6, 8); // Fast situation
 situation->addFuzzySet(threat); // Add FuzzySet fast to situation
 FuzzySet* critical = new FuzzySet(7, 8.5, 10, 10); // very Fast situation
 situation->addFuzzySet(critical); // Add FuzzySet fast to situation

 fuzzy->addFuzzyOutput(situation); // Add FuzzyOutput to Fuzzy object


 

 //Passo 4 - Assembly the Fuzzy rules
 // FuzzyRule "IF distance = samll and carspeed = slow THEN situation = caution"
 FuzzyRuleAntecedent* ifDistanceSmallAndCarspeedSlow = new FuzzyRuleAntecedent();
ifDistanceSmallAndCarspeedSlow->joinWithAND(small, slow); 

// FuzzyRule "IF distance_back = small and carspeed = slow THEN situation = caution"
 FuzzyRuleAntecedent* ifDistanceBACKSmallAndCarspeedSlow = new FuzzyRuleAntecedent();
ifDistanceBACKSmallAndCarspeedSlow->joinWithAND(small_back, slow); 

 FuzzyRuleAntecedent* ifDSmallAndCarspeedSlow = new FuzzyRuleAntecedent();
ifDSmallAndCarspeedSlow->joinWithOR(ifDistanceSmallAndCarspeedSlow, ifDistanceBACKSmallAndCarspeedSlow); 


 FuzzyRuleConsequent* thenSitCaution = new FuzzyRuleConsequent(); // Instantiating a Consequent to expression
 thenSitCaution->addOutput(caution);// Adding corresponding FuzzySet to Consequent object
 // Instantiating a FuzzyRule object
 FuzzyRule* fuzzyRule01 = new FuzzyRule(1, ifDSmallAndCarspeedSlow, thenSitCaution); // Passing the Antecedent and the Consequent of expression
 
 fuzzy->addFuzzyRule(fuzzyRule01); // Adding FuzzyRule to Fuzzy object

/**********************************************************/
 
 
 // FuzzyRule "IF distance = small and carspeed = average THEN situation = threat"
 FuzzyRuleAntecedent* ifDistanceSmallAndCarspeedAverage = new FuzzyRuleAntecedent(); // Instantiating an Antecedent to expression
 ifDistanceSmallAndCarspeedAverage->joinWithAND(small,average); // Adding corresponding FuzzySet to Antecedent object
  // FuzzyRule "IF distanceBACK = small and carspeed = average THEN situation = threat"
 FuzzyRuleAntecedent* ifDistanceBACKSmallAndCarspeedAverage = new FuzzyRuleAntecedent(); // Instantiating an Antecedent to expression
 ifDistanceBACKSmallAndCarspeedAverage->joinWithAND(small_back,average); // Adding corresponding FuzzySet to Antecedent object


 FuzzyRuleAntecedent* ifDSmallAndCarspeedAverage = new FuzzyRuleAntecedent(); // Instantiating an Antecedent to expression
 ifDSmallAndCarspeedAverage->joinWithOR(ifDistanceSmallAndCarspeedAverage,ifDistanceBACKSmallAndCarspeedAverage); // Adding corresponding FuzzySet to Antecedent object
 
 FuzzyRuleConsequent* thenSitThreat = new FuzzyRuleConsequent(); // Instantiating a Consequent to expression
 thenSitThreat->addOutput(threat); // Adding corresponding FuzzySet to Consequent object
 
 // Instantiating a FuzzyRule object
 FuzzyRule* fuzzyRule02 = new FuzzyRule(2, ifDSmallAndCarspeedAverage, thenSitThreat); // Passing the Antecedent and the Consequent of expression
  
 fuzzy->addFuzzyRule(fuzzyRule02); // Adding FuzzyRule to Fuzzy object

/**********************************************************/
 
 // FuzzyRule "IF distance = small and carspeed = fast THEN sitaution = critical"
 FuzzyRuleAntecedent* ifDistanceSmallAndCarspeedFast = new FuzzyRuleAntecedent(); // Instantiating an Antecedent to expression
 ifDistanceSmallAndCarspeedFast->joinWithAND(small,fast); // Adding corresponding FuzzySet to Antecedent object
 
 // FuzzyRule "IF distanceBACK = small and carspeed = fast THEN sitaution = critical"
 FuzzyRuleAntecedent* ifDistanceBACKSmallAndCarspeedFast = new FuzzyRuleAntecedent(); // Instantiating an Antecedent to expression
 ifDistanceBACKSmallAndCarspeedFast->joinWithAND(small_back,fast); // Adding corresponding FuzzySet to Antecedent object

  FuzzyRuleAntecedent* ifDSmallAndCarspeedFast = new FuzzyRuleAntecedent(); // Instantiating an Antecedent to expression
 ifDSmallAndCarspeedFast->joinWithOR(ifDistanceSmallAndCarspeedFast,ifDistanceBACKSmallAndCarspeedFast); // Adding corresponding FuzzySet to Antecedent object
 
 
 FuzzyRuleConsequent* thenSitCritical = new FuzzyRuleConsequent(); // Instantiating a Consequent to expression
 thenSitCritical->addOutput(critical);// Adding corresponding FuzzySet to Consequent object

 
 // Instantiating a FuzzyRule object
 FuzzyRule* fuzzyRule03 = new FuzzyRule(3, ifDSmallAndCarspeedFast, thenSitCritical); // Passing the Antecedent and the Consequent of expression
 
 fuzzy->addFuzzyRule(fuzzyRule03); // Adding FuzzyRule to Fuzzy object

 


/**********************************************************/
 

  // FuzzyRule "IF distance = moderate and carspeed = slow THEN sitaution = safe"
 FuzzyRuleAntecedent* ifDistanceModerateAndCarspeedSlow = new FuzzyRuleAntecedent(); // Instantiating an Antecedent to expression
 ifDistanceModerateAndCarspeedSlow->joinWithAND(moderate,slow); // Adding corresponding FuzzySet to Antecedent object

  // FuzzyRule "IF distanceBACK = moderate and carspeed = slow THEN sitaution = safe"
 FuzzyRuleAntecedent* ifDistanceBACKModerateAndCarspeedSlow = new FuzzyRuleAntecedent(); // Instantiating an Antecedent to expression
 ifDistanceBACKModerateAndCarspeedSlow->joinWithAND(moderate_back,slow); // Adding corresponding FuzzySet to Antecedent object


 FuzzyRuleAntecedent* ifDModerateAndCarspeedSlow = new FuzzyRuleAntecedent(); // Instantiating an Antecedent to expression
 ifDModerateAndCarspeedSlow->joinWithOR(ifDistanceModerateAndCarspeedSlow,ifDistanceBACKModerateAndCarspeedSlow); // Adding corresponding FuzzySet to Antecedent object
 
 FuzzyRuleConsequent* thenSitSafe = new FuzzyRuleConsequent(); // Instantiating a Consequent to expression
 thenSitSafe->addOutput(safe);// Adding corresponding FuzzySet to Consequent object
 // Instantiating a FuzzyRule object
 FuzzyRule* fuzzyRule04 = new FuzzyRule(4, ifDModerateAndCarspeedSlow, thenSitSafe); // Passing the Antecedent and the Consequent of expression
 
 fuzzy->addFuzzyRule(fuzzyRule04); // Adding FuzzyRule to Fuzzy object

 /**********************************************************/

 

  // FuzzyRule "IF distance = moderate and carspeed = average THEN sitaution = caution"
 FuzzyRuleAntecedent* ifDistanceModerateAndCarspeedAverage = new FuzzyRuleAntecedent(); // Instantiating an Antecedent to expression
 ifDistanceModerateAndCarspeedAverage->joinWithAND(moderate,average); // Adding corresponding FuzzySet to Antecedent object

    // FuzzyRule "IF distanceBACK = moderate and carspeed = average THEN sitaution = caution"
 FuzzyRuleAntecedent* ifDistanceBACKModerateAndCarspeedAverage = new FuzzyRuleAntecedent(); // Instantiating an Antecedent to expression
 ifDistanceBACKModerateAndCarspeedAverage->joinWithAND(moderate_back,average); // Adding corresponding FuzzySet to Antecedent object

  FuzzyRuleAntecedent* ifDModerateAndCarspeedAverage = new FuzzyRuleAntecedent(); // Instantiating an Antecedent to expression
 ifDModerateAndCarspeedAverage->joinWithOR(ifDistanceModerateAndCarspeedAverage,ifDistanceBACKModerateAndCarspeedAverage); // Adding corresponding FuzzySet to Antecedent object
 
 FuzzyRuleConsequent* thenSitCaution = new FuzzyRuleConsequent(); // Instantiating a Consequent to expression
 thenSitCaution->addOutput(caution);// Adding corresponding FuzzySet to Consequent object
 // Instantiating a FuzzyRule object
 FuzzyRule* fuzzyRule05 = new FuzzyRule(5, ifDModerateAndCarspeedAverage, thenSitCaution); // Passing the Antecedent and the Consequent of expression
 
 fuzzy->addFuzzyRule(fuzzyRule05); // Adding FuzzyRule to Fuzzy object


  /**********************************************************/


  // FuzzyRule "IF distance = moderate and carspeed = fast THEN sitaution = caution"
 FuzzyRuleAntecedent* ifDistanceModerateAndCarspeedFast = new FuzzyRuleAntecedent(); // Instantiating an Antecedent to expression
 ifDistanceModerateAndCarspeedFast->joinWithAND(moderate,fast); // Adding corresponding FuzzySet to Antecedent object


  // FuzzyRule "IF distanceBACK = moderate and carspeed = fast THEN sitaution = caution"
 FuzzyRuleAntecedent* ifDistanceBACKModerateAndCarspeedFast = new FuzzyRuleAntecedent(); // Instantiating an Antecedent to expression
 ifDistanceBACKModerateAndCarspeedFast->joinWithAND(moderate_back,fast); // Adding corresponding FuzzySet to Antecedent object

 FuzzyRuleAntecedent* ifDModerateAndCarspeedFast = new FuzzyRuleAntecedent(); // Instantiating an Antecedent to expression
 ifDModerateAndCarspeedFast->joinWithOR(ifDistanceModerateAndCarspeedFast,ifDistanceBACKModerateAndCarspeedFast); // Adding corresponding FuzzySet to Antecedent object
 
 
// FuzzyRuleConsequent* thenSitThreat = new FuzzyRuleConsequent(); // Instantiating a Consequent to expression
 thenSitThreat->addOutput(threat);// Adding corresponding FuzzySet to Consequent object
 // Instantiating a FuzzyRule object
 FuzzyRule* fuzzyRule06 = new FuzzyRule(6, ifDModerateAndCarspeedFast, thenSitThreat); // Passing the Antecedent and the Consequent of expression
 
 fuzzy->addFuzzyRule(fuzzyRule06); // Adding FuzzyRule to Fuzzy object

  /**********************************************************/
 

   // FuzzyRule "IF distance = big and carspeed = slow THEN sitaution = safe"
 FuzzyRuleAntecedent* ifDistanceBigAndCarspeedSlow = new FuzzyRuleAntecedent(); // Instantiating an Antecedent to expression
 ifDistanceBigAndCarspeedSlow->joinWithAND(big,slow); // Adding corresponding FuzzySet to Antecedent object
// FuzzyRuleConsequent* thenSitSafe = new FuzzyRuleConsequent(); // Instantiating a Consequent to expression

    // FuzzyRule "IF distanceback = big and carspeed = slow THEN sitaution = safe"
 FuzzyRuleAntecedent* ifDistanceBACKBigAndCarspeedSlow = new FuzzyRuleAntecedent(); // Instantiating an Antecedent to expression
 ifDistanceBACKBigAndCarspeedSlow->joinWithAND(big_back,slow); // Adding corresponding FuzzySet to Antecedent object

 FuzzyRuleAntecedent* ifDBigAndCarspeedSlow = new FuzzyRuleAntecedent(); // Instantiating an Antecedent to expression
 ifDBigAndCarspeedSlow->joinWithOR(ifDistanceBigAndCarspeedSlow,ifDistanceBACKBigAndCarspeedSlow); // Adding corresponding FuzzySet to Antecedent object
 

 thenSitSafe->addOutput(safe);// Adding corresponding FuzzySet to Consequent object
 // Instantiating a FuzzyRule object
 FuzzyRule* fuzzyRule07 = new FuzzyRule(7, ifDBigAndCarspeedSlow, thenSitSafe); // Passing the Antecedent and the Consequent of expression
 
 fuzzy->addFuzzyRule(fuzzyRule07); // Adding FuzzyRule to Fuzzy object

   /**********************************************************/

    // FuzzyRule "IF distance = big and carspeed = average THEN sitaution = safe"
 FuzzyRuleAntecedent* ifDistanceBigAndCarspeedAverage = new FuzzyRuleAntecedent(); // Instantiating an Antecedent to expression
 ifDistanceBigAndCarspeedSlow->joinWithAND(big,average); // Adding corresponding FuzzySet to Antecedent object

    // FuzzyRule "IF distance = big and carspeed = average THEN sitaution = safe"
 FuzzyRuleAntecedent* ifDistanceBACKBigAndCarspeedAverage = new FuzzyRuleAntecedent(); // Instantiating an Antecedent to expression
 ifDistanceBACKBigAndCarspeedSlow->joinWithAND(big_back,average); // Adding corresponding FuzzySet to Antecedent object

  FuzzyRuleAntecedent* ifDBigAndCarspeedAverage = new FuzzyRuleAntecedent(); // Instantiating an Antecedent to expression
 ifDBigAndCarspeedAverage->joinWithOR(ifDistanceBigAndCarspeedSlow,ifDistanceBACKBigAndCarspeedSlow); // Adding corresponding FuzzySet to Antecedent object
 
 FuzzyRuleConsequent* thenSitSafe = new FuzzyRuleConsequent(); // Instantiating a Consequent to expression
 thenSitSafe->addOutput(safe);// Adding corresponding FuzzySet to Consequent object
 // Instantiating a FuzzyRule object
 FuzzyRule* fuzzyRule08 = new FuzzyRule(8, ifDBigAndCarspeedAverage, thenSitSafe); // Passing the Antecedent and the Consequent of expression
 
 fuzzy->addFuzzyRule(fuzzyRule08); // Adding FuzzyRule to Fuzzy object

   /**********************************************************/


   // FuzzyRule "IF distance = big and carspeed = fast THEN sitaution = caution"
 FuzzyRuleAntecedent* ifDistanceBigAndCarspeedFast = new FuzzyRuleAntecedent(); // Instantiating an Antecedent to expression
 ifDistanceBigAndCarspeedFast->joinWithAND(big,fast); // Adding corresponding FuzzySet to Antecedent object


   // FuzzyRule "IF distanceBACK = big and carspeed = fast THEN sitaution = caution"
 FuzzyRuleAntecedent* ifDistanceBACKBigAndCarspeedFast = new FuzzyRuleAntecedent(); // Instantiating an Antecedent to expression
 ifDistanceBACKBigAndCarspeedFast->joinWithAND(big_back,fast); // Adding corresponding FuzzySet to Antecedent object

  FuzzyRuleAntecedent* ifDBigAndCarspeedFast = new FuzzyRuleAntecedent(); // Instantiating an Antecedent to expression
 ifDBigAndCarspeedFast->joinWithOR(ifDistanceBigAndCarspeedFast,ifDistanceBACKBigAndCarspeedFast); // Adding corresponding FuzzySet to Antecedent object
 
 //FuzzyRuleConsequent* thenSitCaution = new FuzzyRuleConsequent(); // Instantiating a Consequent to expression
 thenSitCaution->addOutput(caution);// Adding corresponding FuzzySet to Consequent object
 // Instantiating a FuzzyRule object
 FuzzyRule* fuzzyRule09 = new FuzzyRule(9, ifDistanceBigAndCarspeedFast, thenSitCaution); // Passing the Antecedent and the Consequent of expression
 
 fuzzy->addFuzzyRule(fuzzyRule09); // Adding FuzzyRule to Fuzzy object

   /**********************************************************/

}

void loop() {

  // put your main code here, to run repeatedly:
  delay(1000);
    digitalWrite(redLed,LOW);
  // digitalWrite(greenLed,LOW);
 //   digitalWrite(blueLed,LOW);
  y =analogRead(A1);
  Serial.println("y=");
  Serial.println(y);
  delay(25);

   n = y%255 + 100;
  Serial.print("Speed is: ");
  Serial.println(n);
  if(n>255)
    n=255;
  motorspeed(n);

  
   // establish variables for duration of the ping, 
  // Clears the trigPin
digitalWrite(trigPin, LOW);
delayMicroseconds(2);

//Clears the trigPin_back
digitalWrite(trigPin_back, LOW);
delayMicroseconds(2);



// Sets the trigPin on HIGH state for 10 micro seconds
digitalWrite(trigPin, HIGH);
delayMicroseconds(10);
digitalWrite(trigPin, LOW);

// Sets the trigPin on HIGH state for 10 micro seconds
digitalWrite(trigPin_back, HIGH);
delayMicroseconds(10);
digitalWrite(trigPin_back, LOW);


// Reads the echoPin, returns the sound wave travel time in microseconds
duration = pulseIn(echoPin, HIGH);

// Calculating the distance
distance= duration*0.034/2;

// Prints the distance on the Serial Monitor
Serial.print("Distance: ");
Serial.println(distance);


// Reads the echoPin, returns the sound wave travel time in microseconds
duration_back = pulseIn(echoPin_back, HIGH);

// Calculating the distance
distance_back= duration_back*0.034/2;

// Prints the distance on the Serial Monitor
Serial.print("Distance BACK: ");
Serial.println(distance_back);


// APPLY FUZZY LOGIC

// Step 5 - Report inputs value, passing its ID and value
fuzzy->setInput(1, distance);
fuzzy->setInput(3, distance_back);
fuzzy->setInput(2, n); //speed
 // Step 6 - Exe the fuzzification
 fuzzy->fuzzify(); 
 // Step 7 - Exe the desfuzzyficação for each output, passing its ID
 float output = fuzzy->defuzzify(1);

 Serial.print("Output is: ");
 Serial.println(output);
  digitalWrite(redLed,LOW);
 //  digitalWrite(greenLed,LOW);
 //   digitalWrite(blueLed,LOW);

if(output>7)
{
    digitalWrite(redLed,LOW);
//   digitalWrite(greenLed,LOW);
//    digitalWrite(blueLed,LOW);
  for(int i=0;i<output-3;i++)
  {
  tone(buzzer,1000);
  delay(100);
  noTone(buzzer);
  delay(100);
  }
    String getData = "GET /update?api_key="+ API +"&"+ field1 +"="+String(Serial_Number)+ "&" + field2 + "=" + String(Device_Name)+ "&" + field3 + "=" + String(output);
 sendCommand("AT+CIPMUX=1",5,"OK");
 sendCommand("AT+CIPSTART=0,\"TCP\",\""+ HOST +"\","+ PORT,15,"OK");
 sendCommand("AT+CIPSEND=0," +String(getData.length()+4),4,">");
 esp8266.println(getData);delay(1500);countTrueCommand++;
 sendCommand("AT+CIPCLOSE=0",5,"OK");

  SendMessage();
 MakeCall();
  
  }
  else if(output>5)
  {
     
//   digitalWrite(greenLed,LOW);
//    digitalWrite(blueLed,LOW);
    for(int i=0;i<3;i++)
  {
    digitalWrite(redLed,HIGH);
    // delay(1000);
    digitalWrite(redLed,LOW);
    // delay(1000);

  }
    
    }
    else if(output>3)
    {
        digitalWrite(redLed,LOW);
//   digitalWrite(greenLed,LOW);
   
      for(int i=0;i<3;i++)
  {
//     digitalWrite(blueLed,HIGH);
    //  delay(1000);
//     digitalWrite(blueLed,LOW);
    //  delay(1000);

  }
      
      }
     else
     {

          digitalWrite(redLed,LOW);
//        digitalWrite(blueLed,LOW);
//        for(int i=0;i<3;i++)
//        {
//          digitalWrite(greenLed,HIGH); 
//            // delay(1000);
//           digitalWrite(greenLed,LOW); 
//            // delay(1000);
//        }
      }
      digitalWrite(redLed,LOW);
//   digitalWrite(greenLed,LOW);
//    digitalWrite(blueLed,LOW);

}
void motorspeed(int n)
{
  analogWrite(3, n);
  analogWrite(5, 0);
  analogWrite(6, n);
  analogWrite(11, 0);
}

long microsecondsToCentimeters(long microseconds)
{
 return microseconds / 29 / 2;
}

int getSensorData(){
  return random(1000); // Replace with 
}
void sendCommand(String command, int maxTime, char readReplay[]) {
  Serial.print(countTrueCommand);
  Serial.print(". at command => ");
  Serial.print(command);
  Serial.print(" ");
  while(countTimeCommand < (maxTime*1))
  {
    esp8266.println(command);//at+cipsend
    if(esp8266.find(readReplay))//ok
    {
      found = true;
      break;
    }
  
    countTimeCommand++;
  }
  
  if(found == true)
  {
    Serial.println("OYI");
    countTrueCommand++;
    countTimeCommand = 0;
  }
  
  if(found == false)
  {
    Serial.println("Fail");
    countTrueCommand = 0;
    countTimeCommand = 0;
  }
  
  found = false;
 }


void SendMessage()
{
  Serial.println("Entered");
  mySerial.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
  delay(1000);  // Delay of 1000 milli seconds or 1 second
  mySerial.println("AT+CMGS=\"+918778430793\"\r"); // Replace x with mobile number
  delay(1000);
  mySerial.println("Kandigai");// The SMS text you want to send
  delay(100);
   mySerial.println((char)26);// ASCII code of CTRL+Z
  delay(1000);
}
void MakeCall()
{
  mySerial.println("ATD+916379656247;"); // ATDxxxxxxxxxx; -- watch out here for semicolon at the end!!
  Serial.println("Calling  "); // print response over serial port
  //delay(1000);
}
