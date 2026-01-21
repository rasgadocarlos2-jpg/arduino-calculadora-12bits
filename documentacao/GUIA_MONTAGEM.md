# 🔧 GUIA DE MONTAGEM - Calculadora Arduino

## 📋 Lista de Material

### Componentes Principais
- [x] 1x Arduino Mega 2560
- [x] 1x LCD 16x2 (HD44780)
- [x] 1x Teclado Matricial 4x4
- [x] 12x LEDs Verdes (5mm)
- [x] 1x LED Verde (5mm)
- [x] 1x LED Vermelho (5mm)
- [x] 14x Resistores 220Ω (1/4W)

### Material Adicional
- [x] Protoboard (830 pontos recomendado)
- [x] Jumpers macho-macho (≈40 unidades)
- [x] Jumpers macho-fêmea (≈20 unidades)
- [x] Cabo USB A-B (para programar Arduino)
- [x] Fonte de alimentação 9V (opcional, para uso sem PC)

## 🔌 PINAGEM COMPLETA

### LCD 16x2 → Arduino Mega

| Pino LCD | Função | Pino Arduino | Notas |
|----------|--------|--------------|-------|
| 1 (VSS) | GND | GND | Terra |
| 2 (VDD) | 5V | 5V | Alimentação |
| 3 (V0) | Contraste | 10kΩ para GND | Potenciómetro opcional |
| 4 (RS) | Register Select | 12 | Digital |
| 5 (RW) | Read/Write | GND | Sempre em modo escrita |
| 6 (E) | Enable | 11 | Digital |
| 7-10 (D0-D3) | Data | Não ligado | Modo 4 bits |
| 11 (D4) | Data | 5 | Digital |
| 12 (D5) | Data | 4 | Digital |
| 13 (D6) | Data | 3 | Digital |
| 14 (D7) | Data | 2 | Digital |
| 15 (A) | Backlight + | 5V | Via resistor 220Ω |
| 16 (K) | Backlight - | GND | Terra |

### Teclado 4x4 → Arduino Mega

| Pino Teclado | Tipo | Pino Arduino | Função |
|--------------|------|--------------|--------|
| 1 | Linha 1 | A0 | Linha superior |
| 2 | Linha 2 | A1 | |
| 3 | Linha 3 | A2 | |
| 4 | Linha 4 | A3 | Linha inferior |
| 5 | Coluna 1 | A4 | Coluna esquerda |
| 6 | Coluna 2 | A5 | |
| 7 | Coluna 3 | 6 | |
| 8 | Coluna 4 | 7 | Coluna direita |

**Layout do teclado:**
```
[1] [2] [3] [A]
[4] [5] [6] [B]
[7] [8] [9] [C]
[*] [0] [#] [D]
```

### LEDs Binários (12 unidades) → Arduino Mega

| LED# | Bit | Pino Arduino | Resistor |
|------|-----|--------------|----------|
| LED0 | LSB (bit 0) | 13 | 220Ω para GND |
| LED1 | bit 1 | 10 | 220Ω para GND |
| LED2 | bit 2 | 9 | 220Ω para GND |
| LED3 | bit 3 | 8 | 220Ω para GND |
| LED4 | bit 4 | 22 | 220Ω para GND |
| LED5 | bit 5 | 24 | 220Ω para GND |
| LED6 | bit 6 | 26 | 220Ω para GND |
| LED7 | bit 7 | 28 | 220Ω para GND |
| LED8 | bit 8 | 30 | 220Ω para GND |
| LED9 | bit 9 | 32 | 220Ω para GND |
| LED10 | bit 10 | 34 | 220Ω para GND |
| LED11 | MSB (bit 11) | 36 | 220Ω para GND |

**Ligação de cada LED:**
- Perna LONGA (ânodo +) → Pino Arduino
- Perna CURTA (cátodo -) → Resistor 220Ω → GND

### LEDs Indicadores → Arduino Mega

| LED | Cor | Pino Arduino | Função | Resistor |
|-----|-----|--------------|--------|----------|
| STATUS | Verde | 38 | Sucesso | 220Ω para GND |
| ERROR | Vermelho | 40 | Erro | 220Ω para GND |

## 🛠️ INSTRUÇÕES DE MONTAGEM

### Passo 1: Preparar Protoboard

