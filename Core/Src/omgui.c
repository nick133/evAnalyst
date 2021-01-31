/*
 * omgui.h
 *
 *  OLED MCU GUI Library
 *
 *  Created on: 23 янв. 2021 г.
 *      Author: nick
 */

#include "omgui.h"
#include "main.h"


/* Private methods */
static void _omScreenInit(omScreenT *);


void omDisplayInit(omDisplayT *displ)
{
  displ->InitCallback(displ); // OLED Driver's Init code goes here

  _omScreenInit(displ->Screen);
}


void omDisplayDeInit(omDisplayT *displ)
{
  displ->DeInitCallback(displ); // OLED Driver's DeInit code goes here
}


Bool omScreenSelect(omScreenT *screen)
{
  if (omScreenIsActive(screen) == True)
  {
    return False;
  }

  if (screen->Display->Screen->HideCallback != NULL)
  {
    screen->Display->Screen->HideCallback(screen);
  }

  _omScreenInit(screen);

  return True;
}


static void _omScreenInit(omScreenT *screen)
{
  if (screen->ShowCallback != NULL)
  {
    screen->ShowCallback(screen);
  }

  for (uint16_t i = 0; i < screen->WidgetsNumOf; i++)
  {
    // array[1] equals to *(array + 1)
    // https://stackoverflow.com/questions/16201607/c-pointer-to-array-of-structs
    screen->Widgets[i].InitCallback(screen->Widgets + i);
  }

  screen->Display->Screen = screen;
}


Bool omScreenIsActive(omScreenT *screen)
{
  return (screen->Display->Screen->Id == screen->Id) ? True : False;
}


void omDisplayUpdate(omDisplayT *displ)
{
  displ->UpdateCallback(displ);
  return;
}


void omDrawPixel(omDisplayT *displ, uint32_t x, uint32_t y, uint32_t color)
{
  displ->DrawPixelCallback(displ, x, y, color);
  return;
}


void omDrawBitmap(omBitmapT *bitmap)
{
  uint32_t *data = bitmap->RawData; // Don't increment original pointer

  for (uint32_t y = bitmap->PosY; y < bitmap->PosY+bitmap->Height; y++)
    for (uint32_t x = bitmap->PosX; x < bitmap->PosX+bitmap->Width; x++)
    {
      if (x >= bitmap->Display->ResX || y >= bitmap->Display->ResY)
      {
        continue;
      }
      else if (*data > 0)
      {
        omDrawPixel(bitmap->Display, x, y, 1); // White
      }
      else
      {
        omDrawPixel(bitmap->Display, x, y, 0); // Black
      }

      data++;
    }

  return;
}

void omAnimationStart(omAnimationT *anim)
{
  omBitmapT *bitmap = anim->Bitmaps; // Don't increment original pointer

  for (uint16_t i = 0; i < anim->FramesNumOf; i++)
  {
    omDrawBitmap(bitmap);
    omDisplayUpdate(anim->Display);
    SYS_SLEEP(anim->Interval);

    bitmap++;
  }

  return;
}
