#include "GUI.h"
#include <ncurses.h>
#include <optional>


Interface::Interface(){
    startingScreen();
    getScreenSize();
    refresh();
    initializeDisplay();
    initializeCMDLine();

    int ch;

    while ((ch =  getch()) != KEY_F(1)){
        switch(ch) {
            case 99: // c
                {
                    werase(displayWin);
                    createVocabulary();
                    break;
                }
            case 111: // o
                {
                    werase(displayWin);
                    openVocabulary();
                    break;
                }
        }
        initialDisplay();
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
    initialDisplay();
}

void Interface::initialDisplay(){
    werase(displayWin);
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
    *command = "";

    while ((ch=getch()) != 10){ // <Enter>
        switch(ch) {
            case 263: // backspace
                {
                    if (!command->empty())
                        command->pop_back();
                        mvwdelch(CMDLine, 1, command->length());
                    break;
                }
            case KEY_F(1):
                {
                    command = std::nullopt;
                    werase(CMDLine);
                    return;
                }
            default:
                {
                    char symbol = static_cast<char>(ch);
                    command->push_back(symbol);
                    mvwaddch(CMDLine, 1, command->length() - 1, symbol);
                    break;
                }
        }
        wrefresh(CMDLine);
    }

    werase(CMDLine);
    wrefresh(CMDLine);
    mvwprintw(CMDLine, 3, 3, "%s", "Display window initialized");
}

void Interface::createVocabulary(){
    mvwprintw(displayWin, 3, 3, "%s", "Enter name of the new vocabulary");
    wrefresh(displayWin);
    writeCommand();
    if (!command) return;
    (void)DatabaseCreation{*command};
    werase(displayWin);
    mvwprintw(displayWin, 3, 3, "%s", "New vocabulary created");
    wrefresh(displayWin);
    getch();
}


void Interface::openVocabulary(){
    initialVocabulary();
    if (!command) return;
    DatabaseTools vocabulary{*command};
    defaultVocabularyDisplay();
    int ch;

    while ((ch = getch()) != KEY_F(1)){
        switch(ch) {
            case 97: // a
                {
                    werase(displayWin);
                    mvwprintw(displayWin, 3, 3, "%s", "Enter the word to add");
                    wrefresh(displayWin);
                    writeCommand();
                    if (!command) break;
                    vocabulary.addWord(*command);
                    break;
                }
            case 108: // l
                {
                    werase(displayWin);
                    mvwprintw(displayWin, 3, 3, "%s", "Enter the word to look up");
                    wrefresh(displayWin);
                    writeCommand();
                    if (!command) break;
                    werase(displayWin);
                    std::optional<std::pair<int, bool>>
                        row = vocabulary.lookUpWord(*command);
                    if (row){
                        mvwprintw(displayWin, 3, 3, "%s %d %d",
                                command->c_str(),
                                row->first,
                                row->second
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
                    werase(displayWin);
                    mvwprintw(displayWin, 3, 3, "%s", "Enter the word to see definitions");
                    wrefresh(displayWin);
                    writeCommand();
                    if (!command) break;
                    werase(displayWin);
                    std::optional<std::vector<std::string>>
                        definitions = vocabulary.lookUpDefinitions(*command);
                    if (definitions){
                        for (std::size_t i = 0; i < definitions->size(); i++)
                            mvwprintw(displayWin, i+3, 3, "%s",
                                    (*definitions)[i].c_str());
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
                    werase(displayWin);
                    mvwprintw(displayWin, 3, 3, "%s", "Choose phrase to add definition to");
                    wrefresh(displayWin);
                    writeCommand();
                    if (!command) break;
                    werase(displayWin);
                    std::string phrase = *command;
                    werase(displayWin);
                    mvwprintw(displayWin, 3, 3, "%s", "Type in the definition");
                    wrefresh(displayWin);
                    writeCommand();
                    if (!command) break;
                    vocabulary.addDefinition(phrase, *command);
                    break;
                }
            case 115: //s
                {
                    werase(displayWin);
                    mvwprintw(displayWin, 3, 3, "%s", "Enter the word to see sentences");
                    wrefresh(displayWin);
                    writeCommand();
                    if (!command) break;
                    werase(displayWin);
                    std::optional<std::vector<std::string>>
                        definitions = vocabulary.lookUpSentences(*command);
                    if (definitions){
                        for (std::size_t i = 0; i < definitions->size(); i++)
                            mvwprintw(displayWin, i+3, 3, "%s",
                                    (*definitions)[i].c_str());
                    }
                    else
                        mvwprintw(displayWin, 3, 3, "%s",
                                "There is no such word"
                                );
                    wrefresh(displayWin);
                    getch();
                    break;

                }
            case 106: // j
                {
                    werase(displayWin);
                    mvwprintw(displayWin, 3, 3, "%s", "Choose phrase to add sentence to");
                    wrefresh(displayWin);
                    writeCommand();
                    if (!command) break;
                    werase(displayWin);
                    std::string phrase = *command;
                    werase(displayWin);
                    mvwprintw(displayWin, 3, 3, "%s", "Type in the sentence");
                    wrefresh(displayWin);
                    writeCommand();
                    if (!command) break;
                    vocabulary.addSentence(phrase, *command);
                    break;
                }
            case 114: //r
                {
                    randomWord(vocabulary);
                    break;
                }
        }
        defaultVocabularyDisplay();
    }
}

void Interface::randomWordInitialDisplay(std::string &word){
    werase(displayWin);
    mvwprintw(displayWin, 3, 3, "%s %s", "The random word is", word.c_str());
    mvwprintw(displayWin, 4, 3, "%s", "a - add sentence to the word");
    mvwprintw(displayWin, 5, 3, "%s", "<SPACE> - add sentence to the word and go to next word");
    mvwprintw(displayWin, 6, 3, "%s", "d - see definitions of the word");
    mvwprintw(displayWin, 7, 3, "%s", "m - flag the word as memorized, go to next word");
    wrefresh(displayWin);
}

void Interface::randomWord(DatabaseTools &vocabulary){
    std::optional<std::string> random_word = vocabulary.getRandomFlaggedWord();

    if (!random_word){
        mvwprintw(displayWin, 3, 3, "%s", "There are no words to memorize in the vocabulary");
        wrefresh(displayWin);
        getch();
        return;
    }

    randomWordInitialDisplay(*random_word);

    int ch;

    while((ch = getch()) != KEY_F(1)){
        switch (ch){
        case 97: // a
            {
                werase(displayWin);
                mvwprintw(displayWin, 3, 3, "%s %s",
                        "Add a sentence with context for word ",
                        random_word->c_str()
                        );
                wrefresh(displayWin);
                writeCommand();
                if (!command) break;
                vocabulary.addSentence(*random_word, *command);
                break;
            }
        case 32: // <SPACE>
            {
                werase(displayWin);
                mvwprintw(displayWin, 3, 3, "%s %s",
                        "Add a sentence with context for word ",
                        random_word->c_str()
                        );
                wrefresh(displayWin);
                writeCommand();
                if (!command) break;
                vocabulary.addSentence(*random_word, *command);
                random_word = vocabulary.getRandomFlaggedWord();
                break;
            }
        case 100: //d
            {
                werase(displayWin);
                mvwprintw(displayWin, 3, 3, "%s %s",
                        "The definitions of word ",
                        random_word->c_str());

                std::optional<std::vector<std::string>>
                    definitions = vocabulary.lookUpDefinitions(*random_word);
                for (int i = 0; i < definitions->size(); i++)
                    mvwprintw(displayWin, i+4, 3, "%d %s",
                            i+1,
                            (*definitions)[i].c_str()
                            );
                wrefresh(displayWin);
                getch();
            }
        case 109: //m
            {
                vocabulary.unflagWord(*random_word);
                mvwprintw(displayWin, 3, 3, "%s %s %s",
                        "The word '",
                        random_word->c_str(),
                        "' was unflagged'"
                        );
                random_word = vocabulary.getRandomFlaggedWord();
                getch();
            }
        }

        if (!random_word){
            werase(displayWin);
            mvwprintw(displayWin, 3, 3, "%s", "There are no words to memorize in the vocabulary");
            wrefresh(displayWin);
            getch();
            return;
        }

        randomWordInitialDisplay(*random_word);
    }
}

void Interface::defaultVocabularyDisplay(){
    werase(displayWin);
    mvwprintw(displayWin, 3, 3, "%s", "a - Add word or phrase to the vocabulary");
    mvwprintw(displayWin, 4, 3, "%s", "d - Look at definitions of word");
    mvwprintw(displayWin, 5, 3, "%s", "l - Look up a word");
    mvwprintw(displayWin, 6, 3, "%s", "f - Add definition to chosen word");
    mvwprintw(displayWin, 7, 3, "%s", "s - Look at sentences of chosen word");
    mvwprintw(displayWin, 8, 3, "%s", "j - Add sentence to chosen word");
    mvwprintw(displayWin, 9, 3, "%s", "r - Print random word");
    wrefresh(displayWin);
}

void Interface::initialVocabulary(){
    mvwprintw(displayWin, 3, 3, "%s", "Enter name of the vocabulary to enter");
    wrefresh(displayWin);
    writeCommand();
    if (!command) return;
    while(!DatabaseUtils::checkDatabaseExistence(*command)){
        werase(displayWin);
        mvwprintw(displayWin, 3, 3, "%s", "Vocabulary with given name does not exist");
        mvwprintw(displayWin, 4, 3, "%s", "Enter name of the vocabulary to enter");
        wrefresh(displayWin);
        writeCommand();
        if (!command) return;
    }
}
