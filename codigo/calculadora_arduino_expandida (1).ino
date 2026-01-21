/*
 * CALCULADORA ARITMÉTICA - ARDUINO (VERSÃO EXPANDIDA)
 * Arquitetura de Computadores
 * 
 * Funcionalidades:
 * - Calculadora aritmética (adição, subtração, multiplicação, divisão)
 * - Conversão expandida entre bases:
 *   * Decimal → Binário, Octal, Hexadecimal
 *   * Binário → Decimal
 *   * Octal → Decimal
 *   * Hexadecimal → Decimal
 * - Display LCD 16x2 para interface
 * - Teclado 4x4 para entrada de dados
 * - LEDs para representação binária
 * - Tratamento de erros
 */

#include <LiquidCrystal.h>
#include <Keypad.h>

// ========== CONFIGURAÇÃO LCD ==========
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
const int LED_PINS[12] = {13, 10, 9, 8, 22, 24, 26, 28, 30, 32, 34, 36};
const int LED_VERDE_OK = 38;
const int LED_VERMELHO_ERRO = 40;

// ========== VARIÁVEIS GLOBAIS ==========
enum Estado {
  MENU_PRINCIPAL,
  CALCULADORA,
  ENTRADA_OPERANDO1,
  ENTRADA_OPERADOR,
  ENTRADA_OPERANDO2,
  MOSTRA_RESULTADO,
  MENU_CONVERSAO,
  ENTRADA_DECIMAL,
  ENTRADA_BINARIO,
  ENTRADA_OCTAL,
  ENTRADA_HEXADECIMAL,
  MOSTRA_CONVERSAO
};

Estado estadoAtual = MENU_PRINCIPAL;
int menuOpcao = 0;
int opcaoConversao = 0;

// Variáveis da calculadora
long operando1 = 0;
long operando2 = 0;
char operador = ' ';
long resultado = 0;
bool erroAtivo = false;
String mensagemErro = "";
String entradaAtual = "";

// Constantes
const long MAX_VALOR = 4096;
const int MAX_BITS = 12;

// ========== SETUP ==========
void setup() {
  Serial.begin(9600);
  
  lcd.begin(16, 2);
  lcd.clear();
  
  for (int i = 0; i < 12; i++) {
    pinMode(LED_PINS[i], OUTPUT);
    digitalWrite(LED_PINS[i], LOW);
  }
  pinMode(LED_VERDE_OK, OUTPUT);
  pinMode(LED_VERMELHO_ERRO, OUTPUT);
  
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
    Serial.print("Tecla: ");
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
    case MENU_CONVERSAO:
      processarMenuConversao(tecla);
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
    case ENTRADA_DECIMAL:
      processarEntradaDecimal(tecla);
      break;
    case ENTRADA_BINARIO:
      processarEntradaBinario(tecla);
      break;
    case ENTRADA_OCTAL:
      processarEntradaOctal(tecla);
      break;
    case ENTRADA_HEXADECIMAL:
      processarEntradaHexadecimal(tecla);
      break;
    case MOSTRA_CONVERSAO:
      processarMostraConversao(tecla);
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
    mostrarMenuConversao();
  }
}

// ========== MENU DE CONVERSÃO ==========
void mostrarMenuConversao() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("1-Dec 2-Bin");
  lcd.setCursor(0, 1);
  lcd.print("3-Oct 4-Hex");
  
  estadoAtual = MENU_CONVERSAO;
  
  Serial.println("\n=== MENU CONVERSAO ===");
  Serial.println("1 - Decimal -> Todas");
  Serial.println("2 - Binario -> Decimal");
  Serial.println("3 - Octal -> Decimal");
  Serial.println("4 - Hexadecimal -> Decimal");
}

