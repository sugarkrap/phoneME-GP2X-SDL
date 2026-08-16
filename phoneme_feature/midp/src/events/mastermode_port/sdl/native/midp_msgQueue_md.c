/*
 *   
 *
 * Copyright  1990-2007 Sun Microsystems, Inc. All Rights Reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License version
 * 2 only, as published by the Free Software Foundation.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License version 2 for more details (a copy is
 * included at /legal/license.txt).
 * 
 * You should have received a copy of the GNU General Public License
 * version 2 along with this work; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 * 
 * Please contact Sun Microsystems, Inc., 4150 Network Circle, Santa
 * Clara, CA 95054 or visit www.sun.com if you need additional
 * information or have any questions.
 */

#include <midp_logging.h>
#include <midp_mastermode_port.h>
#include <keymap_input.h>

#include <stdio.h>
#include <stdlib.h>

#include "SDL.h"

static int GP2X_Default_keys[19];

static int piko_eventlog(void)
{
  static int on = -1;
  if (on < 0)
    on = (getenv("PIKO_EVENTLOG") != NULL);
  return on;
}

static unsigned long PikoStatCalls;
static unsigned long PikoStatEvents;
static unsigned long PikoStatKeys;
static unsigned long PikoStatPen;
static unsigned long PikoStatSignals;
static unsigned long PikoStatSpin;
static jlong PikoStatLast;

static void piko_stats(jlong now, MidpReentryData* pNewSignal)
{
  if (!piko_eventlog())
    return;
  if (pNewSignal->waitingFor != 0)
    PikoStatSignals++;
  if (PikoStatLast == 0)
    PikoStatLast = now;
  if (now - PikoStatLast < 5000)
    return;
  fprintf(stderr,
          "piko: pump calls=%lu events=%lu keys=%lu pen=%lu signals=%lu spin_ms=%lu\n",
          PikoStatCalls, PikoStatEvents, PikoStatKeys, PikoStatPen,
          PikoStatSignals, PikoStatSpin);
  fflush(stderr);
  PikoStatCalls = PikoStatEvents = PikoStatKeys = 0;
  PikoStatPen = PikoStatSignals = PikoStatSpin = 0;
  PikoStatLast = now;
}


