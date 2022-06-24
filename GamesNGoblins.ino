//Claudio Belato
//02/06/2022
#include <Arduboy2.h>
#include <time.h>
#include <EEPROM.h>
#include <ArduboyTones.h>
#include "constants.h"

Arduboy2 arduboy;
ArduboyTones sound(arduboy.audio.enabled);

struct HighScore {
  char flag;
  char initials[3];
  int points;
};

HighScore top5[5];
int randomPart;
int randomFace;
int buttonPressed = -1;
int lives;
int score = 0;
unsigned long now;
int wrongMove = 0;
int gamePhase = 0;
int firstRowofClouds = 0;
int secondRowofClouds = 0;
int lightning = 0;
int timerSize = MAX_TIMER;
int isSoundOn = 1;
int minScore = 0;
int leaderBoardCalculated = 0;
int newHighScoreIndex = 0;
int letterPosition = 0;
char initials[3] = "AAA";

//order: left, up, right, down
//four boards represented by this array, -1 means empty
int board[16];

void setup() {
  arduboy.begin();
  arduboy.clear();
  arduboy.initRandomSeed();
  minScore = initializeEeprom();
}

void loop() {
  if (gamePhase == TITLE_SCREEN) {
    arduboy.clear();
    arduboy.pollButtons();

    //draw two layers of clouds
    Sprites::drawOverwrite((firstRowofClouds % 144) - 15, 8, cloud, 0);
    Sprites::drawOverwrite(((firstRowofClouds + 12) % 144) - 15, 7, cloud, 0);
    Sprites::drawOverwrite(((firstRowofClouds + 30) % 144) - 15, 9, cloud, 0);
    Sprites::drawOverwrite(((firstRowofClouds + 42) % 144) - 15, 7, cloud, 0);
    Sprites::drawOverwrite(((firstRowofClouds + 60) % 144) - 15, 8, cloud, 0);
    Sprites::drawOverwrite(((firstRowofClouds + 100) % 144) - 15, 7, cloud, 0);
    Sprites::drawOverwrite(((firstRowofClouds + 115) % 144) - 15, 8, cloud, 0);
    Sprites::drawOverwrite(((firstRowofClouds + 122) % 144) - 15, 9, cloud, 0);

    Sprites::drawOverwrite((secondRowofClouds % 144 + 4) - 15, 12, cloud, 0);
    Sprites::drawOverwrite(((secondRowofClouds + 17) % 144) - 15, 13, cloud, 0);
    Sprites::drawOverwrite(((secondRowofClouds + 28) % 144) - 15, 11, cloud, 0);
    Sprites::drawOverwrite(((secondRowofClouds + 45) % 144) - 15, 14, cloud, 0);
    Sprites::drawOverwrite(((secondRowofClouds + 57) % 144) - 15, 15, cloud, 0);
    Sprites::drawOverwrite(((secondRowofClouds + 71) % 144) - 15, 11, cloud, 0);
    Sprites::drawOverwrite(((secondRowofClouds + 84) % 144) - 15, 12, cloud, 0);
    Sprites::drawOverwrite(((secondRowofClouds + 99) % 144) - 15, 11, cloud, 0);
    Sprites::drawOverwrite(((secondRowofClouds + 110) % 144) - 15, 10, cloud, 0);

    //draw the moon
    arduboy.drawCircle(15, 9, 9);

    //move clouds
    secondRowofClouds += 2;
    firstRowofClouds++;

    //lightning and thunder effects
    if (random(0, 40) == 0) {
      for (int i = 0; i <= random(1, 4); i++) {
        arduboy.invert(++lightning % 2);
        delay(100);
        arduboy.invert(++lightning % 2);
        delay(100);
      }
      int thunderSound = random(35, 45);
      int thunderLength = random(100, 300);
      if (isSoundOn) {
        sound.tone(thunderSound, 50, thunderSound - 10, thunderLength - 50);
      }
    }

    //slow down the cloud movement (the dumb way)
    delay(100);

    arduboy.setCursor(16, 24);
    arduboy.print("Games'N Goblins");
    arduboy.setCursor(40, 34);
    arduboy.print("The Game");
    arduboy.setCursor(10, 56);
    arduboy.print("A: Start");
    drawSoundBox();

    if (arduboy.justPressed(A_BUTTON)) {
      if (isSoundOn) {
        sound.tonesInRAM(jingle);
      }
      arduboy.clear();
      arduboy.setCursor(30, 25);
      arduboy.print("GAME START!");
      arduboy.display();
      delay(3600);
      applyNewGameSettings();
    }
    if (arduboy.justPressed(B_BUTTON)) {
      isSoundOn = (isSoundOn + 1) % 2;
    }
    arduboy.display();
  }
  else if (gamePhase == GAME_SCREEN) {

    arduboy.clear();
    arduboy.pollButtons();

    //*************************************************************************
    //***************************GAME LOGIC************************************
    //*************************************************************************
    checkForClearedFaces();

    if (millis() >= now + timerSize) {
      wrongMove = 1;
      now = millis();
    }

    if (arduboy.justPressed(LEFT_BUTTON)) {
      buttonPressed = 0;
      checkInput();
    }
    if (arduboy.justPressed(UP_BUTTON)) {
      buttonPressed = 1;
      checkInput();
    }
    if (arduboy.justPressed(RIGHT_BUTTON)) {
      buttonPressed = 2;
      checkInput();
    }
    if (arduboy.justPressed(DOWN_BUTTON)) {
      buttonPressed = 3;
      checkInput();
    }

    if (wrongMove) {
      lives--;
      wrongMove = 0;

      if (isSoundOn) {
        if (lives > 0) {
          sound.tone(500, 200, 500, 200, 500, 200);
        }
        else {
          sound.tone(500, 200, 500, 200, 200, 400);
        }
      }

      //check if score is eligible for a Highscore
      if (lives <= 0) {
        if (score <= minScore) {
          gamePhase = GAME_OVER_SCREEN;
        }
        else {
          gamePhase = HIGH_SCORE_SCREEN;
          leaderBoardCalculated = 0;
        }
      }

      timerSize = timerSize <= 4500 ? timerSize + HALF_OF_A_SECOND : MAX_TIMER;
      blinkScreen();
    }

    //*************************************************************************
    //***************************PRINTING SECTION******************************
    //*************************************************************************

    //print all faces
    for (int i = 0; i < 16; i++) {
      if (board[i] != EMPTY_CELL) {
        printFacePartOnPosition( xCoordinates[i], yCoordinates[i], board[i], i % 4);
      }
    }

    printFacePartOnPosition( 27, 27, randomFace, randomPart);

    //draw remaining lives
    if (lives == 3) {
      Sprites::drawOverwrite(70, 0, fullHeart, 0);
      Sprites::drawOverwrite(90, 0, fullHeart, 0);
      Sprites::drawOverwrite(110, 0, fullHeart, 0);

    }
    else if (lives == 2) {
      Sprites::drawOverwrite(70, 0, fullHeart, 0);
      Sprites::drawOverwrite(90, 0, fullHeart, 0);
      Sprites::drawOverwrite(110, 0, emptyHeart, 0);
    }
    else if (lives == 1) {
      Sprites::drawOverwrite(70, 0, fullHeart, 0);
      Sprites::drawOverwrite(90, 0, emptyHeart, 0);
      Sprites::drawOverwrite(110, 0, emptyHeart, 0);
    }

    //draw background
    arduboy.drawRect(21, 0, 22, 22);
    arduboy.drawRect(0, 21, 22, 22);
    arduboy.drawRect(42, 21, 22, 22);
    arduboy.drawRect(21, 42, 22, 22);

    //print score
    arduboy.drawRect(70, 30, 55, 10);
    arduboy.setCursor(75, 30);
    arduboy.print(score);

    //print timer
    arduboy.drawRect(50, 50, 70, 10);
    arduboy.fillRect(50, 50, (now + timerSize - millis()) / 70, 10);

    arduboy.display();
  }
  else if (gamePhase == GAME_OVER_SCREEN) {

    arduboy.clear();
    arduboy.pollButtons();

    arduboy.setCursor(20, 0);
    arduboy.print("GAME OVER");
    arduboy.setCursor(40, 10);
    arduboy.print("Play again");
    arduboy.setCursor(20, 30);
    arduboy.print("Your score is:");
    arduboy.print(score);
    arduboy.setCursor(10, 56);
    arduboy.print("A: Retry!");
    drawSoundBox();

    if (arduboy.justPressed(A_BUTTON)) {
      applyNewGameSettings();
    }

    if (arduboy.justPressed(B_BUTTON)) {
      isSoundOn = (isSoundOn + 1) % 2;
    }

    arduboy.display();
  }
  else if (gamePhase == HIGH_SCORE_SCREEN) {

    //find correct place for new highscore
    if (!leaderBoardCalculated) {
      while (top5[newHighScoreIndex].points > score) {
        newHighScoreIndex++;
      }

      for (int i = 4; i > newHighScoreIndex; i--) {
        top5[i] = top5[i - 1];
      }

      top5[newHighScoreIndex].points = score;
      leaderBoardCalculated = 1;
    }

    arduboy.clear();
    arduboy.pollButtons();
    arduboy.setCursor(30, 0);
    arduboy.print("NAME  SCORE");

    //print highscores
    for (int i = 0; i < 5; i++) {
      arduboy.setCursor(10, 15 + (10 * i));
      arduboy.print(i + 1);
      if (i != newHighScoreIndex) {
        for (int j = 0; j < 3; j++) {
          arduboy.setCursor(30 + (j * 6), 15 + (10 * i));
          arduboy.print(top5[i].initials[j]);
        }
      }
      arduboy.setCursor(67, 15 + (10 * i));
      arduboy.print(top5[i].points);
    }

    for (int i = 0; i < 3; i++) {
      arduboy.setCursor(30 + (i * 6), 15 + (10 * newHighScoreIndex));
      arduboy.print(initials[i]);
    }

    //draw cursor
    arduboy.drawLine(30 + (letterPosition * 6), 23 + (10 * newHighScoreIndex), 34 + (letterPosition * 6), 23 + (10 * newHighScoreIndex));
    
    drawSoundBox();

    if (arduboy.justPressed(UP_BUTTON)) {
      initials[letterPosition] = initials[letterPosition] < Z_ASCII_VALUE ? initials[letterPosition] + 1 : A_ASCII_VALUE;

    }
    if (arduboy.justPressed(DOWN_BUTTON)) {
      initials[letterPosition] = initials[letterPosition] > A_ASCII_VALUE ? initials[letterPosition] - 1 : Z_ASCII_VALUE;
    }
    if (arduboy.justPressed(LEFT_BUTTON)) {
      if (letterPosition > 0) {
        letterPosition--;
      }
    }
    if (arduboy.justPressed(RIGHT_BUTTON)) {
      if (letterPosition < 2) {
        letterPosition++;
      }
    }

    if (arduboy.justPressed(A_BUTTON)) {

      //update highscores in memory and play another game
      top5[newHighScoreIndex].initials[0] = initials[0];
      top5[newHighScoreIndex].initials[1] = initials[1];
      top5[newHighScoreIndex].initials[2] = initials[2];

      for (int i = 0; i < 5; i++) {
        EEPROM.put((i * sizeof(HighScore)) + SCORE_MEMORY_LOCATION, top5[i]);
      }

      minScore = top5[4].points;
      letterPosition = 0;
      newHighScoreIndex = 0;
      initials[0] = 'A';
      initials[1] = 'A';
      initials[2] = 'A';
      applyNewGameSettings();
    }

    if (arduboy.justPressed(B_BUTTON)) {
      isSoundOn = (isSoundOn + 1) % 2;
    }
    arduboy.display();
  }
}

