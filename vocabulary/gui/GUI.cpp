#include "GUI.h"
#include <ncurses.h>


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
    initialVocabulary();
    DatabaseTools vocabulary{command};
    defaultVocabularyDisplay();
    int ch;

    while ((ch = getch()) != KEY_F(1)){
        switch(ch) {
            case 97: // a
                {
                    wclear(displayWin);
                    mvwprintw(displayWin, 3, 3, "%s", "Enter the word to add");
                    wrefresh(displayWin);
                    writeCommand();
                    vocabulary.addWord(command);
                    break;
                }
            case 108: // l
                {
                    wclear(displayWin);
                    mvwprintw(displayWin, 3, 3, "%s", "Enter the word to look up");
                    wrefresh(displayWin);
                    writeCommand();
                    wclear(displayWin);
                    std::unique_ptr<std::array<std::string,2>>
                        row = vocabulary.lookUpWord(command);
                    if (row){
                        mvwprintw(displayWin, 3, 3, "%s %s %s",
                                command.c_str(),
                                (*row)[0].c_str(),
                                (*row)[1].c_str()
                                );
                    }
                    else {
                        mvwprintw(displayWin, 3, 3, "%s",
                                "There is no such word"
                                );
                    }

                    wrefresh(displayWin);
                    getch();
                    break;
                }
            case 100: //d
                {
                    wclear(displayWin);
                    mvwprintw(displayWin, 3, 3, "%s", "Enter the word to see definitions");
                    wrefresh(displayWin);
                    writeCommand();
                    wclear(displayWin);
                    std::unique_ptr<std::vector<std::string>>
                        definitions = vocabulary.lookUpDefinitions(command);
                    if (definitions){
                        for (std::size_t i = 3; i < definitions->size() + 3; i++)
                            mvwprintw(displayWin, i, 3, "%s",
                                    (*definitions)[i-3].c_str());
                    }
                    else
                        mvwprintw(displayWin, 3, 3, "%s",
                                "There is no such word"
                                );
                    wrefresh(displayWin);
                    getch();
                    break;
                }
            case 102: // f
                {
                    wclear(displayWin);
                    mvwprintw(displayWin, 3, 3, "%s", "Choose phrase to add definition to");
                    wrefresh(displayWin);
                    writeCommand();
                    wclear(displayWin);
                    std::string phrase = command;
                    wclear(displayWin);
                    mvwprintw(displayWin, 3, 3, "%s", "Type in the definition");
                    wrefresh(displayWin);
                    writeCommand();
                    vocabulary.addDefinition(phrase, command);
                }
        }

        defaultVocabularyDisplay();
    }
}

void Interface::defaultVocabularyDisplay(){
    wclear(displayWin);
    mvwprintw(displayWin, 3, 3, "%s", "a - Add word or phrase to the vocabulary");
    mvwprintw(displayWin, 4, 3, "%s", "d - Look at definitions of word");
    mvwprintw(displayWin, 5, 3, "%s", "l - Look up a word");
    mvwprintw(displayWin, 6, 3, "%s", "f - Add definition to chosen word");
    wrefresh(displayWin);
}

void Interface::initialVocabulary(){
    mvwprintw(displayWin, 3, 3, "%s", "Enter name of the vocabulary to enter");
    wrefresh(displayWin);
    writeCommand();
    while(!DatabaseUtils::checkDatabaseExistence(command)){
        wclear(displayWin);
        mvwprintw(displayWin, 3, 3, "%s", "Vocabulary with given name does not exist");
        mvwprintw(displayWin, 4, 3, "%s", "Enter name of the vocabulary to enter");
        wrefresh(displayWin);
        writeCommand();
    }
}
