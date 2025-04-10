#ifndef UNIT_TEST

#include <Arduino.h>

#include "Constants.h"
#include "ErrorHandler.h"
#include "PadsComm.h"
#include "BLEComm.h"
#include "Parser.h"
#include "Interpreter.h"

PadsComm *padsComm = PadsComm::getInstance();
BLEComm *btComm = BLEComm::getInstance();

uint8_t activePadCount = 0;

typedef struct GameConfig {
  String name = "";
  String uid = "0";
  uint8_t numberOfPads = 0;
  bool endless = false;
  String soundType = "";
} GameConfig;

GameConfig gameConfig;

void parseKeyValuePair(String pair) {
  Serial.println("parseKeyValuePair");
  // Find the position of the '=' symbol
  int equalsIdx = pair.indexOf('=');
  if (equalsIdx == -1) {
    return;  // Invalid key-value pair, skip it
  }

  String key = pair.substring(0, equalsIdx);
  String value = pair.substring(equalsIdx + 1);

  // Trim leading and trailing spaces
  key.trim();
  value.trim();

  // Handle each key-value pair and assign values to the gameConfig structure
  if (key.equals("name")) {
    gameConfig.name = value;
  } else if (key.equals("uid")) {
    gameConfig.uid = value;
  } else if (key.equals("Number of Pads:")) {
    gameConfig.numberOfPads = value.toInt();
  } else if (key.equals("Endless?")) {
    gameConfig.endless = (value == "true");
  } else if (key.equals("Sound type:")) {
    gameConfig.soundType = value;
  }
}

void parseConfig(String configString) {
  // Split the string by semicolons to get each key-value pair
  int startIdx = 0;
  int endIdx = configString.indexOf(';');

  Serial.print("\"");
  Serial.print(configString);
  Serial.println("\"");

  while (endIdx != -1) {
    String pair = configString.substring(startIdx, endIdx);
    Serial.print("Pair: ");
    Serial.println(pair);
    parseKeyValuePair(pair);

    startIdx = endIdx + 1;
    endIdx = configString.indexOf(';', startIdx);
  }

  // Parse the last pair (after the final semicolon)
  String lastPair = configString.substring(startIdx);
  parseKeyValuePair(lastPair);
}

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

  padsComm->gotMessageFromPad(mac, incomingData);
}


