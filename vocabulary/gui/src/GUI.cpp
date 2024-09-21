#include "GUI.h"

Interface::Interface(){
    startingScreen();
    getScreenSize();
    mvprintw(3, 3, "%s %d %d", "Something on the screen", screenSize.first, screenSize.second);
    refresh();
    initializeDisplay();
    initializeCMDLine();

    int ch;

    while ((ch =  getch()) != KEY_F(1)){
        switch(ch) {
            case 99: // c
                wclear(displayWin);
                createVocabulary();
            case 111: // o
                wclear(displayWin);
                openVocabulary();
            default:
                wclear(displayWin);
                mvwprintw(displayWin, 3, 3, "%s", "Something else was pressed");
                wrefresh(displayWin);
                break;
            }
        }
    }

Interface::~Interface(){
    delwin(displayWin);
    delwin(CMDLine);
    endwin();
}

void Interface::initializeCMDLine(){
    CMDLine = newwin(0, 0, LINES - 2, 0); //setting first two to 0's defaults to filling the window till the end
    wrefresh(CMDLine);
}

void Interface::initializeDisplay(){
    displayWin = newwin(LINES - 2, 0, 0, 0);
    mvwprintw(displayWin, 3, 3, "%s", "o - Open vocabulary");
    mvwprintw(displayWin, 4, 3, "%s", "c - Create new vocabulary");
    wrefresh(displayWin);
}

void Interface::startingScreen(){
    initscr();
    cbreak();
    keypad(stdscr, TRUE);
    noecho();
}

void Interface::getScreenSize(){
    getmaxyx(stdscr, screenSize.first, screenSize.second);
}

void Interface::writeCommand(){
    int ch;
    command = "";

    while ((ch=getch()) != 10){ // <Enter>
        switch(ch) {
            case 263: // backspace
                if (!command.empty())
                    command.pop_back();
                    mvwdelch(CMDLine, 1, command.length());
                break;
            default:
                char symbol = static_cast<char>(ch);
                command.push_back(symbol);
                mvwaddch(CMDLine, 1, command.length() - 1, symbol);
                break;

        }
        wrefresh(CMDLine);
    }

    wclear(CMDLine);
    wrefresh(CMDLine);
    mvwprintw(CMDLine, 3, 3, "%s", "Display window initialized");
}

void Interface::createVocabulary(){
    mvwprintw(displayWin, 3, 3, "%s", "Enter name of the new vocabulary");
    wrefresh(displayWin);
    writeCommand();
    (void)DatabaseCreation{command};
    mvwprintw(displayWin, 3, 3, "%s", "New vocabulary created");
    wrefresh(displayWin);
}


void Interface::openVocabulary(){
    mvwprintw(displayWin, 3, 3, "%s", "Enter name of the vocabulary to enter");
    wrefresh(displayWin);
    writeCommand();
    DatabaseTools vocabulary{command};
    mvwprintw(displayWin, 3, 3, "%s", "a - Add word or phrase to the vocabulary");
    mvwprintw(displayWin, 4, 3, "%s", "d - Add definition to chosen word");
    mvwprintw(displayWin, 5, 3, "%s", "l - Look up a word");
    wrefresh(displayWin);

    int ch;

    while ((ch = getch()) != KEY_F(1)){
        switch(ch) {
            case 97: // a
                mvwprintw(displayWin, 3, 3, "%s", "Enter the word to add");
                wrefresh(displayWin);
                writeCommand();
                vocabulary.addWord(command);
                break;
            case 108: // l
                mvwprintw(displayWin, 3, 3, "%s", "Enter the word to look up");
                wrefresh(displayWin);
                writeCommand();
                std::unique_ptr<std::vector<std::array<std::string,3>>> row = vocabulary.lookUpWord(command);
                mvwprintw(displayWin, 3, 3, "%s %s %s", (*row)[0][0].c_str(), (*row)[0][1].c_str(), (*row)[0][2].c_str());
                wrefresh(displayWin);
        }
    }
}

