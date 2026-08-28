#include "rendering.h"
#include "keyboard.h"


#define INPUT_SIZE 128
#define HISTORY_SIZE 10


static char input[INPUT_SIZE];

static char history[HISTORY_SIZE][INPUT_SIZE];

static int history_count = 0;
static int history_position = -1;

static int ke_mode = 0;


static int string_length(const char *str)
{
    int length = 0;

    while (str[length])
        length++;

    return length;
}


static int string_equals(
    const char *a,
    const char *b
)
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


static int string_starts_with(
    const char *str,
    const char *prefix
)
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


static void string_copy(
    char *destination,
    const char *source
)
{
    int i = 0;

    while (source[i])
    {
        destination[i] = source[i];
        i++;
    }

    destination[i] = '\0';
}


static void clear_current_input(int length)
{
    for (int i = 0; i < length; i++)
        rendering_backspace();
}


static void save_history(const char *command)
{
    if (command[0] == '\0')
        return;


    /*
     * Don't save duplicate consecutive commands.
     */

    if (history_count > 0)
    {
        if (string_equals(
            history[
                (history_count - 1) % HISTORY_SIZE
            ],
            command))
        {
            return;
        }
    }


    int index = history_count % HISTORY_SIZE;

    string_copy(
        history[index],
        command
    );

    history_count++;
}


static void execute_command(char *command)
{
    if (command[0] == '\0')
        return;


    /*
     * HELP
     */

    if (string_equals(command, "help"))
    {
        rendering_println(
            "Unit-OS commands:"
        );

        rendering_println(
            "  help       - Show this help"
        );

        rendering_println(
            "  clear      - Clear the screen"
        );

        rendering_println(
            "  echo       - Print text"
        );

        rendering_println(
            "  ke         - Enter kernel mode"
        );

        rendering_println(
            "  exit       - Halt Unit-OS"
        );

        rendering_println(
            "  ke exit    - Leave kernel mode"
        );

        return;
    }


    /*
     * CLEAR
     */

    if (string_equals(command, "clear"))
    {
        rendering_clear();
        return;
    }


    /*
     * ECHO
     */

    if (string_starts_with(command, "echo "))
    {
        rendering_println(command + 5);
        return;
    }


    /*
     * KE
     */

    if (string_equals(command, "ke"))
    {
        ke_mode = 1;

        rendering_println(
            "[KE] Kernel mode enabled."
        );

        return;
    }


    /*
     * LEAVE KE
     */

    if (string_equals(command, "ke exit"))
    {
        ke_mode = 0;

        rendering_println(
            "[KE] Kernel mode disabled."
        );

        return;
    }


    /*
     * EXIT
     */

    if (string_equals(command, "exit"))
    {
        if (!ke_mode)
        {
            rendering_println(
                "Permission denied."
            );

            rendering_println(
                "Use 'ke' first."
            );

            return;
        }


        rendering_println(
            "Unit-OS shutting down..."
        );


        asm volatile ("cli");

        while (1)
            asm volatile ("hlt");
    }


    /*
     * Unknown command
     */

    rendering_print(
        "Unknown command: "
    );

    rendering_println(command);
}


static int read_line(void)
{
    int length = 0;

    history_position = -1;


    while (1)
    {
        int key = keyboard_getkey();


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


            if (
                history_position <
                history_count - 1
            )
            {
                history_position++;

                int index =
                    history_position %
                    HISTORY_SIZE;


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
         * Regular character
         */

        if (key >= 32 && key <= 126)
        {
            if (length < INPUT_SIZE - 1)
            {
                input[length] =
                    (char)key;

                length++;

                input[length] = '\0';

                rendering_putchar(
                    (char)key
                );
            }
        }
    }
}


void kernel_main(
    unsigned int magic,
    unsigned int multiboot_info
)
{
    (void)magic;
    (void)multiboot_info;


    rendering_init();

    keyboard_init();


    rendering_println(
        "Welcome to Unit OS"
    );

    rendering_println(
        "------------------"
    );

    rendering_println(
        "Unit-OS v0.1"
    );

    rendering_println(
        "Type 'help' for commands."
    );

    rendering_println("");


    while (1)
    {
        if (ke_mode)
            rendering_print("KE> ");
        else
            rendering_print("UnitOS> ");


        int length = read_line();

        save_history(input);

        execute_command(input);

        /*
         * Avoid unused-variable warnings.
         */

        (void)length;
    }
}
