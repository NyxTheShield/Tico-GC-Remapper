#include <switch.h>
#include <stdio.h>
#include <string.h>

// ================= MENU =================

#define MENU_COUNT 4

const char* menuItems[MENU_COUNT] = {
    "Remap All Buttons",
    "Set Defaults For Joycon",
    "Set Defaults For Gamecube Adapter",
    "Exit"
};

// ================= HEADER =================

const char* header =
"[GCPad1]\n"
"Device = Switch/0/Joypad\n"
"Buttons/L = ``\n"
"Buttons/R = ``\n"
"Main Stick/Up = `Y0+`\n"
"Main Stick/Down = `Y0-`\n"
"Main Stick/Left = `X0-`\n"
"Main Stick/Right = `X0+`\n"
"Main Stick/Modifier = \n"
"Main Stick/Calibration = 100.00 141.42 100.00 141.42 100.00 141.42 100.00 141.42\n"
"C-Stick/Up = if(toggle(`Select`), `Y1-`, `Y1+`)\n"
"C-Stick/Down = if(toggle(`Select`), `Y1+`, `Y1-`)\n"
"C-Stick/Left = `X1-`\n"
"C-Stick/Right = `X1+`\n"
"C-Stick/Modifier = \n"
"C-Stick/Calibration = 100.00 141.42 100.00 141.42 100.00 141.42 100.00 141.42\n"
"D-Pad/Up = `Up`\n"
"D-Pad/Down = `Down`\n"
"D-Pad/Left = `Left`\n"
"D-Pad/Right = `Right`\n"
"Triggers/L-Analog = ``\n"
"Triggers/R-Analog = ``\n\n";

// ================= INPUT HELPERS =================

const char* getButtonName(u64 kDown)
{
    if (kDown & HidNpadButton_A) return "A";
    if (kDown & HidNpadButton_B) return "B";
    if (kDown & HidNpadButton_X) return "X";
    if (kDown & HidNpadButton_Y) return "Y";
    if (kDown & HidNpadButton_L) return "L";
    if (kDown & HidNpadButton_R) return "R";
    if (kDown & HidNpadButton_ZL) return "Z";
    if (kDown & HidNpadButton_ZR) return "R2";
    if (kDown & HidNpadButton_Plus) return "Start";
    if (kDown & HidNpadButton_Minus) return "Select";
    return NULL;
}

// Wait until ALL buttons are released
void waitForRelease(PadState* pad)
{
    while (appletMainLoop())
    {
        padUpdate(pad);
        if (padGetButtons(pad) == 0)
            return;

        consoleUpdate(NULL);
    }
}

// Wait for a fresh press (after release)
const char* waitForButton(PadState* pad)
{
    waitForRelease(pad);

    while (appletMainLoop())
    {
        padUpdate(pad);
        u64 kDown = padGetButtonsDown(pad);

        const char* name = getButtonName(kDown);
        if (name)
            return name;

        consoleUpdate(NULL);
    }
    return "";
}

// ================= FILE WRITE =================

void writeConfig(const char* lines[], int count)
{
    FILE* f = fopen("sdmc:/tico/system/gc/User/Config/GCPadNew.ini", "w");

    if (f)
    {
        fprintf(f, "%s", header);

        for (int i = 0; i < count; i++)
        {
            fprintf(f, "%s\n", lines[i]);
        }

        fclose(f);
    }
}

// ================= REMAP =================

typedef struct {
    const char* gc_name;
    const char* value;
} Mapping;

