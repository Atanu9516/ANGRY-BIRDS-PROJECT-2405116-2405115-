#include "iSound.h"
#include "iGraphics.h"
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <direct.h>
#include <windows.h>

char currentuser[50] = "";
char nameinput[50] = "";
int istypingname = 0;
int nameinputpage = 0;     /* 0 = main menu, 7 = name input, 8 = leaderboard */
const int MAX_USERS = 100; // maximum number of users
struct userscore
{
    char username[50];
    int level1score;
    int level2score;
    int level3score;
    int totalscore;
    int level1completed;
    int level2completed;
    int level3completed;
};

struct userscore alluserscores[MAX_USERS];
int totalusers = 0;                  // tracking how many users
int highestscores[4] = {0, 0, 0, 0}; //[0]=total, [1]=level1, [2]=level2, [3]=level3

int totalbirdsused = 0;
#define RED_BIRD 0
#define BLUE_BIRD 1
#define YELLOW_BIRD 2 // speed increases
#define ORANGE_BIRD 3 // size increases

int maxbirds = 6;
int birdsequence[6] = {RED_BIRD, BLUE_BIRD, YELLOW_BIRD, ORANGE_BIRD, RED_BIRD, BLUE_BIRD}; // per level e hobe

// types of birds
#define RED_BIRD 0
#define BLUE_BIRD 1
#define YELLOW_BIRD 2 // speed increases
#define ORANGE_BIRD 3 // size increases
// pig types
#define NORMAL_PIG 0
#define STRONG_PIG 1
#define SMALL_PIG 2
int currentbirdtype = RED_BIRD;
int hassplit = 0;
int issplittingbird = 0;
int hasspeedboost = 0;
int isspeedboost = 0;
int hassizeboost = 0;
int issizeboostbird = 0;
int currentbirdsize = 5;
double flyingpigx = 400;
double flyingpigy = 650;
double flyingpigspeed = 3.0;
int flyingpigdirection = 1;
// 1 basically means right, 0 means left//
int gameOver = 0;
int levelcompleted = 0;
int showstopmenu = 0;
int showlevelcompletedwithoutwarning = 0;
int currentlevel = 1;
int warninglevel = 1;

// trackkng level completion per user
int level1completed = 0;
int level2completed = 0;
int level3completed = 0;
// individual level scores
int level1score = 0;
int level2score = 0;
int level3score = 0;
Image redBirdImg, blueBirdImg, yellowBirdImg, orangeBirdImg, orangeBigBirdImg, pigImg, strongPigImg, smallPigImg, stopMenuImg;
// level 3 pigs
int piggy3Types[15];       // track pigs for level 3
int piggy3Health[15];      // pig health
int piggy3RecentlyHit[15]; // double hit prevent kore

struct MiniBird
{
    int active;
    double x, y;
    double vx, vy;
};

struct MiniBird miniBirds[3];
#define g 9.8 / 100
#define G 9.8
#define PI 3.14159265
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 853
#define GROUND_HEIGHT 100

struct recta
{
    int active;
    int x, y, w, h;
};
struct recta party[15];
struct recta bparty[15];
struct recta party2[15];
struct recta bparty2[15];
struct recta party3[15];
struct recta bparty3[50];

struct circa
{
    int active;
    double a, b, r;
};
struct circa piggy[15];
struct circa piggy2[15];
struct circa piggy3[15];

int xCircle = 100;
int yCircle = 200;
int radius = 80;
double theta = 0;
int xLine = xCircle + (radius + 30) * cos(theta * PI / 180);
int yLine = yCircle + (radius + 30) * sin(theta * PI / 180);
int xBall = xCircle;
int yBall = yCircle;
int radiusball = 5;
int dragging = 0;
int dragradius = 80;

double v = 18.0;
double vx = 0.0;
double vy = 0.0;
int topblock1falling2 = 0;
int topblock2falling2 = 0;
int topblock3falling2 = 0;
int topblock4falling2 = 0;
int circle1falling = 0;
int circle2falling = 0;
int circle1falling2 = 0;
int circle2falling2 = 0;
int topblock1falling3 = 0;
int topblock2falling3 = 0;
int topblock3falling3 = 0;
int topblock4falling3 = 0;
int topblock5falling3 = 0;
int circle1falling3 = 0;
int circle2falling3 = 0;
int topblock1falling = 0;
int topblock2falling = 0;
int topblock3falling = 0;
int p;
int slingLeftX = xCircle - 25;
int slingLeftY = yCircle + 15;
int slingRightX = xCircle + 25;
int slingRightY = yCircle + 15;
int xaxisdraw;
int yaxisdraw;
int check = 0;
int i = 0;
int timechange = 1;
int timemax;
double maxdragdistance = 100.0;
double minvelocitymultiplier = 2.0;
double maxvelocitymultiplier = 8.0;
double restitution = 0.5;
double friction = 0.99;
int bouncecount = 0;
int maxbounces = 5;
int onground = 0;
double minvelocity = 0.5;
int collision = 1;
double ballX = 100.0;
double ballY = 200.0;
int page = 0;
int xbox = 500, ybox = 0;
int score = 0;
int bonusawarded = 0;
// flag for nerd stats
int showNerdStats = 0;
extern int iScreenWidth, H;
Image creditsImg;

int clickchannel = -1; // Channel for click sound
int birdlaughchannel = -1; // Channel for bird laugh sound
int pigdamagechannel = -1; // Channel for pig damage sound

void updatecurrentbirdtype();
void itblocks();
void bblocks();
void bpigs();
void itblocks2();
void bblocks2();
void bpigs2();
void itblocks3();
void bblocks3();
void bpigs3();
void drawlevelcompletedwarningscreen();
void saveuserdata();
void loaduserdata();
void loadallusers();
void makegamesavefolder();
void updatehighscore();
void arrangeleaderboard();
int allpigsdestroyed();
void resetgamestate();
void restartcurrentlevel();

void playclicksound()
{
    clickchannel = iPlaySound("assets/sounds/click.wav", 0, 100);
}

void playbirdlaughsound()
{
    birdlaughchannel = iPlaySound("assets/sounds/birdslaugh.wav", 0, 100);
}

void playpigdamagesound()
{
    pigdamagechannel = iPlaySound("assets/sounds/pigdamage.wav", 0, 100);
}

int getbirdradius(int birdtype, int birdsize)
{
    if (birdtype == ORANGE_BIRD && hassizeboost && issizeboostbird)
    {
        return birdsize * 5;
    }
    else
    {
        return birdsize * 4;
    }
}

double getPigVisualRadius(double pigRadius)
{
    return pigRadius * 1.5;
}

int checkBoundaries(double x, double y, int birdradius)
{
    if (x - birdradius < 0)
        return 1;
    if (x + birdradius > iScreenWidth)
        return 1;
    if (y - birdradius < 0)
        return 1;
    if (y + birdradius > iScreenHeight)
        return 1;
    return 0;
}

int checkRectCollision(double birdX, double birdY, int birdradius, struct recta *block)
{
    if (!block->active)
        return 0;

    double closestX = birdX;
    double closestY = birdY;

    if (birdX < block->x)
        closestX = block->x;
    else if (birdX > block->x + block->w)
        closestX = block->x + block->w;

    if (birdY < block->y)
        closestY = block->y;
    else if (birdY > block->y + block->h)
        closestY = block->y + block->h;

    double dx = birdX - closestX;
    double dy = birdY - closestY;
    double distance = sqrt(dx * dx + dy * dy);

    return distance <= birdradius;
}
int checkCircleCollision(double birdX, double birdY, int birdradius, struct circa *pig)
{
    if (!pig->active)
        return 0;

    double dx = birdX - pig->a;
    double dy = birdY - pig->b;
    double distance = sqrt(dx * dx + dy * dy);

    double pigcollisionradius = pig->r * 1.5;
    double collisiondistance = birdradius + pigcollisionradius;

    return distance <= collisiondistance;
}

void loadLevelState(int level)
{
    char filename[100];
    FILE *file;

    if (strlen(currentuser) == 0)
        return;

    sprintf(filename, "gamesave/%s_level%d_state.txt", currentuser, level);
    file = fopen(filename, "r");
    if (!file)
        return;

    // Load level-specific state
    if (level == 1)
    {
        for (int i = 0; i < 15; i++)
        {
            fscanf(file, "%d", &party[i].active);
            fscanf(file, "%d", &bparty[i].active);
            fscanf(file, "%d", &piggy[i].active);
        }
    }
    else if (level == 2)
    {
        for (int i = 0; i < 15; i++)
        {
            fscanf(file, "%d", &party2[i].active);
            fscanf(file, "%d", &bparty2[i].active);
            fscanf(file, "%d", &piggy2[i].active);
        }
    }
    else if (level == 3)
    {
        for (int i = 0; i < 15; i++)
        {
            fscanf(file, "%d", &party3[i].active);
            fscanf(file, "%d", &piggy3[i].active);
        }
        for (int i = 0; i < 50; i++)
        {
            fscanf(file, "%d", &bparty3[i].active);
        }
    }

    fscanf(file, "%d", &score);
    fscanf(file, "%d", &totalbirdsused);
    fscanf(file, "%d", &bonusawarded);

    fclose(file);
}
void makegamesavefolder()
{
    _mkdir("gamesave");
}

void saveuserdata()
{
    char filename[100];
    FILE *file;

    if (strlen(currentuser) == 0)
        return;

    makegamesavefolder();
    sprintf(filename, "gamesave/%s.txt", currentuser);

    file = fopen(filename, "w");
    if (file != NULL)
    {
        fprintf(file, "%d\n", level1score);
        fprintf(file, "%d\n", level2score);
        fprintf(file, "%d\n", level3score);
        fprintf(file, "%d\n", level1completed ? 1 : 0);
        fprintf(file, "%d\n", level2completed ? 1 : 0);
        fprintf(file, "%d\n", level3completed ? 1 : 0);
        fclose(file);
    }
}

void loadallusers()
{
    WIN32_FIND_DATA findFileData;
    HANDLE hFind;
    char filename[260];
    char *dot;
    struct userscore userscore;
    char filepath[300];
    FILE *file;
    int temp;

    totalusers = 0;

    hFind = FindFirstFile("gamesave/*.txt", &findFileData);

    if (hFind != INVALID_HANDLE_VALUE)
    {
        do
        {
            if (totalusers >= MAX_USERS)
                break;

            strcpy(filename, findFileData.cFileName);

            dot = strrchr(filename, '.');
            if (dot != NULL)
            {
                *dot = '\0';
            }

            strcpy(userscore.username, filename);

            sprintf(filepath, "gamesave/%s", findFileData.cFileName);

            file = fopen(filepath, "r");
            if (file != NULL)
            {
                fscanf(file, "%d", &userscore.level1score);
                fscanf(file, "%d", &userscore.level2score);
                fscanf(file, "%d", &userscore.level3score);

                fscanf(file, "%d", &temp);
                userscore.level1completed = (temp == 1);
                fscanf(file, "%d", &temp);
                userscore.level2completed = (temp == 1);
                fscanf(file, "%d", &temp);
                userscore.level3completed = (temp == 1);

                userscore.totalscore = userscore.level1score + userscore.level2score + userscore.level3score;

                alluserscores[totalusers] = userscore;
                totalusers++;
                fclose(file);
            }
        } while (FindNextFile(hFind, &findFileData) != 0);

        FindClose(hFind);
    }

    arrangeleaderboard();
    updatehighscore();
}
void loaduserdata()
{
    char filename[100];
    FILE *file;
    int temp;

    if (strlen(currentuser) == 0)
        return;

    sprintf(filename, "gamesave/%s.txt", currentuser);

    file = fopen(filename, "r");
    if (file != NULL)
    {
        fscanf(file, "%d", &level1score);
        fscanf(file, "%d", &level2score);
        fscanf(file, "%d", &level3score);

        fscanf(file, "%d", &temp);
        level1completed = (temp == 1);
        fscanf(file, "%d", &temp);
        level2completed = (temp == 1);
        fscanf(file, "%d", &temp);
        level3completed = (temp == 1);

        fclose(file);
    }
    else
    {

        level1score = 0;
        level2score = 0;
        level3score = 0;
        level1completed = 0;
        level2completed = 0;
        level3completed = 0;
    }
    loadLevelState(1);
    loadLevelState(2);
    loadLevelState(3);
}

void arrangeleaderboard()
{
    int i, j;
    struct userscore temp;
    for (i = 0; i < totalusers - 1; i++)
    {
        for (j = 0; j < totalusers - i - 1; j++)
        {

            if (alluserscores[j].totalscore < alluserscores[j + 1].totalscore)
            {

                temp = alluserscores[j];
                alluserscores[j] = alluserscores[j + 1];
                alluserscores[j + 1] = temp;
            }
        }
    }
}

void updatehighscore()
{
    int i;

    highestscores[0] = highestscores[1] = highestscores[2] = highestscores[3] = 0;

    for (i = 0; i < totalusers; i++)
    {
        struct userscore *user = &alluserscores[i];
        if (user->totalscore > highestscores[0])
            highestscores[0] = user->totalscore;
        if (user->level1score > highestscores[1])
            highestscores[1] = user->level1score;
        if (user->level2score > highestscores[2])
            highestscores[2] = user->level2score;
        if (user->level3score > highestscores[3])
            highestscores[3] = user->level3score;
    }
}
int allpigsdestroyed()
{
    int i;

    if (page == 1)
    {
        for (i = 0; i < 15; i++)
        {
            if (piggy[i].active)
                return 0;
        }
    }
    else if (page == 4)
    {
        for (i = 0; i < 15; i++)
        {
            if (piggy2[i].active)
                return 0;
        }
    }
    else if (page == 6)
    {
        for (i = 0; i < 15; i++)
        {
            if (piggy3[i].active)
                return 0;
        }
    }
    return 1;
}

