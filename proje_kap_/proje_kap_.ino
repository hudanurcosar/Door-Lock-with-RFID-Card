#include <MFRC522.h>
#include <SPI.h>
#include <EEPROM.h>

#define RST_PIN 9
#define SS_PIN 10

int bzr = 8; 
int kart[4];
int x;
int role = 3;
int kirmizi = 7;
int yesil = 5;
int reset = 4;
int button = 6;

MFRC522 mfrc522(SS_PIN,RST_PIN);

void setup() {
  
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();

  pinMode(button,INPUT);
  pinMode(reset,INPUT);
  pinMode(role,OUTPUT);
  pinMode(kirmizi,OUTPUT);
  pinMode(yesil,OUTPUT);
  pinMode(bzr,OUTPUT);
  
}

void loop() {
  // reset butonuna basılırsa...
  if(digitalRead(reset) == HIGH){
      digitalWrite(kirmizi,HIGH);
      delay(1000);
      digitalWrite(kirmizi,LOW);
      digitalWrite(yesil,HIGH);
      delay(1000);
      digitalWrite(yesil,LOW);
      digitalWrite(kirmizi,HIGH);
      delay(1000);
      digitalWrite(kirmizi,LOW);

      if( digitalRead(reset) == HIGH){
        digitalWrite(kirmizi,HIGH);
        //haafızadaki bütün kartlar silinir...
        for (int i = 0 ; i < EEPROM.length() ; i++) {
          EEPROM.write(i, 0);
          while(digitalRead(reset) == HIGH);
        }
        digitalWrite(kirmizi,LOW);
    }
  }
  //butona basılırsa...
  if( digitalRead(button) == HIGH ){
    digitalWrite(yesil,HIGH);
    digitalWrite(kirmizi,HIGH);
    
    //kart okutalana kadar baştan başlar...
    if ( ! mfrc522.PICC_IsNewCardPresent()) {  
    return ;
    }
    if ( ! mfrc522.PICC_ReadCardSerial()) {
    return ;
    }
    if (  mfrc522.PICC_ReadCardSerial()) {
    return ;
    }
    //kart okutulunca işlemlere başlar...
    for(int i=0; i<4; i++ ){
      // karttan okunan anlık değerleri geçici bir kart değişkenine atar...
      kart[i] = mfrc522.uid.uidByte[i];
      delay(100);
    }
    //kart eğer kayıtlı değilse...
    if (!findID(kart,4)){
      digitalWrite(yesil,LOW);
      digitalWrite(kirmizi,LOW);
      //boş bir adres belirlenir...
      int y= kayit_adresi();
      
      for ( int i = 0; i <4; i++ ) {
      //ve kart EEPROM a yani Arduinonun hafızasına yazılır...
      EEPROM[y+i] = kart[i];    
  }
      //kartın kaydedildiğini göstermek için yesil ışık yanıp söner...
      digitalWrite(yesil,HIGH);
      delay(1000);
      digitalWrite(yesil,LOW);
      delay(1000);
      digitalWrite(yesil,HIGH);
      delay(1000);
      digitalWrite(yesil,LOW);
      delay(1000);
      digitalWrite(yesil,HIGH);
      delay(1000);
      digitalWrite(yesil,LOW);
    }
    //kart zaten önceden kaydedilmiş ise...
   else{
      digitalWrite(yesil,LOW);
      digitalWrite(kirmizi,LOW);
      //silme fonksiyonu kullanılarak silinir...
      kart_silme(kart,4);
      //silindiğini belirtmek için de kırmızı ışık yanıp söner...
      digitalWrite(kirmizi,HIGH);
      delay(1000);
      digitalWrite(kirmizi,LOW);
      delay(1000);
      digitalWrite(kirmizi,HIGH);
      delay(1000);
      digitalWrite(kirmizi,LOW);
      delay(1000);
      digitalWrite(kirmizi,HIGH);
      delay(1000);
      digitalWrite(kirmizi,LOW);
      delay(1000);
      
    }
  }
  //kontrol(butona basmadığımız halde ledler yanıyorsa söndürülür)...
  if( digitalRead(button) == LOW && (digitalRead(yesil)==HIGH || digitalRead(kirmizi)== HIGH)){
       digitalWrite(yesil,LOW);
       digitalWrite(kirmizi,LOW);
      }
//kart okutulmadığı sürece başa atar ileriye geçemez...
  if ( ! mfrc522.PICC_IsNewCardPresent()) { 
    return ;
  }
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return ;
  }   
  if (  mfrc522.PICC_ReadCardSerial()) {
    return ;
    } 
//kart okutulduğunda işlemler başlar...
  for(int i=0; i<4; i++ ){
    //okutulan kart geçici değişkene atanır...
      kart[i] = mfrc522.uid.uidByte[i];
      delay(100);
    }
    //kart hazfızada kayıtlı ise girişe izin verilir ve yesil led aktif olur...
  if( findID(kart,4)){

    digitalWrite(yesil,HIGH);
    digitalWrite(role, HIGH);
    Serial.print(" KAPI ACIK ");
    delay(2000);
    digitalWrite(role, LOW);
    digitalWrite(yesil,LOW);
    }
  else{
    //kart kayıtlı değil ise kırmızı led aktif olur ve buzzer ile alarm sağlanır...
    digitalWrite(kirmizi, HIGH);
    Serial.print(" ALARM!! ");
    delay(1000);
    digitalWrite(kirmizi, LOW);
  
   
    delay(1000);
    digitalWrite(kirmizi, LOW);
    
    for (int i=0; i <3; i++){
    digitalWrite(bzr,HIGH);
    delay(1000);
    digitalWrite(bzr,LOW);
    delay(1000);
    }
  }  

}
//FONKSİYONLAR


//kart EEPROMDA kayıtlı mı diye kontrol eder...
boolean findID (int kart[],int x){
     int a;
for(int z=0; z<EEPROM.length()/4; z++){
  a = 0; 

  for(int j=z*4; j<4*(z+1); j++){
    if (kart[j-(4*z)] == EEPROM[j]){
      a++;
    }
    else{
      a = 0;
      break;
      }
  }
  if( a == 4 ){
  return true;
  }
}
if( a != 4 ){
  return false;
  }
   
}

//EEPROMda 4 bit 4 bit  tarama yaparak ilk boş adresi kayıt adresi olarak ayarlar...
int kayit_adresi(){

  for(int y=0; y<1024; y+=4){
    if(EEPROM[y] == 0){
      x = y;
      return x;
      break;
      }
    }
}

//kartın kayıtlı olduğu adresi bularak 4 bit boyunca 0 a eşitler...
void kart_silme(int kart[],int x){
  int b = 0;
  int i;
  
  for(i=0; i<EEPROM.length()/4; i++){
    for(int j=i*4; j<4*(i+1); j++){
      if(kart[j-(4*i)] == EEPROM[j]){
        b++;
        }
      else{
        b = 0;
        break;
        }  
      }
      if(b == 4){
        break;
        }
    }
  if( i<EEPROM.length()/4 ){
      for(int j=i*4; j<4*(i+1); j++){
        EEPROM.write(j,0);
      }
    }  
    
}
