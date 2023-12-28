#include <LiquidCrystal.h>
LiquidCrystal lcd(2, 3, 4, 5, 6, 7, 8);

const int PIN_LIGADO = 12;
const int PIN_DESLIGADO = 13;
const int PIN_POTENCIOMETRO = 1;
const int VALOR_MAX_POTENCIOMETRO = 800;
const int PIN_RESET = 11;
const int PIN_ALTO_FALANTE = 9;
const int PIN_SEMENTE_ALEATORIA = 0;
const int VALOR_MAX_DURACAO_PASSO = 300;
const int VALOR_MIN_DURACAO_PASSO = 150;
const int NUMERO_DE_GLIFOS = 6;

// Definição de glifos (matriz de bytes)
byte glifos[NUMERO_DE_GLIFOS][8] = {
  { B00000, B01110, B11111, B01010, B00000, B00000, B00000, B00000 },  // 1: CARRO SUBINDO
  { B00000, B00000, B00000, B00000, B01110, B11111, B01010, B00000 },  // 2: CARRO DESCENDO
  { B00000, B11110, B11111, B01010, B00000, B00000, B00000, B00000 },  // 3: INIMIGO SUBINDO
  { B00000, B00000, B00000, B00000, B11110, B11111, B01010, B00000 },  // 4: INIMIGO DESCENDO
  { B10101, B01110, B01110, B10101, B00000, B00000, B00000, B00000 },  // 5: BATIDA1
  { B00000, B00000, B00000, B10101, B01110, B01110, B10101, B00000 }   // 6: BATIDA2
};

const int NUMERO_DE_POSICOES_CARRO = 4;
const char CARACTER_ESPACO = 32;

// Mapeamento de glifos para carros
char carroParaGlifos[NUMERO_DE_POSICOES_CARRO][2] = {
  {1, CARACTER_ESPACO},
  {2, CARACTER_ESPACO},
  {CARACTER_ESPACO, 1},
  {CARACTER_ESPACO, 2}
};

// Mapeamento de glifos para caminhões
char caminhaoParaGlifos[NUMERO_DE_POSICOES_CARRO][2] = {
  {3, CARACTER_ESPACO},
  {4, CARACTER_ESPACO},
  {CARACTER_ESPACO, 3},
  {CARACTER_ESPACO, 4}
};

// Mapeamento de glifos para colisões
char colisaoParaGlifos[NUMERO_DE_POSICOES_CARRO][2] = {
  {5, CARACTER_ESPACO},
  {6, CARACTER_ESPACO},
  {CARACTER_ESPACO, 5},
  {CARACTER_ESPACO, 6}
};

const int COMPRIMENTO_ESTRADA = 15;

int estrada[COMPRIMENTO_ESTRADA];
char bufferEstrada[2 + COMPRIMENTO_ESTRADA];

int indiceEstrada;
int posicaoCarro;

const int VALOR_MAX_POSICAO_ESTRADA = 3 * NUMERO_DE_POSICOES_CARRO;
int duracaoPasso;

int colisao; 
unsigned int tempoColisao; 

const char *MENSAGEM_INTRO1 = "Desvie dos,";
const char *MENSAGEM_INTRO2 = "Carros";

const int ATRASO_INTRO = 2200;

