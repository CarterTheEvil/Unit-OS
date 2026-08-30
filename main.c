#include "rendering.h"
#include "keyboard.h"
#include "timer.h"

extern void interrupts_init(void);

#define INPUT_SIZE 128
#define HISTORY_SIZE 10

static char input[INPUT_SIZE];

static char history[HISTORY_SIZE][INPUT_SIZE];

static int history_count = 0;
static int history_position = -1;

static int ke_mode = 0;


/* =================================================
   Basic string functions
   ================================================= */

static int string_length(const char *str)
{
    int length = 0;

    while (str[length])
        length++;

    return length;
}


static int string_equals(const char *a, const char *b)
{
    int i = 0;

    while (a[i] && b[i])
    {
        if (a[i] != b[i])
            return 0;

        i++;
    }

    return a[i] == b[i];
}


static int string_starts_with(const char *str, const char *prefix)
{
    int i = 0;

    while (prefix[i])
    {
        if (str[i] != prefix[i])
            return 0;

        i++;
    }

    return 1;
}


static void string_copy(char *destination, const char *source)
{
    int i = 0;

    while (source[i])
    {
        destination[i] = source[i];
        i++;
    }

    destination[i] = '\0';
}


/* =================================================
   Command history
   ================================================= */

static void save_history(const char *command)
{
    if (command[0] == '\0')
        return;

    if (history_count > 0)
    {
        int last = (history_count - 1) % HISTORY_SIZE;

        if (string_equals(history[last], command))
            return;
    }

    int index = history_count % HISTORY_SIZE;

    string_copy(history[index], command);

    history_count++;
}


/* =================================================
   Clear currently typed command
   ================================================= */

static void clear_current_input(int length)
{
    for (int i = 0; i < length; i++)
        rendering_backspace();
}


/* =================================================
   Execute commands
   ================================================= */

static void execute_command(char *command)
{
    /* HELP */

    if (string_equals(command, "help"))
    {
        rendering_println("Unit-OS commands:");
        rendering_println("  help       - Show this help");
        rendering_println("  clear      - Clear the screen");
        rendering_println("  echo       - Print text");
        rendering_println("  history    - Show command history");
        rendering_println("  about      - About Unit-OS");
        rendering_println("  version    - Show Unit-OS version");
        rendering_println("  cpu        - Show CPU information");
        rendering_println("  mem        - Show memory information");
        rendering_println("  uptime     - Show system uptime");
        rendering_println("  ke         - Enter kernel mode");
        rendering_println("  ke exit    - Leave kernel mode");
        rendering_println("  reboot     - Reboot the system");
        rendering_println("  shutdown   - Shut down Unit-OS");
        rendering_println("  exit       - Halt Unit-OS");

        return;
    }


    /* CLEAR */

    if (string_equals(command, "clear"))
    {
        rendering_clear();

        return;
    }


    /* ECHO */

    if (string_starts_with(command, "echo "))
    {
        rendering_println(command + 5);

        return;
    }


    /* HISTORY */

    if (string_equals(command, "history"))
    {
        int start = 0;

        if (history_count > HISTORY_SIZE)
            start = history_count - HISTORY_SIZE;

        for (int i = start; i < history_count; i++)
        {
            int index = i % HISTORY_SIZE;

            rendering_print(history[index]);
            rendering_putchar('\n');
        }

        return;
    }


    /* ABOUT */

    if (string_equals(command, "about"))
    {
        rendering_println("Unit-OS");
        rendering_println("-------");
        rendering_println("A tiny hobby operating system");
        rendering_println("");
        rendering_println("Version: 0.0.3");
        rendering_println("Architecture: x86 (32-bit)");
        rendering_println("Shell: UnitShell");
        rendering_println("Display: VGA Text Mode");
        rendering_println("Keyboard: PS/2");
        rendering_println("Timer: PIT");
        rendering_println("");

        return;
    }


    /* VERSION */

    if (string_equals(command, "version"))
    {
        rendering_println("Unit-OS version 0.0.3");

        return;
    }


    /* CPU */

    if (string_equals(command, "cpu"))
    {
        rendering_println("CPU");
        rendering_println("---");
        rendering_println("Architecture: x86");
        rendering_println("Mode: 32-bit");

        return;
    }


    /* MEMORY */

    if (string_equals(command, "mem"))
    {
        rendering_println("Memory");
        rendering_println("------");
        rendering_println("Memory manager: Not initialized");
        rendering_println("Detailed memory information coming later.");

        return;
    }


    /* UPTIME */

    if (string_equals(command, "uptime"))
    {
        uint32_t seconds = timer_get_seconds();

        uint32_t hours = seconds / 3600;

        seconds %= 3600;

        uint32_t minutes = seconds / 60;

        seconds %= 60;


        rendering_print("Uptime: ");

        if (hours < 10)
            rendering_putchar('0');

        rendering_print_number(hours);

        rendering_putchar(':');

        if (minutes < 10)
            rendering_putchar('0');

        rendering_print_number(minutes);

        rendering_putchar(':');

        if (seconds < 10)
            rendering_putchar('0');

        rendering_print_number(seconds);

        rendering_putchar('\n');

        return;
    }


    /* KE */

    if (string_equals(command, "ke"))
    {
        ke_mode = 1;

        rendering_println("[KE] Kernel mode enabled.");

        return;
    }


    /* LEAVE KE */

    if (string_equals(command, "ke exit"))
    {
        ke_mode = 0;

        rendering_println("[KE] Kernel mode disabled.");

        return;
    }


    /* REBOOT */

    if (string_equals(command, "reboot"))
    {
        if (!ke_mode)
        {
            rendering_println("Permission denied.");
            rendering_println("Use 'ke' first.");

            return;
        }

        rendering_println("Rebooting Unit-OS...");

        asm volatile ("cli");

        while (1)
            asm volatile ("hlt");
    }


    /* SHUTDOWN */

    if (string_equals(command, "shutdown"))
    {
        if (!ke_mode)
        {
            rendering_println("Permission denied.");
            rendering_println("Use 'ke' first.");

            return;
        }

        rendering_println("Unit-OS shutting down...");

        asm volatile ("cli");

        while (1)
            asm volatile ("hlt");
    }


    /* EXIT */

    if (string_equals(command, "exit"))
    {
        if (!ke_mode)
        {
            rendering_println("Permission denied.");
            rendering_println("Use 'ke' first.");

            return;
        }

        rendering_println("Unit-OS halted.");

        asm volatile ("cli");

        while (1)
            asm volatile ("hlt");
    }


    /* UNKNOWN COMMAND */

    rendering_print("Unknown command: ");

    rendering_println(command);
}


