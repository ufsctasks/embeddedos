#include <ChRt.h>

#define LIGANDO         0
#define DESLIGANDO      1
#define ESFRIANDO       2
#define ESQUENTANDO     3

#define m_temp          PIND0
#define d_temp          PIND1
#define botao_ligar     PIND2

#define led_verde       PIND3
#define led_vermelho    PIND4
#define led_azul        PIND5

MUTEX_DECL(mutex);
const uint8_t buffer_size = 4;
msg_t buffer[buffer_size];
mailbox_t fila_mensagem;
//MAILBOX_DECL(fila_mensagem, buffer, buffer_size);

static THD_WORKING_AREA(sensores, 64);
static THD_FUNCTION(Controle_Central, arg) {
  uint8_t status = LIGANDO;

  while (true) {
    uint8_t aumentar_temp = digitalRead(m_temp);
    uint8_t diminuir_temp = digitalRead(d_temp);
    uint8_t iniciar = digitalRead(botao_ligar);
    
    if(iniciar == 1){
      status = DESLIGANDO;
      digitalWrite(led_azul, 1);
    }
    else if(iniciar == 0){
      status = LIGANDO;
      digitalWrite(led_azul, 0);
    }
    else if(iniciar == 0 && aumentar_temp == 0){
      status = ESQUENTANDO;
      digitalWrite(led_azul, 0);
      
      chMBPostTimeout(&fila_mensagem, status, TIME_INFINITE);
      
      chMtxUnlock(&mutex);
    }
    else if(iniciar == 0 && diminuir_temp == 0){
      status = ESFRIANDO;
      digitalWrite(led_azul, 0);
      
      chMBPostTimeout(&fila_mensagem, status, TIME_INFINITE);
      
      chMtxUnlock(&mutex);
    }
  }
}

void aquecer()
{
  digitalWrite(led_vermelho, 1);
    
  return;
}

void esfriar()
{
  digitalWrite(led_verde, 1);
  
  return;
}

static THD_WORKING_AREA(atuadores, 64);
static THD_FUNCTION(Controle_Atuadores, arg) {
  msg_t status_fila;
    
  while(1){
    chMBFetchTimeout(&fila_mensagem, &status_fila, TIME_INFINITE);
    
    chMtxLock(&mutex);
    
    if(status_fila == ESQUENTANDO){
      aquecer();
      chThdSleep(TIME_MS2I(50));
      digitalWrite(led_vermelho, 0);
      
      chMtxUnlock(&mutex);
    }
    else if(status_fila == ESFRIANDO){
      esfriar();
      chThdSleep(TIME_MS2I(50));
      digitalWrite(led_verde, 0);
      
      chMtxUnlock(&mutex);
    }
  }
}

void chSetup() {
  // criacao das tarefas
  chThdCreateStatic(sensores, sizeof(sensores), NORMALPRIO, Controle_Central, NULL);
  chThdCreateStatic(atuadores, sizeof(atuadores), NORMALPRIO, Controle_Atuadores, NULL);
}

void setup() {
  // Configuração do fluxo de dados das portas
  pinMode(m_temp, INPUT);
  pinMode(d_temp, INPUT);
  pinMode(botao_ligar, INPUT);

  pinMode(led_verde, OUTPUT);
  pinMode(led_azul, OUTPUT);
  pinMode(led_vermelho, OUTPUT);

  chMtxObjectInit(&mutex);
  chMBObjectInit(&fila_mensagem, buffer, buffer_size);

  // Inicialização do kernel do SO
  chBegin(chSetup);

  while (true);
  
}

void loop() {

}