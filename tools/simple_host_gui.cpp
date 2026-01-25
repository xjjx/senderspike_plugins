#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <dlfcn.h>
#include <unistd.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "aeffect.h"

//g++ simple_host_gui.cpp -O2 -IVST_SDK_2.4/pluginterfaces/vst2.x -ldl -lX11 -o simple_host_gui

static Display* gDisplay = nullptr;
static Window gWindow = 0;
static bool gRunning = true;

static VstIntPtr VSTCALLBACK hostCallback(
    AEffect* effect,
    VstInt32 opcode,
    VstInt32 index,
    VstIntPtr value,
    void* ptr,
    float opt)
{
    switch (opcode)
    {
        case audioMasterVersion:
            return 2400; // VST 2.4

        default:
            return 0;
    }
}

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        std::fprintf(stderr, "Usage: %s plugin.so\n", argv[0]);
        return 1;
    }

    /* Load plugin */
    void* handle = dlopen(argv[1], RTLD_NOW | RTLD_LOCAL);
    if (!handle)
    {
        std::fprintf(stderr, "dlopen failed: %s\n", dlerror());
        return 1;
    }

    typedef AEffect* (*VSTMainFn)(audioMasterCallback);

    VSTMainFn mainFn =
        (VSTMainFn)dlsym(handle, "VSTPluginMain");
    if (!mainFn)
        mainFn = (VSTMainFn)dlsym(handle, "VSTMain");

    if (!mainFn)
    {
        std::fprintf(stderr, "No VST entry point found\n");
        dlclose(handle);
        return 1;
    }

    AEffect* effect = mainFn(hostCallback);
    if (!effect || effect->magic != kEffectMagic)
    {
        std::fprintf(stderr, "Invalid VST plugin\n");
        dlclose(handle);
        return 1;
    }

    /* Open plugin */
    effect->dispatcher(effect, effOpen, 0, 0, nullptr, 0);

    /* Check editor */
    if (!(effect->flags & effFlagsHasEditor))
    {
        std::fprintf(stderr, "Plugin has no editor\n");
        effect->dispatcher(effect, effClose, 0, 0, nullptr, 0);
        dlclose(handle);
        return 1;
    }

    /* Open X11 */
    gDisplay = XOpenDisplay(nullptr);
    if (!gDisplay)
    {
        std::fprintf(stderr, "Cannot open X display\n");
        return 1;
    }

    int screen = DefaultScreen(gDisplay);

    /* Query editor size */
    ERect* rect = nullptr;
    effect->dispatcher(effect, effEditGetRect, 0, 0, &rect, 0);

    int width  = rect ? (rect->right - rect->left) : 400;
    int height = rect ? (rect->bottom - rect->top) : 300;

    /* Create window */
    gWindow = XCreateSimpleWindow(
        gDisplay,
        RootWindow(gDisplay, screen),
        100, 100,
        width, height,
        1,
        BlackPixel(gDisplay, screen),
        WhitePixel(gDisplay, screen)
    );

    Atom wmDelete = XInternAtom(gDisplay, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(gDisplay, gWindow, &wmDelete, 1);

    XSelectInput(gDisplay, gWindow,
                 ExposureMask | StructureNotifyMask);

    XMapWindow(gDisplay, gWindow);
    XFlush(gDisplay);

    /* Open VST editor */
    effect->dispatcher(effect, effEditOpen, 0, 0,
                       (void*)gWindow, 0);

    /* Event loop */
    while (gRunning)
    {
        while (XPending(gDisplay))
        {
            XEvent event;
            XNextEvent(gDisplay, &event);

            if (event.type == ClientMessage)
            {
                if ((Atom)event.xclient.data.l[0] == wmDelete)
                    gRunning = false;
            }
            else if (event.type == DestroyNotify)
            {
                gRunning = false;
            }
        }

        /* Optional: idle processing */
        effect->dispatcher(effect, effEditIdle, 0, 0, nullptr, 0);

        usleep(10000);
    }

    /* Close editor and plugin */
    effect->dispatcher(effect, effEditClose, 0, 0, nullptr, 0);
    effect->dispatcher(effect, effClose, 0, 0, nullptr, 0);

    XDestroyWindow(gDisplay, gWindow);
    XCloseDisplay(gDisplay);

    dlclose(handle);
    return 0;
}

