/*
 * ========================================
 * CALCULADORA ARITMÉTICA ARDUINO - VERSÃO FINAL
 * Arquitetura de Computadores
 * ========================================
 * 
 * FUNCIONALIDADES:
 * ✓ Calculadora: +, -, *, / (12 bits: 0-4095)
 * ✓ Conversão COMPLETA entre bases:
 *   - Decimal → Binário, Octal, Hexadecimal
 *   - Binário → Decimal
 *   - Octal → Decimal
 *   - Hexadecimal → Decimal
 * ✓ Interface LCD 16x2 + Teclado 4x4
 * ✓ 12 LEDs para representação binária
 * ✓ Tratamento completo de erros
 * ✓ LEDs indicadores (verde/vermelho)
 * 
 * HARDWARE:
 * - Arduino Mega 2560
 * - LCD 16x2 (HD44780): pinos 2-5, 11-12
 * - Teclado 4x4: pinos A0-A5, 6-7
 * - 12 LEDs binários: pinos 13,10,9,8,22,24,26,28,30,32,34,36
 * - LED verde (sucesso): pino 38
 * - LED vermelho (erro): pino 40
 */

#include <LiquidCrystal.h>
#include <Keypad.h>

// ========================================
// CONFIGURAÇÃO HARDWARE
// ========================================

// LCD: RS, E, D4, D5, D6, D7
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// TECLADO 4x4
const byte ROWS = 4;
const byte COLS = 4;

char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},  // A = Adição
  {'4', '5', '6', 'B'},  // B = Subtração
  {'7', '8', '9', 'C'},  // C = Multiplicação
  {'*', '0', '#', 'D'}   // D = Divisão, * = Voltar, # = Confirmar
};

byte rowPins[ROWS] = {A0, A1, A2, A3};
byte colPins[COLS] = {A4, A5, 6, 7};

Keypad teclado = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// LEDs BINÁRIOS (12 bits: bit0 a bit11)
const int LED_BINS[12] = {13, 10, 9, 8, 22, 24, 26, 28, 30, 32, 34, 36};

// LEDs INDICADORES
const int LED_VERDE = 38;    // Sucesso
const int LED_VERMELHO = 40; // Erro

// ========================================
// CONSTANTES
// ========================================

const long MAX_VALOR = 4095;  // 2^12 - 1 (máximo para 12 bits)

// ========================================
// MÁQUINA DE ESTADOS
// ========================================

enum Estado {
  MENU_PRINCIPAL,
  MENU_CONVERSAO,
  
  // Estados da Calculadora
  CALC_OPERANDO1,
  CALC_OPERADOR,
  CALC_OPERANDO2,
  CALC_RESULTADO,
  
  // Estados de Conversão
  CONV_DECIMAL,
  CONV_BINARIO,
  CONV_OCTAL,
  CONV_HEXADECIMAL,
  CONV_RESULTADO
};

Estado estadoAtual = MENU_PRINCIPAL;

// ========================================
// VARIÁVEIS GLOBAIS
// ========================================

// Calculadora
long operando1 = 0;
long operando2 = 0;
char operador = ' ';
long resultado = 0;

// Entrada de dados
String entrada = "";

// Modo de conversão (1=Dec, 2=Bin, 3=Oct, 4=Hex)
int modoConversao = 0;

// ========================================
// SETUP
// ========================================

void setup() {
  Serial.begin(9600);
  
  // Inicializar LCD
  lcd.begin(16, 2);
  lcd.clear();
  
  // Inicializar LEDs
  for (int i = 0; i < 12; i++) {
    pinMode(LED_BINS[i], OUTPUT);
    digitalWrite(LED_BINS[i], LOW);
  }
  pinMode(LED_VERDE, OUTPUT);
  pinMode(LED_VERMELHO, OUTPUT);
  digitalWrite(LED_VERDE, LOW);
  digitalWrite(LED_VERMELHO, LOW);
  
  // Mensagem de boas-vindas
  lcd.setCursor(0, 0);
  lcd.print("Calculadora AC");
  lcd.setCursor(0, 1);
  lcd.print("12 bits (0-4095)");
  delay(2000);
  
  mostrarMenuPrincipal();
  
  Serial.println("============================");
  Serial.println("CALCULADORA ARDUINO - 12 BITS");
  Serial.println("============================");
  Serial.println("Sistema iniciado!");
}

// ========================================
// LOOP PRINCIPAL
// ========================================

void loop() {
  char tecla = teclado.getKey();
  
  if (tecla) {
    Serial.print("Tecla: ");
    Serial.println(tecla);
    processarTecla(tecla);
  }
}