void processarMenuConversao(char tecla) {
  if (tecla == '1') {
    opcaoConversao = 1;
    iniciarEntradaDecimal();
  } else if (tecla == '2') {
    opcaoConversao = 2;
    iniciarEntradaBinario();
  } else if (tecla == '3') {
    opcaoConversao = 3;
    iniciarEntradaOctal();
  } else if (tecla == '4') {
    opcaoConversao = 4;
    iniciarEntradaHexadecimal();
  } else if (tecla == '*') {
    mostrarMenuPrincipal();
  }
}

// ========== ENTRADA DECIMAL ==========
void iniciarEntradaDecimal() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Numero decimal:");
  lcd.setCursor(0, 1);
  
  entradaAtual = "";
  estadoAtual = ENTRADA_DECIMAL;
  
  Serial.println("\nDigite numero decimal (0-4095):");
}

void processarEntradaDecimal(char tecla) {
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
        resultado = numero;
        mostrarBinarioEmLEDs(resultado);
        mostrarConversoesCompletas();
      } else {
        mostrarErro("Numero invalido");
        delay(2000);
        iniciarEntradaDecimal();
      }
    }
  } else if (tecla == '*') {
    mostrarMenuConversao();
  } else if (tecla == 'C') {
    entradaAtual = "";
    lcd.setCursor(0, 1);
    lcd.print("                ");
  }
}

// ========== ENTRADA BINÁRIO ==========
void iniciarEntradaBinario() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Numero binario:");
  lcd.setCursor(0, 1);
  
  entradaAtual = "";
  estadoAtual = ENTRADA_BINARIO;
  
  Serial.println("\nDigite numero binario (12 bits max):");
}

void processarEntradaBinario(char tecla) {
  if (tecla == '0' || tecla == '1') {
    if (entradaAtual.length() < 12) {
      entradaAtual += tecla;
      lcd.setCursor(0, 1);
      lcd.print("                ");
      lcd.setCursor(0, 1);
      lcd.print(entradaAtual);
    }
  } else if (tecla == '#') {
    if (entradaAtual.length() > 0) {
      long numero = binarioParaDecimal(entradaAtual);
      
      if (numero >= 0 && numero < MAX_VALOR) {
        resultado = numero;
        mostrarBinarioEmLEDs(resultado);
        mostrarResultadoConversao("Binario", entradaAtual, resultado);
      } else {
        mostrarErro("Binario invalido");
        delay(2000);
        iniciarEntradaBinario();
      }
    }
  } else if (tecla == '*') {
    mostrarMenuConversao();
  } else if (tecla == 'C') {
    entradaAtual = "";
    lcd.setCursor(0, 1);
    lcd.print("                ");
  }
}

// ========== ENTRADA OCTAL ==========
void iniciarEntradaOctal() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Numero octal:");
  lcd.setCursor(0, 1);
  
  entradaAtual = "";
  estadoAtual = ENTRADA_OCTAL;
  
  Serial.println("\nDigite numero octal (0-7777):");
}

void processarEntradaOctal(char tecla) {
  if (tecla >= '0' && tecla <= '7') {
    if (entradaAtual.length() < 4) {
      entradaAtual += tecla;
      lcd.setCursor(0, 1);
      lcd.print("                ");
      lcd.setCursor(0, 1);
      lcd.print(entradaAtual);
    }
  } else if (tecla == '#') {
    if (entradaAtual.length() > 0) {
      long numero = octalParaDecimal(entradaAtual);
      
      if (numero >= 0 && numero < MAX_VALOR) {
        resultado = numero;
        mostrarBinarioEmLEDs(resultado);
        mostrarResultadoConversao("Octal", entradaAtual, resultado);
      } else {
        mostrarErro("Octal invalido");
        delay(2000);
        iniciarEntradaOctal();
      }
    }
  } else if (tecla == '*') {
    mostrarMenuConversao();
  } else if (tecla == 'C') {
    entradaAtual = "";
    lcd.setCursor(0, 1);
    lcd.print("                ");
  }
}

