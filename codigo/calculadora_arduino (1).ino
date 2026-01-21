/*
 * CALCULADORA ARITMÉTICA - ARDUINO
 * Arquitetura de Computadores
 * 
 * Funcionalidades:
 * - Calculadora aritmética (adição, subtração, multiplicação, divisão)
 * - Conversão entre bases (decimal, binário, octal, hexadecimal)
 * - Display LCD 16x2 para interface
 * - Teclado 4x4 para entrada de dados
 * - LEDs para representação binária
 * - Tratamento de erros
 */

#include <LiquidCrystal.h>
#include <Keypad.h>

// ========== CONFIGURAÇÃO LCD ==========
// Pinos: RS, E, D4, D5, D6, D7
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// ========== CONFIGURAÇÃO KEYPAD ==========
const byte ROWS = 4;
const byte COLS = 4;

char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {A0, A1, A2, A3};
byte colPins[COLS] = {A4, A5, 6, 7};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// ========== CONFIGURAÇÃO LEDs ==========
// 12 LEDs para representação binária (12 bits)
const int LED_PINS[12] = {13, 10, 9, 8, 22, 24, 26, 28, 30, 32, 34, 36};
const int LED_VERDE_OK = 38;   // LED verde para operação OK
const int LED_VERMELHO_ERRO = 40; // LED vermelho para erro

// ========== VARIÁVEIS GLOBAIS ==========
enum Estado {
  MENU_PRINCIPAL,
  CALCULADORA,
  ENTRADA_OPERANDO1,
  ENTRADA_OPERADOR,
  ENTRADA_OPERANDO2,
  MOSTRA_RESULTADO,
  CONVERSAO_BASE,
  ENTRADA_NUMERO_CONV
};

Estado estadoAtual = MENU_PRINCIPAL;
int menuOpcao = 0;

// Variáveis da calculadora
long operando1 = 0;
long operando2 = 0;
char operador = ' ';
long resultado = 0;
bool erroAtivo = false;
String mensagemErro = "";
String entradaAtual = "";

// Constantes
const long MAX_VALOR = 4096;  // 2^12
const int MAX_BITS = 12;

// ========== SETUP ==========
void setup() {
  Serial.begin(9600);
  
  // Inicializar LCD
  lcd.begin(16, 2);
  lcd.clear();
  
  // Inicializar LEDs
  for (int i = 0; i < 12; i++) {
    pinMode(LED_PINS[i], OUTPUT);
    digitalWrite(LED_PINS[i], LOW);
  }
  pinMode(LED_VERDE_OK, OUTPUT);
  pinMode(LED_VERMELHO_ERRO, OUTPUT);
  
  // Mensagem inicial
  lcd.setCursor(0, 0);
  lcd.print("Calculadora AC");
  lcd.setCursor(0, 1);
  lcd.print("Inicializando...");
  delay(2000);
  
  mostrarMenuPrincipal();
  
  Serial.println("=== CALCULADORA ARDUINO ===");
  Serial.println("Sistema iniciado!");
}

// ========== LOOP PRINCIPAL ==========
void loop() {
  char tecla = keypad.getKey();
  
  if (tecla) {
    Serial.print("Tecla pressionada: ");
    Serial.println(tecla);
    processarTecla(tecla);
  }
  
  delay(50);
}

// ========== PROCESSAMENTO DE TECLAS ==========
void processarTecla(char tecla) {
  switch (estadoAtual) {
    case MENU_PRINCIPAL:
      processarMenuPrincipal(tecla);
      break;
      
    case ENTRADA_OPERANDO1:
      processarEntradaOperando1(tecla);
      break;
      
    case ENTRADA_OPERADOR:
      processarEntradaOperador(tecla);
      break;
      
    case ENTRADA_OPERANDO2:
      processarEntradaOperando2(tecla);
      break;
      
    case MOSTRA_RESULTADO:
      processarMostraResultado(tecla);
      break;
      
    case ENTRADA_NUMERO_CONV:
      processarEntradaNumeroConversao(tecla);
      break;
  }
}

// ========== MENU PRINCIPAL ==========
void mostrarMenuPrincipal() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("1-Calc 2-Conv");
  lcd.setCursor(0, 1);
  lcd.print("Escolha opcao:");
  
  estadoAtual = MENU_PRINCIPAL;
  limparLEDs();
  
  Serial.println("\n=== MENU PRINCIPAL ===");
  Serial.println("1 - Calculadora");
  Serial.println("2 - Conversao de Base");
}

void processarMenuPrincipal(char tecla) {
  if (tecla == '1') {
    iniciarCalculadora();
  } else if (tecla == '2') {
    iniciarConversaoBase();
  }
}

// ========== CALCULADORA ==========
void iniciarCalculadora() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Operando 1:");
  lcd.setCursor(0, 1);
  
  entradaAtual = "";
  operando1 = 0;
  operando2 = 0;
  operador = ' ';
  resultado = 0;
  erroAtivo = false;
  
  estadoAtual = ENTRADA_OPERANDO1;
  
  Serial.println("\n=== CALCULADORA ===");
  Serial.println("Digite o primeiro operando (0-4095):");
}

