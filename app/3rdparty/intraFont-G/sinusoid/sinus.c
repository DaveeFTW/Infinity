/*
 * sinus.c
 * This file is used to display the PSP's internal font (pgf and bwfon firmware files)
 * intraFont Version 0.31 by BenHur - http://www.psp-programming.com/benhur
 *
 * Rotation support by Geecko
 * Uses parts of pgeFont by InsertWittyName - http://insomniac.0x89.org
 *
 * This work is licensed under the Creative Commons Attribution-Share Alike 3.0 License.
 * See LICENSE for more details.
 *
 */

#include <pspkernel.h>
#include <pspdisplay.h>
#include <pspgu.h>
#include <math.h>

#include "../libraries/graphics.h"
#include "../intraFont.h"

PSP_MODULE_INFO("intraFontTest", PSP_MODULE_USER, 1, 1);

int exit_callback(int arg1, int arg2, void *common) {
  sceKernelExitGame();
  return 0;
}
int CallbackThread(SceSize args, void *argp) {
  int cbid = sceKernelCreateCallback("Exit Callback", exit_callback, NULL);
  sceKernelRegisterExitCallback(cbid);
  sceKernelSleepThreadCB();
  return 0;
}
int SetupCallbacks(void) {
  int thid = sceKernelCreateThread("CallbackThread", CallbackThread, 0x11, 0xFA0, PSP_THREAD_ATTR_USER, 0);
  if (thid >= 0) sceKernelStartThread(thid, 0, 0);
  return thid;
}

#define WHITE 0xFFFFFFFF
#define GRAY  0xFF7F7F7F
#define BLACK 0xFF000000


int main()
{
  SetupCallbacks();
  initGraphics();

  intraFontInit();
  intraFont* font = intraFontLoad("flash0:/font/ltn8.pgf",0);
  intraFontSetStyle(font,1.f,0,0,0.f,INTRAFONT_ALIGN_CENTER);

  struct Sinusoid
  {
    float angle; // In degrees
    float amplitude;
    float step;
    float speed;
    float speed_inc;
    char str[64];
    int len;
  } sinus = {0.f,35.f,10.f,0.f,0.007f,"intraFont 0.31 - 2009 by BenHur",31};

  float x, tmp_angle;
  int i;

  while (1)
  {
    clearScreen(GRAY);

    guStart();

    // * Draw the sinusoid *

    // Get the x position of the 1st char
    x = 240 - intraFontMeasureText(font,sinus.str)/2;
    // Increment the speed
    if (fabsf(sinus.speed += sinus.speed_inc) > 10.f)
      sinus.speed_inc = -sinus.speed_inc;
    // Increment the angle
    tmp_angle = (sinus.angle += sinus.speed);
    if (sinus.angle > 360.f) sinus.angle -= 360.f;

    // Draw one by one
    for (i=0; i!=sinus.len; i++, tmp_angle+=sinus.step)
    {
      intraFontSetStyle(font,1.f,BLACK,WHITE,45.f*cosf(tmp_angle*GU_PI/180.f),
                        INTRAFONT_ALIGN_LEFT);
      x = intraFontPrintEx(font,x,136 + sinus.amplitude*sinf(tmp_angle*GU_PI/180.f),
                           sinus.str+i,1);
    }

    sceGuFinish();
    sceGuSync(0,0);
    
    // Swap buffers
    sceDisplayWaitVblankStart();
    flipScreen();
  }
    
  intraFontUnload(font);
  intraFontShutdown();
  sceKernelExitGame();

  return 0;
}