// ========== ENTRADA HEXADECIMAL ==========
void iniciarEntradaHexadecimal() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Numero hex:");
  lcd.setCursor(0, 1);
  lcd.print("0-9,A-D,*,#");
  
  entradaAtual = "";
  estadoAtual = ENTRADA_HEXADECIMAL;
  
  Serial.println("\nDigite numero hexadecimal (0-FFF):");
  Serial.println("Use A,B,C,D do teclado");
}

void processarEntradaHexadecimal(char tecla) {
  if ((tecla >= '0' && tecla <= '9') || (tecla >= 'A' && tecla <= 'D')) {
    if (entradaAtual.length() < 3) {
      entradaAtual += tecla;
      lcd.setCursor(0, 1);
      lcd.print("                ");
      lcd.setCursor(0, 1);
      lcd.print(entradaAtual);
    }
  } else if (tecla == '#') {
    if (entradaAtual.length() > 0) {
      long numero = hexadecimalParaDecimal(entradaAtual);
      
      if (numero >= 0 && numero < MAX_VALOR) {
        resultado = numero;
        mostrarBinarioEmLEDs(resultado);
        mostrarResultadoConversao("Hexadecimal", entradaAtual, numero);
      } else {
        mostrarErro("Hex invalido");
        delay(2000);
        iniciarEntradaHexadecimal();
      }
    }
  } else if (tecla == '*') {
    mostrarMenuConversao();
  } else if (tecla == 'C') {
    entradaAtual = "";
    lcd.setCursor(0, 1);
    lcd.print("                ");
  }
}

// ========== CONVERSÕES PARA DECIMAL ==========
// Conversão Binário → Decimal
long binarioParaDecimal(String binario) {
  long decimal = 0;
  long potencia = 1;
  
  for (int i = binario.length() - 1; i >= 0; i--) {
    if (binario.charAt(i) == '1') {
      decimal += potencia;
    }
    potencia *= 2;
  }
  
  return decimal;
}

// Conversão Octal → Decimal
long octalParaDecimal(String octal) {
  long decimal = 0;
  long potencia = 1;
  
  for (int i = octal.length() - 1; i >= 0; i--) {
    int digito = octal.charAt(i) - '0';
    decimal += digito * potencia;
    potencia *= 8;
  }
  
  return decimal;
}

// Conversão Hexadecimal → Decimal
long hexadecimalParaDecimal(String hex) {
  long decimal = 0;
  long potencia = 1;
  
  for (int i = hex.length() - 1; i >= 0; i--) {
    char c = hex.charAt(i);
    int valor;
    
    if (c >= '0' && c <= '9') {
      valor = c - '0';
    } else if (c >= 'A' && c <= 'F') {
      valor = c - 'A' + 10;
    } else {
      return -1; // Inválido
    }
    
    decimal += valor * potencia;
    potencia *= 16;
  }
  
  return decimal;
}

// ========== CONVERSÕES DE DECIMAL ==========
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

// ========== MOSTRAR RESULTADOS ==========
void mostrarResultadoConversao(String baseOrigem, String valorOrigem, long decimal) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(baseOrigem + ":" + valorOrigem);
  lcd.setCursor(0, 1);
  lcd.print("Decimal:" + String(decimal));
  
  digitalWrite(LED_VERDE_OK, HIGH);
  digitalWrite(LED_VERMELHO_ERRO, LOW);
  
  estadoAtual = MOSTRA_CONVERSAO;
  
  Serial.println("\n=== RESULTADO ===");
  Serial.println(baseOrigem + ": " + valorOrigem);
  Serial.println("Decimal: " + String(decimal));
  Serial.println("Binario: " + converterParaBinario(decimal));
  Serial.println("Octal: " + converterParaOctal(decimal));
  Serial.println("Hexadecimal: " + converterParaHexadecimal(decimal));
  Serial.println("\nPressione # para ver todas as conversoes");
  Serial.println("Pressione * para voltar");
}

