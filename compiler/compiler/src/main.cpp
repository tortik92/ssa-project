// -------------------INCLUDE-----------------------
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <espnow.h>

#include "Constants.h"
#include "GlobalFunctions.h"
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

// ----------------HELPER FUNCTIONS------------------
void shuffle(int *array, size_t n) {
  if (n > 1) {
    for (size_t i = 0; i < n; i++) {
      size_t j = random(i, n - 1);

      int tmp = array[j];
      array[j] = array[i];
      array[i] = tmp;
    }
  }
}

// callback functions
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  Serial.print("\nMessage sent to: ");
  for (int i = 0; i < 6; i++) {
    Serial.print(mac_addr[i], HEX);
    if (i < 5)
      Serial.print(":");
  }
  Serial.println(sendStatus == 0 ? " Success" : " Failure");
}

void OnDataRecv(uint8_t *mac, uint8_t *incomingData, uint8_t len) {
  if (*incomingData == padInput_padOccupied) {
    padsComm->setPadOccupied(mac, incomingData);
  }
}

// tostring
void toStringNumericLiteral(const Parser::NumericLiteral *numLit) {
  Serial.print("{\"type\":\"numericLiteral\",\"value\":\"");
  Serial.print(numLit->num);
  Serial.print("\"}");
}

void toStringIdentifier(const Parser::Identifier *ident) {
  Serial.print("{\"type\":\"identifier\",\"symbol\":\"");
  Serial.print(ident->symbol);
  Serial.print("\"}");
}

void toString(const Parser::Stmt *stmt);

void toStringBinaryExpr(const Parser::BinaryExpr *binaryExpr) {
  Serial.print("{\"type\":\"binaryExpr\",");
  Serial.print("\"left\":");
  toString(static_cast<const Parser::Stmt *>(binaryExpr->left));
  Serial.print(",\"operator\":\"");
  Serial.print(binaryExpr->op);
  Serial.print("\",");
  Serial.print("\"right\":");
  toString(static_cast<const Parser::Stmt *>(binaryExpr->right));
  Serial.print("}");
}

void toStringLogicalExpr(const Parser::LogicalExpr *logicalExpr) {
  Serial.print("{\"type\":\"logicalExpr\",");
  Serial.print("\"left\":");
  toString(static_cast<const Parser::Stmt *>(logicalExpr->left));
  Serial.print(",\"operator\":\"");
  Serial.print(logicalExpr->op);
  Serial.print("\",\"right\":");
  toString(static_cast<const Parser::Stmt *>(logicalExpr->right));
  Serial.print("}");
}

void toStringVarDecl(const Parser::VarDeclaration *varDecl) {
  Serial.print("{\"type\":\"varDecl\",");
  Serial.print("\"isConstant\":");
  Serial.print(varDecl->constant ? "\"true\"," : "\"false\",");
  Serial.print("\"identifier\":\"");
  Serial.print(varDecl->ident);
  Serial.print("\",\"value\":");
  if (varDecl->value != nullptr) toString(varDecl->value);
  Serial.print("}");
}

void toStringBlockStmt(const Parser::BlockStmt *blockStmt) {
  Serial.print("{\"type\":\"blockStmt\",\"body\":[");
  for (size_t i = 0; i < maxProgramStatements && blockStmt->body[i] != nullptr; i++) {
    toString(blockStmt->body[i]);
    Serial.print(",");
  }
  Serial.print("]}");
}

void toStringIfStmt(const Parser::IfStmt *ifStmt) {
  Serial.print("{\"type\":\"ifStmt\",");
  Serial.print("\"test\":");
  toString(ifStmt->test);
  Serial.print(",\"consequent\":");
  toStringBlockStmt(ifStmt->consequent);
  Serial.print(",\"alternate\":");
  if (ifStmt->alternate != nullptr) {
    toStringBlockStmt(ifStmt->alternate);
  } else {
    Serial.print("null");
  }
  Serial.print("}");
}

void toStringWhileStmt(const Parser::WhileStmt *whileStmt) {
  Serial.print("{\"type\":\"whileStmt\",");
  Serial.print("\"test\":");
  toString(whileStmt->test);
  Serial.print(",\"body\":");
  toStringBlockStmt(whileStmt->body);
  Serial.print("}");
}