void InitGP2XKeys()
{ char *Value;
  GP2X_Default_keys[0] = KEYMAP_KEY_UP;
  GP2X_Default_keys[1] = KEYMAP_KEY_1;
  GP2X_Default_keys[2] = KEYMAP_KEY_LEFT;
  GP2X_Default_keys[3] = KEYMAP_KEY_7;
  GP2X_Default_keys[4] = KEYMAP_KEY_DOWN;
  GP2X_Default_keys[5] = KEYMAP_KEY_9;
  GP2X_Default_keys[6] = KEYMAP_KEY_RIGHT;
  GP2X_Default_keys[7] = KEYMAP_KEY_3;
  GP2X_Default_keys[8] = KEYMAP_KEY_SOFT2;
  GP2X_Default_keys[9] = KEYMAP_KEY_SOFT1;
  GP2X_Default_keys[10] = KEYMAP_KEY_POUND;
  GP2X_Default_keys[11] = KEYMAP_KEY_ASTERISK;
  GP2X_Default_keys[12] = KEYMAP_KEY_GAMEA;
  GP2X_Default_keys[13] = KEYMAP_KEY_GAMEB;
  GP2X_Default_keys[14] = KEYMAP_KEY_GAMEC;
  GP2X_Default_keys[15] = KEYMAP_KEY_GAMED;
  GP2X_Default_keys[16] = KEYMAP_KEY_CLEAR;
  GP2X_Default_keys[17] = KEYMAP_KEY_SELECT;
  GP2X_Default_keys[18] = KEYMAP_KEY_5;
  if ((Value = getenv("J2ME_GP2X_JOYU")) != NULL) GP2X_Default_keys[0] = atoi(Value);
  if ((Value = getenv("J2ME_GP2X_JOYUL")) != NULL) GP2X_Default_keys[1] = atoi(Value);
  if ((Value = getenv("J2ME_GP2X_JOYL")) != NULL) GP2X_Default_keys[2] = atoi(Value);
  if ((Value = getenv("J2ME_GP2X_JOYDL")) != NULL) GP2X_Default_keys[3] = atoi(Value);
  if ((Value = getenv("J2ME_GP2X_JOYD")) != NULL) GP2X_Default_keys[4] = atoi(Value);
  if ((Value = getenv("J2ME_GP2X_JOYDR")) != NULL) GP2X_Default_keys[5] = atoi(Value);
  if ((Value = getenv("J2ME_GP2X_JOYR")) != NULL) GP2X_Default_keys[6] = atoi(Value);
  if ((Value = getenv("J2ME_GP2X_JOYUR")) != NULL) GP2X_Default_keys[7] = atoi(Value);
  if ((Value = getenv("J2ME_GP2X_START")) != NULL) GP2X_Default_keys[8] = atoi(Value);
  if ((Value = getenv("J2ME_GP2X_SELECT")) != NULL) GP2X_Default_keys[9] = atoi(Value);
  if ((Value = getenv("J2ME_GP2X_LEFT")) != NULL) GP2X_Default_keys[10] = atoi(Value);
  if ((Value = getenv("J2ME_GP2X_RIGHT")) != NULL) GP2X_Default_keys[11] = atoi(Value);
  if ((Value = getenv("J2ME_GP2X_BUTA")) != NULL) GP2X_Default_keys[12] = atoi(Value);
  if ((Value = getenv("J2ME_GP2X_BUTB")) != NULL) GP2X_Default_keys[13] = atoi(Value);
  if ((Value = getenv("J2ME_GP2X_BUTX")) != NULL) GP2X_Default_keys[14] = atoi(Value);
  if ((Value = getenv("J2ME_GP2X_BUTY")) != NULL) GP2X_Default_keys[15] = atoi(Value);
  if ((Value = getenv("J2ME_GP2X_VOLU")) != NULL) GP2X_Default_keys[16] = atoi(Value);
  if ((Value = getenv("J2ME_GP2X_VOLD")) != NULL) GP2X_Default_keys[17] = atoi(Value);
  if ((Value = getenv("J2ME_GP2X_JOYC")) != NULL) GP2X_Default_keys[18] = atoi(Value);
}