void resetgamestate()
{
    int i;

    gameOver = 0;
    levelcompleted = 0;
    showstopmenu = 0;
    showlevelcompletedwithoutwarning = 0;
    check = 0;
    dragging = 0;
    bouncecount = 0;
    onground = 0;
    vx = vy = 0;
    ballX = xCircle;
    ballY = yCircle;
    xBall = xCircle;
    yBall = yCircle;
    totalbirdsused = 0;
    bonusawarded = 0;
    hassplit = 0;
    hasspeedboost = 0;
    hassizeboost = 0;
    currentbirdsize = 5;
    timechange = 1;

    /* Reset flying pig to default position */
    flyingpigx = 400;
    flyingpigy = 650;
    flyingpigdirection = 1;
    topblock1falling = 0;
    topblock2falling = 0;
    topblock3falling = 0;
    circle1falling = 0;
    circle2falling = 0;
    topblock1falling2 = 0;
    topblock2falling2 = 0;
    topblock3falling2 = 0;
    topblock4falling2 = 0;
    circle1falling2 = 0;
    circle2falling2 = 0;
    topblock1falling3 = 0;
    topblock2falling3 = 0;
    topblock3falling3 = 0;
    topblock4falling3 = 0;
    topblock5falling3 = 0;
    circle1falling3 = 0;
    circle2falling3 = 0;

    for (i = 0; i < 3; i++)
    {
        miniBirds[i].active = 0;
    }
    for (i = 0; i < 15; i++)
    {
        piggy3RecentlyHit[i] = 0;
    }

    updatecurrentbirdtype();
}
void restartcurrentlevel()
{
    memset(birdsequence, RED_BIRD, sizeof(birdsequence));

    if (currentlevel == 1)
    {
        page = 1;
        maxbirds = 6;
        birdsequence[0] = RED_BIRD;
        birdsequence[1] = BLUE_BIRD;
        birdsequence[2] = YELLOW_BIRD;
        birdsequence[3] = ORANGE_BIRD;
        birdsequence[4] = RED_BIRD;
        birdsequence[5] = RED_BIRD;
    }
    else if (currentlevel == 2)
    {
        page = 4;
        maxbirds = 6;
        birdsequence[0] = RED_BIRD;
        birdsequence[1] = BLUE_BIRD;
        birdsequence[2] = YELLOW_BIRD;
        birdsequence[3] = ORANGE_BIRD;
        birdsequence[4] = RED_BIRD;
        birdsequence[5] = RED_BIRD;
    }
    else if (currentlevel == 3)
    {
        page = 6;
        maxbirds = 6;
        birdsequence[0] = RED_BIRD;
        birdsequence[1] = BLUE_BIRD;
        birdsequence[2] = YELLOW_BIRD;
        birdsequence[3] = ORANGE_BIRD;
        birdsequence[4] = BLUE_BIRD;
        birdsequence[5] = RED_BIRD;
    }
    score = 0; // Reset score for the new level
    itblocks();
    bblocks();
    bpigs();
    if (currentlevel == 2)
    {
        itblocks2();
        bblocks2();
        bpigs2();
    }
    if (currentlevel == 3)
    {
        itblocks3();
        bblocks3();
        bpigs3();
    }
    resetgamestate();
}
void loadBirdSprites()
{
    iLoadImage(&redBirdImg, "assets/images/red_bird.png");
    iLoadImage(&blueBirdImg, "assets/images/blue_bird.png");
    iLoadImage(&yellowBirdImg, "assets/images/yellow_bird.png");
    iLoadImage(&orangeBirdImg, "assets/images/orange_bird.png");
    iLoadImage(&orangeBigBirdImg, "assets/images/orangebig_bird.png");
    iLoadImage(&pigImg, "assets/images/pig.png");
    iLoadImage(&strongPigImg, "assets/images/strongpig.png");
    iLoadImage(&smallPigImg, "assets/images/smallpig.png");
    iLoadImage(&stopMenuImg, "assets/images/stopmenu.png");
}

void drawBirdSprite(int x, int y, int type, int size)
{
    Image *birdImage;
    int spriteSize = size * 8;

    switch (type)
    {
    case RED_BIRD:
        birdImage = &redBirdImg;
        break;
    case BLUE_BIRD:
        birdImage = &blueBirdImg;
        break;
    case YELLOW_BIRD:
        birdImage = &yellowBirdImg;
        break;
    case ORANGE_BIRD:
        if (hassizeboost && issizeboostbird)
        {
            birdImage = &orangeBigBirdImg;
            spriteSize = size * 10;
        }
        else
        {
            birdImage = &orangeBirdImg;
        }
        break;
    default:
        birdImage = &redBirdImg;
        break;
    }

    iShowLoadedImage(x - spriteSize / 2, y - spriteSize / 2, birdImage);
}

void drawPigSprite(double x, double y, double radius)
{
    int spriteSize = (int)(radius * 4);
    iShowLoadedImage((int)x - spriteSize / 2, (int)y - spriteSize / 2, &pigImg);
}

void drawPigSprite3(double x, double y, double radius, int pigIndex)
{
    int spriteSize;
    Image *pigImage;

    if (piggy3Types[pigIndex] == SMALL_PIG)
    {
        spriteSize = (int)(radius * 3); /* Smaller sprite for small pig */
        pigImage = &smallPigImg;
    }
    else if (piggy3Types[pigIndex] == STRONG_PIG)
    {
        spriteSize = (int)(radius * 4);
        pigImage = &strongPigImg;
    }
    else
    {
        spriteSize = (int)(radius * 4);
        pigImage = &pigImg;
    }

    iShowLoadedImage((int)x - spriteSize / 2, (int)y - spriteSize / 2, pigImage);
}

void updatecurrentbirdtype()
{
    if (totalbirdsused < maxbirds)
    {
        currentbirdtype = birdsequence[totalbirdsused];

        issplittingbird = (currentbirdtype == BLUE_BIRD);
        isspeedboost = (currentbirdtype == YELLOW_BIRD);
        issizeboostbird = (currentbirdtype == ORANGE_BIRD);

        currentbirdsize = (currentbirdtype == ORANGE_BIRD && hassizeboost) ? 15 : 5;
    }
}

void itblocks()
{
    party[0].active = 1;
    party[0].x = 750;
    party[0].y = 94;
    party[0].w = 20;
    party[0].h = 200;
    party[1].active = 1;
    party[1].x = 885;
    party[1].y = 94;
    party[1].w = 20;
    party[1].h = 350;
    party[2].active = 1;
    party[2].x = 750;
    party[2].y = 294;
    party[2].w = 260;
    party[2].h = 20;
    party[3].active = 1;
    party[3].x = 600;
    party[3].y = 94;
    party[3].w = 20;
    party[3].h = 100;
    party[4].active = 1;
    party[4].x = 750;
    party[4].y = 94;
    party[4].w = 20;
    party[4].h = 100;
    party[5].active = 1;
    party[5].x = 600;
    party[5].y = 194;
    party[5].w = 150;
    party[5].h = 20;
    party[6].active = 1;
    party[6].x = 1010;
    party[6].y = 94;
    party[6].w = 20;
    party[6].h = 350;
    party[7].active = 1;
    party[7].x = 885;
    party[7].y = 424;
    party[7].w = 145;
    party[7].h = 20;
    party[8].active = 0;
    party[8].x = 0;
    party[8].y = 0;
    party[8].w = 0;
    party[8].h = 0;
}

void bblocks()
{
    bparty[0].active = 1;
    bparty[0].x = 800;
    bparty[0].y = 94;
    bparty[0].w = 80;
    bparty[0].h = 60;
    bparty[1].active = 1;
    bparty[1].x = 920;
    bparty[1].y = 94;
    bparty[1].w = 80;
    bparty[1].h = 60;
    int i;
    for (i = 2; i < 15; i++)
    {
        bparty[i].active = 0;
        bparty[i].x = 0;
        bparty[i].y = 0;
        bparty[i].w = 0;
        bparty[i].h = 0;
    }
}

void bpigs()
{
    piggy[0].active = 1;
    piggy[0].a = 835;
    piggy[0].b = 180;
    piggy[0].r = 15;
    piggy[1].active = 1;
    piggy[1].a = 970;
    piggy[1].b = 200;
    piggy[1].r = 25;
    piggy[2].active = 0;
    piggy[2].a = 0;
    piggy[2].b = 0;
    piggy[2].r = 0;
    piggy[3].active = 1;
    piggy[3].a = 700;
    piggy[3].b = 120;
    piggy[3].r = 15;
    piggy[4].active = 1;
    piggy[4].a = 970;
    piggy[4].b = 365;
    piggy[4].r = 25;
    piggy[5].active = 1;
    piggy[5].a = 970;
    piggy[5].b = 500;
    piggy[5].r = 25;
    piggy[6].active = 1;
    piggy[6].a = 835;
    piggy[6].b = 360;
    piggy[6].r = 25;
    int i;
    for (i = 7; i < 15; i++)
    {
        piggy[i].active = 0;
        piggy[i].a = 0;
        piggy[i].b = 0;
        piggy[i].r = 0;
    }
}

void itblocks2()
{
    party2[0].active = 1;
    party2[0].x = 750;
    party2[0].y = 94;
    party2[0].w = 20;
    party2[0].h = 200;
    party2[1].active = 1;
    party2[1].x = 885;
    party2[1].y = 94;
    party2[1].w = 20;
    party2[1].h = 350;
    party2[2].active = 1;
    party2[2].x = 750;
    party2[2].y = 294;
    party2[2].w = 260;
    party2[2].h = 20;
    party2[3].active = 1;
    party2[3].x = 600;
    party2[3].y = 94;
    party2[3].w = 20;
    party2[3].h = 320;
    party2[4].active = 1;
    party2[4].x = 750;
    party2[4].y = 94;
    party2[4].w = 20;
    party2[4].h = 100;
    party2[5].active = 1;
    party2[5].x = 600;
    party2[5].y = 194;
    party2[5].w = 150;
    party2[5].h = 20;
    party2[6].active = 1;
    party2[6].x = 1010;
    party2[6].y = 94;
    party2[6].w = 20;
    party2[6].h = 350;
    party2[7].active = 1;
    party2[7].x = 885;
    party2[7].y = 424;
    party2[7].w = 145;
    party2[7].h = 20;
    party2[8].active = 1;
    party2[8].x = 450;
    party2[8].y = 94;
    party2[8].w = 20;
    party2[8].h = 300;
    party2[9].active = 1;
    party2[9].x = 450;
    party2[9].y = 394;
    party2[9].w = 152;
    party2[9].h = 20;
    party2[10].active = 1;
    party2[10].x = 450;
    party2[10].y = 244;
    party2[10].w = 152;
    party2[10].h = 20;
    party2[11].active = 1;
    party2[11].x = 450;
    party2[11].y = 394;
    party2[11].w = 155;
    party2[11].h = 20;
}

void bblocks2()
{
    bparty2[0].active = 1;
    bparty2[0].x = 800;
    bparty2[0].y = 94;
    bparty2[0].w = 80;
    bparty2[0].h = 60;
    bparty2[1].active = 1;
    bparty2[1].x = 920;
    bparty2[1].y = 94;
    bparty2[1].w = 80;
    bparty2[1].h = 60;
    int i;
    for (i = 2; i < 15; i++)
    {
        bparty2[i].active = 0;
        bparty2[i].x = 0;
        bparty2[i].y = 0;
        bparty2[i].w = 0;
        bparty2[i].h = 0;
    }
}

