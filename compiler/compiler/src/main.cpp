#ifndef UNIT_TEST
#include <list>

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <espnow.h>

#include "Constants.h"
#include "ErrorHandler.h"
#include "comm/PadsComm.h"
#include "comm/BLEComm.h"
#include "lexer/Lexer.h"
#include "parser/Parser.h"
#include "interpreter/Interpreter.h"

PadsComm *padsComm = PadsComm::getInstance();
BLEComm *btComm = BLEComm::getInstance();
Lexer lexer;
Parser parser = Parser(&lexer);
Interpreter interpreter;

uint8_t activePadCount = 0;

// callback functions
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  Serial.print("[OUTGOING] Message sent to ");
  for (int i = 0; i < 6; i++) {
    Serial.print(mac_addr[i], HEX);
    if (i < 5)
      Serial.print(":");
  }
  Serial.println(sendStatus == 0 ? " Success" : " Failure");
}

void OnDataRecv(uint8_t *mac, uint8_t *incomingData, uint8_t len) {
  Serial.print("[INCOMING] Message from ");
  for (int i = 0; i < 6; i++) {
    Serial.print(mac[i], HEX);
    Serial.print(":");
  }
  Serial.print(" 0x");
  Serial.println(*incomingData, HEX);

  if (*incomingData == padInput_padOccupied) {
    padsComm->setPadOccupied(mac, incomingData);
  }
}


// ----------------MAIN FUNCTIONS-----------------
void setup() {
  // init connections
  Serial.begin(115200);

  delay(500);
  Serial.setDebugOutput(true);

  Serial.print("\nReset reason: ");
  Serial.println(ESP.getResetReason());
  Serial.println(ESP.getResetInfo());

  padsComm->initEspNow(OnDataSent, OnDataRecv);

  // random seed
  randomSeed(analogRead(0));
}

