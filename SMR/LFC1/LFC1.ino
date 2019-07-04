/**************************************************
   Titulo: Quem Procura Acha v1.0
   Data: 13/11/2018
   Grupo: Henrique Werneck e Raphael Soares
   Prof. Wagner Tanaka Botelho
   Q3.2018
 **************************************************/
#include "QTRSensors.h"
#include "DualVNH5019MotorShield.h"
#include "SharpIR.h"  

DualVNH5019MotorShield motor;

/************************************/
/* SENSORES DE LINHAS               */
const int QRE1113Pin1 = A3; // pino de saida de tensao do sensor
const int QRE1113Pin2 = A4; // pino de saida de tensao do sensor
/************************************/
/* SENSOR DO INFRAVERMELHO          */
const int GP2D120Pin = A2; //pino da saida de tensao do sensor

/************************************/
/* VARIAVEIS DOS MOTORES DUALVNH    */
// Motor 1 eh ver:
// do ponto de vista do Ultrason e Infravermelho na Frente: O ESQUERDO
int VELOCIDADE_MOTOR1 = -20;
// Motor 2 eh ver:
// do ponto de vista do Ultrason e Infravermelho na Frente: O DIREITO
int VELOCIDADE_MOTOR2 = -20;
const int ZERO = 0;

/***************************************/
/* VARIAVEIS DA IDENTIFICAÇÃO DA LINHA */
/*"ultimo" é uma variável de referência utilizada quando o robô executa um movimento
 * sobre a linha.
 * 0 = Robô andando para frente (dois motores ligados)
 * 1 = Robô andando para a direita (motor esquerdo ligado)
 * 2 = Robô andando para a esquerda (motor direito ligado)
 */
int ultimo = 0;
float MEDIDA_LINHA_SENSOR_PRETO = 4;

/************************************/
/* VARIAVEIS DO XBEE                */
char incomingByte;
//boolean tf = true; Outro carro controlando o início da comunicação
//String comando = ""; // String de Comunicação entre os Agentes
/************************************/

//Retorna o valor real dos dados da saida do sensor
float getVoltsSensorLinha1() {
  return analogRead(A3) * 0.0048828125;
}

//Retorna o valor real dos dados da saida do sensor
float getVoltsSensorLinha2() {
  return analogRead(A4) * 0.0048828125;
}

//Retorna o valor real dos dados da saida do sensor
float getGP2D120Cm() {
  float cm = (2914 / (analogRead(A2) + 5)) - 1;
  return cm;
}

int classificacaoCor_DoisSensores(float voltsLinha1, float voltsLinha2) {
  if(voltsLinha1 >= 4.00 and voltsLinha2 >= 4.00) {
    // Se for a Cor Preto
    return 0;
  }
  else if((voltsLinha1 >= 1.05 || voltsLinha1 <= 3.99) &&
          (voltsLinha2 >= 1.05 || voltsLinha2 <= 3.99)) {
    // Se for a Cor Verde
    return 1;
  }
  else if(voltsLinha1 <= 0.60 and voltsLinha2 <= 0.60) {
    // Se for a Cor Branco
    return 2;
  } else{
    return -1; //dummy
  }
}

void setup() {
  Serial.begin(9600);
  motor.init();
  ultimo = 0;
}

void loop(){
  int medidaCor = classificacaoCor_DoisSensores(getVoltsSensorLinha1(), getVoltsSensorLinha2());
  float cmIR = getGP2D120Cm();

  motor.setM1Speed(ZERO);
  motor.setM2Speed(ZERO);

//  if (Serial.available() > 0) {
//    incomingByte = Serial.read();
//    if(incomingByte == 'F'){
//      motor.setM1Speed(ZERO);
//      motor.setM2Speed(ZERO);
//      delay(1000);
//
//      while(true) {
//        int medidaCorVirar = classificacaoCor_DoisSensores(getVoltsSensorLinha1(), getVoltsSensorLinha2());
//        float cmIRVirar = getGP2D120Cm();
//        if(cmIRVirar <= 7.00){
//          if(medidaCorVirar == 0){
//            ultimo = 5;
//            break;
//          }
//        }
//        else {
//          motor.setM1Speed(VELOCIDADE_MOTOR1);
//          motor.setM2Speed(-VELOCIDADE_MOTOR2);
//          delay(20);
//        }
//      }
//    }
//  }
  
  if(cmIR <= 7.00) {
      if(medidaCor == 0) {
          /******************************************/
          /* IDENTIFICAÇÃO E COMUNICAÇÃO COM O XBEE */
          /******************************************/
          //if(tf == true)
          //    Serial.println("0001.GIRAR");
          Serial.println('E');
          if (ultimo == 5){
            while(true){
              motor.setM1Speed(-VELOCIDADE_MOTOR1);
              motor.setM2Speed(VELOCIDADE_MOTOR2);
            }
          }
          else {
            motor.setM1Speed(ZERO);
            motor.setM2Speed(ZERO);
            while(true){
              if (Serial.available() > 0) {
                incomingByte = Serial.read();
                if (incomingByte == 'F'){
                  while(true){
                    motor.setM1Speed(-VELOCIDADE_MOTOR1);
                    motor.setM2Speed(VELOCIDADE_MOTOR2);
                  }
                }
              }
            }
          }
          /**********************************/
          delay(50);
		  
		  //while(true) {
          //  motor.setM1Speed(-VELOCIDADE_MOTOR1);
          //  motor.setM2Speed(VELOCIDADE_MOTOR2);
          //  delay(20);
          //}
      }
      else {
          while(true) {
            motor.setM1Speed(-VELOCIDADE_MOTOR1);
            motor.setM2Speed(VELOCIDADE_MOTOR2);

            int medidaCorVirar = classificacaoCor_DoisSensores(getVoltsSensorLinha1(), getVoltsSensorLinha2());            
            if(medidaCorVirar == 0)
              break;
          }
          delay(50);
      }
  }
  else {
      if(getVoltsSensorLinha1() >= MEDIDA_LINHA_SENSOR_PRETO and getVoltsSensorLinha2() >= MEDIDA_LINHA_SENSOR_PRETO) {
          motor.setM1Speed(VELOCIDADE_MOTOR1);
          motor.setM2Speed(VELOCIDADE_MOTOR2);
          ultimo = 0;
      }
      else if (getVoltsSensorLinha1() >= MEDIDA_LINHA_SENSOR_PRETO and getVoltsSensorLinha2() < MEDIDA_LINHA_SENSOR_PRETO) {
          motor.setM1Speed(VELOCIDADE_MOTOR1);
          motor.setM2Speed(ZERO);
          ultimo = 1;
      }
      else if (getVoltsSensorLinha1() < MEDIDA_LINHA_SENSOR_PRETO and getVoltsSensorLinha2() >= MEDIDA_LINHA_SENSOR_PRETO) {
          motor.setM1Speed(ZERO);
          motor.setM2Speed(VELOCIDADE_MOTOR2);
          ultimo = 2;
      }
      else {
          if(ultimo == 1) {
              motor.setM1Speed(ZERO);
              motor.setM2Speed(VELOCIDADE_MOTOR2);
              delay(20);
          }
          else if (ultimo == 2) {
              motor.setM1Speed(VELOCIDADE_MOTOR1);
              motor.setM2Speed(ZERO);
              delay(20);
          }
          motor.setM1Speed(VELOCIDADE_MOTOR1);
          motor.setM2Speed(VELOCIDADE_MOTOR2);
          delay(20);
      }
  }
  delay(50);
}