void bpigs2()
{
    piggy2[0].active = 1;
    piggy2[0].a = 835;
    piggy2[0].b = 180;
    piggy2[0].r = 15;
    piggy2[1].active = 1;
    piggy2[1].a = 970;
    piggy2[1].b = 200;
    piggy2[1].r = 25;
    piggy2[2].active = 0;
    piggy2[2].a = 0;
    piggy2[2].b = 0;
    piggy2[2].r = 0;
    piggy2[3].active = 1;
    piggy2[3].a = 700;
    piggy2[3].b = 120;
    piggy2[3].r = 15;
    piggy2[4].active = 1;
    piggy2[4].a = 970;
    piggy2[4].b = 365;
    piggy2[4].r = 25;
    piggy2[5].active = 1;
    piggy2[5].a = 970;
    piggy2[5].b = 500;
    piggy2[5].r = 25;
    piggy2[6].active = 1;
    piggy2[6].a = 835;
    piggy2[6].b = 360;
    piggy2[6].r = 25;
    piggy2[7].active = 1;
    piggy2[7].a = 540;
    piggy2[7].b = 137;
    piggy2[7].r = 25;
    piggy2[8].active = 1;
    piggy2[8].a = 540;
    piggy2[8].b = 320;
    piggy2[8].r = 25;
}
void itblocks3()
{
    party3[0].active = 1;
    party3[0].x = 750;
    party3[0].y = 94;
    party3[0].w = 20;
    party3[0].h = 200;
    party3[1].active = 1;
    party3[1].x = 885;
    party3[1].y = 94;
    party3[1].w = 20;
    party3[1].h = 350;
    party3[2].active = 1;
    party3[2].x = 750;
    party3[2].y = 294;
    party3[2].w = 260;
    party3[2].h = 20;
    party3[3].active = 1;
    party3[3].x = 570;
    party3[3].y = 94;
    party3[3].w = 20;
    party3[3].h = 100;
    party3[4].active = 1;
    party3[4].x = 750;
    party3[4].y = 94;
    party3[4].w = 20;
    party3[4].h = 100;
    party3[5].active = 1;
    party3[5].x = 570;
    party3[5].y = 194;
    party3[5].w = 180;
    party3[5].h = 20;
    party3[6].active = 1;
    party3[6].x = 1010;
    party3[6].y = 94;
    party3[6].w = 20;
    party3[6].h = 350;
    party3[7].active = 1;
    party3[7].x = 885;
    party3[7].y = 424;
    party3[7].w = 145;
    party3[7].h = 20;
    party3[8].active = 1;
    party3[8].x = 430;
    party3[8].y = 244;
    party3[8].w = 20;
    party3[8].h = 170;
    party3[9].active = 1;
    party3[9].x = 570;
    party3[9].y = 94;
    party3[9].w = 20;
    party3[9].h = 170;
    party3[10].active = 1;
    party3[10].x = 450;
    party3[10].y = 244;
    party3[10].w = 125;
    party3[10].h = 20;
    party3[11].active = 1;
    party3[11].x = 380;
    party3[11].y = 94;
    party3[11].w = 20;
    party3[11].h = 170;
    party3[12].active = 1;
    party3[12].x = 380;
    party3[12].y = 244;
    party3[12].w = 125;
    party3[12].h = 20;
    party3[13].active = 1;
    party3[13].x = 430;
    party3[13].y = 410;
    party3[13].w = 355;
    party3[13].h = 20;
    party3[14].active = 1;
    party3[14].x = 780;
    party3[14].y = 294;
    party3[14].w = 20;
    party3[14].h = 136;
}

void bblocks3()
{
    bparty3[0].active = 1;
    bparty3[0].x = 800;
    bparty3[0].y = 94;
    bparty3[0].w = 80;
    bparty3[0].h = 60;
    bparty3[1].active = 1;
    bparty3[1].x = 920;
    bparty3[1].y = 94;
    bparty3[1].w = 80;
    bparty3[1].h = 60;

    int i;
    for (i = 2; i < 50; i++)
    {
        bparty3[i].active = 0;
        bparty3[i].x = 0;
        bparty3[i].y = 0;
        bparty3[i].w = 0;
        bparty3[i].h = 0;
    }
}

void bpigs3()
{
    piggy3[0].active = 1;
    piggy3[0].a = 835;
    piggy3[0].b = 190;
    piggy3[0].r = 15;
    piggy3Types[0] = NORMAL_PIG;
    piggy3Health[0] = 1;
    piggy3RecentlyHit[0] = 0;
    /* Strong pig (takes 2 hits) */
    piggy3[1].active = 1;
    piggy3[1].a = 970;
    piggy3[1].b = 210;
    piggy3[1].r = 25;
    piggy3Types[1] = STRONG_PIG;
    piggy3Health[1] = 2;
    piggy3RecentlyHit[1] = 0;
    piggy3[7].active = 1;
    piggy3[7].a = 470;
    piggy3[7].b = 144;
    piggy3[7].r = 25;
    piggy3Types[7] = STRONG_PIG;
    piggy3Health[7] = 2;
    piggy3RecentlyHit[7] = 0;
    piggy3[9].active = 1;
    piggy3[9].a = 670;
    piggy3[9].b = 264;
    piggy3[9].r = 25;
    piggy3Types[9] = STRONG_PIG;
    piggy3Health[9] = 2;
    piggy3RecentlyHit[9] = 0;
    /* Inactive pig */
    piggy3[2].active = 0;
    piggy3[2].a = 0;
    piggy3[2].b = 0;
    piggy3[2].r = 0;
    piggy3Types[2] = NORMAL_PIG;
    piggy3Health[2] = 1;
    piggy3RecentlyHit[2] = 0;
    /* Small pig (harder to hit) */
    piggy3[3].active = 1;
    piggy3[3].a = 700;
    piggy3[3].b = 115;
    piggy3[3].r = 10;
    piggy3Types[3] = SMALL_PIG;
    piggy3Health[3] = 1;
    piggy3RecentlyHit[3] = 0;
    piggy3[8].active = 1;
    piggy3[8].a = 610;
    piggy3[8].b = 115;
    piggy3[8].r = 10;
    piggy3Types[8] = SMALL_PIG;
    piggy3Health[8] = 1;
    piggy3RecentlyHit[8] = 0;
    /* Normal pig */
    piggy3[4].active = 1;
    piggy3[4].a = 970;
    piggy3[4].b = 375;
    piggy3[4].r = 25;
    piggy3Types[4] = NORMAL_PIG;
    piggy3Health[4] = 1;
    piggy3RecentlyHit[4] = 0;
    /* Normal pig */
    piggy3[5].active = 1;
    piggy3[5].a = 970;
    piggy3[5].b = 510;
    piggy3[5].r = 25;
    piggy3Types[5] = NORMAL_PIG;
    piggy3Health[5] = 1;
    piggy3RecentlyHit[5] = 0;
    piggy3[10].active = 1;
    piggy3[10].a = 540;
    piggy3[10].b = 314;
    piggy3[10].r = 25;
    piggy3Types[10] = NORMAL_PIG;
    piggy3Health[10] = 1;
    piggy3RecentlyHit[10] = 0;
    piggy3[11].active = 1;
    piggy3[11].a = 670;
    piggy3[11].b = 474;
    piggy3[11].r = 25;
    piggy3Types[11] = NORMAL_PIG;
    piggy3Health[11] = 1;
    piggy3RecentlyHit[11] = 0;
    /* Flying pig (special bonus)*/
    piggy3[6].active = 1;
    piggy3[6].a = 835;
    piggy3[6].b = 650;
    piggy3[6].r = 25;
    piggy3Types[6] = NORMAL_PIG;
    piggy3Health[6] = 1;
    piggy3RecentlyHit[6] = 0;
}

void saveLevelState(int level)
{
    char filename[100];
    FILE *file;

    if (strlen(currentuser) == 0)
        return;

    sprintf(filename, "gamesave/%s_level%d_state.txt", currentuser, level);
    file = fopen(filename, "w");
    if (!file)
        return;

    if (level == 1)
    {
        for (int i = 0; i < 15; i++)
        {
            fprintf(file, "%d\n", party[i].active);
            fprintf(file, "%d\n", bparty[i].active);
            fprintf(file, "%d\n", piggy[i].active);
        }
    }
    else if (level == 2)
    {
        for (int i = 0; i < 15; i++)
        {
            fprintf(file, "%d\n", party2[i].active);
            fprintf(file, "%d\n", bparty2[i].active);
            fprintf(file, "%d\n", piggy2[i].active);
        }
    }
    else if (level == 3)
    {
        for (int i = 0; i < 15; i++)
        {
            fprintf(file, "%d\n", party3[i].active);
            fprintf(file, "%d\n", piggy3[i].active);
        }
        for (int i = 0; i < 50; i++)
        {
            fprintf(file, "%d\n", bparty3[i].active);
        }
    }

    fprintf(file, "%d\n", score);
    fprintf(file, "%d\n", totalbirdsused);
    fprintf(file, "%d\n", bonusawarded);

    fclose(file);
}

void deleteLevelState(int level)
{
    char filename[100];
    sprintf(filename, "gamesave/%s_level%d_state.txt", currentuser, level);
    remove(filename);
}

void updateFlyingPig()
{
    if (page != 6)
        return;

    if (piggy3[6].active)
    {
        if (flyingpigdirection)
        {
            flyingpigx += flyingpigspeed;
            if (flyingpigx > 1150)
            {
                flyingpigdirection = 0;
            }
        }
        else
        {
            flyingpigx -= flyingpigspeed;
            if (flyingpigx < 130)
            {
                flyingpigdirection = 1;
            }
        }
        piggy3[6].a = flyingpigx;
        piggy3[6].b = flyingpigy;
    }
}

void checkTopBlockFall()
{
    if (!topblock1falling && party[0].active == 0 && party[1].active == 0 && party[2].active)
    {
        topblock1falling = 1;
    }
    if (topblock1falling && party[2].active)
    {
        party[2].y -= g * 20;
        if (party[2].y <= GROUND_HEIGHT)
        {
            party[2].y = GROUND_HEIGHT;
        }
    }

    if (!topblock2falling && party[3].active == 0 && party[4].active == 0 && party[5].active)
    {
        topblock2falling = 1;
    }
    if (topblock2falling && party[5].active)
    {
        party[5].y -= g * 20;
        if (party[5].y <= GROUND_HEIGHT)
        {
            party[5].y = GROUND_HEIGHT;
        }
    }

    if (!topblock3falling && party[6].active == 0 && party[1].active == 0 && party[7].active)
    {
        topblock3falling = 1;
    }
    if (topblock3falling && party[7].active)
    {
        party[7].y -= g * 20;
        if (party[7].y <= GROUND_HEIGHT)
        {
            party[7].y = GROUND_HEIGHT;
        }
    }
}
void iClose()
{
    if (strlen(currentuser) > 0)
    {
        saveLevelState(1);
        saveLevelState(2);
        saveLevelState(3);
    }
}
void checkCircleFall()
{
    if (!circle1falling && bparty[0].active == 0 && piggy[0].active)
    {
        circle1falling = 1;
    }
    if (circle1falling && piggy[0].active)
    {
        piggy[0].b -= g * 15;
        double pigvisualradius = getPigVisualRadius(piggy[0].r);
        if (piggy[0].b - pigvisualradius <= GROUND_HEIGHT)
        {
            piggy[0].b = GROUND_HEIGHT + pigvisualradius;
            circle1falling = 0;
        }
    }

    if (!circle2falling && bparty[1].active == 0 && piggy[1].active)
    {
        circle2falling = 1;
    }
    if (circle2falling && piggy[1].active)
    {
        piggy[1].b -= g * 15;
        double pigvisualradius = getPigVisualRadius(piggy[1].r);
        if (piggy[1].b - pigvisualradius <= GROUND_HEIGHT)
        {
            piggy[1].b = GROUND_HEIGHT + pigvisualradius;
            circle2falling = 0;
        }
    }

    int i;
    for (i = 3; i < 7; i++)
    {
        if (piggy[i].active && piggy[i].b > GROUND_HEIGHT + getPigVisualRadius(piggy[i].r))
        {
            int supported = 0;
            int j;

            for (j = 0; j < 15; j++)
            {
                if (party[j].active &&
                    piggy[i].a >= party[j].x && piggy[i].a <= party[j].x + party[j].w &&
                    piggy[i].b - piggy[i].r <= party[j].y + party[j].h + 5 &&
                    piggy[i].b - piggy[i].r >= party[j].y + party[j].h - 5)
                {
                    supported = 1;
                    break;
                }
            }

            if (!supported)
            {
                for (j = 0; j < 15; j++)
                {
                    if (bparty[j].active &&
                        piggy[i].a >= bparty[j].x && piggy[i].a <= bparty[j].x + bparty[j].w &&
                        piggy[i].b - piggy[i].r <= bparty[j].y + bparty[j].h + 5 &&
                        piggy[i].b - piggy[i].r >= bparty[j].y + bparty[j].h - 5)
                    {
                        supported = 1;
                        break;
                    }
                }
            }

            if (!supported)
            {
                piggy[i].b -= g * 15;
                double pigvisualradius = getPigVisualRadius(piggy[i].r);
                if (piggy[i].b - pigvisualradius <= GROUND_HEIGHT)
                {
                    piggy[i].b = GROUND_HEIGHT + pigvisualradius;
                }
            }
        }
    }
}

