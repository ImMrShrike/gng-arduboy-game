//Claudio Belato
//02/06/2022
#include <Arduboy2.h>
#include <time.h>

Arduboy2 arduboy;

int randomPart = 0;
int randomFace = 0;
int buttonPressed = -1;
int lives = 3;
int score = 0;
unsigned long now = 0;



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


const uint8_t PROGMEM chtulu[] = {
  16, 16,
  0x00, 0xf0, 0x1e, 0x01, 0x39, 0x39, 0x01, 0x01, 0x01, 0x01, 0x39, 0x39, 0x01, 0x1e, 0xf0, 0x00,
  0xfc, 0x07, 0x00, 0x00, 0xff, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0xff, 0x00, 0x00, 0x07, 0xfc,
};

const uint8_t PROGMEM star[] = {
  16, 16,
  0x80, 0x40, 0x20, 0x90, 0x88, 0x84, 0x82, 0xf9, 0xf9, 0x82, 0x84, 0x88, 0x90, 0x20, 0x40, 0x80,
  0x01, 0x02, 0x04, 0x09, 0x11, 0x21, 0x41, 0x9f, 0x9f, 0x41, 0x21, 0x11, 0x09, 0x04, 0x02, 0x01,
};

const uint8_t PROGMEM goblin[] = {
  16, 16,
  0x00, 0x1f, 0xfe, 0x04, 0x34, 0x36, 0x02, 0x02, 0x02, 0x02, 0x26, 0x24, 0x04, 0xfe, 0x1f, 0x00,
  0x00, 0x00, 0x00, 0x1f, 0x20, 0x26, 0x24, 0x76, 0x64, 0x27, 0x24, 0x20, 0x1f, 0x00, 0x00, 0x00,
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
  else if (face == 3) {
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
}


void setup() {
  arduboy.begin();
  arduboy.clear();
  arduboy.initRandomSeed();
  now = millis();

}

void loop() {

  //check for cleared faces
  for (int i = 0; i < 12; i += 4) {
    if (board[i + 0] != -1 && board[i + 1] != -1 && board[i + 2] != -1 && board[i + 3] != -1) {
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

  arduboy.clear();
  arduboy.pollButtons();
  int wrongMove = 0;

  if (millis() >= now + 5000) {
    wrongMove = 1;
    now = millis();
  }

  if (arduboy.justPressed(LEFT_BUTTON)) {
    now = millis();
    buttonPressed = 0;
    if (board[(4 * buttonPressed) + randomPart] == -1) {
      board[(4 * buttonPressed) + randomPart] = randomFace;
    }
    else {
      wrongMove = 1;
    }
  }
  if (arduboy.justPressed(UP_BUTTON)) {
    now = millis();
    buttonPressed = 1;
    if (board[(4 * buttonPressed) + randomPart] == -1) {
      board[(4 * buttonPressed) + randomPart] = randomFace;
    }
    else {
      wrongMove = 1;
    }
  }
  if (arduboy.justPressed(RIGHT_BUTTON)) {
    now = millis();
    buttonPressed = 2;
    if (board[(4 * buttonPressed) + randomPart] == -1) {
      board[(4 * buttonPressed) + randomPart] = randomFace;
    }
    else {
      wrongMove = 1;
    }
  }
  if (arduboy.justPressed(DOWN_BUTTON)) {
    now = millis();
    buttonPressed = 3;
    if (board[(4 * buttonPressed) + randomPart] == -1) {
      board[(4 * buttonPressed) + randomPart] = randomFace;
    }
    else {
      wrongMove = 1;
    }
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
    }*/
  //arduboy.print(millis());

  if (wrongMove) {
    lives--;
    wrongMove = 0;
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
  else {
    Sprites::drawOverwrite(70, 0, emptyHeart, 0);
    Sprites::drawOverwrite(90, 0, emptyHeart, 0);
    Sprites::drawOverwrite(110, 0, emptyHeart, 0);
  }

  //draw background
  arduboy.drawRect(21, 0, 22, 22);
  arduboy.drawRect(0, 21, 22, 22);
  arduboy.drawRect(42, 21, 22, 22);
  arduboy.drawRect(21, 42, 22, 22);

  arduboy.display();
}