void toStringBreakStmt(const Parser::BreakStmt *breakStmt) {
  Serial.print("{\"type\":\"breakStmt\"}");
}

void toStringAssignmentExpr(const Parser::AssignmentExpr *assignmentExpr) {
  Serial.print("{\"type\":\"assignmentExpr\",\"assignee\":");
  toString(assignmentExpr->assignee);
  Serial.print(",\"value\":");
  toString(assignmentExpr->value);
  Serial.print("}");
}

void toStringCallExpr(const Parser::CallExpr *callExpr) {
  Serial.print("{\"type\":\"callExpr\",\"caller\":");
  toString(callExpr->caller);
  Serial.print(",\"args\":[");
  for (size_t i = 0; i < maxFunctionArgs && callExpr->args[i] != nullptr; i++) {
    toString(callExpr->args[i]);
    Serial.print(",");
  }
  Serial.print("]}");
}

void toString(const Parser::Stmt *stmt) {
  switch (stmt->kind) {
    case Parser::NodeType::BinaryExpr:
      toStringBinaryExpr(static_cast<const Parser::BinaryExpr *>(stmt));
      break;
    case Parser::NodeType::LogicalExpr:
      toStringLogicalExpr(static_cast<const Parser::LogicalExpr *>(stmt));
      break;
    case Parser::NodeType::Identifier:
      toStringIdentifier(static_cast<const Parser::Identifier *>(stmt));
      break;
    case Parser::NodeType::NumericLiteral:
      toStringNumericLiteral(static_cast<const Parser::NumericLiteral *>(stmt));
      break;
    case Parser::NodeType::VarDeclaration:
      toStringVarDecl(static_cast<const Parser::VarDeclaration *>(stmt));
      break;
    case Parser::NodeType::IfStmt:
      toStringIfStmt(static_cast<const Parser::IfStmt *>(stmt));
      break;
    case Parser::NodeType::WhileStmt:
      toStringWhileStmt(static_cast<const Parser::WhileStmt *>(stmt));
      break;
    case Parser::NodeType::BreakStmt:
      toStringBreakStmt(static_cast<const Parser::BreakStmt *>(stmt));
      break;
    case Parser::NodeType::BlockStmt:
      toStringBlockStmt(static_cast<const Parser::BlockStmt *>(stmt));
      break;
    case Parser::NodeType::AssignmentExpr:
      toStringAssignmentExpr(static_cast<const Parser::AssignmentExpr *>(stmt));
      break;
    case Parser::NodeType::CallExpr:
      toStringCallExpr(static_cast<const Parser::CallExpr *>(stmt));
      break;
    case Parser::NodeType::Program:
      GlobalFunctions::restart("Found Program in Program, don't know what to do with it");
      break;
    case Parser::NodeType::Undefined:
      GlobalFunctions::restart("Undefined node found");
    default:
      GlobalFunctions::restart("Undefined statement found while printing AST");
      break;
  }
}