void checkTopBlockFall2()
{

    if (!topblock1falling2 && party2[0].active == 0 && party2[1].active == 0 && party2[2].active)
    {
        topblock1falling2 = 1;
    }
    if (topblock1falling2 && party2[2].active)
    {
        party2[2].y -= g * 20;
        if (party2[2].y <= GROUND_HEIGHT)
        {
            party2[2].y = GROUND_HEIGHT;
            topblock1falling2 = 0;
        }
    }

    if (!topblock2falling2 && party2[3].active == 0 && party2[4].active == 0 && party2[5].active)
    {
        topblock2falling2 = 1;
    }
    if (topblock2falling2 && party2[5].active)
    {
        party2[5].y -= g * 20;
        if (party2[5].y <= GROUND_HEIGHT)
        {
            party2[5].y = GROUND_HEIGHT;
            topblock2falling2 = 0;
        }
    }

    if (!topblock3falling2 && party2[1].active == 0 && party2[6].active == 0 && party2[7].active)
    {
        topblock3falling2 = 1;
    }
    if (topblock3falling2 && party2[7].active)
    {
        party2[7].y -= g * 20;
        if (party2[7].y <= GROUND_HEIGHT)
        {
            party2[7].y = GROUND_HEIGHT;
            topblock3falling2 = 0;
        }
    }

    if (!topblock4falling2 && party2[8].active == 0 && party2[9].active)
    {
        topblock4falling2 = 1;
    }
    if (topblock4falling2 && party2[9].active)
    {
        party2[9].y -= g * 20;
        if (party2[9].y <= GROUND_HEIGHT)
        {
            party2[9].y = GROUND_HEIGHT;
            topblock4falling2 = 0;
        }
    }

    int i;
    for (i = 10; i < 12; i++)
    {
        if (party2[i].active && party2[8].active == 0)
        {
            party2[i].y -= g * 15;
            if (party2[i].y <= GROUND_HEIGHT)
            {
                party2[i].y = GROUND_HEIGHT;
            }
        }
    }
}
void checkCircleFall2()
{
    int i, j;
    int supported;

    for (i = 0; i < 9; i++)
    {
        if (piggy2[i].active && piggy2[i].b > GROUND_HEIGHT + getPigVisualRadius(piggy2[i].r))
        {
            supported = 0;

            for (j = 0; j < 15; j++)
            {
                if (party2[j].active &&
                    piggy2[i].a >= party2[j].x && piggy2[i].a <= party2[j].x + party2[j].w &&
                    piggy2[i].b - piggy2[i].r <= party2[j].y + party2[j].h + 5 &&
                    piggy2[i].b - piggy2[i].r >= party2[j].y + party2[j].h - 5)
                {
                    supported = 1;
                    break;
                }
            }

            if (!supported)
            {
                for (j = 0; j < 15; j++)
                {
                    if (bparty2[j].active &&
                        piggy2[i].a >= bparty2[j].x && piggy2[i].a <= bparty2[j].x + bparty2[j].w &&
                        piggy2[i].b - piggy2[i].r <= bparty2[j].y + bparty2[j].h + 5 &&
                        piggy2[i].b - piggy2[i].r >= bparty2[j].y + bparty2[j].h - 5)
                    {
                        supported = 1;
                        break;
                    }
                }

                if (!supported)
                {
                    piggy2[i].b -= g * 15;
                    double pigvisualradius = getPigVisualRadius(piggy2[i].r);
                    if (piggy2[i].b - pigvisualradius <= GROUND_HEIGHT)
                    {
                        piggy2[i].b = GROUND_HEIGHT + pigvisualradius;
                    }
                }
            }
        }
    }
}
void checkTopBlockFall3()
{

    if (!topblock1falling3 && party3[0].active == 0 && party3[1].active == 0 && party3[2].active)
    {
        topblock1falling3 = 1;
    }
    if (topblock1falling3 && party3[2].active)
    {
        party3[2].y -= g * 20;
        if (party3[2].y <= GROUND_HEIGHT)
        {
            party3[2].y = GROUND_HEIGHT;
            topblock1falling3 = 0;
        }
    }

    if (!topblock2falling3 && party3[3].active == 0 && party3[4].active == 0 && party3[5].active)
    {
        topblock2falling3 = 1;
    }
    if (topblock2falling3 && party3[5].active)
    {
        party3[5].y -= g * 20;
        if (party3[5].y <= GROUND_HEIGHT)
        {
            party3[5].y = GROUND_HEIGHT;
            topblock2falling3 = 0;
        }
    }

    if (!topblock3falling3 && party3[1].active == 0 && party3[6].active == 0 && party3[7].active)
    {
        topblock3falling3 = 1;
    }
    if (topblock3falling3 && party3[7].active)
    {
        party3[7].y -= g * 20;
        if (party3[7].y <= GROUND_HEIGHT)
        {
            party3[7].y = GROUND_HEIGHT;
            topblock3falling3 = 0;
        }
    }

    if (!topblock4falling3 && party3[8].active == 0 && party3[9].active == 0 && party3[10].active)
    {
        topblock4falling3 = 1;
    }
    if (topblock4falling3 && party3[10].active)
    {
        party3[10].y -= g * 20;
        if (party3[10].y <= GROUND_HEIGHT)
        {
            party3[10].y = GROUND_HEIGHT;
            topblock4falling3 = 0;
        }
    }
    if (!topblock5falling3 && party3[11].active == 0 && party3[12].active)
    {
        topblock5falling3 = 1;
    }
    if (topblock5falling3 && party3[12].active)
    {
        party3[12].y -= g * 20;
        if (party3[12].y <= GROUND_HEIGHT)
        {
            party3[12].y = GROUND_HEIGHT;
            topblock5falling3 = 0;
        }
    }

    if (party3[13].active)
    {

        int supportCount = 0;
        if (party3[8].active)
            supportCount++;
        if (party3[11].active)
            supportCount++;
        if (party3[1].active)
            supportCount++;
        if (party3[6].active)
            supportCount++;

        if (supportCount <= 2)
        {
            party3[13].y -= g * 15;
            if (party3[13].y <= GROUND_HEIGHT)
            {
                party3[13].y = GROUND_HEIGHT;
            }
        }
    }

    if (party3[14].active)
    {

        if (party3[2].active == 0 || (party3[0].active == 0 && party3[1].active == 0))
        {
            party3[14].y -= g * 15;
            if (party3[14].y <= GROUND_HEIGHT)
            {
                party3[14].y = GROUND_HEIGHT;
            }
        }
    }
}

void checkCircleFall3()
{
    int i, j;
    int supported;

    for (i = 0; i < 12; i++)
    {
        if (piggy3[i].active && piggy3[i].b > GROUND_HEIGHT + getPigVisualRadius(piggy3[i].r))
        {
            supported = 0;

            for (j = 0; j < 15; j++)
            {
                if (party3[j].active &&
                    piggy3[i].a >= party3[j].x && piggy3[i].a <= party3[j].x + party3[j].w &&
                    piggy3[i].b - piggy3[i].r <= party3[j].y + party3[j].h + 5 &&
                    piggy3[i].b - piggy3[i].r >= party3[j].y + party3[j].h - 5)
                {
                    supported = 1;
                    break;
                }
            }

            if (!supported)
            {
                for (j = 0; j < 50; j++)
                {
                    if (bparty3[j].active &&
                        piggy3[i].a >= bparty3[j].x && piggy3[i].a <= bparty3[j].x + bparty3[j].w &&
                        piggy3[i].b - piggy3[i].r <= bparty3[j].y + bparty3[j].h + 5 &&
                        piggy3[i].b - piggy3[i].r >= bparty3[j].y + bparty3[j].h - 5)
                    {
                        supported = 1;
                        break;
                    }
                }
            }

            if (!supported)
            {
                piggy3[i].b -= g * 15;
                double pigvisualradius = getPigVisualRadius(piggy3[i].r);
                if (piggy3[i].b - pigvisualradius <= GROUND_HEIGHT)
                {
                    piggy3[i].b = GROUND_HEIGHT + pigvisualradius;
                }
            }
        }
    }
}

void awardBonusIfNeeded(int partyArrSize, struct recta *partyArr)
{
    int i;
    int alldestroyed = 1;

    if (!bonusawarded)
    {
        for (i = 0; i < partyArrSize; i++)
        {
            if (partyArr[i].active)
            {
                alldestroyed = 0;
                break;
            }
        }
        if (alldestroyed && totalbirdsused < maxbirds)
        {

            score += 10000 * (maxbirds - totalbirdsused);
            bonusawarded = 1;
        }
    }
}

void change()
{
    int i, b;
    int currentBirdRadius = getbirdradius(currentbirdtype, currentbirdsize);

    if (gameOver || levelcompleted || showstopmenu || showlevelcompletedwithoutwarning)
    {
        return;
    }

    if (page == 1)
    {
        // Level 1
        checkCircleFall();
        checkTopBlockFall();
    }
    else if (page == 4)
    {
        // Level 2
        checkCircleFall2();
        checkTopBlockFall2();
    }
    else if (page == 6)
    {
        // Level 3 (includes flying pig)
        updateFlyingPig();
        checkCircleFall3();
        checkTopBlockFall3();
    }

    if ((page == 1 || page == 4 || page == 6) && check == 1)
    {
        // Play bird laugh sound when the bird is in motion
        if (timechange == 1)
        {
            playbirdlaughsound();
        }

        if (ballX < -50 || ballX > SCREEN_WIDTH + 50 ||
            ballY < -50 || ballY > SCREEN_HEIGHT + 50)
        {

            timechange = 201;
            bouncecount = maxbounces;
            vx = 0.0;
            vy = 0.0;
            for (int i = 0; i < 3; i++)
            {
                if (miniBirds[i].active)
                {
                    int mR = getbirdradius(BLUE_BIRD, radiusball);
                    if (checkBoundaries(miniBirds[i].x, miniBirds[i].y, mR))
                    {
                        miniBirds[i].active = 0;
                    }
                }
            }
        }

        if (page == 1)
        {
            //  Level 1 collisions
            for (i = 0; i < 15; i++)
            {
                if (checkRectCollision(ballX, ballY, currentBirdRadius, &party[i]))
                {
                    vx *= 0.7;
                    vy *= 0.7;
                    party[i].active = 0;
                    score += 1000;
                }
            }
            for (i = 0; i < 15; i++)
            {
                if (checkRectCollision(ballX, ballY, currentBirdRadius, &bparty[i]))
                {
                    vx *= 0.4;
                    vy *= 0.4;
                    bparty[i].active = 0;
                    score += 2000;
                }
            }
            for (i = 0; i < 15; i++)
            {
                if (checkCircleCollision(ballX, ballY, currentBirdRadius, &piggy[i]))
                {
                    vx *= 0.4;
                    vy *= 0.4;
                    piggy[i].active = 0;
                    score += 5000;
                    playpigdamagesound(); // Play pig damage sound
                }
            }
        }
        else if (page == 4)
        {
            // level 2
            for (i = 0; i < 15; i++)
            {
                if (checkRectCollision(ballX, ballY, currentBirdRadius, &party2[i]))
                {
                    vx *= 0.7;
                    vy *= 0.7;
                    party2[i].active = 0;
                    score += 1000;
                }
            }
            for (i = 0; i < 15; i++)
            {
                if (checkRectCollision(ballX, ballY, currentBirdRadius, &bparty2[i]))
                {
                    vx *= 0.4;
                    vy *= 0.4;
                    bparty2[i].active = 0;
                    score += 2000;
                }
            }
            for (i = 0; i < 15; i++)
            {
                if (checkCircleCollision(ballX, ballY, currentBirdRadius, &piggy2[i]))
                {
                    vx *= 0.4;
                    vy *= 0.4;
                    piggy2[i].active = 0;
                    score += 5000;
                    playpigdamagesound(); // Play pig damage sound
                }
            }
        }
        else /* page==6 */
        {
            // Level 3 collisions
            for (i = 0; i < 15; i++)
            {
                if (checkRectCollision(ballX, ballY, currentBirdRadius, &party3[i]))
                {
                    vx *= 0.7;
                    vy *= 0.7;
                    party3[i].active = 0;
                    score += 1000;
                }
            }
            for (i = 0; i < 50; i++)
            {
                if (checkRectCollision(ballX, ballY, currentBirdRadius, &bparty3[i]))
                {
                    vx *= 0.4;
                    vy *= 0.4;
                    bparty3[i].active = 0;
                    score += 2000;
                }
            }
            for (i = 0; i < 15; i++)
            {
                if (checkCircleCollision(ballX, ballY, currentBirdRadius, &piggy3[i]) && !piggy3RecentlyHit[i])
                {
                    vx *= 0.4;
                    vy *= 0.4;
                    piggy3RecentlyHit[i] = 1;
                    if (piggy3Types[i] == STRONG_PIG && piggy3Health[i] > 1)
                    {
                        piggy3Health[i] = 1;
                        piggy3Types[i] = NORMAL_PIG;
                        score += 5000;
                    }
                    else
                    {
                        piggy3[i].active = 0;
                        score += (i == 6 ? 8000 : 5000);
                        playpigdamagesound(); // Play pig damage sound
                    }
                }
            }
        }

        for (b = 0; b < 3; b++)
        {
            if (!miniBirds[b].active)
                continue;
            int mR = getbirdradius(BLUE_BIRD, radiusball);

            if (checkBoundaries(miniBirds[b].x, miniBirds[b].y, mR))
            {
                miniBirds[b].active = 0;
                continue;
            }

            if (page == 1)
            {
                for (i = 0; i < 15; i++)
                {
                    if (checkRectCollision(miniBirds[b].x, miniBirds[b].y, mR, &party[i]))
                    {
                        miniBirds[b].vx *= 0.7;
                        miniBirds[b].vy *= 0.7;
                        party[i].active = 0;
                        score += 1000;
                    }
                }
                for (i = 0; i < 15; i++)
                {
                    if (checkRectCollision(miniBirds[b].x, miniBirds[b].y, mR, &bparty[i]))
                    {
                        miniBirds[b].vx *= 0.4;
                        miniBirds[b].vy *= 0.4;
                        bparty[i].active = 0;
                        score += 2000;
                    }
                }
                for (i = 0; i < 15; i++)
                {
                    if (checkCircleCollision(miniBirds[b].x, miniBirds[b].y, mR, &piggy[i]))
                    {
                        miniBirds[b].vx *= 0.4;
                        miniBirds[b].vy *= 0.4;
                        piggy[i].active = 0;
                        score += 5000;
                        playpigdamagesound(); // Play pig damage sound for blue ball
                    }
                }
            }
            else if (page == 4)
            {
                for (i = 0; i < 15; i++)
                {
                    if (checkRectCollision(miniBirds[b].x, miniBirds[b].y, mR, &party2[i]))
                    {
                        miniBirds[b].vx *= 0.7;
                        miniBirds[b].vy *= 0.7;
                        party2[i].active = 0;
                        score += 1000;
                    }
                }
                for (i = 0; i < 15; i++)
                {
                    if (checkRectCollision(miniBirds[b].x, miniBirds[b].y, mR, &bparty2[i]))
                    {
                        miniBirds[b].vx *= 0.4;
                        miniBirds[b].vy *= 0.4;
                        bparty2[i].active = 0;
                        score += 2000;
                    }
                }
                for (i = 0; i < 15; i++)
                {
                    if (checkCircleCollision(miniBirds[b].x, miniBirds[b].y, mR, &piggy2[i]))
                    {
                        miniBirds[b].vx *= 0.4;
                        miniBirds[b].vy *= 0.4;
                        piggy2[i].active = 0;
                        score += 5000;
                        playpigdamagesound(); // Play pig damage sound for blue ball
                    }
                }
            }
            else /* page==6 */
            {
                for (i = 0; i < 15; i++)
                {
                    if (checkRectCollision(miniBirds[b].x, miniBirds[b].y, mR, &party3[i]))
                    {
                        miniBirds[b].vx *= 0.7;
                        miniBirds[b].vy *= 0.7;
                        party3[i].active = 0;
                        score += 1000;
                    }
                }
                for (i = 0; i < 50; i++)
                {
                    if (checkRectCollision(miniBirds[b].x, miniBirds[b].y, mR, &bparty3[i]))
                    {
                        miniBirds[b].vx *= 0.4;
                        miniBirds[b].vy *= 0.4;
                        bparty3[i].active = 0;
                        score += 2000;
                    }
                }
                for (i = 0; i < 15; i++)
                {
                    if (checkCircleCollision(miniBirds[b].x, miniBirds[b].y, mR, &piggy3[i]) && !piggy3RecentlyHit[i])
                    {
                        miniBirds[b].vx *= 0.4;
                        miniBirds[b].vy *= 0.4;
                        piggy3RecentlyHit[i] = 1;
                        if (piggy3Types[i] == STRONG_PIG && piggy3Health[i] > 1)
                        {
                            piggy3Health[i] = 1;
                            piggy3Types[i] = NORMAL_PIG;
                            score += 5000;
                        }
                        else
                        {
                            piggy3[i].active = 0;
                            score += (i == 6 ? 8000 : 5000);
                            playpigdamagesound(); // Play pig damage sound for blue ball
                        }
                    }
                }
            }
        }

        ballX += vx;
        ballY += vy;
        vy -= g;

        if (ballY - currentBirdRadius <= GROUND_HEIGHT)
        {
            ballY = GROUND_HEIGHT + currentBirdRadius;
            if (vy < 0)
            {
                vy = -vy * restitution;
                vx *= friction;
                bouncecount++;
                onground = 1;
            }
        }
        else
        {
            onground = 0;
        }

        xBall = (int)ballX;
        yBall = (int)ballY;

        for (i = 0; i < 3; i++)
        {
            if (!miniBirds[i].active)
                continue;
            int mR = getbirdradius(BLUE_BIRD, radiusball);
            miniBirds[i].x += miniBirds[i].vx;
            miniBirds[i].y += miniBirds[i].vy;
            miniBirds[i].vy -= g;

            if (miniBirds[i].y - mR <= GROUND_HEIGHT)
            {
                miniBirds[i].y = GROUND_HEIGHT + mR;
                if (miniBirds[i].vy < 0)
                {
                    miniBirds[i].vy *= -restitution;

                    miniBirds[i].vx *= friction;
                }
            }

            if (checkBoundaries(miniBirds[i].x, miniBirds[i].y, mR))
                miniBirds[i].active = 0;

            if (fabs(miniBirds[i].vx) < minvelocity && fabs(miniBirds[i].vy) < minvelocity)
                miniBirds[i].active = 0;
        }

        if (bouncecount >= maxbounces ||
            (onground && fabs(vx) < minvelocity && fabs(vy) < minvelocity))
        {
            if (onground)
            {
                ballY = GROUND_HEIGHT + currentBirdRadius;
                yBall = (int)ballY;
                vx = vy = 0;
            }

            if (timechange > 200)
            {

                timechange = 1;
                timemax = 0;
                check = 0;
                ballX = xCircle;
                ballY = yCircle;
                xBall = xCircle;
                yBall = yCircle;
                dragging = 0;
                bouncecount = 0;
                onground = 0;
                vx = vy = 0;
                hassplit = 0;
                hasspeedboost = 0;
                hassizeboost = 0;
                currentbirdsize = 5;
                for (i = 0; i < 3; i++)
                    miniBirds[i].active = 0;
                if (page == 6)
                {

                    for (i = 0; i < 15; i++)
                        piggy3RecentlyHit[i] = 0;
                }
                updatecurrentbirdtype();

                if (allpigsdestroyed())
                {
                    levelcompleted = 1;
                    deleteLevelState(currentlevel);
                    if (page == 1)
                        awardBonusIfNeeded(15, party);
                    else if (page == 4)
                        awardBonusIfNeeded(15, party2);
                    else
                        awardBonusIfNeeded(15, party3);

                    if (page == 1)
                    {
                        level1completed = 1;
                        if (score > level1score)
                            level1score = score;
                    }
                    else if (page == 4)
                    {
                        level2completed = 1;
                        if (score > level2score)
                            level2score = score;
                    }
                    else
                    {
                        level3completed = 1;
                        if (score > level3score)
                            level3score = score;
                    }

                    saveuserdata();
                    loadallusers();
                }
                else if (totalbirdsused >= maxbirds)
                {
                    gameOver = 1;
                    deleteLevelState(currentlevel);
                }
            }
        }
        timechange++;
    }
}