void processarMostraConversao(char tecla) {
  if (tecla == '#') {
    mostrarConversoesCompletas();
  } else if (tecla == '*') {
    digitalWrite(LED_VERDE_OK, LOW);
    mostrarMenuConversao();
  }
}

void mostrarConversoesCompletas() {
  // Tela 1: Decimal e Binário
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Dec:" + String(resultado));
  lcd.setCursor(0, 1);
  String bin = converterParaBinario(resultado);
  if (bin.length() > 12) {
    lcd.print(bin.substring(0, 12));
  } else {
    lcd.print("Bin:" + bin);
  }
  
  delay(3000);
  
  // Tela 2: Octal e Hexadecimal
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Oct:" + converterParaOctal(resultado));
  lcd.setCursor(0, 1);
  lcd.print("Hex:" + converterParaHexadecimal(resultado));
  
  delay(3000);
  
  // Voltar
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Resultado:");
  lcd.setCursor(0, 1);
  lcd.print(String(resultado));
  
  digitalWrite(LED_VERDE_OK, HIGH);
  estadoAtual = MOSTRA_CONVERSAO;
}

// ========== CALCULADORA (CÓDIGO ORIGINAL) ==========
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
    mostrarMenuPrincipal();
  } else if (tecla == 'C') {
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
  Serial.println("Escolha operacao: A(+) B(-) C(*) D(/)");
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
    
    Serial.println("Digite segundo operando:");
  } else if (tecla == '*') {
    iniciarCalculadora();
  }
}

void processarEntradaOperando2(char tecla) {
  if (tecla >= '0' && tecla <= '9') {
    if (entradaAtual.length() < 4) {
      entradaAtual += tecla;
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
  
  if (operador == 'D' && operando2 == 0) {
    mostrarErro("Divisao por zero");
    delay(2000);
    iniciarCalculadora();
    return;
  }
  
  switch(operador) {
    case 'A': resultado = operando1 + operando2; break;
    case 'B': resultado = operando1 - operando2; break;
    case 'C': resultado = operando1 * operando2; break;
    case 'D': resultado = operando1 / operando2; break;
  }
  
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
  
  mostrarResultadoCalculadora();
}

void mostrarResultadoCalculadora() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Resultado:");
  lcd.setCursor(0, 1);
  lcd.print(String(resultado));
  
  digitalWrite(LED_VERDE_OK, HIGH);
  digitalWrite(LED_VERMELHO_ERRO, LOW);
  
  mostrarBinarioEmLEDs(resultado);
  
  estadoAtual = MOSTRA_RESULTADO;
  
  Serial.println("\n=== RESULTADO ===");
  Serial.println("Decimal: " + String(resultado));
  Serial.println("Binario: " + converterParaBinario(resultado));
  Serial.println("Octal: " + converterParaOctal(resultado));
  Serial.println("Hexadecimal: " + converterParaHexadecimal(resultado));
  Serial.println("\nPressione # para ver conversoes");
  Serial.println("Pressione * para voltar");
}

void processarMostraResultado(char tecla) {
  if (tecla == '#') {
    mostrarConversoesCompletas();
  } else if (tecla == '*') {
    digitalWrite(LED_VERDE_OK, LOW);
    mostrarMenuPrincipal();
  }
}

// ========== FUNÇÕES AUXILIARES ==========
bool validarOperando(long numero) {
  return (numero >= 0 && numero < MAX_VALOR);
}

void mostrarBinarioEmLEDs(long numero) {
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

void mostrarErro(String mensagem) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("ERRO:");
  lcd.setCursor(0, 1);
  lcd.print(mensagem);
  
  digitalWrite(LED_VERMELHO_ERRO, HIGH);
  digitalWrite(LED_VERDE_OK, LOW);
  
  for (int i = 0; i < 3; i++) {
    limparLEDs();
    delay(200);
    digitalWrite(LED_VERMELHO_ERRO, HIGH);
    delay(200);
  }
  
  Serial.println("ERRO: " + mensagem);
}
