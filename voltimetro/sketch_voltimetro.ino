#define BUZZER 2              // GPIO2 (D4 NodeMCU)

const float LIMITE = 4.0;
const float TENSAO_MAX = 5.5;

const unsigned long INTERVALO_LEITURA = 500;
const unsigned long INTERVALO_BIP = 250;
const unsigned long TEMPO_ALARME = 5000;
const unsigned long TEMPO_ESPERA = 60000;

enum Estado {
  PARADO,
  ALARME,
  ESPERA
};

Estado estado = PARADO;

float tensao = 0;

bool buzzerLigado = false;

unsigned long ultimaLeitura = 0;
unsigned long trocaBip = 0;
unsigned long inicioEstado = 0;

void setup() {

  Serial.begin(115200);

  pinMode(BUZZER, OUTPUT);

  noTone(BUZZER);

}

void loop() {

  unsigned long agora = millis();

  //-------------------------
  // Leitura da tensão
  //-------------------------

  if (agora - ultimaLeitura >= INTERVALO_LEITURA) {

    ultimaLeitura = agora;

    long soma = 0;

    for (byte i = 0; i < 10; i++) {

      soma += analogRead(A0);

    }

    float adc = soma / 10.0;

    tensao = adc * TENSAO_MAX / 1023.0;

    Serial.println(tensao, 3);

    // Voltou ao normal
    if (tensao >= LIMITE) {

      estado = PARADO;

      noTone(BUZZER);

      buzzerLigado = false;

    }
    else {

      if (estado == PARADO) {

        estado = ALARME;

        inicioEstado = agora;

        trocaBip = agora;

      }

    }

  }

  //-------------------------
  // Máquina de estados
  //-------------------------

  switch (estado) {

    case PARADO:

      break;

    case ALARME:

      // alterna o bip

      if (agora - trocaBip >= INTERVALO_BIP) {

        trocaBip = agora;

        buzzerLigado = !buzzerLigado;

        if (buzzerLigado) {

          tone(BUZZER, 2000);

        }
        else {

          noTone(BUZZER);

        }

      }

      // terminou os 5 segundos?

      if (agora - inicioEstado >= TEMPO_ALARME) {

        noTone(BUZZER);

        buzzerLigado = false;

        estado = ESPERA;

        inicioEstado = agora;

      }

      break;

    case ESPERA:

      // aguardando 1 minuto

      if (agora - inicioEstado >= TEMPO_ESPERA) {

        if (tensao < LIMITE) {

          estado = ALARME;

          inicioEstado = agora;

          trocaBip = agora;

          buzzerLigado = false;

        }
        else {

          estado = PARADO;

        }

      }

      break;

  }

}