void drawBirdQueue()
{
    int i, drawX, drawY;

    iSetColor(255, 255, 255);
    iText(50, 780, "Birds:", GLUT_BITMAP_HELVETICA_18);

    for (i = 0; i < maxbirds; i++)
    {
        drawX = 120 + i * 60;
        drawY = 770;

        if (i < totalbirdsused)
        {
            iSetColor(100, 100, 100);
            iFilledCircle(drawX, drawY, 12);
        }
        else
        {
            drawBirdSprite(drawX, drawY, birdsequence[i], 3);
        }
    }

    // Display current score
    char scoreText[50];
    sprintf(scoreText, "Score: %d", score);
    iSetColor(30, 30, 30);
    iFilledRectangle(1050, 800, 200, 38);
    iSetColor(255, 255, 0);
    iText(1070, 810, scoreText, GLUT_BITMAP_HELVETICA_18);
}

void drawStopMenuButton()
{
    iShowImage(590, 750, "assets/images/stopmenu.png");
}

void drawGameOverScreen()
{

    iSetColor(0, 0, 0);
    iFilledRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    iSetColor(255, 0, 0);
    iText(580, 480, "GAME OVER", GLUT_BITMAP_TIMES_ROMAN_24);

    iSetColor(100, 100, 100);
    iFilledRectangle(490, 350, 300, 150);
    iSetColor(255, 255, 255);
    iText(580, 440, "Try Again?", GLUT_BITMAP_HELVETICA_18);

    iSetColor(0, 255, 0);
    iFilledRectangle(530, 380, 80, 40);
    iSetColor(0, 0, 0);
    iText(565, 395, "Yes", GLUT_BITMAP_HELVETICA_12);

    iSetColor(255, 0, 0);
    iFilledRectangle(670, 380, 80, 40);
    iSetColor(255, 255, 255);
    iText(708, 395, "No", GLUT_BITMAP_HELVETICA_12);
}

void drawLevelCompletedScreen()
{
    char userText[100];
    char scoreText[100];
    char highScoreText[100];
    char totalHighText[100];
    int levelHighScore;

    iSetColor(0, 0, 0);
    iFilledRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    iSetColor(0, 255, 0);
    iText(520, 650, "LEVEL COMPLETED!", GLUT_BITMAP_TIMES_ROMAN_24);

    sprintf(userText, "Player: %s", currentuser);
    iSetColor(255, 255, 255);
    iText(550, 620, userText, GLUT_BITMAP_HELVETICA_18);

    sprintf(scoreText, "Your Score: %d", score);
    iSetColor(255, 255, 0);
    iText(570, 590, scoreText, GLUT_BITMAP_HELVETICA_18);

    /* Show high score for current level */
    levelHighScore = (currentlevel == 1) ? highestscores[1] : (currentlevel == 2) ? highestscores[2]
                                                                                  : highestscores[3];
    sprintf(highScoreText, "Level %d High Score: %d", currentlevel, levelHighScore);
    iSetColor(255, 165, 0);
    iText(540, 560, highScoreText, GLUT_BITMAP_HELVETICA_18);

    /* Show total high score */
    sprintf(totalHighText, "Total High Score: %d", highestscores[0]);
    iSetColor(255, 165, 0);
    iText(550, 530, totalHighText, GLUT_BITMAP_HELVETICA_18);

    if (level1completed && level2completed && level3completed)
    {
        iText(540, 440, "All Levels Complete!", GLUT_BITMAP_HELVETICA_18);

        /* Play Again button (left) */
        iSetColor(255, 165, 0);
        iFilledRectangle(500, 380, 100, 40);
        iSetColor(0, 0, 0);
        iText(525, 395, "Play Again", GLUT_BITMAP_HELVETICA_12);

        /* Level Select button (right) */
        iSetColor(0, 0, 255);
        iFilledRectangle(640, 380, 100, 40);
        iSetColor(255, 255, 255);
        iText(665, 395, "Level Select", GLUT_BITMAP_HELVETICA_12);
    }
    else
    {
        /* Shows three options - Play Again, Next Level, Level Select */
        iText(580, 470, "Choose Action:", GLUT_BITMAP_HELVETICA_18);

        /* Play Again button (left) */
        iSetColor(255, 165, 0);
        iFilledRectangle(460, 400, 100, 40);
        iSetColor(0, 0, 0);
        iText(485, 415, "Play Again", GLUT_BITMAP_HELVETICA_12);

        /* Next Level button (center) */
        iSetColor(0, 255, 0);
        iFilledRectangle(580, 400, 100, 40);
        iSetColor(0, 0, 0);
        iText(605, 415, "Next Level", GLUT_BITMAP_HELVETICA_12);

        /* Level Select button (right) */
        iSetColor(255, 0, 0);
        iFilledRectangle(700, 400, 100, 40);
        iSetColor(255, 255, 255);
        iText(720, 415, "Level Select", GLUT_BITMAP_HELVETICA_12);
    }
}

void drawStopMenuOverlay()
{

    iSetColor(0, 0, 0);
    iFilledRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    iSetColor(100, 100, 100);
    iFilledRectangle(440, 250, 400, 400);

    iSetColor(255, 255, 255);
    iText(610, 520, "PAUSED", GLUT_BITMAP_TIMES_ROMAN_24);

    iSetColor(200, 50, 50);
    iFilledRectangle(580, 460, 120, 40);
    iSetColor(255, 255, 255);
    iText(620, 475, "Main Menu", GLUT_BITMAP_HELVETICA_12);

    iSetColor(50, 200, 50);
    iFilledRectangle(580, 410, 120, 40);
    iSetColor(255, 255, 255);
    iText(615, 425, "Level Select", GLUT_BITMAP_HELVETICA_12);

    iSetColor(50, 50, 200);
    iFilledRectangle(580, 360, 120, 40);
    iSetColor(255, 255, 255);
    iText(625, 375, "Resume", GLUT_BITMAP_HELVETICA_12);

    iSetColor(255, 0, 0);
    iFilledRectangle(530, 310, 220, 40);
    iSetColor(255, 255, 255);
    if (showNerdStats)
    {
        iText(570, 325, "STAT FOR NERDS (ON)", GLUT_BITMAP_HELVETICA_12);
    }
    else
    {
        iText(570, 325, "STAT FOR NERDS (OFF)", GLUT_BITMAP_HELVETICA_12);
    }
}

void drawlevelcompletedwarningscreen()
{
    char levelText[100];
    iSetColor(0, 0, 0);
    iFilledRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    iSetColor(255, 165, 0);
    sprintf(levelText, "YOU HAVE COMPLETED LEVEL %d", warninglevel);
    iText(480, 550, levelText, GLUT_BITMAP_TIMES_ROMAN_24);

    iSetColor(100, 100, 100);
    iFilledRectangle(490, 350, 300, 150);
    iSetColor(255, 255, 255);
    iText(570, 440, "Play Again?", GLUT_BITMAP_HELVETICA_18);

    iSetColor(0, 255, 0);
    iFilledRectangle(530, 380, 80, 40);
    iSetColor(0, 0, 0);
    iText(565, 395, "Yes", GLUT_BITMAP_HELVETICA_12);

    iSetColor(255, 0, 0);
    iFilledRectangle(670, 380, 80, 40);
    iSetColor(255, 255, 255);
    iText(708, 395, "No", GLUT_BITMAP_HELVETICA_12);
}

void drawNameInputScreen()
{
    static int cursorBlink = 0;

    iSetColor(0, 0, 0);
    iFilledRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    iSetColor(255, 255, 255);
    iText(500, 500, "ENTER YOUR NAME", GLUT_BITMAP_TIMES_ROMAN_24);

    iSetColor(100, 100, 100);
    iFilledRectangle(440, 400, 400, 60);

    iSetColor(255, 255, 255);
    iText(460, 425, nameinput, GLUT_BITMAP_HELVETICA_18);
    cursorBlink++;
    if ((cursorBlink / 30) % 2 == 0)
    {
        iText(460 + strlen(nameinput) * 10, 425, "|", GLUT_BITMAP_HELVETICA_18);
    }

    iSetColor(255, 255, 255);
    iText(480, 350, "Press ENTER to continue", GLUT_BITMAP_HELVETICA_18);
    iText(520, 320, "Press ESC to go back", GLUT_BITMAP_HELVETICA_18);
}