void remapAllButtons()
{
    consoleClear();

    PadState pad;
    padConfigureInput(1, HidNpadStyleSet_NpadStandard);
    padInitializeDefault(&pad);

    Mapping map[] = {
        {"Buttons/A", ""},
        {"Buttons/B", ""},
        {"Buttons/X", ""},
        {"Buttons/Y", ""},
        {"Buttons/Z", ""},
        {"Buttons/Start", ""},
        {"Triggers/L", ""},
        {"Triggers/R", ""}
    };

    int count = sizeof(map)/sizeof(map[0]);

    for (int i = 0; i < count; i++)
    {
        consoleClear();
        printf("Press a button for %s\n", map[i].gc_name);

        const char* input = waitForButton(&pad);
        map[i].value = input;
    }

    FILE* f = fopen("sdmc:/tico/system/gc/User/Config/GCPadNew.ini", "w");

    if (f)
    {
        fprintf(f, "%s", header);

        for (int i = 0; i < count; i++)
        {
            fprintf(f, "%s = `%s`\n", map[i].gc_name, map[i].value);
        }

        fclose(f);
    }

    consoleClear();
    printf("Saved config:\n\n");

    for (int i = 0; i < count; i++)
    {
        printf("%s = `%s`\n", map[i].gc_name, map[i].value);
    }

    printf("\nPress + to return\n");

    while (appletMainLoop())
    {
        padUpdate(&pad);
        if (padGetButtonsDown(&pad) & HidNpadButton_Plus)
            break;

        consoleUpdate(NULL);
    }
}

// ================= DEFAULTS =================

void setDefaultsJoycon()
{
    const char* lines[] = {
        "Buttons/A = `A`",
        "Buttons/B = `B`",
        "Buttons/X = `X`",
        "Buttons/Y = `Y`",
        "Buttons/Z = `R`",
        "Buttons/Start = `Start`",
        "Triggers/L = `Z`",
        "Triggers/R = `R2`"
    };

    writeConfig(lines, 8);

    consoleClear();
    printf("Joycon defaults applied.\n\nPress + to return\n");

    PadState pad;
    padInitializeDefault(&pad);

    while (appletMainLoop())
    {
        padUpdate(&pad);
        if (padGetButtonsDown(&pad) & HidNpadButton_Plus)
            break;

        consoleUpdate(NULL);
    }
}

void setDefaultsGC()
{
    const char* lines[] = {
        "Buttons/A = `A`",
        "Buttons/B = `B`",
        "Buttons/X = `X`",
        "Buttons/Y = `Y`",
        "Buttons/Z = `R2`",
        "Buttons/Start = `Start`",
        "Triggers/L = `L`",
        "Triggers/R = `R`"
    };

    writeConfig(lines, 8);

    consoleClear();
    printf("GameCube adapter defaults applied.\n\nPress + to return\n");

    PadState pad;
    padInitializeDefault(&pad);

    while (appletMainLoop())
    {
        padUpdate(&pad);
        if (padGetButtonsDown(&pad) & HidNpadButton_Plus)
            break;

        consoleUpdate(NULL);
    }
}

// ================= MENU =================

void drawMenu(int selected)
{
    consoleClear();

    printf("Tico GC Remapper\n");
    printf("-----------------\n\n");

    for (int i = 0; i < MENU_COUNT; i++)
    {
        if (i == selected)
            printf("\x1b[7m> %s\x1b[0m\n", menuItems[i]);
        else
            printf("  %s\n", menuItems[i]);
    }

    printf("\nUp/Down to move, A to select\n");
}

// ================= MAIN =================

int main(int argc, char* argv[])
{
    consoleInit(NULL);
    fsInitialize();

    PadState pad;
    padConfigureInput(1, HidNpadStyleSet_NpadStandard);
    padInitializeDefault(&pad);

    int selected = 0;

    bool exitBool = false;
    while (appletMainLoop() && !exitBool)
    {
        padUpdate(&pad);
        u64 kDown = padGetButtonsDown(&pad);

        if (kDown & HidNpadButton_Up)
        {
            selected--;
            if (selected < 0) selected = MENU_COUNT - 1;
        }

        if (kDown & HidNpadButton_Down)
        {
            selected++;
            if (selected >= MENU_COUNT) selected = 0;
        }

        if (kDown & HidNpadButton_A)
        {
            waitForRelease(&pad); // 🔥 FIX: prevent A from leaking into next screen

            switch (selected)
            {
                case 0: remapAllButtons(); break;
                case 1: setDefaultsJoycon(); break;
                case 2: setDefaultsGC(); break;
                case 3: exitBool = true; break;
            }
        }

        drawMenu(selected);
        consoleUpdate(NULL);
    }

    consoleExit(NULL);
    fsExit();
    return 0;
}