
#include <chrono>
#include <thread>

#include <curses.h>

using namespace std::chrono;

int main()
{
    initscr();
    cbreak();
    noecho();
    nonl();
    intrflush(stdscr, FALSE);
    keypad(stdscr, TRUE);
    clear();

    for (int c = getch(); c != 'q'; c = getch())
    {
        switch(c)
        {
        case KEY_LEFT: mvaddch(21, 10, '<'); break;
        case KEY_RIGHT: mvaddch(21, 10, '>'); break;
        case KEY_UP: mvaddch(21, 10, '|'); break;
        case ERR: mvaddch(21, 10, '|'); break;
        default: mvaddch(10, 0, c);
        }
        refresh();
        std::this_thread::sleep_for(milliseconds{100});
    }

    nl();
    echo();
    nocbreak();
    endwin();
}
