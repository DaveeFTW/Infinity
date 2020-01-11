/*
 * fonttest.c
 * This file is used to display the PSP's internal font (pgf and bwfon firmware files)
 * intraFont Version 0.31 by BenHur - http://www.psp-programming.com/benhur
 *
 * Uses parts of pgeFont by InsertWittyName - http://insomniac.0x89.org
 * G-spec code by Geecko
 *
 * This work is licensed under the Creative Commons Attribution-Share Alike 3.0 License.
 * See LICENSE for more details.
 *
 */

#include <pspkernel.h>
#include <stdio.h>
#include <string.h>
#include <pspgu.h>
#include <pspdisplay.h>
#include <time.h>     //only needed for the blinking effect
#include <pspdebug.h> //only needed for indicating loading progress

#include "../libraries/graphics.h"
#include "../intraFont.h"

PSP_MODULE_INFO("intraFontTest",PSP_MODULE_USER,1,1);

static int running = 1;

int exit_callback(int arg1,int arg2,void *common) {
  running = 0;
  return 0;
}

int CallbackThread(SceSize args,void *argp) {
  int cbid = sceKernelCreateCallback("Exit Callback",exit_callback,NULL);
  sceKernelRegisterExitCallback(cbid);
  sceKernelSleepThreadCB();
  return 0;
}

int SetupCallbacks(void) {
  int thid = sceKernelCreateThread("CallbackThread",CallbackThread,0x11,0xFA0,PSP_THREAD_ATTR_USER,0);
  if (thid >= 0) sceKernelStartThread(thid,0,0);
  return thid;
}

