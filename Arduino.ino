

#define IR A0
 #define BOUNCE_TIME 500

const int ledPin = 8;
int piscando = 0; 
int Control = 1;  
int reading=1;            
int contador=0;
static uint32_t lastPressTime = 0;
bool auth=false;
void setup() {
Serial.begin(9600);
Serial1.begin(9600);
pinMode(ledPin, OUTPUT); 
}
 
void loop()
{
   reading = analogRead(IR);
  
  Serial.println(reading);

     if(reading==0){
       contador++;
       lastPressTime=millis();
       digitalWrite(ledPin, HIGH);
        delay(100);
        digitalWrite(ledPin, LOW);
        auth=true;
      
  }
     if(millis() - lastPressTime > BOUNCE_TIME && auth){
          Serial.print("\n Contador: ");
          Serial.print(contador);
          Serial.print("\n");
          Serial1.write(contador);
          contador=0;
          delay(500);

          
         
          lastPressTime=millis();
          auth=false;
    }

      
  }



