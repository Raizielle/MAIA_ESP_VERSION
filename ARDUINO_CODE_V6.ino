//////////////////////////////////// MAIA - MOBILE AWARE INTERMODAL ASSISTANT /////////////////////////////////
/////////////////////////////// CENTRO DE PESQUISA, DESENVOLVIMENTO E INOVAÇÃO DELL ///////////////////////////

///////////////////////////////////////////////// BIBLIOTECAS /////////////////////////////////////////////////

#include <SoftwareSerial.h>
#include <HamShield.h> 

///////////////////////////////////////////////// DEFINIÇÕES /////////////////////////////////////////////////

#define MIC_PIN 3         // PINO RESPONÁVEL POR ENVIAR OS ÁUDIOS ( Não será utilizado pois o áudio é enviado pelo TRRS jack)
#define RESET_PIN A3      // PINO DE RESET
#define SWITCH_PIN 2      // PINO RESPONÁVEL POR ALTERNAR ENTRE OS MODOS EMISSOR E TRANSMISSOR (switch)
#define ARDUINO_RX 5      // Conectar ao TX do módulo
#define ARDUINO_TX 6      // Conectar ao RX do módulo
#define ESP_TX 9          // Conectar ao RX do módulo
#define ESP_TX 10         // Conectar ao RX do módulo

#define RADIO_FLAG 7      // LED indicador da operação de transmissão
#define CMD_SEL_DEV 0X09  // Seleciona o dispositivo de armazenamento para o cartão TF  ----> Requer complemento 00 00
#define DEV_TF 0X02

///////////////////////////////////////////////// OBJETOS /////////////////////////////////////////////////

HamShield radio;
SoftwareSerial mp3(ARDUINO_RX, ARDUINO_TX);

//////////////////////////////////////////////// VARIÁVEIS ////////////////////////////////////////////////

static int8_t Send_buf[8] = {0}; // Buffer para comandos a serem enviados.
uint32_t freq;                   // Armazenará a frequência do rádio
char requests[50];
String lider = "";
String linha = "";
String area = "";
String req = "";
int8_t audio1index = 0;
int8_t audio2index = 0;
int8_t audio3index = 0;
char aux = ' ';                  // Auxiliar na recepção dos dados da comunicação serial
byte i;
boolean rec_flag;
boolean conf_flag;
boolean message_detected;
boolean conf_message_detected;
byte randNumber;
long coleta;
byte detec;
int calibration[250];
int j;
int noise_limit;

///////////////////////////FREQUÊNCIAS EM MHz DOS CANAIS UTILIZADOS NA FÁBRICA///////////////////////////

/*
    EMR Team (Time de reparo)       01         (462562 MHz)
    ARB Team                        02         (462587 MHz)
    IT                              03         (462612 MHz)
    PC - Production Control         04         (462637 MHz)
    EHS – EMERGÊNCIAS               05         (462662 MHz)
    CFI                             06         (462687 MHz)
    Control Room / MDT / Office     07         (462712 MHz)
    Eng Processo                    08         (467562 MHz)
    Manufatura                      09         (467587 MHz)
    Materiais                       10         (467612 MHz)
    Manutenção                      14         (467712 MHz)
    Eng Teste                       15         (462550 MHz)
    NPI                             17         (462600 MHz)
    Qualidade                       20         (462675 MHz)

    Exemplo de string: EMR/GL1/QT/01
    
*/

////////////////////////////////////////////////////////////////////////////////////////////////////////////