void processarEntradaOperando1(char tecla) {
  if (tecla >= '0' && tecla <= '9') {
    if (entradaAtual.length() < 4) {
      entradaAtual += tecla;
      lcd.setCursor(0, 1);
      lcd.print("                ");
      lcd.setCursor(0, 1);
      lcd.print(entradaAtual);
    }
  } else if (tecla == '#') {
    // Confirmar entrada
    if (entradaAtual.length() > 0) {
      operando1 = entradaAtual.toInt();
      
      if (validarOperando(operando1)) {
        mostrarBinarioEmLEDs(operando1);
        pedirOperador();
      } else {
        mostrarErro("Operando invalido");
        delay(2000);
        iniciarCalculadora();
      }
    }
  } else if (tecla == '*') {
    // Voltar ao menu
    mostrarMenuPrincipal();
  } else if (tecla == 'C') {
    // Limpar entrada
    entradaAtual = "";
    lcd.setCursor(0, 1);
    lcd.print("                ");
  }
}

void pedirOperador() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Operador:");
  lcd.setCursor(0, 1);
  lcd.print("A+ B- C* D/");
  
  estadoAtual = ENTRADA_OPERADOR;
  
  Serial.println("Operando 1: " + String(operando1));
  Serial.println("Escolha a operacao:");
  Serial.println("A = Adicao (+)");
  Serial.println("B = Subtracao (-)");
  Serial.println("C = Multiplicacao (*)");
  Serial.println("D = Divisao (/)");
}

void processarEntradaOperador(char tecla) {
  if (tecla == 'A' || tecla == 'B' || tecla == 'C' || tecla == 'D') {
    operador = tecla;
    
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(String(operando1) + " ");
    
    switch(operador) {
      case 'A': lcd.print("+"); break;
      case 'B': lcd.print("-"); break;
      case 'C': lcd.print("*"); break;
      case 'D': lcd.print("/"); break;
    }
    
    lcd.setCursor(0, 1);
    lcd.print("Operando 2:");
    
    entradaAtual = "";
    estadoAtual = ENTRADA_OPERANDO2;
    
    Serial.println("Operador selecionado");
    Serial.println("Digite o segundo operando:");
  } else if (tecla == '*') {
    iniciarCalculadora();
  }
}

void processarEntradaOperando2(char tecla) {
  if (tecla >= '0' && tecla <= '9') {
    if (entradaAtual.length() < 4) {
      entradaAtual += tecla;
      
      // Mostrar na segunda linha, após o label
      lcd.setCursor(12, 1);
      lcd.print("    ");
      lcd.setCursor(12, 1);
      lcd.print(entradaAtual);
    }
  } else if (tecla == '#') {
    if (entradaAtual.length() > 0) {
      operando2 = entradaAtual.toInt();
      
      if (validarOperando(operando2)) {
        calcularResultado();
      } else {
        mostrarErro("Operando invalido");
        delay(2000);
        iniciarCalculadora();
      }
    }
  } else if (tecla == '*') {
    pedirOperador();
  } else if (tecla == 'C') {
    entradaAtual = "";
    lcd.setCursor(12, 1);
    lcd.print("    ");
  }
}

void calcularResultado() {
  erroAtivo = false;
  
  // Verificar divisão por zero
  if (operador == 'D' && operando2 == 0) {
    mostrarErro("Divisao por zero");
    delay(2000);
    iniciarCalculadora();
    return;
  }
  
  // Realizar operação
  switch(operador) {
    case 'A':
      resultado = operando1 + operando2;
      break;
    case 'B':
      resultado = operando1 - operando2;
      break;
    case 'C':
      resultado = operando1 * operando2;
      break;
    case 'D':
      resultado = operando1 / operando2;
      break;
  }
  
  // Validar resultado
  if (resultado < 0) {
    mostrarErro("Resultado negativo");
    delay(2000);
    iniciarCalculadora();
    return;
  }
  
  if (resultado >= MAX_VALOR) {
    mostrarErro("Resultado > 4096");
    delay(2000);
    iniciarCalculadora();
    return;
  }
  
  mostrarResultado();
}

void mostrarResultado() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Resultado:");
  lcd.setCursor(0, 1);
  lcd.print(String(resultado));
  
  // Acender LED verde
  digitalWrite(LED_VERDE_OK, HIGH);
  digitalWrite(LED_VERMELHO_ERRO, LOW);
  
  // Mostrar em binário nos LEDs
  mostrarBinarioEmLEDs(resultado);
  
  estadoAtual = MOSTRA_RESULTADO;
  
  Serial.println("\n=== RESULTADO ===");
  Serial.println("Decimal: " + String(resultado));
  Serial.println("Binario: " + converterParaBinario(resultado));
  Serial.println("Octal: " + converterParaOctal(resultado));
  Serial.println("Hexadecimal: " + converterParaHexadecimal(resultado));
  Serial.println("\nPressione # para ver conversoes");
  Serial.println("Pressione * para voltar ao menu");
}

