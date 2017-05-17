
  
#include <LiquidCrystal.h>  
#include <math.h>


LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

// define some values used by the panel and buttons
int lcd_key     = 0;
int adc_key_in  = 0;
#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5
 
int val_calibrado, time_1, time_2, analogPin=8, idx=0, val_teste, controlo=0;
float val_medido =0, val_sensor, i, minimumInput, maximumInput, minimumRs_R0, maximumRs_R0, slope, b, inicialMG, finalMG, rs_r0, slopeReal, bReal, mg_l;

void setup()   
{  
   pinMode(analogPin, INPUT);
   pinMode(50, OUTPUT);
   pinMode(51, OUTPUT);
   lcd.begin(16, 2);  
   lcd.setCursor(0,0);  
   lcd.print("Alcoolimetro    ");  
   lcd.setCursor(0,1);  
   lcd.print("DTSD - 2015/2016");  
   Serial.begin(115200); 
   
}  

void func_calibrar(){ //Calibrar o sensor
   val_sensor=0;  
   i=0;
   lcd.setCursor(0,0);  
   lcd.print("   Inicio de    ");
   lcd.setCursor(0,1);  
   lcd.print(" Calibracao     ");
   time_1=millis();
   time_2=millis();
   while(time_2-time_1<=5000){ //Retirar uma quantidade x de valores durante 5 segundos
     val_sensor= val_sensor+analogRead(analogPin);
     time_2=millis();
     i++;
   }
   val_calibrado=val_sensor/i; //Fazer a média dos valores
   /*Serial.println("Valor calibrado em: ");
   Serial.println(val_calibrado);*/
   lcd.setCursor(0,0);  
   lcd.print("   Fim    de    ");
   lcd.setCursor(0,1);  
   lcd.print(" Calibracao     ");
   delay(2000);
   lcd.setCursor(0,0);  
   lcd.print("Alcoolimetro    ");  
   lcd.setCursor(0,1);  
   lcd.print(" Stand-By       ");  
}
void func_medir_alcool(){ //Fazer o teste

/*****************************DATASHEET INFO*************************************/
   
   minimumInput = val_calibrado;  //Valor base correspondente a 0.1 mg/l
   maximumInput = 1024; //Valor máximo admitido pelo sensor
   minimumRs_R0 = 0.122;  //Valor mínimo da relação entre resistências
   maximumRs_R0 = 0.9;  //Valor máximo da relação entre resistências, temp ambiente 20º

   slope = minimumInput-maximumInput/(log10(maximumRs_R0/minimumRs_R0)); //Declive da rampa
   b = minimumInput - slope*log10(maximumRs_R0);  //Ordenada na origem

/*****************************DATASHEET INFO*************************************/   
   val_sensor=0;
   val_medido=0;  
   i=0;
   lcd.setCursor(0,0);  
   lcd.print("  Registo   de   ");
   lcd.setCursor(0,1);  
   lcd.print("     Valores     ");
   time_1=millis();
   time_2=millis();
   while(time_2-time_1<=10000){ 
     val_sensor= val_sensor+analogRead(analogPin);
     time_2=millis();
     i++;
   }
   digitalWrite(50,HIGH);
   delay(1000);
   digitalWrite(50,LOW);
   val_medido= val_sensor / i;
   //val_medido=val_medido - val_calibrado;
  /* Serial.println("Valor calculado");
   Serial.println(val_medido);*/
      if(val_medido<0){
        val_medido=0;
      }

/***********************Processamento dos valores***************************/
  inicialMG = 0.1; 
  finalMG = 10;

  rs_r0 = pow(10,(val_medido-b)/slope); //Relação das resistências do sensor
  slopeReal = log10(finalMG/inicialMG)/log10(minimumRs_R0/maximumRs_R0);  //Declive real
  bReal = log10(finalMG)-slopeReal*log10(minimumRs_R0); //Ordenada na origem  real
  mg_l = pow(10,log10(rs_r0)*slopeReal+bReal); //Valor do teste de alcoolemia em [mg/l]
  
/***********************Processamento dos valores***************************/   

    // write idx 'i' in sheet 'Example' column 'C' row 'idx + 11'
    Serial.print("XLS,write,Example,C");
    Serial.print(idx + 11);
    Serial.print(",");
    Serial.print(val_medido);
    Serial.print("\n");
    // write the value from A0 pin to sheet 'Example' column 'D' row 'idx + 11'
    Serial.print("XLS,write,Example,D");
    Serial.print(idx + 11);
    Serial.print(",");
    Serial.print(rs_r0,2);
    Serial.print("\n");
    // write the value from A0 pin to sheet 'Example' column 'D' row 'idx + 11'
    Serial.print("XLS,write,Example,E");
    Serial.print(idx + 11);
    Serial.print(",");
    Serial.print(mg_l,2);
    Serial.print("\n");
    idx++;
 /*  Serial.println("medido");
   Serial.println(val_medido);
   Serial.println("Taxa de alcoolemia");
   Serial.println(mg_l);*/
   lcd.setCursor(0,0);  
   lcd.print("   Em analise   ");
   lcd.setCursor(0,1);  
   lcd.print(" de resultados  ");    
   delay(2000);
   lcd.setCursor(0,1);
   lcd.print("                ");
   lcd.setCursor(0,0);  
   lcd.print("   Resultado    ");  
   lcd.setCursor(0,1);  
   lcd.print(mg_l);
   lcd.print("mg/L");
   delay(5000);
   if(mg_l<0.2){  //Apto para conduzir
      lcd.setCursor(0,0);  
      lcd.print("Apto para       ");  
      lcd.setCursor(0,1);  
      lcd.print("conduzir        ");
      delay(5000);    
   }
    if(mg_l>0.2 && mg_l<0.5){ //Apto para conduzir se tiver carta há menos de 2 anos
      lcd.setCursor(0,0);  
      lcd.print("Apto para       ");  
      lcd.setCursor(0,1);  
      lcd.print("conduzir        ");
      delay(5000);    
      lcd.setCursor(0,0);  
      lcd.print("Se nao for recem");  
      lcd.setCursor(0,1);  
      lcd.print("cartado, <2 anos");
      delay(5000);    
   }
   if(mg_l>0.5){  //COMPLETAMENTE BÊBEDO
        
      digitalWrite(51,HIGH);
      delay(1000);
      digitalWrite(51,LOW);
      lcd.setCursor(0,0);  
      lcd.print("  Inapto para   ");  
      lcd.setCursor(0,1);  
      lcd.print("  conduzir      ");
      delay(5000);    
   }
    lcd.setCursor(0,0);  
    lcd.print("Incerteza       ");  
    lcd.setCursor(0,1);  
    lcd.print("+/- 0.05 mg/L   ");
    delay(5000);

      //FALTA PÔR OS LEDS A PISCAR A FREQUÊNCIAS DIFERENTES
      
   lcd.setCursor(0,0);
   lcd.print("Alcoolimetro    ");  
   lcd.setCursor(0,1);  
   lcd.print(" Stand-By       ");  
   
}
// read the buttons
int read_LCD_buttons()
{
 adc_key_in = analogRead(0);      // read the value from the sensor 
 // my buttons when read are centered at these valies: 0, 144, 329, 504, 741
 // we add approx 50 to those values and check to see if we are close
 if (adc_key_in > 1000) return btnNONE; // We make this the 1st option for speed reasons since it will be the most likely result
 // For V1.1 us this threshold
 if (adc_key_in < 50)   return btnRIGHT;  
 if (adc_key_in < 250)  return btnUP; 
 if (adc_key_in < 450)  return btnDOWN; 
 if (adc_key_in < 650)  return btnLEFT; 
 if (adc_key_in < 850)  return btnSELECT;  
 return btnNONE;  // when all others fail, return this...
}