/* =================================================
   Read keyboard input
   ================================================= */

static int read_line(void)
{
    int length = 0;

    history_position = -1;


    while (1)
    {
        int key = keyboard_getkey();


        /*
         * No key available.
         */

        if (key == 0)
        {
            asm volatile ("hlt");

            continue;
        }


        /*
         * ENTER
         */

        if (key == '\n')
        {
            input[length] = '\0';

            rendering_putchar('\n');

            return length;
        }


        /*
         * BACKSPACE
         */

        if (key == '\b')
        {
            if (length > 0)
            {
                length--;

                input[length] = '\0';

                rendering_backspace();
            }

            continue;
        }


        /*
         * UP ARROW
         */

        if (key == KEY_UP)
        {
            if (history_count == 0)
                continue;

            if (history_position == -1)
            {
                history_position =
                    history_count - 1;
            }
            else if (
                history_position > 0 &&
                history_position >=
                    history_count - HISTORY_SIZE
            )
            {
                history_position--;
            }


            int index =
                history_position % HISTORY_SIZE;


            clear_current_input(length);

            string_copy(
                input,
                history[index]
            );

            length =
                string_length(input);

            rendering_print(input);

            continue;
        }


        /*
         * DOWN ARROW
         */

        if (key == KEY_DOWN)
        {
            if (history_position == -1)
                continue;


            if (history_position < history_count - 1)
            {
                history_position++;

                int index =
                    history_position % HISTORY_SIZE;

                clear_current_input(length);

                string_copy(
                    input,
                    history[index]
                );

                length =
                    string_length(input);

                rendering_print(input);
            }
            else
            {
                history_position = -1;

                clear_current_input(length);

                length = 0;

                input[0] = '\0';
            }

            continue;
        }


        /*
         * NORMAL CHARACTERS
         */

        if (key >= 32 && key <= 126)
        {
            if (length < INPUT_SIZE - 1)
            {
                input[length] = (char)key;

                length++;

                input[length] = '\0';

                rendering_putchar((char)key);
            }
        }
    }
}


/* =================================================
   Kernel main
   ================================================= */

void kernel_main(
    unsigned int magic,
    unsigned int multiboot_info
)
{
    /*
     * Multiboot values aren't being used yet.
     */

    (void)magic;
    (void)multiboot_info;


    /*
     * Initialize display.
     */

    rendering_init();


    /*
     * Initialize keyboard.
     */

    keyboard_init();


    /*
     * Initialize PIT timer BEFORE
     * interrupts are enabled.
     */

    timer_init(100);


    /*
     * Initialize IDT + PIC + IRQs.
     *
     * interrupts_init() enables CPU interrupts
     * with STI, so it must come after timer_init().
     */

    interrupts_init();


    /*
     * Startup screen.
     */

    rendering_println("Welcome to Unit OS");
    rendering_println("------------------");
    rendering_println("");
    rendering_println("Unit-OS v0.0.3");
    rendering_println("Kernel initialized successfully.");
    rendering_println("VGA rendering initialized.");
    rendering_println("PS/2 keyboard initialized.");
    rendering_println("PIT timer initialized.");
    rendering_println("Interrupts initialized.");
    rendering_println("");
    rendering_println("Type 'help' for a list of commands.");
    rendering_println("");


    /*
     * Main shell loop.
     */

    while (1)
    {
        if (ke_mode)
            rendering_print("KE> ");
        else
            rendering_print("UnitOS> ");


        read_line();


        save_history(input);


        execute_command(input);
    }
}