// ========================================
// PROCESSAMENTO DE TECLAS
// ========================================

void processarTecla(char tecla) {
  switch (estadoAtual) {
    case MENU_PRINCIPAL:
      if (tecla == '1') iniciarCalculadora();
      else if (tecla == '2') mostrarMenuConversao();
      break;
      
    case MENU_CONVERSAO:
      if (tecla == '1') { modoConversao = 1; iniciarConvDecimal(); }
      else if (tecla == '2') { modoConversao = 2; iniciarConvBinario(); }
      else if (tecla == '3') { modoConversao = 3; iniciarConvOctal(); }
      else if (tecla == '4') { modoConversao = 4; iniciarConvHexadecimal(); }
      else if (tecla == '*') mostrarMenuPrincipal();
      break;
      
    case CALC_OPERANDO1:
      processarCalcOperando1(tecla);
      break;
      
    case CALC_OPERADOR:
      processarCalcOperador(tecla);
      break;
      
    case CALC_OPERANDO2:
      processarCalcOperando2(tecla);
      break;
      
    case CALC_RESULTADO:
      if (tecla == '#') mostrarTodasConversoes(resultado);
      else if (tecla == '*') mostrarMenuPrincipal();
      break;
      
    case CONV_DECIMAL:
      processarConvDecimal(tecla);
      break;
      
    case CONV_BINARIO:
      processarConvBinario(tecla);
      break;
      
    case CONV_OCTAL:
      processarConvOctal(tecla);
      break;
      
    case CONV_HEXADECIMAL:
      processarConvHexadecimal(tecla);
      break;
      
    case CONV_RESULTADO:
      if (tecla == '#') mostrarTodasConversoes(resultado);
      else if (tecla == '*') mostrarMenuConversao();
      break;
  }
}

// ========================================
// MENUS
// ========================================

void mostrarMenuPrincipal() {
  limparLEDs();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("1-Calc  2-Conv");
  lcd.setCursor(0, 1);
  lcd.print("Escolha opcao:");
  
  estadoAtual = MENU_PRINCIPAL;
  
  Serial.println("\n=== MENU PRINCIPAL ===");
  Serial.println("1 - Calculadora");
  Serial.println("2 - Conversao de Base");
}

void mostrarMenuConversao() {
  limparLEDs();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("1-Dec  2-Bin");
  lcd.setCursor(0, 1);
  lcd.print("3-Oct  4-Hex");
  
  estadoAtual = MENU_CONVERSAO;
  
  Serial.println("\n=== CONVERSAO DE BASE ===");
  Serial.println("1 - Decimal -> Todas");
  Serial.println("2 - Binario -> Decimal");
  Serial.println("3 - Octal -> Decimal");
  Serial.println("4 - Hexadecimal -> Decimal");
  Serial.println("* - Voltar");
}

// ========================================
// CALCULADORA
// ========================================

void iniciarCalculadora() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Operando 1:");
  lcd.setCursor(0, 1);
  
  entrada = "";
  operando1 = 0;
  operando2 = 0;
  operador = ' ';
  resultado = 0;
  
  estadoAtual = CALC_OPERANDO1;
  
  Serial.println("\n=== CALCULADORA ===");
  Serial.println("Digite operando 1 (0-4095):");
}

void processarCalcOperando1(char tecla) {
  if (tecla >= '0' && tecla <= '9') {
    if (entrada.length() < 4) {
      entrada += tecla;
      lcd.setCursor(0, 1);
      lcd.print("                ");
      lcd.setCursor(0, 1);
      lcd.print(entrada);
    }
  } 
  else if (tecla == '#') {
    if (entrada.length() > 0) {
      operando1 = entrada.toInt();
      
      if (operando1 >= 0 && operando1 <= MAX_VALOR) {
        mostrarBinario(operando1);
        pedirOperador();
      } else {
        mostrarErro("Fora do limite!");
        delay(2000);
        iniciarCalculadora();
      }
    }
  } 
  else if (tecla == '*') {
    mostrarMenuPrincipal();
  } 
  else if (tecla == 'C') {
    entrada = "";
    lcd.setCursor(0, 1);
    lcd.print("                ");
  }
}

void pedirOperador() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Operacao:");
  lcd.setCursor(0, 1);
  lcd.print("A+  B-  C*  D/");
  
  estadoAtual = CALC_OPERADOR;
  
  Serial.println("Operando 1: " + String(operando1));
  Serial.println("Escolha: A(+), B(-), C(*), D(/)");
}