void calibracao(){
  noise_limit = 0;
  for(j=0; j<250; j++){
    calibration[j++] = analogRead(2);
    delay(10);
  }
  
  for(j=0; j<250; j++){
    if(calibration[j]>noise_limit){
      noise_limit = calibration[j];
    }
    j++;
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////

void zeraString(){
  for(i=0;i<50;i++){
    requests[i] = ' ';
  }
  req = "";
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////

//DEFINIRÁ A FREQUÊNCIA DE OPERAÇÃO DO RÁDIO COM BASE NO LÍDER DESEJADO
void seleionaLider(String lider){
  if(lider.equals("EMR")){
    freq = 462562;
    configura();
  }
  else{
    if(lider.equals("ARB")){
      freq = 462587;
      configura();
    }
    else{
      if(lider.equals("IT")){
        freq = 462612;
        configura();
      }
      else{
        if(lider.equals("PC")){
          freq = 462637;
          configura();
        }
        else{
          if(lider.equals("EHS")){
            freq = 462662;
            configura();
          }
          else{
            if(lider.equals("CFI")){
              freq = 462687;
              configura();
            }
            else{
              if(lider.equals("MDT")){
                freq = 462712;
                configura();
              }
              else{
                if(lider.equals("EP")){
                  freq = 467562;
                  configura();
                }
                else{
                  if(lider.equals("MFT")){
                    freq = 467587;
                    configura();
                  }
                  else{
                    if(lider.equals("MTR")){
                      freq = 467612;
                      configura();
                    }
                    else{
                      if(lider.equals("MNT")){
                        freq = 467712;
                        configura();
                      }
                      else{
                        if(lider.equals("ET")){
                          freq = 462550;
                          configura();
                        }
                        else{
                          if(lider.equals("NPI")){
                            freq = 462600;
                            configura();
                          }
                          else{
                            if(lider.equals("QLD")){
                              freq = 462675;
                              configura();
                            }
                            else{
                            }                            
                          }
                        }
                      }
                    }
                  }              
                }        
              }
            }
          }
        }
      }
    }
  }
   
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////

//DEFINIRÁ A PRIMEIRA PARTE DO ÁUDIO (LINHA ONDE O USUÁRIO ESTÁ)
void seleionaLinha(String linha){
  audio1index = 0;
  if(linha.equals("GL1") or linha.equals("GL01")){
    audio1index = 0x01;
  }
  else{
    if(linha.equals("GL2") or linha.equals("GL02")){
      audio1index = 0x02;
    }
    else{
      if(linha.equals("GL3") or linha.equals("GL03")){
        audio1index = 0x03;
      }
      else{
        if(linha.equals("GL4") or linha.equals("GL04")){
          audio1index = 0x04;
        }
        else{
          if(linha.equals("GL5") or linha.equals("GL05")){
            audio1index = 0x05;
          }
          else{
            if(linha.equals("GL6") or linha.equals("GL06")){
              audio1index = 0x06;
            }
            else{
              if(linha.equals("GL7") or linha.equals("GL07")){
                audio1index = 0x07;
              }
              else{
                if(linha.equals("GL8") or linha.equals("GL08")){
                  audio1index = 0x08;
                }
                else{
                  if(linha.equals("GL9") or linha.equals("GL09")){
                    audio1index = 0x09;
                  }
                  else{
                    if(linha.equals("GL10")){
                      audio1index = 0x0A;
                    }
                    else{
                      if(linha.equals("GL11")){
                        audio1index = 0x0B;
                      }
                      else{
                        if(linha.equals("GL12")){
                          audio1index = 0x0C;
                        }
                        else{
                          if(linha.equals("GL13")){
                            audio1index = 0x0D;
                          }
                          else{
                            if(linha.equals("GL14")){
                              audio1index = 0x0E;
                            }
                            else{
                              if(linha.equals("GL15")){
                                audio1index = 0x0F;
                              }
                              else{
                                if(linha.equals("GL16")){
                                  audio1index = 0x10;
                                }
                                else{
                                  if(linha.equals("GL17")){
                                    audio1index = 0x11;
                                  }
                                  else{
                                    if(linha.equals("GL18")){
                                      audio1index = 0x12;
                                    }
                                    else{
                                      if(linha.equals("GL19")){
                                        audio1index = 0x13;
                                      }
                                      else{
                                        if(linha.equals("GL20")){
                                          audio1index = 0x14;
                                        }
                                        else{
                                          if(linha.equals("Burn")){
                                            audio1index = 0x15;
                                          }
                                          else{
                                            if(linha.equals("Boxing")){
                                              audio1index = 0x16;
                                            }
                                            else{
                                            }
                                          }
                                        }
                                      }
                                    }
                                  }
                                }
                              }
                            }                            
                          }
                        }
                      }
                    }
                  }              
                }        
              }
            }
          }
        }
      }
    }
  }

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////

void seleionaArea(String area){
  audio2index = 0;
  if(area.equals("QT")){
    audio2index = 0x01;
  }
  else{
    if(area.equals("EMR")){
      audio2index = 0x02;
    }
    else{
      if(area.equals("Kitting")){
        audio2index = 0x03;
      }
      else{
        if(area.equals("Montagem")){
          audio2index = 0x04;
        }
        else{
          if(area.equals("Desktop")){
            audio2index = 0x05;
          }
          else{
            if(area.equals("Notebook")){
              audio2index = 0x06;
            }
            else{
              if(area.equals("Servidor")){
                audio2index = 0x07;
              }
              else{
              }
            }
          }
        }
      }
    }
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////

void separaString(){
  byte c = 0;
  int j = 0;
  lider = "";
  linha = "";
  area = "";
  for(j=0;j<50;j++){
    if(requests[j]!='/'){
      if(requests[j]=='\0'){
        break;
      }
      else{
        if(c==0){
          lider+=requests[j];
        }
        else{
              if(c==1){
                linha+=requests[j];
              }
              else{
                if(c==2){
                  area+=requests[j];
                }
                else{
                  if(c==3){
                    break;
                  }
                }
              }
        }
      }
    }
    else{
      c++;
    }
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////

void setMaxVolume(){
  sendCommand(0x06, 0x1E); 
} // Fim do setMaxVolume

////////////////////////////////////////////////////////////////////////////////////////////////////////////

void sendCommand(int8_t command, int16_t dat){
  delay(20);
  Send_buf[0] = 0x7E;   
  Send_buf[1] = 0xFF;   
  Send_buf[2] = 0x06;   
  Send_buf[3] = command;
  Send_buf[4] = 0x01;  
  Send_buf[5] = (int8_t)(dat >> 8);  
  Send_buf[6] = (int8_t)(dat);     
  Send_buf[7] = 0xEF; 
  for (uint8_t i = 0; i < 8; i++){
    mp3.write(Send_buf[i]) ;
  } // Fim do for
} // Fim do sendCommand

////////////////////////////////////////////////////////////////////////////////////////////////////////////

void selectSong(int8_t pasta, int8_t song){
  delay(20);
  Send_buf[0] = 0x7E;   
  Send_buf[1] = 0xFF;   
  Send_buf[2] = 0x06;   
  Send_buf[3] = 0x0F;
  Send_buf[4] = 0x01;  
  Send_buf[5] = pasta;
  Send_buf[6] = song;  
  Send_buf[7] = 0xEF; 
  for (uint8_t i = 0; i < 8; i++){
    mp3.write(Send_buf[i]) ;
  } // Fim do for
} // Fim do sendCommand

////////////////////////////////////////////////////////////////////////////////////////////////////////////

void sendMessage(){
  delay(500);
  selectSong(01, audio1index);
  delay(3000);
  selectSong(02, audio2index);
  delay(2000);
} // Fim do sendMessage

////////////////////////////////////////////////////////////////////////////////////////////////////////////

void configura(){
    rec_flag = true;
    digitalWrite(RADIO_FLAG, HIGH);
    radio.frequency(freq);
    radio.setModeTransmit();
    delay(200);
    radio.setUnmute();
    delay(100);
    radio.setUnmute();
    delay(100);
    sendMessage();
    radio.setMute();
    delay(1500);
    radio.setUnmute();
    radio.setModeReceive();
    radio.setMute();
    radio.setUnmute();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////

void feedBack(char fb){
  if (fb == 'n'){
    audio3index = 0x01;
  }
  else{
    audio3index = 0x02;
  }
  radio.frequency(freq);
  radio.setModeTransmit();
  delay(200);
  radio.setUnmute();
  delay(100);
  radio.setUnmute();
  delay(100);
  selectSong(03, audio3index);
  delay(2000);
  radio.setMute();
  delay(1500);
  radio.setUnmute();
  radio.setModeReceive();
  radio.setMute();
  radio.setUnmute();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////

void setup() {
  // Configurações do HamShield
  pinMode(MIC_PIN, OUTPUT);
  digitalWrite(MIC_PIN, LOW); // Permanecer em LOW porque não se usará PWM
  pinMode(RADIO_FLAG, OUTPUT);
  digitalWrite(RADIO_FLAG, LOW);
  pinMode(SWITCH_PIN, INPUT_PULLUP);
  pinMode(RESET_PIN, OUTPUT);
  digitalWrite(RESET_PIN, LOW);
  
  Serial.begin(9600);
  delay(500);
  digitalWrite(RESET_PIN, HIGH);
  delay(5); // Espera o sistema se estabilizar
  radio.initialize(); // Inicializa automaticamente UHF 12.5kHz channel
  freq = 462562;
  radio.dangerMode();
  radio.frequency(freq);
  radio.setModeReceive();
  radio.setSQLoThresh(-90);
  radio.setSQHiThresh(-1);
  radio.setSQOn();
  radio.setRfPower(7);
  radio.setCtcss(66.98);
  radio.enableCtcss();
  rec_flag = false;
  message_detected = false;
  conf_flag = false;
  // Configurações do MP3
  mp3.begin(9600);
  //         (  Comando  ,  Dado )
  delay(100);
  sendCommand(CMD_SEL_DEV, DEV_TF); // Seleciona o dispositivo de armazenamento para o cartão TF
  delay(100);
  setMaxVolume();
  delay(100);
  calibracao();
  Serial.println(noise_limit);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////

void loop() {
  Serial.flush();
  if(Serial.available()){
    radio.setUnmute();
    zeraString();
    i = 0;
    aux = ' ';
    while(Serial.available()){
      aux = Serial.read();
      requests[i]=aux;
      req+= aux;
      i++;
      delay(20);
    }
    Serial.print("Mensagem da Serial: ");
    Serial.println(req);
    separaString();
    seleionaLinha(linha);
    seleionaArea(area);
    seleionaLider(lider);
    digitalWrite(RADIO_FLAG, LOW);
    message_detected == false;
    conf_message_detected = false;
  }
  
  if(rec_flag == true){
    Serial.println("INICIO DA COLETA");
    for(coleta=0; coleta<2000; coleta++){
      if(analogRead(2)>(noise_limit+30)){
        message_detected = true;
      }
      delay(5);
    }
    Serial.println("FIM DA COLETA");
    
    if(message_detected == true){
      delay(3000);
      randNumber = random(0, 4);
      if(randNumber==0){
        feedBack('n');
        detec = 0;
        Serial.println("Negativo");
      }
      else{
        feedBack('p');
        detec = 1;
        Serial.println("Positivo");
      }
      conf_flag = true;
    }
    else{
      Serial.println("Nada foi dito. Status: indisponível.");
      conf_flag = false;
      req+="/indisponivel";
      Serial.print(req);
    }
  rec_flag = false;
  }
  
  if(conf_flag==true){
    Serial.println("INICIO DA SEGUNDA COLETA");
    for(coleta=0; coleta<2000; coleta++){
      if(analogRead(2)>(noise_limit+30)){
        conf_message_detected = true;
      }
      delay(5);
    }
    Serial.println("FIM DA SEGUNDA COLETA");
    if((detec == 0)&&(conf_message_detected == true)){
      req+="/positivo";
      Serial.print(req);
    }
    else{
      if((detec == 1)&&(conf_message_detected == true)){
        req+="/negativo";
        Serial.print(req);
      }
      else{
        if((detec == 0)&&(conf_message_detected == false)){
          req+="/negativo";
          Serial.print(req);
        }
        else{
          if((detec == 1)&&(conf_message_detected == false)){
            req+="/positivo";
            Serial.print(req);
          }
        }
      }
    }
    conf_flag = false;
  }
} // Fim do loop

////////////////////////////////////////////////////////////////////////////////////////////////////////////
