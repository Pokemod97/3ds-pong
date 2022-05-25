#include <citro2d.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <3ds.h>
typedef struct
{
  float x;
  float y;
  float height;
  float width;
} Rect;

typedef struct
{
  Rect rect;
  float speedx;
  float speedy;
} Ball;
#define WHITE 0xFFFFFFFF
void update(Rect *w, int up_or_down, float speed)
{

  if (up_or_down == 1)
  {
    w->y -= speed;
  }
  else if (up_or_down == -1)
  {
    w->y += speed;
  }
  w->y = C2D_Clamp(w->y, 0, 190);
}

void draw(Rect *w)
{
  C2D_DrawRectSolid(w->x, w->y, 1, w->width, w->height, WHITE);
}

void collide(Ball *ball, Rect *leftRect, Rect *rightRect)
{
  if (ball->rect.x < (leftRect->x) || (ball->rect.x > (rightRect->x)))
  {
    if (ball->rect.x < (leftRect->x) && ball->rect.y > (leftRect->y) &&
        ball->rect.y < (leftRect->y + leftRect->height))
    {
      ball->speedx *= -1;
      ball->rect.x = leftRect->x + 5;
    }
    else if (ball->rect.x > (rightRect->x) && ball->rect.y > (rightRect->y) &&
             ball->rect.y < (rightRect->y + rightRect->height))
    {
      ball->speedx *= -1;
      ball->rect.x = rightRect->x - 5;
    }
  }
}
Ball *createBall()
{
  Ball *a = (Ball *)malloc(sizeof(Ball));
  a->rect = (Rect){200.0, 95, 20, 20};
  a->speedx = ((float)rand() / (float)(RAND_MAX / 5)) + 1;
  a->speedy = ((float)rand() / (float)(RAND_MAX / 5)) + .1;
  return a;
}

int main(int argc, char **argv)
{
  srand(time(NULL));
  gfxInitDefault();
  C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
  C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
  C2D_Prepare();
  C3D_RenderTarget *top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
  consoleInit(GFX_BOTTOM, NULL);
  Rect leftRect = {10, 95, 50, 10};
  Rect rightRect = {380, 95, 50, 10};
  Ball *ball = createBall();
  int loop = 0;
  int leftPoints = 0;
  int rightPoints = 0;
  // Main loop
  while (aptMainLoop())
  {

    hidScanInput();
    u32 kDown = hidKeysHeld();
    int up_and_down = 0;
    if (kDown & KEY_DUP)
    {
      up_and_down = 1;
    }
    if (kDown & KEY_DDOWN)
    {
      up_and_down = -1;
    }

    update(&rightRect, up_and_down, 5);
    if (ball->rect.y < leftRect.y)
    {
      up_and_down = 1;
    }
    else
    {
      up_and_down = -1;
    }

    update(&leftRect, up_and_down, 1);
    if (ball->rect.y > 210 || ball->rect.y < 0)
    {
      ball->speedy *= -1;
    }
    ball->rect.y += ball->speedy;
    ball->rect.x += ball->speedx;
    collide(ball, &leftRect, &rightRect);
    // Your code goes here
    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
    C2D_TargetClear(top, C2D_Color32f(0.0f, 0.0f, 0.0f, 1.0f));
    C2D_SceneBegin(top);
    draw(&leftRect);
    draw(&rightRect);
    draw(&(ball->rect));
    C3D_FrameEnd(0);
    if (ball->rect.x < 0 || ball->rect.x > 400)
    {
      if (ball->rect.x < 0)
      {
        leftPoints++;
      }
      else if (ball->rect.x > 400)
      {
        rightPoints++;
      }
      printf("%d %-d \n", leftPoints, rightPoints);
      free(ball);
      ball = createBall();
      loop = 1;
      rightRect.height = 100;
      leftRect.height = 100;
    }
    if (loop % 100 == 0)
    {
      ball->rect.height -= 1;
      ball->rect.width -= 1;
      rightRect.height -= 1;
      leftRect.height -= 1;
      loop = 0;
    }
    ball->rect.width = C2D_Clamp(ball->rect.width, 5, 20);
    ball->rect.height = C2D_Clamp(ball->rect.height, 5, 20);
    rightRect.height = C2D_Clamp(rightRect.height, 10, 50);
    leftRect.height = C2D_Clamp(leftRect.height, 10, 50);
    loop++;
    if (kDown & KEY_START)
      break; // break in order to return to hbmenu
  }
  // Deinit libs
  C2D_Fini();
  C3D_Fini();
  gfxExit();
  return 0;
}
