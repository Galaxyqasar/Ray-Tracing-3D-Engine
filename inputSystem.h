#ifndef INPUTSYSTEM_H
#define INPUTSYSTEM_H

#include "eventSystem.h"
#include "mouse.h"

void initInput();
void getKeyInput(SlotArgs *args);

struct InputSystem{
    int s_mouseMoved;   //public
    int s_mouseClicked; //public
    int s_keyPressed;   //public
    int s_mouseInput;
    int s_keyboardInput;

    int currentKey;
    const void (*init)(void (*x)(), void (*y)(), void (*z)());
} InputSystem = (struct InputSystem){-1,-1,-1,-1,-1,-1,initInput};

void initInput(void (*keyPressSlot)(SlotArgs *args),
               void (*mouseMoveSlot)(SlotArgs *args),
               void (*mouseClickSlot)(SlotArgs *args)){

    InputSystem.s_keyboardInput = addConnection(getKeyInput, emptyArgs, true);  //internal slots
    InputSystem.s_mouseInput = addConnection(getMouseInput, emptyArgs, true);   //will emit external signals

    InputSystem.s_mouseMoved = addConnection(mouseMoveSlot, NULL, false);       //external signals
    InputSystem.s_mouseClicked = addConnection(mouseClickSlot, NULL, false);
    InputSystem.s_keyPressed = addConnection(keyPressSlot, NULL, false);

    initMouse(InputSystem.s_mouseMoved, InputSystem.s_mouseClicked);
}

void getKeyInput(SlotArgs *args){
    InputSystem.currentKey = getch();
    if(InputSystem.currentKey != -1){
        SlotArgs *args = malloc(sizeof(SlotArgs));
        int *data = malloc(sizeof(int));
        memcpy(data, &InputSystem.currentKey, sizeof(int));
        memcpy(args, &(SlotArgs){createString("%i"), (void*)data}, sizeof(SlotArgs));
        emit(InputSystem.s_keyPressed, args);
    }
}

#endif