void processarCalcOperador(char tecla) {
  if (tecla == 'A' || tecla == 'B' || tecla == 'C' || tecla == 'D') {
    operador = tecla;
    
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(String(operando1) + " ");
    if (tecla == 'A') lcd.print("+");
    else if (tecla == 'B') lcd.print("-");
    else if (tecla == 'C') lcd.print("*");
    else if (tecla == 'D') lcd.print("/");
    
    lcd.setCursor(0, 1);
    lcd.print("Operando 2:");
    
    entrada = "";
    estadoAtual = CALC_OPERANDO2;
    
    Serial.println("Digite operando 2:");
  } 
  else if (tecla == '*') {
    iniciarCalculadora();
  }
}

void processarCalcOperando2(char tecla) {
  if (tecla >= '0' && tecla <= '9') {
    if (entrada.length() < 4) {
      entrada += tecla;
      lcd.setCursor(12, 1);
      lcd.print("    ");
      lcd.setCursor(12, 1);
      lcd.print(entrada);
    }
  } 
  else if (tecla == '#') {
    if (entrada.length() > 0) {
      operando2 = entrada.toInt();
      
      if (operando2 >= 0 && operando2 <= MAX_VALOR) {
        calcular();
      } else {
        mostrarErro("Fora do limite!");
        delay(2000);
        pedirOperador();
      }
    }
  } 
  else if (tecla == '*') {
    pedirOperador();
  } 
  else if (tecla == 'C') {
    entrada = "";
    lcd.setCursor(12, 1);
    lcd.print("    ");
  }
}

void calcular() {
  // Verificar divisão por zero
  if (operador == 'D' && operando2 == 0) {
    mostrarErro("Divisao por 0!");
    delay(2000);
    iniciarCalculadora();
    return;
  }
  
  // Calcular resultado
  if (operador == 'A') resultado = operando1 + operando2;
  else if (operador == 'B') resultado = operando1 - operando2;
  else if (operador == 'C') resultado = operando1 * operando2;
  else if (operador == 'D') resultado = operando1 / operando2;
  
  // Validar resultado
  if (resultado < 0) {
    mostrarErro("Resultado < 0!");
    delay(2000);
    iniciarCalculadora();
    return;
  }
  
  if (resultado > MAX_VALOR) {
    mostrarErro("Overflow!");
    delay(2000);
    iniciarCalculadora();
    return;
  }
  
  // Mostrar resultado
  mostrarResultado();
}

void mostrarResultado() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Resultado:");
  lcd.setCursor(0, 1);
  lcd.print(String(resultado));
  
  digitalWrite(LED_VERDE, HIGH);
  digitalWrite(LED_VERMELHO, LOW);
  
  mostrarBinario(resultado);
  
  estadoAtual = CALC_RESULTADO;
  
  Serial.println("\n=== RESULTADO ===");
  Serial.println("Decimal: " + String(resultado));
  Serial.println("Binario: " + decParaBin(resultado));
  Serial.println("Octal: " + decParaOct(resultado));
  Serial.println("Hexadecimal: " + decParaHex(resultado));
  Serial.println("\n# = Ver conversoes | * = Menu");
}

// ========================================
// CONVERSÕES - ENTRADA DECIMAL
// ========================================

void iniciarConvDecimal() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Dec -> Todas");
  lcd.setCursor(0, 1);
  lcd.print("Numero:");
  
  entrada = "";
  estadoAtual = CONV_DECIMAL;
  
  Serial.println("\nDigite numero decimal (0-4095):");
}

void processarConvDecimal(char tecla) {
  if (tecla >= '0' && tecla <= '9') {
    if (entrada.length() < 4) {
      entrada += tecla;
      lcd.setCursor(8, 1);
      lcd.print("        ");
      lcd.setCursor(8, 1);
      lcd.print(entrada);
    }
  } 
  else if (tecla == '#') {
    if (entrada.length() > 0) {
      long num = entrada.toInt();
      
      if (num >= 0 && num <= MAX_VALOR) {
        resultado = num;
        mostrarBinario(resultado);
        mostrarResultadoConversao("Decimal", entrada);
      } else {
        mostrarErro("Fora do limite!");
        delay(2000);
        iniciarConvDecimal();
      }
    }
  } 
  else if (tecla == '*') {
    mostrarMenuConversao();
  } 
  else if (tecla == 'C') {
    entrada = "";
    lcd.setCursor(8, 1);
    lcd.print("        ");
  }
}

// ========================================
// CONVERSÕES - ENTRADA BINÁRIO
// ========================================