void loop() {
  if (btComm->hasUnreadBytes()) {
    switch (btComm->readByte()) {
      case phoneInput_makeSound_pad1:
        padsComm->playSingleSound(soundsArray[0], 1000, 0);
        break;
      case phoneInput_makeSound_pad2:
        padsComm->playSingleSound(soundsArray[1], 1000, 1);
        break;
      case phoneInput_makeSound_pad3:
        padsComm->playSingleSound(soundsArray[2], 1000, 2);
        break;
      case phoneInput_makeSound_pad4:
        padsComm->playSingleSound(soundsArray[3], 1000, 3);
        break;
      // all hex codes from here on out are undocumented and just for testing purposes
      case 0x05:
        padsComm->play8Sounds(soundsArray, correctActionDurations);
        break;
      case 0x06:
        padsComm->playWinnerJingle();
        padsComm->waitWithEventChecks(3000);
        padsComm->playLoserJingle();
        padsComm->waitWithEventChecks(3000);
        padsComm->playCorrectActionJingle();
        padsComm->waitWithEventChecks(3000);
        padsComm->playWrongActionJingle();
        break;
      case 0x07:
        padsComm->waitForPlayerOnAnyPad();
        if (padsComm->isPadOccupied(1)) {
          padsComm->playWinnerJingle();
        } else {
          padsComm->playLoserJingle();
        }
        break;
      case 0x08:
        padsComm->waitForPlayerOnPad(1);
        if (padsComm->isPadOccupied(1)) {
          Serial.println("SUCCESS");
        } else {
          Serial.println("FAILURE");
        }
        break;
      case phoneInput_gameSelection_Memory:
        {
          Serial.println("Memory start");

          bool easyMemory = true;

          if (easyMemory) {
            Serial.println("Playing easy memory");
            uint8_t selectedPad = anyPad;
            uint8_t selectedLastRound = anyPad;

            while (true) {
              // select random pad and deactivate for next round
              while (selectedPad == selectedLastRound) {
                selectedPad = random(maxAllowedPads);
              }
              selectedLastRound = selectedPad;
              Serial.print("Selected correct pad: ");
              Serial.println(selectedPad, DEC);

              // play tone on correct pad
              padsComm->playSingleSound(soundsArray[selectedPad], 1000, selectedPad);
              Serial.println("Played correct sound once");

              // check if player jumps on correct pad
              padsComm->waitForPlayerOnAnyPad();

              // if not on the correct pad
              if (!padsComm->isPadOccupied(selectedPad)) {
                Serial.println("Wrong pad selected!!!");
                padsComm->playLoserJingle();
                Serial.println("Returning...");
                break;
              } else /* Correct pad */ {
                Serial.println("Correct pad selected!!!");
                padsComm->playWinnerJingle();
              }
            }
          } else {
            const uint8_t maxRounds = 4;
            uint8_t soundSeq[maxRounds];
            memset(soundSeq, UINT8_MAX, maxRounds);

            for (size_t round = 0; round < maxRounds; round++) {
              soundSeq[round] = random(maxAllowedPads);

              // play sound sequence on pads in order
              Serial.print("Pad order: [");
              for (size_t j = 0; j <= round; j++) {
                uint8_t padIndex = soundSeq[j];

                Serial.print(padIndex);
                Serial.print(j + 1 != round ? "," : "]\n");

                padsComm->playSingleSound(soundsArray[padIndex], 1000, padIndex);
              }

              // check for correct pads
              for (size_t j = 0; j <= round; j++) {
                padsComm->waitForPlayerOnAnyPad();

                if (padsComm->isPadOccupied(soundSeq[j])) {
                  padsComm->playLoserJingle();
                  return;
                } else /* Correct pad */ {
                  padsComm->playCorrectActionJingle();
                }
              }
            }

            padsComm->waitWithEventChecks(1000);
            padsComm->playWinnerJingle();
          }

          Serial.println("Memory end");
          break;
        }
      case phoneInput_gameSelection_Reaktion:
        {
          Serial.println("Reaktion start");

          activePadCount = 2;
          int correctIndex = random(chordLen);
          std::vector<int> usable;
          for (int i = 0; i < paramLen; i++) {
            if (soundsArray[i] != 0) {
              usable.push_back(soundsArray[i]);
            }
          }
          usable.shrink_to_fit();


          // play all possible sounds so players know
          {
            int soundLenArray[paramLen] = { 500, 500, 500, 500, 0, 0, 0, 0 };
            padsComm->play8Sounds(soundsArray, soundLenArray);

            padsComm->waitWithEventChecks(3000);
          }

          int countdownTones[paramLen] = { 440, 440, 440, 880, 0, 0, 0, 0 };
          int countdownLen[paramLen] = { 1000, 1000, 1000, 1000, 0, 0, 0, 0 };
          while (activePadCount > 1) {
            padsComm->waitWithEventChecks(3000);

            // play correct sound
            padsComm->playSingleSound(soundsArray[correctIndex], 500);
            padsComm->waitWithEventChecks(3000);

            // play countdown before game begins
            padsComm->play8Sounds(countdownTones, countdownLen);

            // repeat sound playing until correct tone is played
            while (!usable.empty()) {
              int randomIndex = random(usable.size());
              padsComm->playSingleSound(usable[randomIndex], 500);
              usable.erase(usable.begin() + randomIndex);
              if (!usable.empty()) {
                padsComm->waitWithEventChecks(3000);
              }
            }

            Serial.println("Correct tone played, waiting for player on pads");

            PadsComm::WaitResult ret = padsComm->waitForPlayerOnAnyPad();
            if (ret == PadsComm::WaitResult::PadOccupied) {
              for (int i = 0; i < maxAllowedPads; i++) {
                if (padsComm->isPadOccupied(i)) {
                  padsComm->playWinnerJingle(i);
                }
              }
            }

            activePadCount--;
          }

          padsComm->playCorrectActionJingle();
          Serial.println("---Reaktion End---");
          break;
        }
      case phoneInput_tokenize:
        {
          char code[] = "variable_name anotherVar"; // "let something = \"anything\"; if(something == \"anything\") {print(\"hello\");}";

          Serial.println(code);

          ErrorHandler::printMemoryStats("before tokenizing");

          std::queue<Lexer::Token> tokens = lexer.tokenize(code, strlen(code));

          ErrorHandler::printMemoryStats("finished tokenizing");

          Serial.println("Tokenized!");

          while (!tokens.empty()) {
            Lexer::Token token = std::move(tokens.front());
            tokens.pop();
            Serial.print("Value: \"");
            Serial.print(token.value);
            Serial.print("\"\nTokenType: ");
            switch (token.type) {
              case Lexer::TokenType::Let:
                Serial.println("Let");
                break;
              case Lexer::TokenType::Const:
                Serial.println("Const");
                break;
              case Lexer::TokenType::If:
                Serial.println("If");
                break;
              case Lexer::TokenType::Else:
                Serial.println("Else");
                break;
              case Lexer::TokenType::While:
                Serial.println("While");
                break;
              case Lexer::TokenType::Break:
                Serial.println("Break");
                break;
              case Lexer::TokenType::Identifier:
                Serial.println("Identifier");
                break;
              case Lexer::TokenType::Equals:
                Serial.println("Equals");
                break;
              case Lexer::TokenType::ArithmeticOperator:
                Serial.println("ArithmeticOperator");
                break;
              case Lexer::TokenType::LogicalOperator:
                Serial.println("LogicalOperator");
                break;
              case Lexer::TokenType::RelationalOperator:
                Serial.println("RelationalOperator");
                break;
              case Lexer::TokenType::OpenParen:
                Serial.println("OpenParen");
                break;
              case Lexer::TokenType::CloseParen:
                Serial.println("CloseParen");
                break;
              case Lexer::TokenType::OpenBracket:
                Serial.println("OpenBracket");
                break;
              case Lexer::TokenType::CloseBracket:
                Serial.println("CloseBracket");
                break;
              case Lexer::TokenType::OpenBrace:
                Serial.println("OpenBrace");
                break;
              case Lexer::TokenType::CloseBrace:
                Serial.println("CloseBrace");
                break;
              case Lexer::TokenType::Number:
                Serial.println("Number");
                break;
              case Lexer::TokenType::StringLiteral:
                Serial.println("StringLiteral");
                break;
              case Lexer::TokenType::Semicolon:
                Serial.println("Semicolon");
                break;
              case Lexer::TokenType::Colon:
                Serial.println("Colon");
                break;
              case Lexer::TokenType::Comma:
                Serial.println("Comma");
                break;
              case Lexer::TokenType::Dot:
                Serial.println("Dot");
                break;
              case Lexer::TokenType::EndOfFile:
                Serial.println("EOF");
                break;
                /*default:
                break;*/
            }
          }

          break;
        }
      case phoneInput_parse:
        {
          char code[] = "let something = \"anything\"; if(something == \"anything\") {print(\"hello\");} else {let obj = {num:5,str:\"hello\"}; print(obj.num);}";

          ErrorHandler::printMemoryStats("before producing AST");

          Parser::Program *program = parser.produceAST(code, strlen(code));

          ErrorHandler::printMemoryStats("after producing AST");

          if (program->kind == Parser::NodeType::Program)
            Serial.println("Program parsed successfully!");

          parser.printAST(program);
          break;
        }
      case phoneInput_interpret:
        {
          ErrorHandler::printMemoryStats("before program");

          Environment env;

          ErrorHandler::printMemoryStats("after env decl");

          Serial.println("\nReady to interpret!");

          while (true) {
            if (Serial.available()) {
              String code = Serial.readStringUntil('\n');

              if (code.startsWith("AT") || code.startsWith("OK")) {
                Serial.print("Got HC-05 code ");
                Serial.println(code);
                if (code.equals("OK+LOST")) {
                  return;
                }
                continue;
              }

              if (code.equalsIgnoreCase("exit")) {
                break;
              }

              Serial.println("Got string: ");
              Serial.println(code);

              char *code_cstr = new char[code.length() + 1];
              code.toCharArray(code_cstr, code.length() + 1, 0);

              Serial.println("Producing AST");
              ErrorHandler::printMemoryStats("before AST production");
              Parser::Program *program = parser.produceAST(code_cstr, code.length() + 1);
              parser.printAST(program);
              Serial.println("Successfully parsed program");
              ErrorHandler::printMemoryStats("after AST production");
              ESP.wdtFeed();

              std::unique_ptr<Values::RuntimeVal> val = interpreter.evaluate(program, &env);
              ErrorHandler::printMemoryStats("after evaluation");
              ESP.wdtFeed();


              Serial.print("Value: ");
              switch (val->type) {
                case Values::ValueType::Null:
                  Serial.println("Null");
                  break;
                case Values::ValueType::Boolean:
                  {
                    Serial.print("Boolean: ");
                    Values::BooleanVal *boolVal = static_cast<Values::BooleanVal *>(val.get());
                    Serial.println(boolVal->value);
                    break;
                  }
                case Values::ValueType::Number:
                  {
                    Serial.print("Number: ");
                    Values::NumberVal *numVal = static_cast<Values::NumberVal *>(val.get());
                    Serial.println(numVal->value);
                    break;
                  }
                case Values::ValueType::String:
                  {
                    Serial.println("String: ");
                    Values::StringVal *stringVal = static_cast<Values::StringVal *>(val.get());
                    Serial.println(stringVal->str);
                    break;
                  }
                case Values::ValueType::NativeFn:
                  Serial.println("NativeFn");
                  break;
                case Values::ValueType::ObjectVal:
                  Serial.println("ObjectVal");
                  break;
                case Values::ValueType::Break:
                  Serial.println("Break");
                  break;
              }

              delete[] code_cstr;

              ErrorHandler::printMemoryStats("after program");
            }
          }

          Serial.println("Exiting...");
        }
        break;
      default:
        break;
    }
  }
}

#endif