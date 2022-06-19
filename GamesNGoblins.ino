//Claudio Belato
//02/06/2022
#include <Arduboy2.h>
#include <time.h>
#include <EEPROM.h>
#include <ArduboyTones.h>

Arduboy2 arduboy;

struct HighScore {
  char flag;
  int initials[3];
  int score;
};

int randomPart = 0;
int randomFace = 0;
int buttonPressed = -1;
int lives = 3;
int score = 0;
unsigned long now = 0;
int wrongMove = 0;
int gamePhase = 0;
int firstRowofClouds = 0;
int secondRowofClouds = 0;
int lightning = 0;
int timerSize = 5000;
ArduboyTones sound(arduboy.audio.enabled);
int toggleSound = 1;
HighScore scores[5];
int minScore = 0;
int leaderBoardCalculated = 0;
int scoreIndex = 0;
int letterPosition = 0;
int initials[3] = {65, 65, 65};



//ordine sinistra sopra destra sotto
//i quattro slot delle quattro facce sono rappresentate con un array di interi, -1 equivale a vuoto
int board[16] = {
  -1, -1, -1, -1,
  -1, -1, -1, -1,
  -1, -1, -1, -1,
  -1, -1, -1, -1
};


int xCoordinates[16] = {3, 11, 3, 11, 24, 32, 24, 32, 45, 53, 45, 53, 24, 32, 24, 32};
int yCoordinates[16] = {24, 24, 32, 32, 3, 3, 11, 11, 24, 24, 32, 32, 45, 45, 53, 53};

//posizione centro 27 27
//posizioni casella sinistra : 3/24 11/24 3/32 11/32
//posizioni casella sopra : 24/3 32/3 24/11 32/11
//posizioni casella destra : 45/24 53/24 45/32 53/32
//posizioni casella sotto : 24/45 32/45 24/53 32/53


//ogre = 0, goblin = 1, skeleton = 2, ghost = 3

const uint8_t PROGMEM ogreTopLeft[] = {
  8, 8,
  0xc0, 0x78, 0x0c, 0x04, 0x96, 0x92, 0xa3, 0x01,
};

const uint8_t PROGMEM ogreTopRight[] = {
  8, 8,
  0x01, 0xa3, 0x92, 0x96, 0x04, 0x0c, 0x78, 0xc0,
};

const uint8_t PROGMEM ogreBottomLeft[] = {
  8, 8,
  0x03, 0x3e, 0x30, 0x60, 0x4c, 0x50, 0x98, 0x90,
};

const uint8_t PROGMEM ogreBottomRight[] = {
  8, 8,
  0x90, 0x98, 0x50, 0x4c, 0x60, 0x30, 0x3e, 0x03,
};


const uint8_t PROGMEM goblinTopLeft[] = {
  8, 8,
  0x00, 0x1f, 0xfe, 0x04, 0x34, 0x36, 0x02, 0x02,
};


const uint8_t PROGMEM goblinTopRight[] = {
  8, 8,
  0x02, 0x02, 0x26, 0x24, 0x04, 0xfe, 0x1f, 0x00,
};


const uint8_t PROGMEM goblinBottomLeft[] = {
  8, 8,
  0x00, 0x00, 0x00, 0x1f, 0x20, 0x26, 0x24, 0x76,
};

const uint8_t PROGMEM goblinBottomRight[] = {
  8, 8,
  0x64, 0x27, 0x24, 0x20, 0x1f, 0x00, 0x00, 0x00,
};

const uint8_t PROGMEM emptyHeart[] = {
  16, 16,
  0xfc, 0x82, 0x01, 0x01, 0x01, 0x01, 0x03, 0x06, 0x06, 0x03, 0x01, 0x01, 0x01, 0x01, 0x82, 0xfc,
  0x00, 0x03, 0x06, 0x0c, 0x18, 0x30, 0x60, 0xc0, 0xc0, 0x60, 0x30, 0x18, 0x0c, 0x06, 0x03, 0x00,
};

const uint8_t PROGMEM fullHeart[] = {
  16, 16,
  0xfc, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xfc,
  0x00, 0x03, 0x07, 0x0f, 0x1f, 0x3f, 0x7f, 0xff, 0xff, 0x7f, 0x3f, 0x1f, 0x0f, 0x07, 0x03, 0x00,
};