void drawLeaderboardScreen()
{
    int yPos, maxUsersToShow, i;
    char rankText[10], level1Text[20], level2Text[20], level3Text[20], totalText[20];
    struct userscore *user;

    iSetColor(0, 0, 0);
    iFilledRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    iSetColor(255, 255, 0);
    iText(580, 800, "LEADERBOARD", GLUT_BITMAP_TIMES_ROMAN_24);

    iSetColor(255, 255, 255);
    iText(200, 750, "Player", GLUT_BITMAP_HELVETICA_18);
    iText(400, 750, "Level 1", GLUT_BITMAP_HELVETICA_18);
    iText(500, 750, "Level 2", GLUT_BITMAP_HELVETICA_18);
    iText(600, 750, "Level 3", GLUT_BITMAP_HELVETICA_18);
    iText(700, 750, "Total", GLUT_BITMAP_HELVETICA_18);
    iSetColor(128, 128, 128);
    iLine(150, 740, 1100, 740);

    yPos = 700;
    maxUsersToShow = (totalusers < 15) ? totalusers : 15;
    for (i = 0; i < maxUsersToShow; i++)
    {
        user = &alluserscores[i];
        if (strcmp(user->username, currentuser) == 0)
        {
            iSetColor(255, 255, 0);
        }
        else
        {
            iSetColor(255, 255, 255);
        }

        sprintf(rankText, "%d.", i + 1);
        iText(150, yPos, rankText, GLUT_BITMAP_HELVETICA_12);

        iText(200, yPos, user->username, GLUT_BITMAP_HELVETICA_12);

        sprintf(level1Text, "%d", user->level1score);
        sprintf(level2Text, "%d", user->level2score);
        sprintf(level3Text, "%d", user->level3score);
        sprintf(totalText, "%d", user->totalscore);

        iText(400, yPos, level1Text, GLUT_BITMAP_HELVETICA_12);
        iText(500, yPos, level2Text, GLUT_BITMAP_HELVETICA_12);
        iText(600, yPos, level3Text, GLUT_BITMAP_HELVETICA_12);
        iText(700, yPos, totalText, GLUT_BITMAP_HELVETICA_12);

        yPos -= 40;
    }
    iSetColor(200, 50, 50);
    iFilledRectangle(590, 100, 100, 40);
    iSetColor(255, 255, 255);
    iText(625, 115, "Back", GLUT_BITMAP_HELVETICA_12);
}

Image bg;
int channel = -1;
int currentvolume = 80;
int Paused = 1;