void KeyboardCheck(SDL_Event *event, MidpReentryData* pNewSignal, MidpEvent* pNewMidpEvent)
{ int Key = KEYMAP_KEY_INVALID;
  switch(event->key.keysym.sym)
        { case SDLK_LSUPER: Key = KEYMAP_KEY_SOFT1; break;
          case SDLK_MENU:   Key = KEYMAP_KEY_SOFT2; break;
          case SDLK_UP:     Key = KEYMAP_KEY_UP; break;
          case SDLK_DOWN:   Key = KEYMAP_KEY_DOWN; break;
          case SDLK_LEFT:   Key = KEYMAP_KEY_LEFT; break;
          case SDLK_RIGHT:  Key = KEYMAP_KEY_RIGHT; break;
          case SDLK_RETURN: Key = KEYMAP_KEY_SELECT; break;
          case SDLK_CLEAR:  Key = KEYMAP_KEY_CLEAR; break;
          case SDLK_KP0:    Key = KEYMAP_KEY_0; break;
          case SDLK_KP1:    Key = KEYMAP_KEY_7; break; // KEYPADNUM reverse KEYPHONE
          case SDLK_KP2:    Key = KEYMAP_KEY_8; break; // KEYPADNUM reverse KEYPHONE
          case SDLK_KP3:    Key = KEYMAP_KEY_9; break; // KEYPADNUM reverse KEYPHONE
          case SDLK_KP4:    Key = KEYMAP_KEY_4; break;
          case SDLK_KP5:    Key = KEYMAP_KEY_5; break;
          case SDLK_KP6:    Key = KEYMAP_KEY_6; break;
          case SDLK_KP7:    Key = KEYMAP_KEY_1; break; // KEYPADNUM reverse KEYPHONE
          case SDLK_KP8:    Key = KEYMAP_KEY_2; break; // KEYPADNUM reverse KEYPHONE
          case SDLK_KP9:    Key = KEYMAP_KEY_3; break; // KEYPADNUM reverse KEYPHONE
          case SDLK_KP_MULTIPLY: Key = KEYMAP_KEY_ASTERISK; break;
          case SDLK_KP_PLUS:     Key = KEYMAP_KEY_POUND; break;
          case SDLK_HOME:   Key = KEYMAP_KEY_SEND; break;
          case SDLK_END:    Key = KEYMAP_KEY_END; break;
          case SDLK_PAGEUP: Key = -9; break;
          case SDLK_F11:    Key = KEYMAP_KEY_SELECT; break;
          case SDLK_SPACE:  Key = KEYMAP_KEY_SELECT; break;
          case SDLK_F12:    Key = KEYMAP_KEY_SOFT1; break;
          case SDLK_F10:    Key = KEYMAP_KEY_SOFT2; break;
          case SDLK_BACKSPACE: Key = KEYMAP_KEY_CLEAR; break;
          case SDLK_0:      Key = KEYMAP_KEY_0; break;
          case SDLK_1:      Key = KEYMAP_KEY_1; break;
          case SDLK_2:      Key = KEYMAP_KEY_2; break;
          case SDLK_3:      Key = KEYMAP_KEY_3; break;
          case SDLK_4:      Key = KEYMAP_KEY_4; break;
          case SDLK_5:      Key = KEYMAP_KEY_5; break;
          case SDLK_6:      Key = KEYMAP_KEY_6; break;
          case SDLK_7:      Key = KEYMAP_KEY_7; break;
          case SDLK_8:      Key = KEYMAP_KEY_8; break;
          case SDLK_9:      Key = KEYMAP_KEY_9; break;
          case SDLK_ASTERISK: Key = KEYMAP_KEY_ASTERISK; break;
          case SDLK_HASH:   Key = KEYMAP_KEY_POUND; break;
          default: Key = KEYMAP_KEY_INVALID;
        }
  pNewSignal->waitingFor = UI_SIGNAL;
  pNewMidpEvent->type = MIDP_KEY_EVENT;
  pNewMidpEvent->CHR = Key;
  pNewMidpEvent->ACTION = (event->key.state == SDL_PRESSED) ? KEYMAP_STATE_PRESSED : KEYMAP_STATE_RELEASED;
}

void JoystickCheck(SDL_Event *event, MidpReentryData* pNewSignal, MidpEvent* pNewMidpEvent)
{ int Key = event->jbutton.button;
  if ((Key>=0) && (Key<=18)) Key = GP2X_Default_keys[Key];
  else Key = KEYMAP_KEY_INVALID;
  pNewSignal->waitingFor = UI_SIGNAL;
  pNewMidpEvent->type = MIDP_KEY_EVENT;
  pNewMidpEvent->CHR = Key;
  pNewMidpEvent->ACTION = (event->jbutton.state == SDL_PRESSED) ? KEYMAP_STATE_PRESSED : KEYMAP_STATE_RELEASED;
}

extern void piko_screen_to_canvas(int sx, int sy, int *cx, int *cy);

static int PikoPenX = -1;
static int PikoPenY = -1;

void MouseCheck(SDL_Event *event, MidpReentryData* pNewSignal, MidpEvent* pNewMidpEvent)
{ int sx, sy, cx, cy, action, pressed;
  if (event->type == SDL_MOUSEMOTION)
     { pressed = (event->motion.state & SDL_BUTTON(SDL_BUTTON_LEFT)) ? 1 : 0;
       sx = event->motion.x;
       sy = event->motion.y;
     }
  else
     { if (event->button.button != SDL_BUTTON_LEFT) return;
       pressed = (event->button.state == SDL_PRESSED);
       sx = event->button.x;
       sy = event->button.y;
     }
  piko_screen_to_canvas(sx, sy, &cx, &cy);
  if (cx != PikoPenX || cy != PikoPenY)
     action = pressed ? KEYMAP_STATE_DRAGGED : -1;
  else
     action = pressed ? KEYMAP_STATE_PRESSED : KEYMAP_STATE_RELEASED;
  PikoPenX = cx;
  PikoPenY = cy;
  if (action < 0) return;
  pNewSignal->waitingFor = UI_SIGNAL;
  pNewMidpEvent->type = MIDP_PEN_EVENT;
  pNewMidpEvent->X_POS = cx;
  pNewMidpEvent->Y_POS = cy;
  pNewMidpEvent->ACTION = action;
}