const uint8_t PROGMEM cloud[] = {
  16, 8,
  0x50, 0x9c, 0x86, 0x02, 0x02, 0x06, 0x0c, 0x18, 0x34, 0x26, 0x22, 0x12, 0x11, 0x23, 0x0e, 0x08,
};

const uint8_t PROGMEM tsugoTopLeft[] = {
  8, 8,
  0x00, 0xe0, 0xf8, 0x8c, 0x8e, 0x9f, 0xbf, 0xff,
};

const uint8_t PROGMEM tsugoTopRight[] = {
  8, 8,
  0xff, 0xbf, 0x9f, 0x8e, 0x8c, 0xf8, 0xe0, 0x00,
};

const uint8_t PROGMEM tsugoBottomLeft[] = {
  8, 8,
  0x01, 0x07, 0x3d, 0x79, 0xc3, 0xb7, 0xa4, 0x96,
};

const uint8_t PROGMEM tsugoBottomRight[] = {
  8, 8,
  0xa6, 0x94, 0xb7, 0xc3, 0x79, 0x3d, 0x07, 0x01,
};


const uint8_t PROGMEM maskTopLeft[] = {
  8, 8,
  0x0c, 0x02, 0x12, 0x31, 0x75, 0x65, 0x45, 0x09,
};

const uint8_t PROGMEM maskTopRight[] = {
  8, 8,
  0x09, 0x45, 0x65, 0x75, 0x31, 0x12, 0x02, 0x0c,
};

const uint8_t PROGMEM maskBottomLeft[] = {
  8, 8,
  0x06, 0x09, 0x12, 0x24,  0x44,  0x54, 0x88, 0xa9,
};

const uint8_t PROGMEM maskBottomRight[] = {
  8, 8,
  0xa9, 0x88, 0x54, 0x44, 0x24, 0x12, 0x09, 0x06,
};

const uint8_t PROGMEM speakerOn[] = {
  8, 8,
  0x18, 0x3c, 0x7e, 0xff, 0xff, 0x00, 0x4a, 0x89,
};

const uint8_t PROGMEM speakerOff[] = {
  8, 8,
  0x18, 0x3c, 0x7e, 0xff, 0xff, 0x00, 0x00, 0x00,
};

uint16_t jingle[] = {
  NOTE_G5, 150, NOTE_G5, 150, NOTE_G5, 150, NOTE_G5, 200, NOTE_REST, 250, NOTE_GS5, 250, NOTE_A5, 250, NOTE_AS5, 250,
  NOTE_B5, 250,  NOTE_C6, 250, NOTE_CS6, 250, NOTE_D6, 200, NOTE_REST, 250 , NOTE_D5, 150, NOTE_D5, 300,
  TONES_END
};