void setup() {
   // Configura os pinos como saídas ou entradas

  pinMode(PIN_LIGADO, OUTPUT);
  pinMode(PIN_DESLIGADO, OUTPUT);
  pinMode(PIN_RESET, INPUT);
  pinMode(PIN_ALTO_FALANTE, OUTPUT);

  // Inicializa os estados dos pinos

  digitalWrite(PIN_DESLIGADO, LOW);
  digitalWrite(PIN_LIGADO, HIGH);

 // Inicializa o alto-falante com volume zero
  analogWrite(PIN_ALTO_FALANTE, 0);

 // Inicializa as variáveis de controle
  colisao = tempoColisao = indiceEstrada = 0;
  duracaoPasso = VALOR_MAX_DURACAO_PASSO;
  bufferEstrada[1 + COMPRIMENTO_ESTRADA] = '\0'; // nulo
  randomSeed(analogRead(PIN_SEMENTE_ALEATORIA));

// Cria glifos personalizados no LCD

  for (int i = 0; i < NUMERO_DE_GLIFOS; i++) {
    lcd.createChar(i + 1, glifos[i]);
  }
    // Inicializa a matriz de estrada com valores vazios
  for (int i = 0; i < COMPRIMENTO_ESTRADA; i++) {
    estrada[i] = -1;
  }

  lcd.begin(16, 2);
  obterVolante();
  desenharEstrada();

  lcd.setCursor(1, 0);
  lcd.print(MENSAGEM_INTRO1);
  lcd.setCursor(1, 1);
  lcd.print(MENSAGEM_INTRO2);

  delay(ATRASO_INTRO);
}

void desenharEstrada() {
  for (int i = 0; i < 2; i++) {
    if (colisao) {
      bufferEstrada[0] = colisaoParaGlifos[posicaoCarro][i];
      digitalWrite(PIN_LIGADO, LOW);
      digitalWrite(PIN_DESLIGADO, HIGH);
    }
    else {
      bufferEstrada[0] = carroParaGlifos[posicaoCarro][i];
    }
    for (int j = 0; j < COMPRIMENTO_ESTRADA; j++) {
      int posicao = estrada[(j + indiceEstrada) % COMPRIMENTO_ESTRADA];
      bufferEstrada[j + 1] = (posicao >= 0 && posicao < NUMERO_DE_POSICOES_CARRO) ? caminhaoParaGlifos[posicao][i] : CARACTER_ESPACO;
    }
    lcd.setCursor(0, i);
    lcd.print(bufferEstrada);
  }
}

void obterVolante() {
  posicaoCarro = map(analogRead(PIN_POTENCIOMETRO), 0, 1024, 0, NUMERO_DE_POSICOES_CARRO);
}

void tocarMusicaColisao() {
  int notas[] = {262, 330, 392, 523, 262, 262, 330, 392, 523, 262, 262, 330, 392, 523, 262};  // Frequências das notas (C4, E4, G4, C5)
  int duracaoNota = 250;  // Duração de cada nota em milissegundos
  
  for (int i = 0; i < 12; i++) {
    tone(PIN_ALTO_FALANTE, notas[i], duracaoNota);
    delay(duracaoNota);
    noTone(PIN_ALTO_FALANTE);
    delay(60);  // Pequeno atraso entre as notas
  }
}

void loop() {
  unsigned long agora = millis() - ATRASO_INTRO;
  if (digitalRead(PIN_RESET) == LOW) {
    setup();
  }
   // Verifica se não há colisão
  if (!colisao) {
    obterVolante();
    colisao = (posicaoCarro == estrada[indiceEstrada]);
  }
   // Trata a colisão
  if (colisao) {
    if (!tempoColisao) {
      tempoColisao = agora;
      desenharEstrada();
      lcd.setCursor(2, 0);
      lcd.print("Tempo:");
      lcd.setCursor(2, 1);
      lcd.print(agora / 1000);
      lcd.print(" segundos.");
      tocarMusicaColisao();
    }
  }
  else {
      // Gera novas posições na estrada
    int posicaoAnterior = estrada[(indiceEstrada - 1) % COMPRIMENTO_ESTRADA];
    int estaPosicao = random(VALOR_MAX_POSICAO_ESTRADA);
        // Evita posições muito próximas
    while (abs(estaPosicao - posicaoAnterior) < 2) { //
      estaPosicao = random(VALOR_MAX_POSICAO_ESTRADA);
    }
      // Atualiza a estrada e avança

    estrada[indiceEstrada] = estaPosicao;
    indiceEstrada = (indiceEstrada + 1) % COMPRIMENTO_ESTRADA;
    desenharEstrada();
     // Reduz a duração do passo ao longo do tempo
    delay(duracaoPasso);
    if (duracaoPasso > VALOR_MIN_DURACAO_PASSO) {
      duracaoPasso--;
    }
  }
}