//Modo Teste
void modoTeste(){
  lcd.setCursor(0,0);
  lcd.print("                ");
  lcd.setCursor(0,1);
  lcd.print("                ");
  lcd.setCursor(0,0);
  lcd.print(" Modo de Teste  ");
  digitalWrite(51,HIGH); //Activar led vermelho
  delay(2000);
  digitalWrite(51,LOW); //Activar led vermelho
  digitalWrite(50,HIGH);
  delay(2000);
  digitalWrite(50,LOW);
  lcd.setCursor(0,0);
  lcd.print("                ");
  lcd_key = read_LCD_buttons();
  while(lcd_key!=btnUP){
    lcd_key = read_LCD_buttons();
    if(lcd_key==btnLEFT){
      lcd.setCursor(0,0);
      lcd.print("Left button     "); //Botão esquerdo funciona? LCD linha 1 Funciona?
      delay(2000);
      lcd.setCursor(0,0);
      lcd.print("                ");
      lcd.setCursor(0,1);
      lcd.print("                ");
    }
    if(lcd_key==btnRIGHT){
      lcd.setCursor(0,1);
      lcd.print("Right button    ");  //Botão direito funciona? LCD linha 2 Funciona?
      delay(2000);
      lcd.setCursor(0,0);
      lcd.print("                ");
      lcd.setCursor(0,1);
      lcd.print("                ");
    }
     if(lcd_key==btnNONE){
      lcd.setCursor(0,0);
      lcd.print("Push any button ");
      lcd.setCursor(0,1);
      lcd.print("Push Up to exit ");
    }
  }

  lcd.setCursor(0,0);
  lcd.print("                ");
  lcd.setCursor(0,1);
  lcd.print("                ");  
  lcd.setCursor(0,0);
  
  for(i=1;i<10;i++){
      val_teste= analogRead(analogPin);
      lcd.setCursor(0,0);
      lcd.print(val_teste);
      delay(2000);
      lcd.setCursor(0,0);
      lcd.print("                ");
      
  }

  lcd.setCursor(0,0);
  lcd.print(" Sensor working ");
  delay(2000);
  lcd.setCursor(0,0);
  lcd.print("                ");
  lcd.setCursor(0,1);
  lcd.print("                ");  
  lcd.setCursor(0,0);
  lcd.print("Alcoolimetro    ");  
  lcd.setCursor(0,1);  
  lcd.print(" Stand-By       ");  
}


void loop()  
{  
   
    lcd_key = read_LCD_buttons();  // read the buttons

 switch (lcd_key)               // depending on which button was pushed, we perform an action
 {
   case btnNONE:
     {
      break;
     }
   case btnRIGHT:
     {
      if(controlo==0){
         lcd.setCursor(0,0);
         lcd.print("                ");
         lcd.setCursor(0,1);
         lcd.print("                ");
         lcd.setCursor(0,0);
         lcd.print("Por favor       ");
         lcd.setCursor(0,1);
         lcd.print("calibrar antes  ");
         delay(2000);
         lcd.setCursor(0,0);
         lcd.print("de realizar o   ");
         lcd.setCursor(0,1);
         lcd.print("teste           ");
         delay(2000);
         lcd.setCursor(0,0);
         lcd.print("                ");
         lcd.setCursor(0,1);
         lcd.print("                ");  
         lcd.setCursor(0,0);
         lcd.print("Alcoolimetro    ");  
         lcd.setCursor(0,1);  
         lcd.print(" Stand-By       ");  
      }
      else{
        func_medir_alcool();
      }
     break;
     }
   case btnLEFT:
     {
     controlo = 1;
     func_calibrar();
     break;
     }
   case btnUP:
     {
     modoTeste();
     break;
     }
  }
}
