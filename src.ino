// --- DEFINIÇÕES DE HARDWARE ---
const int pinosLED[] = {2,3,4,5,6,7,8,9,10}; 
const int pinoBotaoMover = 12;      
const int pinoBotaoSelecionar = 13; 

// --- VARIÁVEIS DO JOGO ---
int tabuleiro[3][3] = {{0,0,0},{0,0,0},{0,0,0}}; 
int tabuleiroTemp[3][3] = {{0,0,0},{0,0,0},{0,0,0}};
int jogadorAtual = 1; 
int cursor = 0; 
int pontuacaoVitoria[] = {0, 0, 0, 0, 0, 0, 0, 0}; 
int jogoAtivo = 1; 

// --- ESTADOS DOS BOTÕES ---
int ultimoEstadoBotaoMover = HIGH;
int ultimoEstadoBotaoSelect = HIGH;

// --- TEMPORIZADORES ---
unsigned long millisAtual; 
unsigned long millisAnteriorCursor = 0;
const long intervaloCursor = 300; 
int estadoCursor = LOW;
unsigned long millisAnteriorRapido = 0;
const long intervaloRapido = 70; 
int estadoRapido = LOW;

void setup() {
  // --- MISSÃO 1: COMUNICAÇÃO ---
  // Precisamos de iniciar a porta Série. O valor padrão é 9600.
  Serial.begin(9600); 
  
  // --- MISSÃO 2: CONFIGURAÇÃO ---
  // Os LEDs são OUTPUT (Saída de luz) ou INPUT (Entrada de dados)?
  for (int i = 0; i < 9; i++) {
    pinMode(pinosLED[i], OUTPUT); 
  }
  
  pinMode(pinoBotaoMover, INPUT_PULLUP);
  pinMode(pinoBotaoSelecionar, INPUT_PULLUP);
}

void loop() {
  millisAtual = millis(); 

  // Temporizadores visuais
  if (millisAtual - millisAnteriorCursor >= intervaloCursor) {
    millisAnteriorCursor = millisAtual;
    estadoCursor = !estadoCursor; 
  }
  if (millisAtual - millisAnteriorRapido >= intervaloRapido) {
    millisAnteriorRapido = millisAtual;
    estadoRapido = !estadoRapido; 
  }

  lerBotoes();
  
  // Se o jogo estiver ativo (1), desenhamos o tabuleiro.
  // Se não estiver (0), mostramos a vitória.
  if (jogoAtivo == 1) {
    desenharTabuleiro();
  } else {
    exibirVitoria();
  }
}

void lerBotoes() {
  // --- MISSÃO 3: LER SENSORES ---
  // Qual é a função para LER um pino digital? (Dica: começa com digital...)
  int leituraMover = digitalRead(pinoBotaoMover);
  
  // O botão ativa quando vai para LOW (0 Volts)
  if (leituraMover == LOW && ultimoEstadoBotaoMover == HIGH) {
     moverCursor(); 
     delay(50); 
  }
  ultimoEstadoBotaoMover = leituraMover;

  int leituraSelect = digitalRead(pinoBotaoSelecionar);
  if (leituraSelect == LOW && ultimoEstadoBotaoSelect == HIGH) {
      if (tabuleiro[cursor/3][cursor%3] == 0 && jogoAtivo == 1) {
        confirmarJogada();
        delay(50); 
      }
  }
  ultimoEstadoBotaoSelect = leituraSelect;
}

void moverCursor() {
  int posicaoInicial = cursor;
  do {
    // --- MISSÃO 4: MATEMÁTICA ---
    // Queremos que o cursor ande para a frente (+1). 
    // Como escrevemos isso em código?
    cursor++; 
    
    // --- MISSÃO 5: LIMITES ---
    // O tabuleiro vai de 0 a 8. Se o cursor for maior (>) que 8,
    // ele deve voltar para o início. Qual é o número do início?
    if (cursor > 8) cursor = 0;
    
    if (cursor == posicaoInicial) break; 
  } while (tabuleiro[cursor/3][cursor%3] != 0);
  
  Serial.print("Cursor: ");
  Serial.println(cursor);
}

void confirmarJogada() {
  tabuleiro[cursor/3][cursor%3] = jogadorAtual;
  
  verificarVitoria(); 

  if (jogoAtivo == 1) {
    // Troca de jogador: Se for 1 passa a 2, senão passa a 1.
    jogadorAtual = (jogadorAtual == 1) ? 2 : 1;
    moverCursor();
  }
}

void desenharTabuleiro() {
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      int indicePino = i * 3 + j;
      int valorCasa = tabuleiro[i][j];

      // --- MISSÃO 6: CONTROLAR LUZES ---
      // Qual é a função para ESCREVER (ligar/desligar) um pino digital?
      if (jogoAtivo == 1 && indicePino == cursor) {
         digitalWrite(pinosLED[indicePino], estadoCursor);
      }
      else if (valorCasa == 1) {
        digitalWrite(pinosLED[indicePino], HIGH); 
      }
      else if (valorCasa == 2) {
        digitalWrite(pinosLED[indicePino], estadoRapido); 
      }
      else {
        digitalWrite(pinosLED[indicePino], LOW);
      }
    }
  }
}

void verificarVitoria() {
  // Converte 0s em 9s para a matemática funcionar
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      if(tabuleiro[i][j] == 0) tabuleiroTemp[i][j] = 9;
      else tabuleiroTemp[i][j] = tabuleiro[i][j];
    }
  }

   // Calcular Linhas e Colunas
   for (int i = 0; i < 3; i++) {
      int somaLinha = 0;
      int somaColuna = 0;
      for (int j = 0; j < 3; j++) {
         somaLinha += tabuleiroTemp[i][j];
         somaColuna += tabuleiroTemp[j][i];
      }
      pontuacaoVitoria[i] = somaLinha;
      pontuacaoVitoria[3+i] = somaColuna;
   }

   // --- MISSÃO 7: LÓGICA DE JOGO ---
   // Diagonal Principal: Do canto superior esquerdo [0][0]
   // até ao canto inferior direito [2][2].
   // Falta a casa do meio! Qual é a coordenada do meio?
   
   pontuacaoVitoria[6] = tabuleiroTemp[0][0] + tabuleiroTemp[1][1] + tabuleiroTemp[2][2];
   
   // Diagonal Secundária:
   pontuacaoVitoria[7] = tabuleiroTemp[0][2] + tabuleiroTemp[1][1] + tabuleiroTemp[2][0];


  // Verificar se alguém ganhou
  for(int p = 0; p < 8; p++) {
    if (pontuacaoVitoria[p] == 3) { // Vitória Player 1
      jogoAtivo = 0;
      int novo_tabuleiro[3][3] = {{1,0,1},{0,1,0},{1,0,1}}; 
      memcpy(tabuleiro, novo_tabuleiro, sizeof(novo_tabuleiro));
      return;
    }  
    if (pontuacaoVitoria[p] == 6) { // Vitória Player 2
      jogoAtivo = 0;
      int novo_tabuleiro[3][3] = {{1,1,1},{1,0,1},{1,1,1}}; 
      memcpy(tabuleiro, novo_tabuleiro, sizeof(novo_tabuleiro));
      return;
    } 
  }
}

void exibirVitoria() {
  if ((millisAtual / 500) % 2 == 0) {
     for (int i = 0; i < 9; i++) {
        if(tabuleiro[i/3][i%3] == 1) digitalWrite(pinosLED[i], HIGH);
        else digitalWrite(pinosLED[i], LOW);
     }
  } else {
     for (int i = 0; i < 9; i++) digitalWrite(pinosLED[i], LOW);
  }
}