void drawSoundBox() {
  arduboy.setCursor(95, 56);
  arduboy.print("B:");
  if (isSoundOn) {
    Sprites::drawOverwrite(110, 55, speakerOn, 0);
  }
  else {
    Sprites::drawOverwrite(110, 55, speakerOff, 0);
  }
}


void applyNewGameSettings() {
  clearBoard();
  lives = MAX_LIVES;
  gamePhase = GAME_SCREEN;
  score = 0;
  timerSize = MAX_TIMER;
  now = millis();
}

void clearBoard() {
  for (int i = 0; i < 16; i++) {
    board[i] = EMPTY_CELL;
  }
}

//generate a random piece who can be fitted in the empty spots
int generateRandomPart() {
  int rnd = 0;
  while (true) {
    rnd = random(0, 4);
    for (int i = rnd; i < 16 ; i += 4) {
      if (board[i] == EMPTY_CELL) {
        return rnd;
      }
    }
  }
  return  rnd;
}

void printFacePartOnPosition(int x, int y, int face, int part ) {
  if (face == 0) {
    if (part == 0) {
      Sprites::drawOverwrite(x, y, ogreTopLeft, 0);
    }
    else if (part == 1) {
      Sprites::drawOverwrite(x, y, ogreTopRight, 0);
    }
    else if (part == 2) {
      Sprites::drawOverwrite(x, y, ogreBottomLeft, 0);
    }
    else if (part == 3) {
      Sprites::drawOverwrite(x, y, ogreBottomRight, 0);
    }
  }
  else if (face == 1) {
    if (part == 0) {
      Sprites::drawOverwrite(x, y, goblinTopLeft, 0);
    }
    else if (part == 1) {
      Sprites::drawOverwrite(x, y, goblinTopRight, 0);
    }
    else if (part == 2) {
      Sprites::drawOverwrite(x, y, goblinBottomLeft, 0);
    }
    else if (part == 3) {
      Sprites::drawOverwrite(x, y, goblinBottomRight, 0);
    }
  }
  else if (face == 2) {
    if (part == 0) {
      Sprites::drawOverwrite(x, y, tsugoTopLeft, 0);
    }
    else if (part == 1) {
      Sprites::drawOverwrite(x, y, tsugoTopRight, 0);
    }
    else if (part == 2) {
      Sprites::drawOverwrite(x, y, tsugoBottomLeft, 0);
    }
    else if (part == 3) {
      Sprites::drawOverwrite(x, y, tsugoBottomRight, 0);
    }
  }
  else if (face == 3) {
    if (part == 0) {
      Sprites::drawOverwrite(x, y, maskTopLeft, 0);
    }
    else if (part == 1) {
      Sprites::drawOverwrite(x, y, maskTopRight, 0);
    }
    else if (part == 2) {
      Sprites::drawOverwrite(x, y, maskBottomLeft, 0);
    }
    else if (part == 3) {
      Sprites::drawOverwrite(x, y, maskBottomRight, 0);
    }
  }
}