void iDraw()
{
    int i;
    double tGap, t, dotX, dotY;
    char scoreText[50];
    char userText[100];
    char scoresText[200];
    iClear();

    if (showstopmenu)
    {
        drawStopMenuOverlay();
        return;
    }
    else if (gameOver)
    {
        drawGameOverScreen();
        return;
    }
    else if (levelcompleted)
    {
        drawLevelCompletedScreen();
        return;
    }
    else if (showlevelcompletedwithoutwarning)
    {
        drawlevelcompletedwarningscreen();
        return;
    }

    if (page == 0) /* Main Menu */
    {
        iShowImage(0, 0, "assets/images/ABBG.png");
    }
    else if (page == 7) /* Name Input Screen */
    {
        drawNameInputScreen();
    }
    else if (page == 8) /* Leaderboard Screen */
    {
        drawLeaderboardScreen();
    }
    else if (page == 9) /* How to Play */
    {
        iShowImage(0, 0, "assets/images/ABBG.png");
        iShowImage(400, 150, "assets/images/howtoplay.jpg");
    }
    else if (page == 10) /* Controls */
    {
        iShowImage(0, 0, "assets/images/ABBG.png");
        iShowImage(400, 150, "assets/images/controls.png");
    }
    else if (page == 11) /* Tips */
    {
        iShowImage(0, 0, "assets/images/ABBG.png");
        iShowImage(400, 150, "assets/images/tips.png");
    }
    else if (page == 1) /* Level 1 */
    {
        iText(550, 750, "Level 1 - Enhanced Castle - Press 'q' to return", GLUT_BITMAP_TIMES_ROMAN_24);

        for (i = 0; i < 204; i++)
        {
            iSetColor(i, 102 + i, 204 + i);
            iFilledRectangle(0, 853 - (i * 4), 1280, 4);
        }

        iSetColor(153, 76, 0);
        for (i = 0; i < 15; i++)
        {
            if (party[i].active)
            {
                iFilledRectangle(party[i].x, party[i].y, party[i].w, party[i].h);
            }
        }
        iSetColor(88, 179, 240);
        for (i = 0; i < 15; i++)
        {
            if (bparty[i].active)
            {
                iFilledRectangle(bparty[i].x, bparty[i].y, bparty[i].w, bparty[i].h);
            }
        }

        for (i = 0; i < 15; i++)
        {
            if (piggy[i].active)
            {
                drawPigSprite(piggy[i].a, piggy[i].b, piggy[i].r);
            }
        }
        
        iSetColor(255, 255, 255);
        iFilledEllipse(100, 780, 90, 35);
        iFilledEllipse(150, 800, 70, 30);
        iFilledEllipse(50, 800, 60, 25);
        iFilledEllipse(100, 820, 80, 30);
        iFilledEllipse(350, 720, 100, 40);
        iFilledEllipse(400, 740, 80, 35);
        iFilledEllipse(300, 740, 70, 30);
        iFilledEllipse(350, 760, 90, 35);
        iFilledEllipse(600, 780, 110, 42);
        iFilledEllipse(650, 800, 90, 38);
        iFilledEllipse(550, 800, 80, 33);
        iFilledEllipse(600, 820, 100, 37);
        iFilledEllipse(850, 720, 95, 38);
        iFilledEllipse(900, 740, 75, 33);
        iFilledEllipse(800, 740, 65, 28);
        iFilledEllipse(850, 760, 85, 33);
        iFilledEllipse(1100, 780, 85, 35);
        iFilledEllipse(1150, 800, 65, 30);
        iFilledEllipse(1050, 800, 55, 25);
        iFilledEllipse(1100, 820, 75, 30);
        iFilledEllipse(200, 650, 60, 25);
        iFilledEllipse(250, 670, 40, 20);
        iFilledEllipse(150, 670, 50, 22);
        iFilledEllipse(500, 680, 55, 23);
        iFilledEllipse(550, 700, 35, 18);
        iFilledEllipse(450, 700, 45, 20);
        iFilledEllipse(800, 650, 50, 22);
        iFilledEllipse(850, 670, 30, 17);
        iFilledEllipse(750, 670, 40, 19);
        iFilledEllipse(1100, 680, 45, 21);
        iFilledEllipse(1150, 700, 25, 16);
        iFilledEllipse(1050, 700, 35, 18);

        iSetColor(51, 25, 0);
        iFilledRectangle(0, 0, 1280, GROUND_HEIGHT);

        for (i = 0; i < 30; i++)
        {
            iSetColor(51 + (i * 4), 25 + (i * 2), i);
            iFilledRectangle(0, i * 3, 1280, 3);
        }

        tGap = 1;
        t = 0;
        while (1)
        {
            dotX = xCircle + -v * cos(theta * PI / 180) * t;
            dotY = yCircle + -v * sin(theta * PI / 180) * t - 0.5 * g * t * t;
            if (dotY <= 90)
                break;
            iSetColor(0, 0, 0);
            iPoint(dotX, dotY);
            t += tGap;
        }
        if ((!hassplit || !issplittingbird) &&
            ballX >= 0 && ballX <= SCREEN_WIDTH &&
            ballY >= 0 && ballY <= SCREEN_HEIGHT)
        {
            drawBirdSprite(xBall, yBall, currentbirdtype, currentbirdsize);
        }
        for (i = 0; i < 3; i++)
        {
            if (miniBirds[i].active)
            {
                drawBirdSprite((int)miniBirds[i].x, (int)miniBirds[i].y, BLUE_BIRD, radiusball);
            }
        }

        if (!check || dragging)
        {
            iSetColor(100, 50, 0);
            iLine(slingLeftX, slingLeftY, xBall, yBall);
            iLine(slingRightX, slingRightY, xBall, yBall);
        }
        else
        {
            iSetColor(100, 50, 0);
            iLine(slingLeftX, slingLeftY, xCircle, yCircle);
            iLine(slingRightX, slingRightY, xCircle, yCircle);
        }

        iSetColor(80, 42, 0);
        iFilledRectangle(xCircle - 5, yCircle - 40, 10, 40);
        iFilledRectangle(slingLeftX - 3, slingLeftY - 3, 6, 6);
        iFilledRectangle(slingRightX - 3, slingRightY - 3, 6, 6);

        drawBirdQueue();
        drawStopMenuButton();

        sprintf(scoreText, "Score: %d", score);
        iSetColor(30, 30, 30);
        iFilledRectangle(1050, 800, 200, 38);
        iSetColor(255, 255, 0);
        iText(1070, 810, scoreText, GLUT_BITMAP_HELVETICA_18);

        /* Show user info */
        sprintf(userText, "Player: %s", currentuser);
        iSetColor(255, 255, 255);
        iText(50, 50, userText, GLUT_BITMAP_HELVETICA_18);

        iSetColor(255, 255, 255);
    }
    else if (page == 12)
    {
        iShowImage(0, 0, "assets/images/ABBG.png");
        iShowImage(400, 150, "assets/images/credits.png");
    }
    else if (page == 2) /* Help Menu */
    {
        iShowImage(0, 0, "assets/images/ABBG.png");
        iSetColor(66, 135, 245);
        iFilledRectangle(465, 545, 350, 120);
        iSetColor(255, 255, 255);
        iText(570, 605, "How to Play", GLUT_BITMAP_TIMES_ROMAN_24);
        iSetColor(76, 175, 80);
        iFilledRectangle(465, 395, 350, 120);
        iSetColor(255, 255, 255);
        iText(555, 445, "Controls", GLUT_BITMAP_TIMES_ROMAN_24);
        iSetColor(244, 67, 54);
        iFilledRectangle(465, 245, 350, 120);
        iSetColor(255, 255, 255);
        iText(575, 295, "Tips", GLUT_BITMAP_TIMES_ROMAN_24);
        iSetColor(255, 255, 0);
        iText(520, 180, "Press 'q' to go back to the menu", GLUT_BITMAP_HELVETICA_18);
    }
    else if (page == 3) /* Sound Page */
    {
        iShowImage(0, 0, "assets/images/ABBG.png");
        iShowImage(400, 150, "assets/images/ABSB.png");
    }
    else if (page == 4) /* Level 2 */
    {
        iText(600, 400, "Level 2 - Press 'q' to return", GLUT_BITMAP_TIMES_ROMAN_24);

        for (i = 0; i < 204; i++)
        {
            iSetColor(i, 102 + i, 204 + i);
            iFilledRectangle(0, 853 - (i * 4), 1280, 4);
        }

        iSetColor(153, 76, 0);
        for (i = 0; i < 15; i++)
        {
            if (party2[i].active)
            {
                iFilledRectangle(party2[i].x, party2[i].y, party2[i].w, party2[i].h);
            }
        }
        iSetColor(88, 179, 240);
        for (i = 0; i < 15; i++)
        {
            if (bparty2[i].active)
            {
                iFilledRectangle(bparty2[i].x, bparty2[i].y, bparty2[i].w, bparty2[i].h);
            }
        }

        for (i = 0; i < 15; i++)
        {
            if (piggy2[i].active)
            {
                drawPigSprite(piggy2[i].a, piggy2[i].b, piggy2[i].r);
            }
        }

        
        iSetColor(255, 255, 255);
        iFilledEllipse(100, 780, 90, 35);
        iFilledEllipse(150, 800, 70, 30);
        iFilledEllipse(50, 800, 60, 25);
        iFilledEllipse(100, 820, 80, 30);
        iFilledEllipse(350, 720, 100, 40);
        iFilledEllipse(400, 740, 80, 35);
        iFilledEllipse(300, 740, 70, 30);
        iFilledEllipse(350, 760, 90, 35);
        iFilledEllipse(600, 780, 110, 42);
        iFilledEllipse(650, 800, 90, 38);
        iFilledEllipse(550, 800, 80, 33);
        iFilledEllipse(600, 820, 100, 37);
        iFilledEllipse(850, 720, 95, 38);
        iFilledEllipse(900, 740, 75, 33);
        iFilledEllipse(800, 740, 65, 28);
        iFilledEllipse(850, 760, 85, 33);
        iFilledEllipse(1100, 780, 85, 35);
        iFilledEllipse(1150, 800, 65, 30);
        iFilledEllipse(1050, 800, 55, 25);
        iFilledEllipse(1100, 820, 75, 30);
        iFilledEllipse(200, 650, 60, 25);
        iFilledEllipse(250, 670, 40, 20);
        iFilledEllipse(150, 670, 50, 22);
        iFilledEllipse(500, 680, 55, 23);
        iFilledEllipse(550, 700, 35, 18);
        iFilledEllipse(450, 700, 45, 20);
        iFilledEllipse(800, 650, 50, 22);
        iFilledEllipse(850, 670, 30, 17);
        iFilledEllipse(750, 670, 40, 19);
        iFilledEllipse(1100, 680, 45, 21);
        iFilledEllipse(1150, 700, 25, 16);
        iFilledEllipse(1050, 700, 35, 18);

        iSetColor(51, 25, 0);
        iFilledRectangle(0, 0, 1280, GROUND_HEIGHT);

        for (i = 0; i < 30; i++)
        {
            iSetColor(51 + (i * 4), 25 + (i * 2), i);
            iFilledRectangle(0, i * 3, 1280, 3);
        }

        tGap = 1;
        t = 0;
        while (1)
        {
            dotX = xCircle + -v * cos(theta * PI / 180) * t;
            dotY = yCircle + -v * sin(theta * PI / 180) * t - 0.5 * g * t * t;
            if (dotY <= 90)
                break;
            iSetColor(0, 0, 0);
            iPoint(dotX, dotY);
            t += tGap;
        }
        if ((!hassplit || !issplittingbird) &&
            ballX >= 0 && ballX <= SCREEN_WIDTH &&
            ballY >= 0 && ballY <= SCREEN_HEIGHT)
        {
            drawBirdSprite(xBall, yBall, currentbirdtype, currentbirdsize);
        }
        for (i = 0; i < 3; i++)
        {
            if (miniBirds[i].active)
            {
                drawBirdSprite((int)miniBirds[i].x, (int)miniBirds[i].y, BLUE_BIRD, radiusball);
            }
        }

        if (!check || dragging)
        {
            iSetColor(100, 50, 0);
            iLine(slingLeftX, slingLeftY, xBall, yBall);
            iLine(slingRightX, slingRightY, xBall, yBall);
        }
        else
        {
            iSetColor(100, 50, 0);
            iLine(slingLeftX, slingLeftY, xCircle, yCircle);
            iLine(slingRightX, slingRightY, xCircle, yCircle);
        }

        iSetColor(80, 42, 0);
        iFilledRectangle(xCircle - 5, yCircle - 40, 10, 40);
        iFilledRectangle(slingLeftX - 3, slingLeftY - 3, 6, 6);
        iFilledRectangle(slingRightX - 3, slingRightY - 3, 6, 6);

        drawBirdQueue();
        drawStopMenuButton();

        sprintf(scoreText, "Score: %d", score);
        iSetColor(30, 30, 30);
        iFilledRectangle(1050, 800, 200, 38);
        iSetColor(255, 255, 0);
        iText(1070, 810, scoreText, GLUT_BITMAP_HELVETICA_18);

        /* Show user info */
        sprintf(userText, "Player: %s", currentuser);
        iSetColor(255, 255, 255);
        iText(50, 50, userText, GLUT_BITMAP_HELVETICA_18);
        iSetColor(255, 255, 255);
    }
    else if (page == 5) /* Level Selection */
    {
        iShowImage(0, 0, "assets/images/levelpage.jpg");
        iSetColor(200, 50, 50);
        iFilledRectangle(20, 20, 100, 40);
        iSetColor(255, 255, 255);
        iText(52, 37, "Back", GLUT_BITMAP_HELVETICA_12);
        /* Leaderboard button on top right */
        iSetColor(255, 215, 0);
        iFilledRectangle(1100, 750, 150, 40);
        iSetColor(0, 0, 0);
        iText(1130, 765, "LEADERBOARD", GLUT_BITMAP_HELVETICA_12);
        /* Show current user info */
        sprintf(userText, "Welcome, %s!", currentuser);
        iSetColor(255, 255, 255);
        iText(50, 800, userText, GLUT_BITMAP_HELVETICA_18);
        /* Show user's scores */
        sprintf(scoresText, "Your Scores - Level 1: %d, Level 2: %d, Level 3: %d, Total: %d",
                level1score, level2score, level3score, level1score + level2score + level3score);
        iText(50, 770, scoresText, GLUT_BITMAP_HELVETICA_12);
    }
    else if (page == 6) /* Level 3*/
    {
        iText(500, 750, "Level 3 - Ultimate Challenge - Press 'q' to return", GLUT_BITMAP_TIMES_ROMAN_24);

        for (i = 0; i < 204; i++)
        {
            iSetColor(i, 102 + i, 204 + i);
            iFilledRectangle(0, 853 - (i * 4), 1280, 4);
        }

        iSetColor(64, 64, 64);
        for (i = 0; i < 3; i++)
        {
            if (bparty3[i].active)
            {
                iFilledRectangle(bparty3[i].x, bparty3[i].y, bparty3[i].w, bparty3[i].h);
            }
        }

        iSetColor(153, 76, 0);
        for (i = 0; i < 15; i++)
        {
            if (party3[i].active)
            {
                iFilledRectangle(party3[i].x, party3[i].y, party3[i].w, party3[i].h);
            }
        }

        iSetColor(173, 216, 230);
        for (i = 3; i < 50; i++)
        {
            if (bparty3[i].active)
            {
                iFilledRectangle(bparty3[i].x, bparty3[i].y, bparty3[i].w, bparty3[i].h);
            }
        }

        /* Draw Level 3 pigs with different types */
        for (i = 0; i < 12; i++)
        {
            if (piggy3[i].active)
            {
                drawPigSprite3(piggy3[i].a, piggy3[i].b, piggy3[i].r, i);
            }
        }

       
        iSetColor(255, 255, 255);
        iFilledEllipse(100, 780, 90, 35);
        iFilledEllipse(150, 800, 70, 30);
        iFilledEllipse(50, 800, 60, 25);
        iFilledEllipse(100, 820, 80, 30);
        iFilledEllipse(350, 720, 100, 40);
        iFilledEllipse(400, 740, 80, 35);
        iFilledEllipse(300, 740, 70, 30);
        iFilledEllipse(350, 760, 90, 35);
        iFilledEllipse(600, 780, 110, 42);
        iFilledEllipse(650, 800, 90, 38);
        iFilledEllipse(550, 800, 80, 33);
        iFilledEllipse(600, 820, 100, 37);
        iFilledEllipse(850, 720, 95, 38);
        iFilledEllipse(900, 740, 75, 33);
        iFilledEllipse(800, 740, 65, 28);
        iFilledEllipse(850, 760, 85, 33);
        iFilledEllipse(1100, 780, 85, 35);
        iFilledEllipse(1150, 800, 65, 30);
        iFilledEllipse(1050, 800, 55, 25);
        iFilledEllipse(1100, 820, 75, 30);
        iFilledEllipse(200, 650, 60, 25);
        iFilledEllipse(250, 670, 40, 20);
        iFilledEllipse(150, 670, 50, 22);
        iFilledEllipse(500, 680, 55, 23);
        iFilledEllipse(550, 700, 35, 18);
        iFilledEllipse(450, 700, 45, 20);
        iFilledEllipse(800, 650, 50, 22);
        iFilledEllipse(850, 670, 30, 17);
        iFilledEllipse(750, 670, 40, 19);
        iFilledEllipse(1100, 680, 45, 21);
        iFilledEllipse(1150, 700, 25, 16);
        iFilledEllipse(1050, 700, 35, 18);
        iSetColor(51, 25, 0);
        iFilledRectangle(0, 0, 1280, GROUND_HEIGHT);

        for (i = 0; i < 30; i++)
        {
            iSetColor(51 + (i * 4), 25 + (i * 2), i);
            iFilledRectangle(0, i * 3, 1280, 3);
        }

        tGap = 1;
        t = 0;
        while (1)
        {
            dotX = xCircle + -v * cos(theta * PI / 180) * t;
            dotY = yCircle + -v * sin(theta * PI / 180) * t - 0.5 * g * t * t;
            if (dotY <= 90)
                break;
            iSetColor(0, 0, 0);
            iPoint(dotX, dotY);
            t += tGap;
        }
        if ((!hassplit || !issplittingbird) &&
            ballX >= 0 && ballX <= SCREEN_WIDTH &&
            ballY >= 0 && ballY <= SCREEN_HEIGHT)
        {
            drawBirdSprite(xBall, yBall, currentbirdtype, currentbirdsize);
        }

        for (i = 0; i < 3; i++)
        {
            if (miniBirds[i].active)
            {
                drawBirdSprite((int)miniBirds[i].x, (int)miniBirds[i].y, BLUE_BIRD, radiusball);
            }
        }

        if (!check || dragging)
        {
            iSetColor(100, 50, 0);
            iLine(slingLeftX, slingLeftY, xBall, yBall);
            iLine(slingRightX, slingRightY, xBall, yBall);
        }
        else
        {
            iSetColor(100, 50, 0);
            iLine(slingLeftX, slingLeftY, xCircle, yCircle);
            iLine(slingRightX, slingRightY, xCircle, yCircle);
        }

        iSetColor(80, 42, 0);
        iFilledRectangle(xCircle - 5, yCircle - 40, 10, 40);
        iFilledRectangle(slingLeftX - 3, slingLeftY - 3, 6, 6);
        iFilledRectangle(slingRightX - 3, slingRightY - 3, 6, 6);

        drawBirdQueue();
        drawStopMenuButton();

        sprintf(scoreText, "Score: %d", score);
        iSetColor(30, 30, 30);
        iFilledRectangle(1050, 800, 200, 38);
        iSetColor(255, 255, 0);
        iText(1070, 810, scoreText, GLUT_BITMAP_HELVETICA_18);

        /* Show user info */
        sprintf(userText, "Player: %s", currentuser);
        iSetColor(255, 255, 255);
        iText(50, 50, userText, GLUT_BITMAP_HELVETICA_18);
    }
    if ((page == 1 || page == 4 || page == 6) && showNerdStats)
    {
        int nerdY = 700;
        char nerdText[100];
        iSetColor(255, 0, 0);
        // Pull distance (drag)
        double dx = xBall - xCircle;
        double dy = yBall - yCircle;
        double pullDist = sqrt(dx * dx + dy * dy);
        sprintf(nerdText, "Pull Distance: %.2f", pullDist);
        iText(20, nerdY, nerdText, GLUT_BITMAP_HELVETICA_18);
        nerdY -= 30;
        // Velocity
        sprintf(nerdText, "Velocity: (%.2f, %.2f)", vx, vy);
        iText(20, nerdY, nerdText, GLUT_BITMAP_HELVETICA_18);
        nerdY -= 30;
        // Next bird type
        const char *birdNames[] = {"RED", "BLUE", "YELLOW", "ORANGE"};
        int nextBird = (totalbirdsused < maxbirds) ? birdsequence[totalbirdsused] : -1;
        if (nextBird >= 0 && nextBird <= 3)
            sprintf(nerdText, "Next Bird: %s", birdNames[nextBird]);
        else
            sprintf(nerdText, "Next Bird: NONE");
        iText(20, nerdY, nerdText, GLUT_BITMAP_HELVETICA_18);
        nerdY -= 30;
        // Reward (bonus)
        int bonus = 0;
        if (page == 1)
            bonus = 10000 * (maxbirds - totalbirdsused);
        else if (page == 4)
            bonus = 10000 * (maxbirds - totalbirdsused);
        else if (page == 6)
            bonus = 10000 * (maxbirds - totalbirdsused);
        sprintf(nerdText, "Potential Bonus: %d", bonus);
        iText(20, nerdY, nerdText, GLUT_BITMAP_HELVETICA_18);
        nerdY -= 30;
        // Current Score
        sprintf(nerdText, "Score: %d", score);
        iText(20, nerdY, nerdText, GLUT_BITMAP_HELVETICA_18);
        nerdY -= 30;
        // Bird in flight?
        sprintf(nerdText, "Bird Flying: %s", (check == 1) ? "YES" : "NO");
        iText(20, nerdY, nerdText, GLUT_BITMAP_HELVETICA_18);
    }
}

void iMouseMove(int mx, int my)
{
    /* place your codes here */
}

void iMouseDrag(int mx, int my)
{
    double dx, dy, dist;

    if (check == 0 && (page == 1 || page == 4 || page == 6) && !gameOver && !levelcompleted && !showstopmenu && !showlevelcompletedwithoutwarning)
    {
        dragging = 1;
        dx = mx - xCircle;
        dy = my - yCircle;

        theta = atan2(dy, dx) * 180 / PI;

        dist = sqrt(dx * dx + dy * dy);
        if (dist > dragradius)
            dist = dragradius;
        v = (dist / dragradius) * 10;

        xBall = xCircle + dist * cos(theta * PI / 180);
        yBall = yCircle + dist * sin(theta * PI / 180);

        vx = -v * cos(theta * PI / 180);
        vy = -v * sin(theta * PI / 180);
        timemax = v * sin(theta * PI / 180) / g;
    }
}