1. Coloca o Arduino Mega numa superfície plana
2. Posiciona a protoboard ao lado
3. Liga os barramentos de alimentação:
   - **Vermelho (+)** → 5V do Arduino
   - **Azul/Preto (-)** → GND do Arduino

### Passo 2: Montar LCD

1. **Ligações de dados (4 bits):**
   - LCD D4 → Arduino pino 5
   - LCD D5 → Arduino pino 4
   - LCD D6 → Arduino pino 3
   - LCD D7 → Arduino pino 2

2. **Ligações de controlo:**
   - LCD RS → Arduino pino 12
   - LCD E → Arduino pino 11
   - LCD RW → GND (sempre em modo escrita)

3. **Alimentação:**
   - LCD VSS (pino 1) → GND
   - LCD VDD (pino 2) → 5V
   - LCD V0 (pino 3) → GND (ou potenciómetro 10kΩ)

4. **Backlight:**
   - LCD A (pino 15) → 5V (via resistor 220Ω)
   - LCD K (pino 16) → GND

### Passo 3: Montar Teclado 4x4

**Configuração das teclas:**
```
Pino 1 (A0) ─┐
Pino 2 (A1) ─┤  Linhas
Pino 3 (A2) ─┤
Pino 4 (A3) ─┘

Pino 5 (A4) ─┐
Pino 6 (A5) ─┤  Colunas
Pino 7 (6)  ─┤
Pino 8 (7)  ─┘
```

**Ligações diretas:**
- Cada pino do teclado liga diretamente ao Arduino
- NÃO usar resistores pull-up (biblioteca Keypad já trata)

### Passo 4: Montar LEDs Binários (12 unidades)

**Para cada LED (repetir 12 vezes):**

1. Inserir LED na protoboard
2. Identificar pernas:
   - **LONGA** = ânodo (+) → vai ao pino Arduino
   - **CURTA** = cátodo (-) → vai a resistor → GND

3. Ligar conforme tabela de pinagem
4. Organizar em fila para facilitar leitura visual

**Sugestão de layout:**
```
[LED11] [LED10] [LED9] [LED8] ... [LED1] [LED0]
  ↓       ↓       ↓      ↓           ↓      ↓
 bit11   bit10   bit9   bit8       bit1   bit0
 (MSB)                                    (LSB)
```

### Passo 5: Montar LEDs Indicadores

**LED Verde (Sucesso):**
- Ânodo (+) → Arduino pino 38
- Cátodo (-) → Resistor 220Ω → GND

**LED Vermelho (Erro):**
- Ânodo (+) → Arduino pino 40
- Cátodo (-) → Resistor 220Ω → GND

## ⚡ ALIMENTAÇÃO

### Opção 1: Via USB (Desenvolvimento)
- Liga cabo USB entre PC e Arduino
- Fornece 5V regulados
- Permite programar e monitorizar Serial

### Opção 2: Fonte Externa (Standalone)
- Fonte 7-12V DC no jack do Arduino
- Recomendado: 9V, 1A
- Permite funcionamento sem PC

## ✅ VERIFICAÇÃO DA MONTAGEM

### Checklist Visual

- [ ] Todos os jumpers bem encaixados
- [ ] LEDs na orientação correta (perna longa no +)
- [ ] Resistores de 220Ω em TODOS os LEDs
- [ ] LCD com backlight aceso (se ligado)
- [ ] Sem curto-circuitos (fios tocando)
- [ ] Alimentação 5V e GND bem distribuídos

### Teste Inicial (Antes de programar)

1. **Liga apenas alimentação** (sem código)
2. **Verifica:**
   - Backlight do LCD acende? ✓
   - Arduino LED interno (pino 13) pisca? ✓
   - Nenhum componente aquece? ✓
   - Sem fumo ou cheiro? ✓

3. **Se tudo OK**, prosseguir para programação

## 💻 PROGRAMAÇÃO

### Passo 1: Instalar Arduino IDE

1. Download em: https://www.arduino.cc/en/software
2. Instalar no PC
3. Abrir Arduino IDE

### Passo 2: Instalar Bibliotecas

1. **Menu:** Sketch → Include Library → Manage Libraries
2. **Procurar e instalar:**
   - `Keypad` by Mark Stanley, Alexander Brevig
   - `LiquidCrystal` (já incluída)

### Passo 3: Configurar Arduino IDE

