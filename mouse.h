#ifndef MOUSE_H
#define MOUSE_H

#include <curses.h>
#include <stdlib.h>
#include <string.h>

#include "eventSystem.h"
#include "header.h"


void getMouseInput(SlotArgs *args);
int initMouse(int moveSignal, int clickSignal);
int cleanMouse();

int mouse_moveSignal = 0;
int mouse_clickSignal = 0;

#define WINDOWS

#ifdef WINDOWS

//#include <windows.h>
//#include <unistd.h>

typedef struct MouseEvent{
    int type;
    int x, y, dx, dy;
} MouseEvent;
/*
HANDLE hConsole;
DWORD dwPreviousMode;
DWORD dwNewMode;

DWORD dwEvents;
INPUT_RECORD input;
*/
MouseEvent event;
pthread_t mouseThread;
bool stopMouse = false;

void getMouseInput(SlotArgs *args){
    if(event.type > 0){
        MouseEvent *e = malloc(sizeof(MouseEvent));
        if(e != NULL)
            memcpy(e, &event, sizeof(MouseEvent));
        char *message;
        int signal;
        if(event.type == 1){
            signal = mouse_moveSignal;
            message = createString("mouse moved   ");
        }
        else if(event.type == 2){
            signal = mouse_moveSignal;
            message = createString("mouse dragged ");
        }
        else if(event.type == 3){
            signal = mouse_clickSignal;
            message = createString("mouse clicked ");
        }
        else if(event.type == 4){
            signal = mouse_clickSignal;
            message = createString("mouse released");
        }
        SlotArgs *args = malloc(sizeof(SlotArgs));
        memcpy(args, &(SlotArgs){message, (void*)e}, sizeof(SlotArgs));
        emit(signal, args);
    }
    event.type = -1;
}
/*
void *readMouse(void *data){
    while(!stopMouse){
        Sleep(100);
    //if (GetNumberOfConsoleInputEvents(hConsole, 1) > 0)
    //if(PeekConsoleInput(hConsole, &input, 1, &dwEvents) & MOUSE_EVENT)
        if(ReadConsoleInput(hConsole, &input, 1, &dwEvents) && dwEvents > 0) {
            switch (input.EventType) {
                case MOUSE_EVENT:
                    event.dx = input.Event.MouseEvent.dwMousePosition.X - event.x;
                    event.dy = input.Event.MouseEvent.dwMousePosition.Y - event.y;
                    event.x = input.Event.MouseEvent.dwMousePosition.X;
                    event.y = input.Event.MouseEvent.dwMousePosition.Y;
                    event.type = 1;
                break;
            }
        }
    }
}
*/
int initMouse(int moveSignal, int clickSignal){
    mouse_moveSignal = moveSignal;
    mouse_clickSignal =  clickSignal;/*
    hConsole = GetStdHandle(STD_INPUT_HANDLE);
    dwPreviousMode = 0;
    GetConsoleMode(hConsole, &dwPreviousMode);
    dwNewMode = dwPreviousMode | ENABLE_MOUSE_INPUT;
    dwNewMode &= ~ENABLE_QUICK_EDIT_MODE;
    SetConsoleMode(hConsole, dwNewMode | ENABLE_EXTENDED_FLAGS);*/
    //pthread_create(&mouseThread, NULL, readMouse, NULL);
}

int cleanMouse(){
    //SetConsoleMode(hConsole, dwPreviousMode | ENABLE_EXTENDED_FLAGS);
}

#else
#include <gpm.h>

typedef Gpm_Event MouseEvent;
int gpm = 0;
Gpm_Connect gpmConnect;
MouseEvent gpmEvent;
int result = 0;
struct timeval tv;
fd_set fds;


void getMouseInput(SlotArgs *args){
    FD_ZERO(&fds);
    FD_SET(gpm, &fds);

    while(select(gpm+1, &fds, NULL, NULL, &tv) > 1)
        Gpm_GetEvent(&gpmEvent);
    result = select(gpm+1, &fds, NULL, NULL, &tv);
    if(result > 0 && Gpm_GetEvent(&gpmEvent) == 1){
        MouseEvent *e = malloc(sizeof(MouseEvent));
        if(e != NULL)
            memcpy(e, &gpmEvent, sizeof(MouseEvent));
        char *message;
        int signal = -1;
        if(gpmEvent.type & GPM_MOVE){
            signal = mouse_moveSignal;
            message = createString("mouse moved   ");
        }
        else if(gpmEvent.type & GPM_DRAG){
            signal = mouse_moveSignal;
            message = createString("mouse dragged ");
        }
        else if(gpmEvent.type & GPM_DOWN){
            signal = mouse_clickSignal;
            message = createString("mouse clicked ");
        }
        else if(gpmEvent.type & GPM_UP){
            signal = mouse_clickSignal;
            message = createString("mouse released");
        }
        SlotArgs *args = malloc(sizeof(SlotArgs));
        memcpy(args, &(SlotArgs){message, (void*)e}, sizeof(SlotArgs));
        emit(signal, args);
    }
}

int initMouse(int moveSignal, int clickSignal){
    mouse_moveSignal = moveSignal;
    mouse_clickSignal =  clickSignal;
    tv.tv_sec = 0;
    tv.tv_usec = 0;

    gpmConnect.eventMask = GPM_MOVE | GPM_DRAG | GPM_UP | GPM_DOWN;
    gpmConnect.defaultMask = 0;
    gpmConnect.minMod = 0;
    gpmConnect.maxMod = ~0;
    if((gpm = Gpm_Open(&gpmConnect, 0)) == -1)
        return -1;
    return 0;
}

int cleanMouse(){
    Gpm_Close();
}

#endif // WIN_32

#endif
