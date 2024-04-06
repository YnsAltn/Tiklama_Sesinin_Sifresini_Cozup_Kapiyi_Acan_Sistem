const int vurulumSensor = 0;
const int programDugmesi = 2;
const int kilitMotoru = 3;
const int kirmiziLED = 4;
const int yesilLED = 5;

const int esik = 3;
const int reddetmeDegeri = 25;
const int ortalamaReddetmeDegeri = 15;
const int vurulumSolmasiSuresi = 150;
const int kilitDonmeSuresi = 2000;

const int maksimumVurulumlar = 20;
const int vurulumTamamlandi = 1200;

int gizliKod[maksimumVurulumlar] = {50, 25, 25, 50, 100, 50, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int vurulumOkumalari[maksimumVurulumlar];
int vurulumSensorDegeri = 0;
int programDugmesiBasilan = false;

void setup() {
  pinMode(kilitMotoru, OUTPUT);
  pinMode(kirmiziLED, OUTPUT);
  pinMode(yesilLED, OUTPUT);
  pinMode(programDugmesi, INPUT);
  
  Serial.begin(9600);
  Serial.println("Program basladi.");
  
  digitalWrite(yesilLED, HIGH);
}

void loop() {
  vurulumSensorDegeri = analogRead(vurulumSensor);
  
  if (digitalRead(programDugmesi)==HIGH){
    programDugmesiBasilan = true;
    digitalWrite(kirmiziLED, HIGH);
  } else {
    programDugmesiBasilan = false;
    digitalWrite(kirmiziLED, LOW);
  }
  
  if (vurulumSensorDegeri >=esik){
    gizliVurulumuDinle();
  }
} 

void gizliVurulumuDinle(){
  Serial.println("Vurus basliyor");   

  int i = 0;
  for (i=0;i<maksimumVurulumlar;i++){
    vurulumOkumalari[i]=0;
  }
  
  int mevcutVurulumNumarasi=0;
  int baslangicZamani=millis();
  int simdikiZaman=millis();
  
  digitalWrite(yesilLED, LOW);
  if (programDugmesiBasilan==true){
     digitalWrite(kirmiziLED, LOW);
  }
  delay(vurulumSolmasiSuresi);
  digitalWrite(yesilLED, HIGH);  
  if (programDugmesiBasilan==true){
     digitalWrite(kirmiziLED, HIGH);                        
  }
  do {
    vurulumSensorDegeri = analogRead(vurulumSensor);
    if (vurulumSensorDegeri >=esik){
      Serial.println("Vurus.");
      simdikiZaman=millis();
      vurulumOkumalari[mevcutVurulumNumarasi] = simdikiZaman-baslangicZamani;
      mevcutVurulumNumarasi ++;
      baslangicZamani=simdikiZaman;          
      digitalWrite(yesilLED, LOW);  
      if (programDugmesiBasilan==true){
        digitalWrite(kirmiziLED, LOW);
      }
      delay(vurulumSolmasiSuresi);
      digitalWrite(yesilLED, HIGH);
      if (programDugmesiBasilan==true){
        digitalWrite(kirmiziLED, HIGH);                         
      }
    }

    simdikiZaman=millis();
    
  } while ((simdikiZaman-baslangicZamani < vurulumTamamlandi) && (mevcutVurulumNumarasi < maksimumVurulumlar));
  
  if (programDugmesiBasilan==false){
    if (vurulumuDogrula() == true){
      kapiKilidiniAc(); 
    } else {
      Serial.println("Gizli vurus basarisiz.");
      digitalWrite(yesilLED, LOW);
      for (i=0;i<4;i++){          
        digitalWrite(kirmiziLED, HIGH);
        delay(100);
        digitalWrite(kirmiziLED, LOW);
        delay(100);
      }
      digitalWrite(yesilLED, HIGH);
    }
  } else {
    vurulumuDogrula();
    Serial.println("Yeni kilit kaydedildi.");
    digitalWrite(kirmiziLED, LOW);
    digitalWrite(yesilLED, HIGH);
    for (i=0;i<3;i++){
      delay(100);
      digitalWrite(kirmiziLED, HIGH);
      digitalWrite(yesilLED, LOW);
      delay(100);
      digitalWrite(kirmiziLED, LOW);
      digitalWrite(yesilLED, HIGH);      
    }
  }
}


void kapiKilidiniAc(){
  Serial.println("Kapi acildi!");
  int i=0;
  
  digitalWrite(kilitMotoru, HIGH);
  digitalWrite(yesilLED, HIGH);
  
  delay (kilitDonmeSuresi);
  
  digitalWrite(kilitMotoru, LOW);
  
  for (i=0; i < 5; i++){   
      digitalWrite(yesilLED, LOW);
      delay(100);
      digitalWrite(yesilLED, HIGH);
      delay(100);
  }
   
}

boolean vurulumuDogrula(){
  int i=0;
 
  int mevcutVurulumSayisi = 0;
  int gizliVurulumSayisi = 0;
  int maksVurulumAraligi = 0;
  
  for (i=0;i<maksimumVurulumlar;i++){
    if (vurulumOkumalari[i] > 0){
      mevcutVurulumSayisi++;
    }
    if (gizliKod[i] > 0){
      gizliVurulumSayisi++;
    }
    
    if (vurulumOkumalari[i] > maksVurulumAraligi){
      maksVurulumAraligi = vurulumOkumalari[i];
    }
  }
  
  if (programDugmesiBasilan==true){
      for (i=0;i<maksimumVurulumlar;i++){
        gizliKod[i]= map(vurulumOkumalari[i],0, maksVurulumAraligi, 0, 100); 
      }
      digitalWrite(yesilLED, LOW);
      digitalWrite(kirmiziLED, LOW);
      delay(1000);
      digitalWrite(yesilLED, HIGH);
      digitalWrite(kirmiziLED, HIGH);
      delay(50);
      for (i = 0; i < maksimumVurulumlar ; i++){
        digitalWrite(yesilLED, LOW);
        digitalWrite(kirmiziLED, LOW);  
        if (gizliKod[i] > 0){                                   
          delay( map(gizliKod[i],0, 100, 0, maksVurulumAraligi));
          digitalWrite(yesilLED, HIGH);
          digitalWrite(kirmiziLED, HIGH);
        }
        delay(50);
      }
    return false;
  }
  
  if (mevcutVurulumSayisi != gizliVurulumSayisi){
    return false; 
  }
  int toplamZamanFarki=0;
  int zamanFarki=0;
  for (i=0;i<maksimumVurulumlar;i++){
    vurulumOkumalari[i]= map(vurulumOkumalari[i],0, maksVurulumAraligi, 0, 100);      
    zamanFarki = abs(vurulumOkumalari[i]-gizliKod[i]);
    if (zamanFarki > reddetmeDegeri){
      return false;
    }
    toplamZamanFarki += zamanFarki;
  }
  if (toplamZamanFarki/gizliVurulumSayisi>ortalamaReddetmeDegeri){
    return false; 
  }
  
  return true;
  
}
