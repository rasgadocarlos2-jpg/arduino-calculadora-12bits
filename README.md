# 🔢 Calculadora Aritmética Arduino (12 bits)

> Projeto de Arquitetura de Computadores - Calculadora com conversão entre bases

[![Arduino](https://img.shields.io/badge/Arduino-Mega%202560-00979D?logo=arduino)](https://www.arduino.cc/)
[![Wokwi](https://img.shields.io/badge/Simulador-Wokwi-4CAF50)](https://wokwi.com/)
[![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)

## 📋 Descrição

Calculadora aritmética implementada em Arduino Mega 2560 capaz de:
- ✅ Operações básicas: +, -, *, / (0-4095)
- ✅ Conversão bidirecional entre bases (Dec, Bin, Oct, Hex)
- ✅ Interface LCD 16x2 + Teclado 4x4
- ✅ 12 LEDs para representação binária
- ✅ Tratamento completo de erros

## 🎥 Demo

https://wokwi.com/projects/453788020414292993 

![Screenshot](imagens/wokwi_screenshot.png)

## 🔧 Hardware

- Arduino Mega 2560
- LCD 16x2 (HD44780)
- Teclado matricial 4x4
- 12 LEDs verdes + 2 LEDs indicadores
- 14 resistores 220Ω

## 🚀 Funcionalidades

### Calculadora
- Operações aritméticas com validação completa
- Range: 0-4095 (12 bits)
- Deteta overflow, divisão por zero, resultados negativos

### Conversões
1. **Decimal → Todas as bases**
2. **Binário → Decimal**
3. **Octal → Decimal**
4. **Hexadecimal → Decimal**

*Todas as funções de conversão desenvolvidas de raiz (sem bibliotecas)*

## 📖 Documentação

- [Relatório Completo](documentacao/relatorio_projeto.pdf)
- [Guia de Montagem](documentacao/guia_montagem.md)
- [Casos de Teste](documentacao/casos_teste.md)

## 💻 Como Usar

### Wokwi (Online)
1. Acede a [Wokwi](https://wokwi.com/)
2. Copia o código de `codigo/calculadora_final.ino`
3. Importa `codigo/diagram.json`
4. Clica em "Start Simulation"

### Arduino Real
Ver [Guia de Montagem](documentacao/guia_montagem.md)

## 🧪 Testes

Taxa de sucesso: **100%** (25+ casos de teste)

Exemplos:
```
150 + 75 = 225
- Dec: 225
- Bin: 11100001
- Oct: 341
- Hex: E1

FF (hex) → 255 (dec)
377 (oct) → 255 (dec)
```

## 📚 Tecnologias

- C++ (Arduino)
- Bibliotecas: LiquidCrystal, Keypad
- Hardware: ATmega2560

## 👨‍💻 Autor

**CARLOS RASGADO**
- LinkedIn: [link]
- GitHub: [@rasgadocarlos2-jpg](https://github.com/teu-user)

## 📄 Licença

Este projeto está sob a licença MIT - vê [LICENSE](LICENSE) para detalhes.

## 🙏 Agradecimentos

- Professor Tiago Martins - Arquitetura de Computadores
- Comunidade Arduino