void checkInput() {
  now = millis();
  if (board[(4 * buttonPressed) + randomPart] == EMPTY_CELL) {
    board[(4 * buttonPressed) + randomPart] = randomFace;
    if (isSoundOn) {
      sound.tone(1000, 50);
    }
  }
  else {
    wrongMove = 1;
  }
  randomFace = random(0, 4);
  randomPart = generateRandomPart();
  buttonPressed = -1;
}

//initialize Eeprom if dirty and retrieve lowest high score
int initializeEeprom() {
  HighScore score;
  for (int i = 0; i < 5; i++) {
    EEPROM.get((i * sizeof(HighScore)) + SCORE_MEMORY_LOCATION, score);
    if (score.flag != MEMORY_OK_FLAG) {
      score.flag = MEMORY_OK_FLAG;
      score.initials[0] = 'A';
      score.initials[1] = 'A';
      score.initials[2] = 'A';
      score.points = 0;
      EEPROM.put((i * sizeof(HighScore)) + SCORE_MEMORY_LOCATION, score);
    }
    top5[i] = score;
    if (i == 4) {
      return score.points;
    }

  }
}

//check if a cell contains all 4 pieces of a face
void checkForClearedFaces() {

  int checkAllClear = 0;

  for (int i = 0; i <= 12; i += 4) {
    if (board[i + 0] != EMPTY_CELL && board[i + 1] != EMPTY_CELL && board[i + 2] != EMPTY_CELL && board[i + 3] != EMPTY_CELL) {

      checkAllClear = 1;

      if (board[i + 0] == board[i + 1] && board[i + 0] == board[i + 2] && board[i + 0] == board[i + 3]) {
        score += THREE_HOUNDRED_POINTS;
      }
      else if ((board[i + 0] == board[i + 1] && board[i + 0] == board[i + 2]) || (board[i + 0] == board[i + 1] && board[i + 0] == board[i + 3]) || (board[i + 0] == board[i + 2] && board[i + 0] == board[i + 3]) || (board[i + 1] == board[i + 2] && board[i + 1] == board[i + 3])) {
        score += ONE_HOUNDRED_POINTS;
      }
      else if ((board[i + 0] == board[i + 1]) || (board[i + 0] == board[i + 2]) || (board[i + 0] == board[i + 3]) || (board[i + 1] == board[i + 2]) || (board[i + 1] == board[i + 3]) || (board[i + 2] == board[i + 3])) {
        score += FOURTY_POINTS;
      }
      else {
        score += TEN_POINTS;
      }
      board[i + 0] = EMPTY_CELL;
      board[i + 1] = EMPTY_CELL;
      board[i + 2] = EMPTY_CELL;
      board[i + 3] = EMPTY_CELL;

      if (timerSize < ONE_SECOND) {
        timerSize = ONE_SECOND;
      }
      else {
        timerSize -= TENTH_OF_A_SECOND;
      }
      now = millis();
    }
  }

  if (checkAllClear) {
    int allClear = 1;
    checkAllClear = 0;
    for (int i = 0; i < 16; i++) {
      if (board[i] != EMPTY_CELL) {
        allClear = 0;
        break;
      }
    }
    if (allClear) {
      if (lives < MAX_LIVES) {
        lives++;
      }
      score += FIVE_HOUNDRED_POINTS;
      if (isSoundOn) {
        sound.tone(1200, 200, 1500, 200, 1200, 200);
      }
    }
    else {
      if (isSoundOn) {
        sound.tone(1200, 200, 1200, 200, 1200, 200);
      }
    }

    blinkScreen();
  }
}

void blinkScreen() {
  for (int i = 1; i <= 6; i++) {
    arduboy.invert(i % 2);
    delay(TENTH_OF_A_SECOND);
  }
}