static int piko_quit_key(void)
{
  static int key = -2;
  if (key == -2)
    { const char *v = getenv("PIKO_QUIT_KEY");
      key = v ? atoi(v) : SDLK_F4;
    }
  return key;
}

static int piko_quit_hard(void)
{
  static int hard = -1;
  if (hard < 0)
    hard = (getenv("PIKO_QUIT_HARD") != NULL);
  return hard;
}

static void piko_shutdown(MidpReentryData* pNewSignal, MidpEvent* pNewMidpEvent)
{
  if (piko_quit_hard())
     { SDL_Quit();
       exit(0);
     }
  pNewSignal->waitingFor = AMS_SIGNAL;
  pNewMidpEvent->type = SHUTDOWN_EVENT;
}

static int piko_keylog(void)
{
  static int on = -1;
  if (on < 0)
    on = (getenv("PIKO_KEYLOG") != NULL);
  return on;
}

void CheckEvent(SDL_Event *event, MidpReentryData* pNewSignal, MidpEvent* pNewMidpEvent)
{ if (event->type == SDL_QUIT)
     { piko_shutdown(pNewSignal, pNewMidpEvent);
       return;
     }
  if ((event->type == SDL_KEYDOWN) || (event->type == SDL_KEYUP))
     { if (piko_keylog() && event->type == SDL_KEYDOWN)
          { fprintf(stderr, "piko: keysym %d (0x%x) mod 0x%x\n",
                    (int)event->key.keysym.sym, (int)event->key.keysym.sym,
                    (int)event->key.keysym.mod);
            fflush(stderr);
          }
       if (event->type == SDL_KEYDOWN
           && piko_quit_key() >= 0
           && (int)event->key.keysym.sym == piko_quit_key())
          { piko_shutdown(pNewSignal, pNewMidpEvent);
            return;
          }
       PikoStatKeys++;
       KeyboardCheck(event, pNewSignal, pNewMidpEvent);
       return;
     }
  if ((event->type == SDL_JOYBUTTONDOWN) || (event->type == SDL_JOYBUTTONUP))
     { JoystickCheck(event, pNewSignal, pNewMidpEvent);
       return;
     }
  if ((event->type == SDL_MOUSEBUTTONDOWN) || (event->type == SDL_MOUSEBUTTONUP)
      || (event->type == SDL_MOUSEMOTION))
     { PikoStatPen++;
       MouseCheck(event, pNewSignal, pNewMidpEvent);
       return;
     }
}

/*
 * This function is called by the VM periodically. It has to check if
 * system has sent a signal to MIDP and return the result in the
 * structs given.
 *
 * Values for the <timeout> paramater:
 *  >0 = Block until a signal sent to MIDP, or until <timeout> milliseconds
 *       has elapsed.
 *   0 = Check the system for a signal but do not block. Return to the
 *       caller immediately regardless of the if a signal was sent.
 *  -1 = Do not timeout. Block until a signal is sent to MIDP.
 */
void checkForSystemSignal(MidpReentryData* pNewSignal, MidpEvent* pNewMidpEvent, jlong timeout) 
{ SDL_Event event;
  jlong currentTime = JVM_JavaMilliSeconds(), stopTime;
  PikoStatCalls++;
  if (timeout == -1)
     { if (SDL_WaitEvent(&event))
          { PikoStatEvents++;
            CheckEvent(&event, pNewSignal, pNewMidpEvent);
          }
       piko_stats(JVM_JavaMilliSeconds(), pNewSignal);
       return;
     }
  do { if (SDL_PollEvent(&event))
          { PikoStatEvents++;
            CheckEvent(&event, pNewSignal, pNewMidpEvent);
            piko_stats(JVM_JavaMilliSeconds(), pNewSignal);
            return;
          }
       stopTime = JVM_JavaMilliSeconds();
     } while((stopTime - currentTime) < timeout);
  PikoStatSpin += (unsigned long)(stopTime - currentTime);
  piko_stats(stopTime, pNewSignal);
}