1. **Selecionar placa:**
   - Tools → Board → Arduino Mega or Mega 2560
2. **Selecionar processador:**
   - Tools → Processor → ATmega2560
3. **Selecionar porta:**
   - Tools → Port → (porta COM que aparece)

### Passo 4: Upload do Código

1. Abrir `calculadora_final.ino`
2. Clicar em **Verify** (✓) para compilar
3. Se compilou sem erros, clicar em **Upload** (→)
4. Aguardar "Done uploading"

## 🧪 TESTE FUNCIONAL

### Teste 1: LCD e Menu
- **Esperado:** LCD mostra "1-Calc 2-Conv"
- **Se não:** Verificar ligações LCD e contraste (V0)

### Teste 2: Teclado
- **Ação:** Pressionar tecla [1]
- **Esperado:** Menu muda para calculadora
- **Se não:** Verificar ligações do teclado

### Teste 3: LEDs Binários
- **Ação:** Entrar número 255 (11111111 binário)
- **Esperado:** Todos os 8 primeiros LEDs acendem
- **Se não:** Verificar orientação dos LEDs

### Teste 4: Operação Completa
- **Ação:** 10 + 5 = 15
- **Esperado:** 
  - Resultado: 15
  - LEDs mostram: 00001111 (binário)
  - LED verde acende
- **Se não:** Verificar código e serial monitor

### Teste 5: Conversões
- **Ação:** Converter FF (hex) → decimal
- **Esperado:** Resultado 255
- **Se não:** Verificar lógica de conversão

## 🔧 TROUBLESHOOTING

### LCD não mostra nada
- ✓ Verificar alimentação (VSS, VDD)
- ✓ Ajustar contraste (V0 → potenciómetro ou GND)
- ✓ Verificar ligações RS, E, D4-D7

### LCD mostra quadrados
- ✓ Contraste errado → ligar V0 a GND diretamente
- ✓ Inicialização falhou → reset Arduino

### Teclado não responde
- ✓ Verificar ligações de todas as 8 linhas
- ✓ Testar cada tecla individualmente
- ✓ Verificar instalação da biblioteca Keypad

### LEDs não acendem
- ✓ Orientação correta? (perna longa no +)
- ✓ Resistor presente? (220Ω obrigatório)
- ✓ LED queimado? (testar com 5V direto)

### Valores errados
- ✓ Monitor Serial aberto? (9600 baud)
- ✓ Código correto carregado?
- ✓ Testar com valores simples (1+1)

## 📸 FOTOS DE REFERÊNCIA

### Vista Geral
```
┌─────────────────────────────────────┐
│  LCD 16x2                           │
│  ┌──────────────┐                   │
│  │ 1-Calc 2-Conv│                   │
│  │ Escolha:     │                   │
│  └──────────────┘                   │
├─────────────────────────────────────┤
│  Teclado 4x4                        │
│  [1][2][3][A]                       │
│  [4][5][6][B]                       │
│  [7][8][9][C]                       │
│  [*][0][#][D]                       │
├─────────────────────────────────────┤
│  LEDs Binários (12)                 │
│  ● ● ● ● ● ● ● ● ● ● ● ●          │
│  11 10 9 8 7 6 5 4 3 2 1 0         │
│                                     │
│  Status: ● (verde)  ● (vermelho)   │
└─────────────────────────────────────┘
```

## 📦 TRANSPORTE E ARMAZENAMENTO

### Desmontagem
1. Remover alimentação
2. Fotografar montagem (referência)
3. Etiquetar jumpers importantes
4. Guardar componentes em caixa

### Remontagem
1. Seguir fotos de referência
2. Verificar checklist visual
3. Testar antes de demonstrar

## 🎓 DICAS FINAIS

- ✅ **Organização:** Usar jumpers de cores diferentes (5V vermelho, GND preto)
- ✅ **Documentação:** Fotografar cada etapa da montagem
- ✅ **Backup:** Guardar código em múltiplos locais
- ✅ **Teste incremental:** Montar e testar em partes
- ✅ **Serial Monitor:** Usar para debug (9600 baud)

## 📞 SUPORTE

Em caso de dúvidas:
1. Consultar esquema de ligações
2. Verificar Serial Monitor (mensagens de debug)
3. Testar componentes individualmente
4. Rever documentação Arduino oficial

---

**Boa montagem! 🚀**
