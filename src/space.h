#ifndef SPACE_H
#define SPACE_H
// This math has added trig function approximations
#include "trigmath.h"
#include "program.h"
#include "images.h"
#include "draw.h"

//Function declarations for space functions.
void renderShip(struct SpaceGlobals *mySpaceGlobals);
void renderInitialPlayers(struct SpaceGlobals *mySpaceGlobals);
void renderReset(struct SpaceGlobals *mySpaceGlobals);
void reset(struct SpaceGlobals *mySpaceGlobals);
void resetRenderFlags(struct SpaceGlobals *mySpaceGlobals);
void render(struct SpaceGlobals *mySpaceGlobals);
void p1Move(struct SpaceGlobals *mySpaceGlobals);
void renderTexts(struct SpaceGlobals *mySpaceGlobals);
void initStars(struct SpaceGlobals *mySpaceGlobals);
void handleCollisions(struct SpaceGlobals * mySpaceGlobals);
void displayTitle(struct SpaceGlobals * mySpaceGlobals);
void doMenuAction(struct SpaceGlobals *mySpaceGlobals);
void drawMenuCursor(struct SpaceGlobals *mySpaceGlobals);
void moveBullets(struct SpaceGlobals *mySpaceGlobals);
void renderStars(struct SpaceGlobals *mySpaceGlobals);
void decompress_sprite(int arraysize, int width, int height, const signed char* input, unsigned char target[][width], char transIndex);
void renderTexts(struct SpaceGlobals *mySpaceGlobals);
void render(struct SpaceGlobals *mySpaceGlobals);
void blackout();
void totallyRefreshState(struct SpaceGlobals *mySpaceGlobals);
void p1Shoot(struct SpaceGlobals * mySpaceGlobals);
void displayPause(struct SpaceGlobals * mySpaceGlobals);
void makeRotationScaleMatrix(float angle, int frame, int width, unsigned char original[][width], unsigned char target[][width], int transIndex);
void doPasswordMenuAction(struct SpaceGlobals * mySpaceGlobals);
void displayPasswordScreen(struct SpaceGlobals * mySpaceGlobals);
void checkPause(struct SpaceGlobals * mySpaceGlobals);
void addNewEnemies(struct SpaceGlobals * mySpaceGlobals);
void tryPassword(struct SpaceGlobals * mySpaceGlobals);
void initGameState(struct SpaceGlobals * mySpaceGlobals);
void displayGameOver(struct SpaceGlobals *mySpaceGlobals);
void handleExplosions(struct SpaceGlobals* mySpaceGlobals);

#endif /* SPACE_H */