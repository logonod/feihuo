#include <interception.h>
#include <Windows.h>
#include <utils.h>
#include <iostream>
#include <ctime> 
 
typedef struct MyData
{
    long long val;
}MYDATA;

long long get_time_ms()
{
    struct tm tm; 
    SYSTEMTIME time;
    GetSystemTime(&time);
    memset(&tm, 0, sizeof(tm));  
    tm.tm_year = time.wYear;
    tm.tm_mon = time.wMonth;
    tm.tm_mday = time.wDay;
    tm.tm_hour = time.wHour;
    tm.tm_min = time.wMinute;
    tm.tm_sec = time.wSecond;
    tm.tm_year -= 1900;
    tm.tm_mon--;
    long long time_ms = mktime(&tm);
    return time_ms * 1000 + time.wMilliseconds;
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
    
    std::cout << "Boom At " << get_time_ms() << ".\n";
    return 0; 
}
 
DWORD WINAPI threadProc(LPVOID lpParam)
{
    MYDATA *pmd = (MYDATA *)lpParam;
    Sleep ((int)pmd->val);
    right_click();
    return 0;
}
 
int main()
{
    InterceptionContext context;
    InterceptionDevice device;
    InterceptionStroke stroke;
 
    HANDLE threadit = 0;
    DWORD exitcode;
    DWORD threadit_status = NULL;
    MYDATA mydt[1];
    long long last_fire_ms = 0;
    long long current_ms = 0;
 
    raise_process_priority();
 
    context = interception_create_context();
 
    interception_set_filter(context, interception_is_keyboard, INTERCEPTION_FILTER_KEY_DOWN | INTERCEPTION_FILTER_KEY_UP);
    interception_set_filter(context, interception_is_mouse, INTERCEPTION_FILTER_MOUSE_LEFT_BUTTON_UP | INTERCEPTION_FILTER_MOUSE_MIDDLE_BUTTON_DOWN);
 
    while(interception_receive(context, device = interception_wait(context), &stroke, 1) > 0)
    {
        if(interception_is_mouse(device))
        {
            InterceptionMouseStroke &mstroke = *(InterceptionMouseStroke *) &stroke;
 
            if(mstroke.state == INTERCEPTION_MOUSE_LEFT_BUTTON_UP)
            {
                last_fire_ms = get_time_ms();
                std::cout << "Fire At " << last_fire_ms << ".\n";
            }
            else if (mstroke.state == INTERCEPTION_MOUSE_MIDDLE_BUTTON_DOWN)
            {
                current_ms = get_time_ms();
                std::cout << "Need Boom At " << current_ms << ".\n";
                if (current_ms >= (last_fire_ms + 250))
                {
                    std::cout << "Boom Immediately.\n";
                    right_click();

                    //mydt[0].val = 0;
                    //threadit = CreateThread (NULL, 0, (LPTHREAD_START_ROUTINE)threadProc, mydt, 0, NULL);
                    
                } 
                else {
                    threadit_status = WaitForSingleObject( threadit, 0);

                    if (threadit_status == WAIT_FAILED)
                    {
                        mydt[0].val = (last_fire_ms - current_ms + 250);
                        threadit = CreateThread (NULL, 0, (LPTHREAD_START_ROUTINE)threadProc, mydt, 0, NULL);
                        std::cout << "Boom Delay At " << mydt[0].val << " Ms Later.\n";
                    }
                    else if (threadit_status == WAIT_OBJECT_0)
                    {
                        mydt[0].val = (last_fire_ms - current_ms + 250);
                        threadit = CreateThread (NULL, 0, (LPTHREAD_START_ROUTINE)threadProc, mydt, 0, NULL);
                        std::cout << "Boom Delay At " << mydt[0].val << " Ms Later.\n";
                    } else
                    {
                        std::cout << "Boom Already In Process, Deserted.\n";
                    }
                }
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
/*
int main()
{
    HANDLE threadit = 0;
    DWORD exitcode;
    MYDATA mydt[1];
    mydt[0].val = 2000;
    std::cout << get_time_ms() << "\n";
    std::cout << GetExitCodeThread(threadit, &exitcode) << "Create Thread.\n";
    threadit = CreateThread (NULL, 0, (LPTHREAD_START_ROUTINE)threadProc, mydt, 0, NULL);
    std::cout << GetExitCodeThread(threadit, &exitcode) << "Create Thread.\n";
    Sleep (10000);
    std::cout << GetExitCodeThread(threadit, &exitcode) << "Finish Thread.\n";    
    TerminateThread(threadit, 0);
    CloseHandle(threadit);
    std::cout << GetExitCodeThread(threadit, &exitcode) << "Close Thread.\n";
 
    int i;
    std::cin >> i;
 
    return 0;
}
*/