void toString(const Parser::Program *program) {
  Serial.print("{\"type\":\"program\",\"body\":[");

  size_t i = 0;
  while (true) {
    if (i < maxProgramStatements && program->body[i] != nullptr) {
      toString(program->body[i]);
      Serial.println(",");
      i++;
    } else {
      break;
    }
  }
  Serial.print("]}");
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
      case phoneInput_gameSelection_Memory:
        {
          Serial.println("Memory start");

          uint8_t selectedLastRound = UINT8_MAX;
          uint8_t selectedPad = UINT8_MAX;

          bool easyMemory = true;

          if (easyMemory) {
            while (true) {
              // select random pad and deactivate for next round
              while (selectedPad == selectedLastRound) {
                selectedPad = random(maxAllowedPads);
              }
              selectedLastRound = selectedPad;

              // play tone on correct pad
              padsComm->playSingleSound(soundsArray[selectedPad], 1000, selectedPad);

              // check if player jumps on correct pad
              padsComm->waitForPlayerOnAnyPad();

              // if not on the correct pad
              if ((*padsComm->getPad(selectedPad)).isOccupied) {
                padsComm->playLoserJingle();
                break;
                return;
              } else /* Correct pad */ {
                padsComm->playWinnerJingle();
              }
            }
          } else {
            const uint8_t soundSeqLen = 32;
            uint8_t correctSelectionsCount = 0;
            uint8_t soundSeq[soundSeqLen];
            memset(soundSeq, UINT8_MAX, soundSeqLen);

            while (true) {
              // select random pad and deactivate for next round
              while (selectedPad == selectedLastRound) {
                selectedPad = random(maxAllowedPads);
              }
              selectedLastRound = selectedPad;

              soundSeq[correctSelectionsCount] = selectedPad;
              correctSelectionsCount++;

              // play sound sequence on pads in order
              for (int i = 0; i < soundSeqLen && soundSeq[i] < maxAllowedPads; i++) {
                uint8_t padIndex = soundSeq[i];
                padsComm->playSingleSound(soundsArray[padIndex], 1000, padIndex);
              }

              for (int i = 0; i < soundSeqLen; i++) {
                // check if player jumps on correct pad
                padsComm->waitForPlayerOnAnyPad();

                // if not on the correct pad
                PadsComm::Pad *pad = padsComm->getPad(selectedPad);

                if (!pad->isOccupied) {
                  padsComm->playWrongActionJingle();
                  break;
                } else /* Correct pad */ {
                  if (correctSelectionsCount >= soundSeqLen) {
                    padsComm->playWinnerJingle();
                  } else {
                    padsComm->playCorrectActionJingle();
                  }
                  return;
                }
              }
            }
          }

          Serial.println("Memory end");
          break;
        }
      case phoneInput_gameSelection_Reaktion:
        {
          Serial.println("---Reaktion selected---");

          activePadCount = 2;
          //int shuffledToneOrder[chordAMajorLen] = { 0 };
          //memcpy(&shuffledToneOrder, &soundsArray, sizeof(shuffledToneOrder));
          //int correctToneOrder[chordAMajorLen] = { 0 };
          int randomTone = soundsArray[random(chordLen)];
          int correctTone = soundsArray[random(chordLen)];

          while (activePadCount > 1) {
            padsComm->waitWithEventChecks(3000);

            // play all possible sounds so players know
            {
              int soundLenArray[paramLen] = { 500, 500, 500, 500, 0, 0, 0, 0 };
              padsComm->play8Sounds(soundsArray, soundLenArray);
              padsComm->waitWithEventChecks(3000);
            }

            // play correct sound
            //shuffle(shuffledToneOrder, chordAMajorLen);
            //memcpy(&correctToneOrder, &shuffledToneOrder, sizeof(correctToneOrder));
            padsComm->playSingleSound(correctTone, 500);
            padsComm->waitWithEventChecks(3000);
            /*while (memcmp(shuffledToneOrder, correctToneOrder, sizeof(shuffledToneOrder)) == 0) {
            shuffle(shuffledToneOrder, chordAMajorLen);
          }*/

            // initial shuffle
            while (randomTone == correctTone) {
              randomTone = soundsArray[random(4)];
            }

            /*while (memcmp(shuffledToneOrder, correctToneOrder, sizeof(shuffledToneOrder)) != 0) {
          shuffle(shuffledToneOrder, chordAMajorLen);
          if (padsComm->play8Sounds(shuffledToneOrder, soundLenArray) == PadsComm::WaitResult::CANCEL_GAME) return;
        }*/

            padsComm->playSingleSound(randomTone, 500);

            do {
              padsComm->waitWithEventChecks(3000);
              randomTone = soundsArray[random(4)];
              padsComm->playSingleSound(randomTone, 500);
            } while (randomTone != correctTone);

            Serial.println("Correct tone played, waiting for player on pads");

            PadsComm::WaitResult ret = padsComm->waitForPlayerOnAnyPad();
            if (ret == PadsComm::WaitResult::Timeout) {
              for (int i = 0; i < maxAllowedPads; i++) {
                if ((*padsComm->getPad(i)).isOccupied) {
                  padsComm->playWinnerJingle(i);
                }
              }
            }

            activePadCount--;

            /*Serial.println("Auswertung");
          // play correct for all except winner and loser
          for (int i = 1; i < activePadCount - 1; i++) {
            uint8_t padIndex = eventOrder[i];
            if (playCorrectActionJingle(padIndex) == PadsComm::WaitResult::CANCEL_GAME) return;
          }

          // play winner and loser hingle
          uint8_t winnerPad = eventOrder[0];
          uint8_t loserPad = eventOrder[activePadCount - 1];

          if (playWinnerJingle(winnerPad) == PadsComm::WaitResult::CANCEL_GAME) return;
          if (playWrongActionJingle(loserPad) == PadsComm::WaitResult::CANCEL_GAME) return;

          // remove loser from round
          padsArray[loserPad].isActive = false;
          activePadCount--;

          for (int i = 0; i < maxAllowedPads; i++) {
            eventOrder[i] = 0;
          }*/
          }

          padsComm->playCorrectActionJingle();
          Serial.println("---Reaktion End---");
          break;
        }
      case phoneInput_tokenize:
        {
          char code[maxCharsAllowed];  // = "var something if(13 + 4 / (7*300) + foo * bar)";

          String codeAsStr = Serial.readStringUntil('\n');

          strncpy(code, codeAsStr.c_str(), maxCharsAllowed);

          Serial.println(code);

          std::queue<Lexer::Token> tokens = lexer.tokenize(code, sizeof(code));

          Serial.println("Tokenized!");

          while (!tokens.empty()) {
            Lexer::Token token = tokens.front();
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
              case Lexer::TokenType::OpenBrace:
                Serial.println("OpenBrace");
                break;
              case Lexer::TokenType::CloseBrace:
                Serial.println("CloseBrace");
                break;
              case Lexer::TokenType::Number:
                Serial.println("Number");
                break;
              case Lexer::TokenType::Semicolon:
                Serial.println("Semicolon");
                break;
              case Lexer::TokenType::Comma:
                Serial.println("Colon");
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
          char code[maxCharsAllowed] = "var something if(13 + 4 / (7*300) + foo * bar)";

          Parser::Program *program = parser.produceAST(code, sizeof(code));

          if (program->kind == Parser::NodeType::Program)
            Serial.println("Program parsed successfully!");

          toString(program);
          break;
        }
      case phoneInput_interpret:
        {
          GlobalFunctions::printMemoryStats("before program");

          Environment env;

          Serial.println("Ready to interpret!");

          while (true) {
            if (Serial.available() > 0) {
              String code = Serial.readStringUntil('\n');

              if(code.equals("exit")) {
                break;
              }


              Serial.println("Got string: ");
              Serial.println(code);

              char* code_cstr = new char[code.length() + 1];
              code.toCharArray(code_cstr, code.length() + 1, 0);

              Parser::Program *program = parser.produceAST(code_cstr, code.length() + 1);
              toString(program);
              Serial.println("Successfully parsed program");
              GlobalFunctions::printMemoryStats("after AST printing");
              ESP.wdtFeed();

              Values::RuntimeVal *val = interpreter.evaluate(program, &env);
              GlobalFunctions::printMemoryStats("after evaluation");
              ESP.wdtFeed();


              Serial.print("Value: ");
              switch (val->type) {
                case Values::ValueType::Null:
                  Serial.println("Null");
                  break;
                case Values::ValueType::Boolean:
                  {
                    Serial.print("Boolean: ");
                    Values::BooleanVal *boolVal = static_cast<Values::BooleanVal *>(val);
                    Serial.println(boolVal->value);
                    break;
                  }
                case Values::ValueType::Number:
                  {
                    Serial.print("Number: ");
                    Values::NumberVal *numVal = static_cast<Values::NumberVal *>(val);
                    Serial.println(numVal->value);
                    break;
                  }
                case Values::ValueType::NativeFn:
                  Serial.println("NativeFn");
                  break;
                default:
                  GlobalFunctions::restart("Undefined ValueType");
                  break;
              }

              delete[] code_cstr;

              GlobalFunctions::printMemoryStats("after program");
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
