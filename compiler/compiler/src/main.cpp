// -------------------INCLUDE-----------------------
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <espnow.h>

#include "Constants.h"
#include "comm/PadsComm.h"
#include "comm/BLEComm.h"

enum class ProgramState {
  Idle,
  PlayingReaktion,
  PlayingMemory,
  Abort
};

PadsComm *padsComm = PadsComm::getInstance();
BLEComm *btComm = BLEComm::getInstance();

ProgramState programState = ProgramState::Idle;

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

void printWithMac(const char *msg, uint8_t *mac) {
  Serial.print(msg);
  Serial.write(' ');
  for (int i = 0; i < 6; i++) {
    Serial.print(mac[i], HEX);
    if (i < 5)
      Serial.print(":");
    else
      Serial.print("\n");
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

// ----------------MAIN FUNCTIONS-----------------
void setup() {
  // init connections
  Serial.begin(115200);

  padsComm->initEspNow(OnDataSent, OnDataRecv);

  // random seed
  randomSeed(analogRead(0));

  programState = ProgramState::Idle;
}

void loop() {
  switch (programState) {
    case ProgramState::Idle:
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
            programState = ProgramState::PlayingMemory;
            break;
          case phoneInput_gameSelection_Reaktion:
            programState = ProgramState::PlayingReaktion;
            break;
          default:
            break;
        }
      }
      break;
    case ProgramState::PlayingMemory:
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
            if (padsComm->waitForPlayerOnAnyPad() == PadsComm::WaitResult::CancelGame) return;


            // if not on the correct pad
            if ((*padsComm->getPad(selectedPad)).isOccupied) {
              padsComm->playLoserJingle();
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
              if (padsComm->waitForPlayerOnAnyPad() == PadsComm::WaitResult::CancelGame) return;

              // if not on the correct pad
              PadsComm::Pad *pad = padsComm->getPad(selectedPad);

              if (!(*pad).isOccupied) {
                if (padsComm->playWrongActionJingle() == PadsComm::WaitResult::CancelGame) return;
                else break;
              } else /* Correct pad */ {
                if (correctSelectionsCount >= soundSeqLen) {
                  padsComm->playWinnerJingle();
                  return;
                } else {
                  PadsComm::WaitResult ret = padsComm->playCorrectActionJingle();
                  if (ret == PadsComm::WaitResult::CancelGame) return;
                }
              }
            }
          }
        }

        Serial.println("Memory end");
        programState = ProgramState::Idle;
        break;
      }
    case ProgramState::PlayingReaktion:
      {
        Serial.println("---Reaktion selected---");

        activePadCount = 2;
        //int shuffledToneOrder[chordAMajorLen] = { 0 };
        //memcpy(&shuffledToneOrder, &soundsArray, sizeof(shuffledToneOrder));
        //int correctToneOrder[chordAMajorLen] = { 0 };
        int randomTone = soundsArray[random(chordLen)];
        int correctTone = soundsArray[random(chordLen)];

        while (activePadCount > 1) {
          if (padsComm->waitWithEventChecks(3000) == PadsComm::WaitResult::CancelGame) return;

          // play all possible sounds so players know
          {
            int soundLenArray[paramLen] = { 500, 500, 500, 500, 0, 0, 0, 0 };
            if (padsComm->play8Sounds(soundsArray, soundLenArray) == PadsComm::WaitResult::CancelGame) return;
            if (padsComm->waitWithEventChecks(3000) == PadsComm::WaitResult::CancelGame) return;
          }

          // play correct sound
          //shuffle(shuffledToneOrder, chordAMajorLen);
          //memcpy(&correctToneOrder, &shuffledToneOrder, sizeof(correctToneOrder));
          if (padsComm->playSingleSound(correctTone, 500) == PadsComm::WaitResult::CancelGame) return;
          if (padsComm->waitWithEventChecks(3000) == PadsComm::WaitResult::CancelGame) return;
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

          if (padsComm->playSingleSound(randomTone, 500) == PadsComm::WaitResult::CancelGame) return;

          do {
            if (padsComm->waitWithEventChecks(3000) == PadsComm::WaitResult::CancelGame) return;
            randomTone = soundsArray[random(4)];
            if (padsComm->playSingleSound(randomTone, 500) == PadsComm::WaitResult::CancelGame) return;
          } while (randomTone != correctTone);

          Serial.println("Correct tone played, waiting for player on pads");

          PadsComm::WaitResult ret = padsComm->waitForPlayerOnAnyPad();
          if (ret == PadsComm::WaitResult::CancelGame) {
            Serial.println("canceled game");
            padsComm->playLoserJingle();
            return;
          } else if (ret == PadsComm::WaitResult::Timeout) {
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
        programState = ProgramState::Idle;
        break;
      }
    case ProgramState::Abort:
      Serial.println("Aborting...");
      programState = ProgramState::Idle;
      break;
  }
}