void iMouse(int button, int state, int mx, int my)
{
    int i;
    double currentX, currentY, currentVX, currentVY;

    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && page == 0) /* Main Menu */
    {
        playclicksound();                                    // Play click sound
        if (mx >= 500 && mx <= 780 && my >= 30 && my <= 130) /* Play button */
        {
            if (strlen(currentuser) == 0)
            {
                page = 7; /* Go to name input screen */
                istypingname = 1;
                strcpy(nameinput, "");
            }
            else
            {
                page = 5; /* Go directly to level select */
            }
        }
        else if (mx >= 30 && mx <= 230 && my >= 30 && my <= 130) /* Exit button */
        {
            exit(0);
        }
        else if (mx >= 1050 && mx <= 1250 && my >= 30 && my <= 130) /* Help button */
        {
            page = 2;
        }
        else if (mx >= 30 && mx <= 230 && my >= 700 && my <= 800) /* Sound button */
        {
            page = 3;
        }
        else if (mx >= 1050 && mx <= 1250 && my >= 700 && my <= 800) /* Credits button */
        {
            page = 12;
        }
    }

    /* NEW: Enhanced Help menu navigation from second file */
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && page == 2)
    {
        playclicksound();                                     // Play click sound
        if (mx >= 465 && mx <= 815 && my >= 545 && my <= 665) /* How to Play */
        {
            page = 9;
        }
        if (mx >= 465 && mx <= 815 && my >= 395 && my <= 515) /* Controls */
        {
            page = 10;
        }
        if (mx >= 465 && mx <= 815 && my >= 245 && my <= 365) /* Tips */
        {
            page = 11;
        }
    }

    /* Name input screen navigation */
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && page == 7)
    {
        playclicksound(); // Play click sound
        /* Focus on text input */
        istypingname = 1;
    }

    /* Leaderboard screen navigation */
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && page == 8)
    {
        playclicksound(); // Play click sound
        /* Back button */
        if (mx >= 590 && mx <= 690 && my >= 100 && my <= 140)
        {
            page = 5; /* Return to level select */
        }
    }

    /* Level selection screen */
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && page == 5)
    {
        playclicksound(); // Play click sound
        /* Leaderboard button */
        if (mx >= 1100 && mx <= 1250 && my >= 750 && my <= 790)
        {
            loadallusers();
            page = 8;
        }
        if (mx >= 20 && mx <= 120 && my >= 20 && my <= 60)
        {
            page = 0; /* Return to level select */
        }
        /* Level 1 button */
        else if (mx >= 285 && mx <= 465 && my >= 180 && my <= 260)
        {
            if (level1completed)
            {
                showlevelcompletedwithoutwarning = 1;
                warninglevel = 1;
            }
            else{
            page = 1;
            currentlevel = 1;
            itblocks();
            bblocks();
            bpigs();
            resetgamestate();
            loadLevelState(1); // Load saved state if exists
            score = 0; // Reset score for Level 1
        }
        }
        /* Level 2 button */
        else if (mx >= 550 && mx <= 730 && my >= 375 && my <= 455)
        {
            if (level2completed)
            {
                showlevelcompletedwithoutwarning = 1;
                warninglevel = 2;
            }
            else
            {
                page = 4;
                currentlevel = 2;
                itblocks2();
                bblocks2();
                bpigs2();
                resetgamestate();
                loadLevelState(2); 
                score = 0; // Reset score for Level 2
            }
        }
        /* Level 3 button */
        else if (mx >= 815 && mx <= 995 && my >= 570 && my <= 650)
        {
            if (level3completed)
            {
                showlevelcompletedwithoutwarning = 1;
                warninglevel = 3;
            }
            else
            {
                page = 6;
                currentlevel = 3;
                itblocks3();
                bblocks3();
                bpigs3();
                resetgamestate();
                loadLevelState(3);
                score = 0; // Reset score for Level 3
            }
        }
    }

    /* Stop menu button click */
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && (page == 1 || page == 4 || page == 6))
    {
        if (mx >= 590 && mx <= 690 && my >= 750 && my <= 800)
        {
            showstopmenu = 1;
        }
    }

    /* Stop menu overlay clicks */
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && showstopmenu)
    {
        playclicksound(); // Play click sound
        /* Main Menu button */
        if (mx >= 580 && mx <= 700 && my >= 460 && my <= 500)
        {
            page = 0;
            resetgamestate();

            saveLevelState(currentlevel);
        }
        /* Level Page button */
        else if (mx >= 580 && mx <= 700 && my >= 410 && my <= 450)
        {
            page = 5;
            resetgamestate();
            saveLevelState(currentlevel);
        }
        /* Resume button */
        else if (mx >= 580 && mx <= 700 && my >= 360 && my <= 400)
        {
            showstopmenu = 0;
        }
        /* STAT FOR NERDS button */
        else if (mx >= 550 && mx <= 800 && my >= 310 && my <= 350)
        {
            showNerdStats = !showNerdStats;
        }
    }

    /* Game over screen clicks */
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && gameOver)
    {
        playclicksound(); // Play click sound
        /* Yes button */
        if (mx >= 530 && mx <= 610 && my >= 380 && my <= 420)
        {
            deleteLevelState(currentlevel);
            restartcurrentlevel();
        }
        /* No button */
        else if (mx >= 670 && mx <= 750 && my >= 380 && my <= 420)
        {
            page = 5;
            resetgamestate();
        }
    }

    /* Level completed screen clicks */
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && levelcompleted)
    {
        playclicksound(); // Play click sound
        if (currentlevel < 3)
        {

            if (mx >= 460 && mx <= 560 && my >= 400 && my <= 440)
            {
                if (page == 1)
                { /* Currently in Level 1 */
                    page = 1;
                    currentlevel = 1;
                    itblocks();
                    bblocks();
                    bpigs();
                }
                else if (page == 4)
                { /* Currently in Level 2 */
                    page = 4;
                    currentlevel = 2;
                    itblocks2();
                    bblocks2();
                    bpigs2();
                }
                else if (page == 6)
                { /* Currently in Level 3 */
                    page = 6;
                    currentlevel = 3;
                    itblocks3();
                    bblocks3();
                    bpigs3();
                }
                resetgamestate();
            }
            /* Next Level button (center) - Go to next level */
            else if (mx >= 580 && mx <= 680 && my >= 400 && my <= 440)
            {
                if (page == 1)
                { /* From Level 1 to Level 2 */
                    currentlevel = 2;
                    page = 4;
                    itblocks2();
                    bblocks2();
                    bpigs2();
                }
                else if (page == 4)
                { /* From Level 2 to Level 3 */
                    currentlevel = 3;
                    page = 6;
                    itblocks3();
                    bblocks3();
                    bpigs3();
                }
                score = 0; // Reset score for the new level
                resetgamestate();
            }
            /* Level Select button (right) */
            else if (mx >= 700 && mx <= 800 && my >= 400 && my <= 440)
            {
                page = 5;
                resetgamestate();
            }
        }
        else
        {
            /* Play Again button for level 3 */
            if (mx >= 500 && mx <= 600 && my >= 380 && my <= 420)
            {
                page = 6;
                currentlevel = 3;
                itblocks3();
                bblocks3();
                bpigs3();
                resetgamestate();
            }
            /* Level Select button for level 3 */
            else if (mx >= 640 && mx <= 740 && my >= 380 && my <= 420)
            {
                page = 5;
                resetgamestate();
            }
        }
    }

    /* Level completed warning screen clicks */
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && showlevelcompletedwithoutwarning)
    {
        playclicksound(); // Play click sound

        if (mx >= 530 && mx <= 610 && my >= 380 && my <= 420)
        {
            showlevelcompletedwithoutwarning = 0;

            if (warninglevel == 1)
            {
                page = 1;
                currentlevel = 1;
                itblocks();
                bblocks();
                bpigs();
                resetgamestate();
            }
            else if (warninglevel == 2)
            {
                page = 4;
                currentlevel = 2;
                itblocks2();
                bblocks2();
                bpigs2();
                resetgamestate();
            }
            else if (warninglevel == 3)
            {
                page = 6;
                currentlevel = 3;
                itblocks3();
                bblocks3();
                bpigs3();
                resetgamestate();
            }
        }
        /* No button - return to level select page */
        else if (mx >= 670 && mx <= 750 && my >= 380 && my <= 420)
        {
            showlevelcompletedwithoutwarning = 0;
            page = 5;
        }
    }

    if (button == GLUT_LEFT_BUTTON && state == GLUT_UP && (page == 1 || page == 4 || page == 6) && dragging && !gameOver && !levelcompleted && !showstopmenu && !showlevelcompletedwithoutwarning)
    {
        if (totalbirdsused < maxbirds)
        {
            dragging = 0;
            check = 1;
            timechange = 1;
            bouncecount = 0;
            onground = 0;
            totalbirdsused++;

            hassplit = 0;
            hasspeedboost = 0;
            hassizeboost = 0;
        }
    }


    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && (page == 1 || page == 4 || page == 6) && check == 1 && !dragging && !gameOver && !levelcompleted && !showstopmenu && !showlevelcompletedwithoutwarning)
    {
        if (issplittingbird && !hassplit)
        {
            hassplit = 1;
            currentX = ballX;
            currentY = ballY;
            currentVX = vx;
            currentVY = vy;

            for (i = 0; i < 3; i++)
            {
                miniBirds[i].active = 1;
                miniBirds[i].x = currentX;
                miniBirds[i].y = currentY;
                miniBirds[i].vx = currentVX + (i - 1) * 1.5;
                miniBirds[i].vy = currentVY + (i - 1) * 1.0;
            }

            vx = 0.01;
            vy = 0.01;
            bouncecount = maxbounces;
        }

        if (isspeedboost && !hasspeedboost)
        {
            hasspeedboost = 1;
            vx *= 2.0;
            vy *= 2.0;
        }

        if (issizeboostbird && !hassizeboost)
        {
            hassizeboost = 1;
            currentbirdsize = 15;
        }
    }

    
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && page == 12)
    {
        playclicksound(); // Play click sound
        // Back button
        if (mx >= 50 && mx <= 150 && my >= 50 && my <= 90)
        {
            page = 0;
        }
    }
}
void iMouseWheel(int dir, int mx, int my)
{
    /* place your code here */
}
void iKeyPress(unsigned char key)
{
    int len, i;
    double currentX, currentY, currentVX, currentVY;

    /* Handle name input */
    if (page == 7 && istypingname)
    {
        if (key == '\r' || key == '\n') /* Enter key */
        {
            if (strlen(nameinput) > 0)
            {
                strcpy(currentuser, nameinput);
                loaduserdata(); 
                loadallusers(); 
                page = 5;       
                istypingname = 0;
            }
        }
        else if (key == 27) /* ESC key */
        {
            page = 0; /* Return to main menu */
            istypingname = 0;
            strcpy(nameinput, "");
        }
        else if (key == '\b' || key == 127) /* Backspace */
        {
            len = strlen(nameinput);
            if (len > 0)
            {
                nameinput[len - 1] = '\0';
            }
        }
        else if (key >= 32 && key <= 126 && strlen(nameinput) < 15) /* Printable characters */
        {
            len = strlen(nameinput);
            nameinput[len] = key;
            nameinput[len + 1] = '\0';
        }
        return;
    }

    /* Regular game controls */
    if (key == 'q')
    {

        if (page == 1 || page == 4 || page == 6)
        {
            page = 5; /* Go to level page */
        }
        else if (page == 9 || page == 10 || page == 11)
        {
            page = 2; /* Go back to help menu */
        }
        else
        {
            page = 0; /* Go to main menu */
        }
        resetgamestate();
    }
    /* Sound page */

    if (key == 'p' || key == 'P')
    {
        if (Paused)
        {
            iPauseSound(channel);
            Paused = 0;
        }
    }
    else if (key == 'r' || key == 'R')
    {
        if (!Paused)
        {
            iResumeSound(channel);
            Paused = 1;
        }
    }

    
    if (key == ' ' && check == 1 && (page == 1 || page == 4 || page == 6) && !dragging && !gameOver && !levelcompleted && !showstopmenu && !showlevelcompletedwithoutwarning)
    {
        if (issplittingbird && !hassplit)
        {
            hassplit = 1;
            currentX = ballX;
            currentY = ballY;
            currentVX = vx;
            currentVY = vy;

            for (i = 0; i < 3; i++)
            {
                miniBirds[i].active = 1;
                miniBirds[i].x = currentX;
                miniBirds[i].y = currentY;
                miniBirds[i].vx = currentVX + (i - 1) * 1.5;
                miniBirds[i].vy = currentVY + (i - 1) * 1.0;
            }

            vx = 0.01;
            vy = 0.01;
            bouncecount = maxbounces;
        }

        if (isspeedboost && !hasspeedboost)
        {
            hasspeedboost = 1;
            vx *= 2.0;
            vy *= 2.0;
        }

        if (issizeboostbird && !hassizeboost)
        {
            hassizeboost = 1;
            currentbirdsize = 15;
        }
    }
}


void iSpecialKeyPress(unsigned char key)
{
    /* Sound controls */

    if (key == GLUT_KEY_UP)
    {
        if (currentvolume < 100)
        {
            currentvolume += 10;
            if (currentvolume > 100)
                currentvolume = 100;
            iSetVolume(channel, currentvolume);
        }
    }
    else if (key == GLUT_KEY_DOWN)
    {
        if (currentvolume > 0)
        {
            currentvolume -= 10;
            if (currentvolume < 0)
                currentvolume = 0;
            iSetVolume(channel, currentvolume);
        }
    }
}

int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    iLoadImage(&bg, "assets/images/ABBG.png");
    loadBirdSprites();
    iInitializeSound();
    channel = iPlaySound("assets/sounds/AB1.wav", 1, currentvolume);
    ballX = xCircle;
    ballY = yCircle;

    updatecurrentbirdtype();
    itblocks();
    bblocks();
    bpigs();
    itblocks2();
    bblocks2();
    bpigs2();
    itblocks3();
    bblocks3();
    bpigs3();
    makegamesavefolder();
    loadallusers();
    p = iSetTimer(10, change);
    iInitialize(1280, 853, "Angry Birds");
    iLoadImage(&creditsImg, "assets/images/credits.png");

    return 0;
}