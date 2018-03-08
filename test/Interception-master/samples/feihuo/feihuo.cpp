#include <interception.h>
#include <Windows.h>
#include <utils.h>
#include <iostream>

DWORD WINAPI threadProc()
{
    for (int i = 0; ; ++i)
    {
        std::cout << i << '\n';
        Sleep (1000);
    }

    return 0;
}

enum ScanCode
{
    SCANCODE_EXIT = 0x4F
};

int right_click()
{
    InterceptionContext context = interception_create_context();  
  
    InterceptionMouseStroke mouseStroke[2];  
    ZeroMemory(mouseStroke, sizeof(mouseStroke));  

    mouseStroke[0].state = INTERCEPTION_MOUSE_RIGHT_BUTTON_DOWN;  
    mouseStroke[1].state = INTERCEPTION_MOUSE_RIGHT_BUTTON_UP;  
    interception_send(context, INTERCEPTION_MOUSE(0), (InterceptionStroke*)mouseStroke, _countof(mouseStroke));  
      
    interception_destroy_context(context); 
    return 0; 
}

/*
int main()
{
    InterceptionContext context;
    InterceptionDevice device;
    InterceptionStroke stroke;

    raise_process_priority();

    context = interception_create_context();

    interception_set_filter(context, interception_is_keyboard, INTERCEPTION_FILTER_KEY_DOWN | INTERCEPTION_FILTER_KEY_UP);
    interception_set_filter(context, interception_is_mouse, INTERCEPTION_FILTER_MOUSE_LEFT_BUTTON_DOWN | INTERCEPTION_FILTER_MOUSE_MIDDLE_BUTTON_DOWN);

    while(interception_receive(context, device = interception_wait(context), &stroke, 1) > 0)
    {
        if(interception_is_mouse(device))
        {
            InterceptionMouseStroke &mstroke = *(InterceptionMouseStroke *) &stroke;

            if(mstroke.state == INTERCEPTION_MOUSE_LEFT_BUTTON_DOWN) std::cout << "Left Click.\n";

            if(mstroke.state == INTERCEPTION_MOUSE_MIDDLE_BUTTON_DOWN)
            {
                std::cout << "Middle Click.\n";
                right_click();
            } 

            interception_send(context, device, &stroke, 1);
        }

        if(interception_is_keyboard(device))
        {
            InterceptionKeyStroke &kstroke = *(InterceptionKeyStroke *) &stroke;

            interception_send(context, device, &stroke, 1);

            if(kstroke.code == SCANCODE_EXIT) break;
        }
    }

    interception_destroy_context(context);

    return 0;
}
*/

int main()
{
    CreateThread (NULL, 0, (LPTHREAD_START_ROUTINE)threadProc, NULL, 0, NULL);

    int i;
    std::cin >> i;

    return 0;
}