int main() {
  pspDebugScreenInit();
  SetupCallbacks();

  // Colors
  enum colors {
    RED =  0xFF0000FF,
    GREEN =  0xFF00FF00,
    BLUE =  0xFFFF0000,
    WHITE =  0xFFFFFFFF,
    LITEGRAY = 0xFFBFBFBF,
    GRAY =  0xFF7F7F7F,
    DARKGRAY = 0xFF3F3F3F,    
    BLACK = 0xFF000000,
  };
    
  pspDebugScreenPrintf("intraFont 0.31 - 2009 by BenHur\n\nLoading fonts: 0%%");
        
  // Init intraFont library
  intraFontInit();
    
  // Load fonts
  intraFont* ltn[16];                                         //latin fonts (large/small,with/without serif,regular/italic/bold/italic&bold)
  char file[40];
  int i;
  for (i = 0; i < 16; i++) {
    sprintf(file,"flash0:/font/ltn%d.pgf",i); 
    ltn[i] = intraFontLoad(file,0);                                             //<- this is where the actual loading happens 
    intraFontSetStyle(ltn[i],1.0f,WHITE,DARKGRAY,0.f,0);
    pspDebugScreenSetXY(15,2);
    pspDebugScreenPrintf("%d%%",(i+1)*100/20);
  }

  intraFont* jpn0 = intraFontLoad("flash0:/font/jpn0.pgf",INTRAFONT_STRING_SJIS); //japanese font with SJIS text string encoding
  intraFontSetStyle(jpn0,0.8f,WHITE,DARKGRAY,0.f,0);                              //scale to 80%
  pspDebugScreenSetXY(15,2);
  pspDebugScreenPrintf("%d%%",17*100/20);
        
  intraFont* kr0 = intraFontLoad("flash0:/font/kr0.pgf",INTRAFONT_STRING_UTF8);  //Korean font (not available on all systems) with UTF-8 encoding
  intraFontSetStyle(kr0,0.8f,WHITE,DARKGRAY,0.f,0);                               //scale to 80%
  pspDebugScreenSetXY(15,2);
  pspDebugScreenPrintf("%d%%",18*100/20);
           
  intraFont* arib = intraFontLoad("flash0:/font/arib.pgf",0);                     //Symbols (not available on all systems)
  intraFontSetStyle(arib,0.8f,WHITE,DARKGRAY,0.f,0);                              //scale to 80%
  pspDebugScreenSetXY(15,2);
  pspDebugScreenPrintf("%d%%",19*100/20);

  intraFont* chn = intraFontLoad("flash0:/font/gb3s1518.bwfon",0);               //chinese font
  intraFontSetStyle(chn,0.8f,WHITE,DARKGRAY,0.f,0);                               //scale to 80%
  pspDebugScreenSetXY(15,2);
  pspDebugScreenPrintf("done\n");

  // Make sure the important fonts for this application are loaded
  if(!ltn[0] || !ltn[4] || !ltn[8]) sceKernelExitGame();

  // Set alternative fonts that are used in case a certain char does not exist in the main font
  intraFontSetAltFont(ltn[8],jpn0);                     //japanese font is used for chars that don't exist in latin
  intraFontSetAltFont(jpn0,chn);                        //chinese font (bwfon) is used for chars that don't exist in japanese (and latin)
  intraFontSetAltFont(chn,kr0);                         //korean font is used for chars that don't exist in chinese (and jap and ltn)
  intraFontSetAltFont(kr0,arib);                        //symbol font is used for chars that don't exist in korean (and chn,jap & ltn)
  // NB: This is an extreme case - usually you need only one alternative font (e.g. japanese & chinese)
  // Also: if one of the fonts failed to load (e.g. chn) the chain breaks and all subequent fonts are not used (e.g. kr0 and arib)
  
  initGraphics();
  float x_scroll1 = 80,x_scroll2 = 225,x_scroll3 = 370, x_scroll4 = 385;

  while(running)  {
    clearScreen(GRAY);

    // Must be called before any of the intraFont functions
    guStart();

    // Draw various text
    float x,y = 20;
    intraFontSetStyle(ltn[4],1.0f,BLACK,WHITE,0.f,INTRAFONT_ALIGN_CENTER);
    intraFontPrint(ltn[4],240,y,"intraFont 0.31 - 2009 by BenHur");
    intraFontSetStyle(ltn[4],1.0f,WHITE,DARKGRAY,0.f,0);
            
    y += 21;
    intraFontPrint(ltn[8], 10,y,"Latin Sans-Serif:");
    intraFontPrint(ltn[0],180,y,"regular,");
    intraFontPrint(ltn[2],270,y,"italic,");
    intraFontPrint(ltn[4],330,y,"bold,");
    intraFontPrint(ltn[6],390,y,"both");
        
    y += 17;
    intraFontPrint(ltn[8], 10,y,"Latin Sans-Serif small:");
    intraFontPrint(ltn[8],180,y,"regular,");
    intraFontPrint(ltn[10],270,y,"italic,");
    intraFontPrint(ltn[12],330,y,"bold,");
    intraFontPrint(ltn[14],390,y,"both");
        
    y += 17;
    intraFontPrint(ltn[8], 10,y,"Latin with Serif:");
    intraFontPrint(ltn[1],180,y,"regular,");
    intraFontPrint(ltn[3],270,y,"italic,");
    intraFontPrint(ltn[5],330,y,"bold,");
    intraFontPrint(ltn[7],390,y,"both");
        
    y += 17;
    intraFontPrint(ltn[8], 10,y,"Latin with Serif small:");
    intraFontPrint(ltn[9],180,y,"regular,");
    intraFontPrint(ltn[11],270,y,"italic,");
    intraFontPrint(ltn[13],330,y,"bold,");
    intraFontPrint(ltn[15],390,y,"both");
    
    y += 17;
    intraFontSetEncoding(ltn[8],INTRAFONT_STRING_UTF8);   //set text string encoding to UTF-8
    intraFontPrint(ltn[8],10,y,"LTN (UTF8):");          //(has no effect on std ascii)
    intraFontPrint(ltn[8],110,y,"\xC3\xA5 \xC3\xA8 \xC3\xAD \xC3\xB4 \xC3\xBC \xC3\xB1"); //UTF-8 encoded chars with accents on top of them
    
    intraFontPrint(ltn[8],250,y,"Symbols: ");
    unsigned short ucs2_arib[] = { 57786,57787,57788,57789,57790,0 };
    x = intraFontPrintUCS2(arib,350,y,ucs2_arib);
    if (x == 350) intraFontPrint(ltn[8],350,y,"[n/a]");

    y += 17;
    intraFontPrint(ltn[8],10,y,"JPN (UTF8):");
    char utf8_jpn[] = {0xE3,0x81,0x93,0xE3,0x82,0x93,0xE3,0x81,0xAB,0xE3,0x81,0xA1,0xE3,0x81,0xAF,0x20,0xE4,0xB8,0x96,0xE7,0x95,0x8C,0};
    intraFontSetEncoding(jpn0,INTRAFONT_STRING_UTF8);     //temporarely switch to UTF-8 (INTRAFONT_STRING_SJIS was set in intraFontLoad call)
    x = intraFontPrint(jpn0,110,y,utf8_jpn);            //print UTF-8 encoded string
    if (x == 110) intraFontPrint(ltn[8],110,y,"[n/a]");
    intraFontSetEncoding(jpn0,INTRAFONT_STRING_SJIS);     //switch back to S-JIS
        
    intraFontPrint(ltn[8],250,y,"JPN (S-JIS):");
    x = intraFontPrint(jpn0,350,y,"イントラフォント");  //S-JIS encoded text string (flag INTRAFONT_STRING_SJIS set in intraFontLoad call)
    if (x == 350) intraFontPrint(ltn[8],350,y,"[n/a]");

    y += 17;
    intraFontPrint(ltn[8],10,y,"CHN (GBK):");
    char gbk_chn[] = { 0xbc,0xf2,0xcc,0xe5,0xd6,0xd0,0xce,0xc4,0};
    intraFontSetEncoding(chn,INTRAFONT_STRING_GBK);
    x = intraFontPrint(chn,110,y,gbk_chn);              //print GBK-encoded string (simplified chinese)
    if (x == 110) intraFontPrint(ltn[8],110,y,"[n/a]");
    intraFontPrint(ltn[8],250,y,"CHN (BIG5):");
    char big5_chn[] = { 0xc1,0x63,0xc5,0xe9,0xa4,0xa4,0xa4,0xe5,0};
    intraFontSetEncoding(chn,INTRAFONT_STRING_BIG5);
    x = intraFontPrint(chn,350,y,big5_chn);             //print BIG5-encoded string (trad. chinese)
    if (x == 350) intraFontPrint(ltn[8],350,y,"[n/a]");

    y += 17;   
    intraFontPrint(ltn[8],10,y,"KOR (UTF8):");
    char utf8_kr[] = {0xed,0x99,0x98,0xec,0x98,0x81,0x20,0xeb,0x8c,0x80,0xed,0x95,0x9c,0xeb,0xaf,0xbc,0xea,0xb5,0xad,0};
    x = intraFontPrint(kr0,110,y,utf8_kr);              //print UTF-8 string (flag INTRAFONT_STRING_UTF8 set in intraFontLoad call)
    if (x == 110) intraFontPrint(ltn[8],110,y,"[n/a]");

    intraFontPrint(ltn[8],250,y,"MIX (UCS2):");
    unsigned short ucs2_all[] = { 0x0041,0x0192,0x3401,0x3402,0x4E01,0x4E02,0xAC01,0xAC02,0xE1BE,0};
    x = intraFontPrintUCS2(ltn[8],350,y,ucs2_all);      //print chars from all fonts (using alternative fonts,which were set after font loading)
    

    y += 17;
        intraFontPrint(ltn[8],10,y,"Colors: ");
        intraFontSetStyle(ltn[8],1.0f,RED,BLUE,0.f,0);
    x = intraFontPrint(ltn[8],80,y,"colorful,");
        intraFontSetStyle(ltn[8],1.0f,WHITE,0,0.f,0);
    x = intraFontPrint(ltn[8],x,y,"no shadow,");
        intraFontSetStyle(ltn[8],1.0f,0,DARKGRAY,0.f,0);
    x = intraFontPrint(ltn[8],x,y,"no text,");
        intraFontSetStyle(ltn[8],1.0f,0x7FFFFFFF,DARKGRAY,0.f,0);
    x = intraFontPrint(ltn[8],x,y,"transparent,");    
        intraFontSetStyle(ltn[8],1.0f,GRAY,WHITE,0.f,0);
    x = intraFontPrint(ltn[8],x,y,"glowing,");
    float t = ((float)(clock() % CLOCKS_PER_SEC)) / ((float)CLOCKS_PER_SEC);
    int val = (t < 0.5f) ? t*511 : (1.0f-t)*511;
    intraFontSetStyle(ltn[8],1.0f,LITEGRAY,(0xFF<<24)+(val<<16)+(val<<8)+(val),0.f,0);
    x = intraFontPrint(ltn[8],x,y,"flashing");
        intraFontSetStyle(ltn[8],1.0f,WHITE,DARKGRAY,0.f,0);
        
    y += 17;
        intraFontPrint(ltn[8],10,y,"Spacing: ");
        intraFontSetStyle(ltn[8],1.0f,WHITE,DARKGRAY,0.f,INTRAFONT_WIDTH_FIX);
    x = intraFontPrint(ltn[8],80,y,"fixed (default),");
        intraFontSetStyle(ltn[8],1.0f,WHITE,DARKGRAY,0.f,INTRAFONT_WIDTH_FIX | 12);
    x = intraFontPrint(ltn[8],x,y,"fixed (12),");    
        intraFontSetStyle(ltn[8],1.0f,WHITE,DARKGRAY,0.f,0);
    x = intraFontPrint(ltn[8],x,y,"variable width");
        
    y += 22;
        intraFontPrint(ltn[8],10,y,"Scaling: ");
    intraFontSetStyle(ltn[0],0.5f,WHITE,DARKGRAY,0.f,0);
    x = intraFontPrint(ltn[0],80,y,"tiny,");
        intraFontSetStyle(ltn[0],0.75f,WHITE,DARKGRAY,0.f,0);
    x = intraFontPrint(ltn[0],x,y,"small,");
        intraFontSetStyle(ltn[0],1.0f,WHITE,DARKGRAY,0.f,0);
    x = intraFontPrint(ltn[0],x,y,"regular,");
        intraFontSetStyle(ltn[0],1.25f,WHITE,DARKGRAY,0.f,0);
    x = intraFontPrint(ltn[0],x,y,"large,");
        intraFontSetStyle(ltn[0],1.5f,WHITE,DARKGRAY,0.f,0);
    x = intraFontPrint(ltn[0],x,y,"huge"); 
    intraFontSetStyle(ltn[0],1.0f,WHITE,DARKGRAY,0.f,0);
    
        y += 17;
        intraFontPrint(ltn[8],10,y,"Align: ");
    intraFontSetStyle(ltn[8],1.0f,WHITE,DARKGRAY,0.f,INTRAFONT_ALIGN_LEFT);
    t = ((float)(clock() % (CLOCKS_PER_SEC*10))) / ((float)CLOCKS_PER_SEC);
    int length = (t < 5.0f) ? t*5.8f : (10.0f-t)*5.8f;
    intraFontPrintColumnEx(ltn[8], 80,y, 90,"left aligned w. linebreaks  ",length);
    //NB: intraFontPrintColumnEx() is used to print a sub-string of a given length (last parameter)
    //    if you want to print the whole string,simply use intraFontPrintColumn() and omit the length parameter
    intraFontSetStyle(ltn[8],1.0f,WHITE,DARKGRAY,0.f,INTRAFONT_ALIGN_CENTER);
    intraFontPrintColumnEx(ltn[8],225,y,110,"center aligned w. linebreaks",length);
    intraFontSetStyle(ltn[8],1.0f,WHITE,DARKGRAY,0.f,INTRAFONT_ALIGN_RIGHT);
    intraFontPrintColumnEx(ltn[8],370,y, 90,"right aligned w. linebreaks ",length);
    intraFontSetStyle(ltn[8],1.0f,WHITE,DARKGRAY,0.f,INTRAFONT_ALIGN_FULL);
    intraFontPrintColumnEx(ltn[8],380,y, 90,"full justified w. linebreaks",length);
    intraFontSetStyle(ltn[8],1.0f,WHITE,DARKGRAY,0.f,0);

    y += 28;
    intraFontPrint(ltn[8],10,y,"Scrolling: ");
    intraFontSetStyle(ltn[8],1.0f,WHITE,DARKGRAY,0.f,INTRAFONT_SCROLL_LEFT);
    x_scroll1 = intraFontPrintColumn(ltn[8],x_scroll1,y,80,"This text is scrolled to the left.");
    intraFontSetStyle(ltn[8],1.0f,WHITE,DARKGRAY,0.f,INTRAFONT_SCROLL_SEESAW);
    x_scroll2 = intraFontPrintColumn(ltn[8],x_scroll2,y,90,"Back & forth like a seesaw.");
    intraFontSetStyle(ltn[8],1.0f,WHITE,DARKGRAY,0.f,INTRAFONT_SCROLL_RIGHT);
    x_scroll3 = intraFontPrintColumn(ltn[8],x_scroll3,y,80,"Scrolling to the right...");
    intraFontSetStyle(ltn[8],1.0f,WHITE,DARKGRAY,0.f,INTRAFONT_SCROLL_THROUGH);
    x_scroll4 = intraFontPrintColumn(ltn[8],x_scroll4,y,80,"This text is scrolled through.");
    intraFontSetStyle(ltn[8],1.0f,WHITE,DARKGRAY,0.f,0);

    // End drawing
    sceGuFinish();
    sceGuSync(0,0);
    
    // Swap buffers
    sceDisplayWaitVblankStart();
    flipScreen();
  }
    
    //create screenshot (optional)
    //saveImage("scrshot.png",getVramDisplayBuffer(),SCREEN_WIDTH,SCREEN_HEIGHT,PSP_LINE_SIZE,0);

  // Unload our fonts
  for (i = 0; i < 16; i++) {
    intraFontUnload(ltn[i]);
  }
  intraFontUnload(jpn0);
  intraFontUnload(kr0);
  intraFontUnload(arib);
  intraFontUnload(chn);
  
  // Shutdown font library
  intraFontShutdown();

  sceKernelExitGame();

  return 0;
}
