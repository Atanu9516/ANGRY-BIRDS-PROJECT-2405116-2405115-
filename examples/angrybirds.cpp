#include "iGraphics.h"
#include <cmath>

// Window size
const int screenWidth = 800;
const int screenHeight = 450;

// Bird
Sprite bird;
bool isBirdFlying = false;
float birdVelX = 0, birdVelY = 0;
float gravity = -0.5;

// Target (Pig)
Sprite pig;

// Mouse dragging
bool dragging = false;
int dragStartX = 0, dragStartY = 0;

// Load a sprite
void loadSprite(Sprite &s, const char *filename) {
    Image img;
    iLoadImage(&img, filename);
    Sprite tmp;
    iInitSprite(&tmp);
    iChangeSpriteFrames(&tmp, &img, 1);
    iFreeImage(&img);
    s = tmp;
}

void resetBird() {
    iSetSpritePosition(&bird, 150, 100);
    birdVelX = 0;
    birdVelY = 0;
    isBirdFlying = false;
}

void iDraw() {
    iClear();

    // Draw slingshot
    iSetColor(139, 69, 19);
    iFilledRectangle(140, 100, 20, 80);

    // Draw dragging line
    if (dragging && !isBirdFlying) {
        iSetColor(255, 0, 0);
        iLine(bird.x + 25, bird.y + 25, iMouseX, iMouseY);
    }

    iShowSprite(&pig);
    iShowSprite(&bird);

    // Win text
    if (iCheckCollision(&bird, &pig)) {
        iText(350, 400, "You Win!", GLUT_BITMAP_TIMES_ROMAN_24);
    }
}

void iMouse(int button, int state, int mx, int my) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && !isBirdFlying) {
        dragging = true;
        dragStartX = mx;
        dragStartY = my;
    }

    if (button == GLUT_LEFT_BUTTON && state == GLUT_UP && dragging) {
        dragging = false;

        // Calculate velocity
        int dx = dragStartX - iMouseX;
        int dy = dragStartY - iMouseY;
        birdVelX = dx * 0.2;
        birdVelY = dy * 0.2;
        isBirdFlying = true;
    }
}

void iMouseDrag(int mx, int my) {
    // Optional: update real-time drag feedback
}

void iKeyboard(unsigned char key) {
    if (key == 'r') {
        resetBird();
    }
}

void updateBird() {
    if (isBirdFlying) {
        bird.x += birdVelX;
        bird.y += birdVelY;
        birdVelY += gravity;

        // Off screen reset
        if (bird.x < 0 || bird.y < 0 || bird.x > screenWidth || bird.y > screenHeight) {
            resetBird();
        }
    }
}

int main(int argc, char *argv[]) {
    glutInit(&argc, argv);
    iInitialize(screenWidth, screenHeight, "Angry Birds Mini Game");

    // Load bird and pig
    loadSprite(bird, "bird.png");
    loadSprite(pig, "pig.png");

    iSetSpritePosition(&pig, 600, 100);
    resetBird();

    iSetTimer(20, updateBird);
    return 0;
}