void iniciarConvBinario() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Bin -> Dec");
  lcd.setCursor(0, 1);
  lcd.print("Binario:");
  
  entrada = "";
  estadoAtual = CONV_BINARIO;
  
  Serial.println("\nDigite binario (max 12 bits):");
}

void processarConvBinario(char tecla) {
  if (tecla == '0' || tecla == '1') {
    if (entrada.length() < 12) {
      entrada += tecla;
      lcd.setCursor(0, 1);
      lcd.print("                ");
      lcd.setCursor(0, 1);
      lcd.print(entrada);
    }
  } 
  else if (tecla == '#') {
    if (entrada.length() > 0) {
      long num = binParaDec(entrada);
      
      if (num >= 0 && num <= MAX_VALOR) {
        resultado = num;
        mostrarBinario(resultado);
        mostrarResultadoConversao("Binario", entrada);
      } else {
        mostrarErro("Binario invalido");
        delay(2000);
        iniciarConvBinario();
      }
    }
  } 
  else if (tecla == '*') {
    mostrarMenuConversao();
  } 
  else if (tecla == 'C') {
    entrada = "";
    lcd.setCursor(0, 1);
    lcd.print("                ");
  }
}

// ========================================
// CONVERSÕES - ENTRADA OCTAL
// ========================================

void iniciarConvOctal() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Oct -> Dec");
  lcd.setCursor(0, 1);
  lcd.print("Octal:");
  
  entrada = "";
  estadoAtual = CONV_OCTAL;
  
  Serial.println("\nDigite octal (0-7777):");
}

void processarConvOctal(char tecla) {
  if (tecla >= '0' && tecla <= '7') {
    if (entrada.length() < 4) {
      entrada += tecla;
      lcd.setCursor(7, 1);
      lcd.print("         ");
      lcd.setCursor(7, 1);
      lcd.print(entrada);
    }
  } 
  else if (tecla == '#') {
    if (entrada.length() > 0) {
      long num = octParaDec(entrada);
      
      if (num >= 0 && num <= MAX_VALOR) {
        resultado = num;
        mostrarBinario(resultado);
        mostrarResultadoConversao("Octal", entrada);
      } else {
        mostrarErro("Octal invalido!");
        delay(2000);
        iniciarConvOctal();
      }
    }
  } 
  else if (tecla == '*') {
    mostrarMenuConversao();
  } 
  else if (tecla == 'C') {
    entrada = "";
    lcd.setCursor(7, 1);
    lcd.print("         ");
  }
}

// ========================================
// CONVERSÕES - ENTRADA HEXADECIMAL
// ========================================

void iniciarConvHexadecimal() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Hex -> Dec");
  lcd.setCursor(0, 1);
  lcd.print("Hex (0-9,A-D):");
  
  entrada = "";
  estadoAtual = CONV_HEXADECIMAL;
  
  Serial.println("\nDigite hexadecimal (0-FFF):");
  Serial.println("Use teclas A, B, C, D");
}

void processarConvHexadecimal(char tecla) {
  if ((tecla >= '0' && tecla <= '9') || (tecla >= 'A' && tecla <= 'D')) {
    if (entrada.length() < 3) {
      entrada += tecla;
      lcd.setCursor(0, 1);
      lcd.print("                ");
      lcd.setCursor(0, 1);
      lcd.print("Hex: " + entrada);
    }
  } 
  else if (tecla == '#') {
    if (entrada.length() > 0) {
      long num = hexParaDec(entrada);
      
      if (num >= 0 && num <= MAX_VALOR) {
        resultado = num;
        mostrarBinario(resultado);
        mostrarResultadoConversao("Hexadecimal", entrada);
      } else {
        mostrarErro("Hex invalido!");
        delay(2000);
        iniciarConvHexadecimal();
      }
    }
  } 
  else if (tecla == '*') {
    mostrarMenuConversao();
  } 
  else if (tecla == 'C') {
    entrada = "";
    lcd.setCursor(0, 1);
    lcd.print("                ");
    lcd.setCursor(0, 1);
    lcd.print("Hex (0-9,A-D):");
  }
}