// ----------------MAIN FUNCTIONS-----------------
void setup() {
  // init connections
  Serial.begin(115200);

  // debugging
  delay(500);
  Serial.setDebugOutput(true);

  Serial.print("\nReset reason: ");
  Serial.println(ESP.getResetReason());
  Serial.println(ESP.getResetInfo());

  // ESP-NOW
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
      // all hex codes from here on out are undocumented and just for testing purposes
      case 0x05:
        padsComm->play8Sounds(soundsArray, correctActionDurations);
        break;
      case 0x06:
        padsComm->playWinnerJingle();
        padsComm->waitWithCancelCheck(3000);
        padsComm->playLoserJingle();
        padsComm->waitWithCancelCheck(3000);
        padsComm->playCorrectActionJingle();
        padsComm->waitWithCancelCheck(3000);
        padsComm->playWrongActionJingle();
        break;
      case 0x07:
        padsComm->waitForPlayerOnAnyPad();
        if (padsComm->isPadOccupied(0)) {
          padsComm->playWinnerJingle();
        } else {
          padsComm->playLoserJingle();
        }
        break;
      case 0x08:
        padsComm->waitForPlayersOnAllActivePads();
        if (padsComm->isPadOccupied(0) && padsComm->isPadOccupied(1) && padsComm->isPadOccupied(2)) {
          padsComm->playWinnerJingle();
        } else {
          padsComm->playLoserJingle();
        }
        break;
      case 0x09:
        padsComm->waitForPlayerOnAnyPad();
        padsComm->playLoserJingle();
        break;
      case phoneInput_gameSelection_Memory:
        {
          Serial.println("Memory start");

          bool easyMemory = false;

          if (easyMemory) {
            Serial.println("Playing easy memory");
            uint8_t selectedPad = anyPad;
            uint8_t selectedLastRound = anyPad;

            while (true) {
              // select random pad and deactivate for next round
              while (selectedPad == selectedLastRound) {
                selectedPad = random(padsCount);
              }
              selectedLastRound = selectedPad;
              Serial.print("Selected correct pad: ");
              Serial.println(selectedPad, DEC);

              // wait a bit to avoid confusion
              padsComm->waitWithCancelCheck(1000);

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

            for (size_t currentRound = 0; currentRound < maxRounds; currentRound++) {
              soundSeq[currentRound] = random(padsCount);

              // play sound sequence on pads in order
              Serial.print("Pad order: [");
              for (size_t j = 0; j <= currentRound; j++) {
                uint8_t padIndex = soundSeq[j];

                Serial.print(padIndex);
                Serial.print(j + 1 <= currentRound ? "," : "]\n");

                padsComm->playSingleSound(soundsArray[padIndex], 1000, padIndex);
                padsComm->waitWithCancelCheck(500);
              }

              // check for correct pads
              for (size_t j = 0; j <= currentRound; j++) {
                padsComm->waitForPlayerOnAnyPad();

                if (!padsComm->isPadOccupied(soundSeq[j])) {
                  padsComm->playLoserJingle();
                  return;
                } else /* Correct pad */ {
                  padsComm->playCorrectActionJingle();
                  padsComm->waitWithCancelCheck(1000);
                }
              }
            }

            padsComm->playWinnerJingle();
          }

          Serial.println("Memory end");
          break;
        }
      case phoneInput_gameSelection_Reaktion:
        {
          Serial.println("Reaktion start");

          while (true) {
            // countdown
            padsComm->play8Sounds(countdownTones, countdownLen);
            padsComm->waitWithCancelCheck(random(2000, 5000));  // wait between 2-5 seconds
            padsComm->playSingleSound(880, 250);                // starting signal (JUMP ON PAD AS FAST AS POSSIBLE)
            std::variant<int, PadsComm::WaitResult> firstOccupiedIndex = padsComm->waitForPlayerOnAnyPad();

            if (std::holds_alternative<PadsComm::WaitResult>(firstOccupiedIndex)) {
              switch (std::get<PadsComm::WaitResult>(firstOccupiedIndex)) {
                case PadsComm::WaitResult::Timeout:
                  Serial.println("No pad occupied, returning...");
                  break;
                case PadsComm::WaitResult::UserAbort:
                  Serial.println("User aborted, returning...");
                  break;
                case PadsComm::WaitResult::PadMsgDeliveryError:
                  Serial.println("Error occurred sending message to pad, returning...");
                  break;
                case PadsComm::WaitResult::PadOccupied:
                  Serial.println("Pad occupied, but no index returned (should not happen), returning...");
                  break;
              }
              padsComm->playLoserJingle();
              break;
            } else {
              padsComm->playCorrectActionJingle(std::get<int>(firstOccupiedIndex));
              padsComm->waitWithCancelCheck(1000);
            }
          }
          Serial.println("Reaktion End");
          break;
        }
      case phoneInput_interpret:
        {
          Parser parser;
          Interpreter interpreter;
          Environment env;
          bool configProvided = false;
          String code;
          Serial.println("\nReady to interpret!");

          while (true) {
            if (btComm->hasUnreadBytes()) {

              String currentInput = btComm->readCode();

              if (currentInput.startsWith("AT") || currentInput.startsWith("OK")) {
                Serial.print("Got HC-05 code ");
                Serial.println(currentInput);
                if (currentInput.equals("OK+LOST")) {
                  Serial.println("Exiting interpreter...");
                  break;
                }
                continue;
              }

              if (currentInput.equalsIgnoreCase("exit")) {
                break;
              }

              Serial.println("Got string: ");
              Serial.println(currentInput);

              if (!configProvided) {
                Serial.println("Setting gameConfig...");
                parseConfig(currentInput);
                Serial.println("Parsed config");
                currentInput = "";
                configProvided = true;
                continue;
              }

              Serial.println("Parsing code...");

              if (!currentInput.endsWith("EOF")) {
                Serial.println("Code not finished yet, waiting for EOF...");
                code += currentInput;
                continue;
              } else {
                currentInput.remove(currentInput.length() - 3);  // remove EOF
                Serial.println("Code finished, removing EOF...");
              }

              char *code_cstr = new char[currentInput.length() + 1];
              currentInput.toCharArray(code_cstr, currentInput.length() + 1, 0);
              AstNodes::Program *program = parser.produceAST(code_cstr, currentInput.length() + 1);
              yield();
              parser.printAST(program);
              yield();
              interpreter.evaluate(program, &env);

              delete[] code_cstr;

              break;
            }
          }

          gameConfig.uid = "0";
          gameConfig.name = "";
          gameConfig.numberOfPads = 0;
          gameConfig.endless = false;
          gameConfig.soundType = "";

          btComm->sendGameEnded();
          Serial.println("Exiting...");
        }
        break;
      default:
        break;
    }
  }
}

#endif