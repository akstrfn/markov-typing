#include <vector>
#include <iterator>
#include <ncurses.h>

char sentence[] = "This is a test sentence";
std::vector<char> buffer;
int row, col;
int y = -1, x = -1;
int mid_y, mid_x;
int ch;

//int main(int argc, char *argv[])
int main()
{
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, 1);
    start_color();
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    init_pair(3, COLOR_RED, COLOR_RED);

    getmaxyx(stdscr, row, col);
    mid_x = (col - std::size(sentence))/2;
    mid_y = row/2 - 3;

    mvprintw(mid_y, mid_x, "%s", sentence);
    move(mid_y + 1, mid_x);
    for(int i = 0; i != std::size(sentence) - 1; ++i)
        addch('-');

    move(mid_y + 2, mid_x);
    while(buffer.size() != std::size(sentence) - 1){
        ch = getch();
        buffer.push_back((int)ch);
        getyx(stdscr, y, x);
        if (ch == KEY_BACKSPACE || ch == '\b' || ch == 127 || ch == KEY_DC) {
            move(y, --x);
            delch();
            buffer.pop_back();
        } else if (sentence[x - mid_x] == ch) {
            addch(ch | COLOR_PAIR(2));
        } else {
            if (ch == ' ') 
                addch(ch | COLOR_PAIR(3));
            else 
                addch(ch | COLOR_PAIR(1));
        }
    }
    //refresh();
    while(getch() != KEY_F(1)){ }
    endwin();
    return 0;
}