void mostrarResultadoConversao(String baseOrigem, String valorOrigem) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(baseOrigem.substring(0, 3) + ":" + valorOrigem);
  lcd.setCursor(0, 1);
  lcd.print("Dec:" + String(resultado));
  
  digitalWrite(LED_VERDE, HIGH);
  digitalWrite(LED_VERMELHO, LOW);
  
  estadoAtual = CONV_RESULTADO;
  
  Serial.println("\n=== RESULTADO ===");
  Serial.println(baseOrigem + ": " + valorOrigem);
  Serial.println("Decimal: " + String(resultado));
  Serial.println("Binario: " + decParaBin(resultado));
  Serial.println("Octal: " + decParaOct(resultado));
  Serial.println("Hexadecimal: " + decParaHex(resultado));
  Serial.println("\n# = Ver todas | * = Menu");
}

// ========================================
// MOSTRAR TODAS AS CONVERSÕES
// ========================================

void mostrarTodasConversoes(long num) {
  // Tela 1: Decimal e Binário
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Dec: " + String(num));
  lcd.setCursor(0, 1);
  String bin = decParaBin(num);
  if (bin.length() > 12) {
    lcd.print(bin.substring(0, 12));
  } else {
    lcd.print("Bin: " + bin);
  }
  delay(3000);
  
  // Tela 2: Octal e Hexadecimal
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Oct: " + decParaOct(num));
  lcd.setCursor(0, 1);
  lcd.print("Hex: " + decParaHex(num));
  delay(3000);
  
  // Voltar à tela de resultado
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Resultado:");
  lcd.setCursor(0, 1);
  lcd.print(String(num));
}

// ========================================
// FUNÇÕES DE CONVERSÃO (DESENVOLVIDAS DE RAIZ)
// ========================================

// DECIMAL → BINÁRIO
String decParaBin(long n) {
  if (n == 0) return "0";
  
  String bin = "";
  while (n > 0) {
    bin = String(n % 2) + bin;
    n /= 2;
  }
  return bin;
}

// DECIMAL → OCTAL
String decParaOct(long n) {
  if (n == 0) return "0";
  
  String oct = "";
  while (n > 0) {
    oct = String(n % 8) + oct;
    n /= 8;
  }
  return oct;
}

// DECIMAL → HEXADECIMAL
String decParaHex(long n) {
  if (n == 0) return "0";
  
  String hex = "";
  char hexChars[] = "0123456789ABCDEF";
  
  while (n > 0) {
    hex = String(hexChars[n % 16]) + hex;
    n /= 16;
  }
  return hex;
}

// BINÁRIO → DECIMAL
long binParaDec(String bin) {
  long dec = 0;
  long potencia = 1;
  
  for (int i = bin.length() - 1; i >= 0; i--) {
    if (bin.charAt(i) == '1') {
      dec += potencia;
    }
    potencia *= 2;
  }
  return dec;
}

// OCTAL → DECIMAL
long octParaDec(String oct) {
  long dec = 0;
  long potencia = 1;
  
  for (int i = oct.length() - 1; i >= 0; i--) {
    int digito = oct.charAt(i) - '0';
    dec += digito * potencia;
    potencia *= 8;
  }
  return dec;
}

// HEXADECIMAL → DECIMAL
long hexParaDec(String hex) {
  long dec = 0;
  long potencia = 1;
  
  for (int i = hex.length() - 1; i >= 0; i--) {
    char c = hex.charAt(i);
    int valor;
    
    if (c >= '0' && c <= '9') {
      valor = c - '0';
    } else if (c >= 'A' && c <= 'F') {
      valor = c - 'A' + 10;
    } else {
      return -1;
    }
    
    dec += valor * potencia;
    potencia *= 16;
  }
  return dec;
}

// ========================================
// FUNÇÕES DE LED
// ========================================

void mostrarBinario(long num) {
  for (int i = 0; i < 12; i++) {
    int bit = (num >> i) & 1;
    digitalWrite(LED_BINS[i], bit);
  }
}

void limparLEDs() {
  for (int i = 0; i < 12; i++) {
    digitalWrite(LED_BINS[i], LOW);
  }
  digitalWrite(LED_VERDE, LOW);
  digitalWrite(LED_VERMELHO, LOW);
}

// ========================================
// FUNÇÃO DE ERRO
// ========================================

void mostrarErro(String msg) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("ERRO:");
  lcd.setCursor(0, 1);
  lcd.print(msg);
  
  digitalWrite(LED_VERDE, LOW);
  digitalWrite(LED_VERMELHO, HIGH);
  
  // Piscar LEDs
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 12; j++) {
      digitalWrite(LED_BINS[j], HIGH);
    }
    delay(200);
    for (int j = 0; j < 12; j++) {
      digitalWrite(LED_BINS[j], LOW);
    }
    delay(200);
  }
  
  digitalWrite(LED_VERMELHO, LOW);
  
  Serial.println("ERRO: " + msg);
}