void processarMostraResultado(char tecla) {
  if (tecla == '#') {
    mostrarConversoes(resultado);
  } else if (tecla == '*') {
    digitalWrite(LED_VERDE_OK, LOW);
    mostrarMenuPrincipal();
  }
}

void mostrarConversoes(long numero) {
  // Mostrar decimal e binário
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Dec:" + String(numero));
  lcd.setCursor(0, 1);
  String bin = converterParaBinario(numero);
  if (bin.length() > 16) {
    lcd.print(bin.substring(0, 16));
  } else {
    lcd.print("Bin:" + bin);
  }
  
  delay(3000);
  
  // Mostrar octal e hexadecimal
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Oct:" + converterParaOctal(numero));
  lcd.setCursor(0, 1);
  lcd.print("Hex:" + converterParaHexadecimal(numero));
  
  delay(3000);
  
  // Voltar para tela de resultado
  mostrarResultado();
}

// ========== CONVERSÃO DE BASE ==========
void iniciarConversaoBase() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Numero decimal:");
  lcd.setCursor(0, 1);
  
  entradaAtual = "";
  estadoAtual = ENTRADA_NUMERO_CONV;
  
  Serial.println("\n=== CONVERSAO DE BASE ===");
  Serial.println("Digite um numero decimal (0-4095):");
}

void processarEntradaNumeroConversao(char tecla) {
  if (tecla >= '0' && tecla <= '9') {
    if (entradaAtual.length() < 4) {
      entradaAtual += tecla;
      lcd.setCursor(0, 1);
      lcd.print("                ");
      lcd.setCursor(0, 1);
      lcd.print(entradaAtual);
    }
  } else if (tecla == '#') {
    if (entradaAtual.length() > 0) {
      long numero = entradaAtual.toInt();
      
      if (validarOperando(numero)) {
        mostrarBinarioEmLEDs(numero);
        mostrarConversoes(numero);
      } else {
        mostrarErro("Numero invalido");
        delay(2000);
        iniciarConversaoBase();
      }
    }
  } else if (tecla == '*') {
    mostrarMenuPrincipal();
  } else if (tecla == 'C') {
    entradaAtual = "";
    lcd.setCursor(0, 1);
    lcd.print("                ");
  }
}

// ========== FUNÇÕES DE CONVERSÃO ==========
// Conversão para binário (desenvolvida de raiz)
String converterParaBinario(long numero) {
  if (numero == 0) return "0";
  
  String binario = "";
  long temp = numero;
  
  while (temp > 0) {
    if (temp % 2 == 0) {
      binario = "0" + binario;
    } else {
      binario = "1" + binario;
    }
    temp = temp / 2;
  }
  
  return binario;
}

// Conversão para octal (desenvolvida de raiz)
String converterParaOctal(long numero) {
  if (numero == 0) return "0";
  
  String octal = "";
  long temp = numero;
  
  while (temp > 0) {
    int resto = temp % 8;
    octal = String(resto) + octal;
    temp = temp / 8;
  }
  
  return octal;
}

// Conversão para hexadecimal (desenvolvida de raiz)
String converterParaHexadecimal(long numero) {
  if (numero == 0) return "0";
  
  String hex = "";
  long temp = numero;
  char hexChars[] = "0123456789ABCDEF";
  
  while (temp > 0) {
    int resto = temp % 16;
    hex = String(hexChars[resto]) + hex;
    temp = temp / 16;
  }
  
  return hex;
}

// ========== FUNÇÕES DE VALIDAÇÃO ==========
bool validarOperando(long numero) {
  return (numero >= 0 && numero < MAX_VALOR);
}

// ========== FUNÇÕES DE LEDS ==========
void mostrarBinarioEmLEDs(long numero) {
  // Mostrar número em binário nos 12 LEDs
  for (int i = 0; i < 12; i++) {
    int bit = (numero >> i) & 1;
    digitalWrite(LED_PINS[i], bit);
  }
}

void limparLEDs() {
  for (int i = 0; i < 12; i++) {
    digitalWrite(LED_PINS[i], LOW);
  }
  digitalWrite(LED_VERDE_OK, LOW);
  digitalWrite(LED_VERMELHO_ERRO, LOW);
}

// ========== FUNÇÕES DE ERRO ==========
void mostrarErro(String mensagem) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("ERRO:");
  lcd.setCursor(0, 1);
  lcd.print(mensagem);
  
  // Acender LED vermelho
  digitalWrite(LED_VERMELHO_ERRO, HIGH);
  digitalWrite(LED_VERDE_OK, LOW);
  
  // Piscar LEDs de erro
  for (int i = 0; i < 3; i++) {
    limparLEDs();
    delay(200);
    digitalWrite(LED_VERMELHO_ERRO, HIGH);
    delay(200);
  }
  
  Serial.println("ERRO: " + mensagem);
}