//genera un pezzo a caso che sia possibile inserire nelle caselle vuote disponibili
int generateRandomPart() {
  int rnd = 0;
  while (true) {
    rnd = random(0, 4);
    for (int i = rnd; i < 16 ; i += 4) {
      if (board[i] == -1) {
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


void setup() {
  HighScore score;
  for (int i = 0; i < 5; i++) {
    EEPROM.get(i * sizeof(HighScore), score);
    if (score.flag != 'T') {
      score.flag = 'T';
      score.initials[0] = 65;
      score.initials[1] = 65;
      score.initials[2] = 65;
      score.score = 0;
      EEPROM.put(i * sizeof(HighScore), score);
    }
    if (i == 4) {
      minScore = score.score;
    }
    scores[i] = score;
  }
  arduboy.begin();
  arduboy.clear();
  arduboy.initRandomSeed();
  now = millis();

}

void checkInput() {
  now = millis();
  if (board[(4 * buttonPressed) + randomPart] == -1) {
    board[(4 * buttonPressed) + randomPart] = randomFace;
    if (toggleSound) {
      sound.tone(1000, 50);
    }
  }
  else {
    wrongMove = 1;
    timerSize = timerSize <= 4500 ? timerSize + 500 : 5000;
  }
}

void checkForClearedFaces() {
  //check for cleared faces
  int checkAllClear = 0;

  for (int i = 0; i <= 12; i += 4) {
    if (board[i + 0] != -1 && board[i + 1] != -1 && board[i + 2] != -1 && board[i + 3] != -1) {

      checkAllClear = 1;

      if (board[i + 0] == board[i + 1] && board[i + 0] == board[i + 2] && board[i + 0] == board[i + 3]) {
        score += 300;
      }
      else if ((board[i + 0] == board[i + 1] && board[i + 0] == board[i + 2]) || (board[i + 0] == board[i + 1] && board[i + 0] == board[i + 3]) || (board[i + 0] == board[i + 2] && board[i + 0] == board[i + 3]) || (board[i + 1] == board[i + 2] && board[i + 1] == board[i + 3])) {
        score += 100;
      }
      else if ((board[i + 0] == board[i + 1]) || (board[i + 0] == board[i + 2]) || (board[i + 0] == board[i + 3]) || (board[i + 1] == board[i + 2]) || (board[i + 1] == board[i + 3]) || (board[i + 2] == board[i + 3])) {
        score += 40;
      }
      else {
        score += 10;
      }
      board[i + 0] = -1;
      board[i + 1] = -1;
      board[i + 2] = -1;
      board[i + 3] = -1;

      if (timerSize < 1000) {
        timerSize = 1000;
      }
      else {
        timerSize -= 100;
      }
      now = millis();
    }
  }

  if (checkAllClear) {
    int allClear = 1;
    checkAllClear = 0;
    for (int i = 0; i < 16; i++) {
      if (board[i] != -1) {
        allClear = 0;
        break;
      }
    }
    if (allClear) {
      if (lives < 3) {
        lives++;
      }
      score += 500;
      if (toggleSound) {
        sound.tone(1200, 200, 1500, 200, 1200, 200);
      }
    }
    else {
      if (toggleSound) {
        sound.tone(1200, 200, 1200, 200, 1200, 200);
      }
    }
    arduboy.invert(true);
    delay(100);
    arduboy.invert(false);
    delay(100);
    arduboy.invert(true);
    delay(100);
    arduboy.invert(false);
    delay(100);
    arduboy.invert(true);
    delay(100);
    arduboy.invert(false);
  }

}

void loop() {
  if (gamePhase == 0) {

    arduboy.clear();

    arduboy.pollButtons();

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
    arduboy.drawCircle(15, 9, 9);
    secondRowofClouds += 2;
    firstRowofClouds++;

    if (random(0, 40) == 0) {
      for (int i = 0; i <= random(1, 4); i++) {
        arduboy.invert(++lightning % 2);
        delay(100);
        arduboy.invert(++lightning % 2);
        delay(100);
      }
      int thunderSound = random(35, 45);
      int thunderLength = random(100, 300);
      if (toggleSound) {
        sound.tone(thunderSound, 50, thunderSound - 10, thunderLength - 50);
      }
    }

    delay(100);


    arduboy.setCursor(16, 24);
    arduboy.print("Games'N Goblins");
    arduboy.setCursor(40, 32);
    arduboy.print("The Game");
    arduboy.setCursor(0, 46);
    arduboy.print("A: Start Game");
    arduboy.setCursor(0, 56);
    arduboy.print("B: Toggle Sound");
    if (toggleSound) {
      Sprites::drawOverwrite(110, 56, speakerOn, 0);
    }
    else {
      Sprites::drawOverwrite(110, 56, speakerOff, 0);
    }
    if (arduboy.justPressed(A_BUTTON)) {
      if (toggleSound) {
        sound.tonesInRAM(jingle);
      }
      arduboy.clear();
      arduboy.setCursor(30, 25);
      arduboy.print("GAME START!");
      arduboy.display();
      delay(3600);
      gamePhase++;
      now = millis();
    }
    if (arduboy.justPressed(B_BUTTON)) {
      toggleSound = (toggleSound + 1) % 2;
    }
    arduboy.display();
  }
  else if (gamePhase == 1) {

    checkForClearedFaces();

    arduboy.clear();
    arduboy.pollButtons();

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

    if (buttonPressed >= 0) {
      randomFace = random(0, 4);
      randomPart = generateRandomPart();
      buttonPressed = -1;
    }

    //debugging tools
    /*int ht = 0;
      arduboy.setCursor(70, ht);
      arduboy.print("face: ");
      arduboy.print(randomFace);
      arduboy.setCursor(70, ht += 10);
      arduboy.print("part: ");
      arduboy.print(randomPart);
      arduboy.setCursor(70, ht += 10);
      for (int i = 0; i < 16; i++) {
      if (i % 4 == 0 && i != 0) {
        arduboy.setCursor(70, ht += 10);
      }
      arduboy.print(board[i]);
      }
      arduboy.print(timerSize);*/

    if (wrongMove) {
      lives--;
      wrongMove = 0;

      if (toggleSound) {
        if (lives > 0) {

          sound.tone(500, 200, 500, 200, 500, 200);
        }
        else {
          sound.tone(500, 200, 500, 200, 200, 400);
        }
      }

      arduboy.invert(true);
      delay(100);
      arduboy.invert(false);
      delay(100);
      arduboy.invert(true);
      delay(100);
      arduboy.invert(false);
      delay(100);
      arduboy.invert(true);
      delay(100);
      arduboy.invert(false);
    }

    //********************
    //**PRINTING SECTION**
    //********************

    //print all faces
    for (int i = 0; i < 16; i++) {
      if (board[i] != -1) {
        printFacePartOnPosition( xCoordinates[i], yCoordinates[i], board[i], i % 4);
      }
    }

    printFacePartOnPosition( 27, 27, randomFace, randomPart);

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

    // arduboy.print((now + 5000-millis())/500);


    if (lives <= 0) {
      if (score < minScore) {
        gamePhase = 2;
      }
      else {
        gamePhase = 3;
        leaderBoardCalculated = 0;
      }

    }

    arduboy.display();
  }
  else if (gamePhase == 2) {

    arduboy.clear();
    arduboy.pollButtons();

    arduboy.setCursor(20, 0);
    arduboy.print("GAME OVER");
    arduboy.setCursor(40, 10);
    arduboy.print("Play again");
    arduboy.setCursor(20, 20);
    arduboy.print("Your score is:");
    arduboy.print(score);
    arduboy.setCursor(20, 50);
    arduboy.print("Press Button A!");

    if (arduboy.justPressed(A_BUTTON)) {
      gamePhase = 1;
      lives = 3;
      score = 0;
      timerSize = 5000;
      for (int i = 0; i < 16; i++) {
        board[i] = -1;
      }
      now = millis();
    }
    arduboy.display();
  }
  else if (gamePhase == 3) {
    if (!leaderBoardCalculated) {
      while (scores[scoreIndex].score > score) {
        scoreIndex++;
      }

      for (int i = 4; i > scoreIndex; i--) {
        scores[i] = scores[i - 1];
      }

      scores[scoreIndex].score = score;
      leaderBoardCalculated = 1;
    }

    arduboy.clear();
    arduboy.pollButtons();
    arduboy.setCursor(30, 0);
    arduboy.print("NAME  SCORE");

    for (int i = 0; i < 5; i++) {
      arduboy.setCursor(10, 15 + (10 * i));
      arduboy.print(i + 1);
      //arduboy.setCursor(30, 15 + (10 * i));
      if (i != scoreIndex) {
        for (int j = 0; j < 3; j++) {
          arduboy.setCursor(30 + (i * 6), 15 + (10 * i));
          arduboy.print(char(scores[i].initials[j]));
        }
      }
      arduboy.setCursor(67, 15 + (10 * i));
      arduboy.print(scores[i].score);
    }

    for (int i = 0; i < 3; i++) {
      arduboy.setCursor(30 + (i * 6), 15 + (10 * scoreIndex));
      arduboy.print(char(initials[i]));
    }

    //arduboy.setCursor(30 + (letterPosition * 6), 17 + (10 * scoreIndex));
    arduboy.drawLine(30 + (letterPosition * 6), 23 + (10 * scoreIndex), 34 + (letterPosition * 6), 23 + (10 * scoreIndex));

    if (arduboy.justPressed(UP_BUTTON)) {
      initials[letterPosition] = initials[letterPosition] < 90 ? initials[letterPosition] + 1 : 65;

    }
    if (arduboy.justPressed(DOWN_BUTTON)) {
      initials[letterPosition] = initials[letterPosition] > 65 ? initials[letterPosition] - 1 : 90;
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
      scores[scoreIndex].initials[0] = initials[0];
      scores[scoreIndex].initials[1] = initials[1];
      scores[scoreIndex].initials[2] = initials[2];

      for (int i = 0; i < 5; i++) {
        EEPROM.put(i * sizeof(HighScore), scores[i]);
      }

      scoreIndex = 0;
      gamePhase = 1;
      lives = 3;
      score = 0;
      timerSize = 5000;
      for (int i = 0; i < 16; i++) {
        board[i] = -1;
      }
      now = millis();
    }

    arduboy.display();
  }
}
