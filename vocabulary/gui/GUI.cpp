#include "GUI.h"

#include <array>
#include <cmath>
#include <optional>

#include <ncurses.h>

Interface::Interface(){
    startingScreen();
    getScreenSize();
    refresh();
    initializeDisplay();
    initializeCMDLine();
}

Interface::~Interface(){
    delwin(m_displayWin);
    delwin(m_CMDLine);
    endwin();
}

void Interface::run(){
    int ch;

    while ((ch =  getch()) != ASCIICodes::ESC){
        switch(ch) {
            case ASCIICodes::c:
                {
                    Command vocName = createVocabulary();
                    if (vocName)
                        openVocabulary(*vocName);
                    break;
                }
                break;
            case ASCIICodes::o:
                {
                    openVocabulary();
                }
                break;
            default:
                break;
        }
        initialDisplay();
    }
}

void Interface::initializeCMDLine(){
    //setting first two to 0's defaults to filling the window till the end
    m_CMDLine = newwin(0, 0, LINES - 2, 0);
    wrefresh(m_CMDLine);
}

void Interface::initializeDisplay(){
    m_displayWin = newwin(LINES - 2, 0, 0, 0);
    m_centerRow = std::floor(static_cast<float>(getmaxy(m_displayWin))/2);
    initialDisplay();
}

void Interface::initialDisplay(){
    std::array<std::string, 2> text = {{
        "o - Open vocabulary",
        "c - Create new vocabulary"
    }};

    printInCenter(text);
}

void Interface::startingScreen(){
    initscr();
    cbreak();
    keypad(stdscr, TRUE);
    noecho();
    set_escdelay(25);
}

void Interface::getScreenSize(){
    getmaxyx(stdscr, m_screenSize.first, m_screenSize.second);
}

template <stringArray T, std::size_t N>
void Interface::printInCenter(const std::array<T,N> &array){
    werase(m_displayWin);
    int raiseBy = std::ceil((float)N / 2);
    for (int i = 0; i < N; i++){
        mvwprintw(m_displayWin, m_centerRow + i - raiseBy, 1, "%s", array[i].c_str());
    }
    wrefresh(m_displayWin);
}

void Interface::printInCenter(const std::string &text){
    wclear(m_displayWin);
    mvwprintw(m_displayWin, m_centerRow, 1, "%s", text.c_str());
    wrefresh(m_displayWin);
}

template <stringArray T, std::size_t N>
void Interface::printInCenter(const std::array<T,N> &array, const std::vector<T> &vec){
    werase(m_displayWin);
    int raiseBy = std::ceil((float)N / 2);
    for (int i = 0; i < N; i++){
        mvwprintw(m_displayWin, m_centerRow + i - raiseBy, 1, "%s", array[i].c_str());
    }
    for (int i = 0; i < vec.size(); i++){
        mvwprintw(m_displayWin, m_centerRow + i + N - raiseBy, 1, "%s", vec[i].c_str());
    }
    wrefresh(m_displayWin);
}

Command Interface::writeCommand(){
    int ch;
    std::string cmd = "";

    while ((ch=getch()) != ASCIICodes::ENTER){
        switch(ch) {
            case ASCIICodes::BACKSPACE:
                {
                    if (!cmd.empty())
                        cmd.pop_back();
                        mvwdelch(m_CMDLine, 1, cmd.length());
                }
                break;
            case ASCIICodes::ESC:
                {
                    werase(m_CMDLine);
                    wrefresh(m_CMDLine);
                    return std::nullopt;
                }
                break;
            default:
                {
                    char symbol = static_cast<char>(ch);
                    cmd.push_back(symbol);
                    mvwaddch(m_CMDLine, 1, cmd.length() - 1, symbol);
                }
                break;
        }
        wrefresh(m_CMDLine);
    }

    werase(m_CMDLine);
    wrefresh(m_CMDLine);

    return cmd;
}

std::optional<std::string> Interface::createVocabulary(){
    printInCenter("Enter name of the new vocabulary");

    Command vocName = writeCommand();
    if (!vocName){
        return std::nullopt;
    }

    (void)DatabaseCreation{*vocName};

    printInCenter("New vocabulary created");
    getch();

    return std::move(*vocName);
}

void Interface::addWord(DatabaseTools &voc){
    printInCenter("Enter the word to add");
    Command word = writeCommand();
    if (!word) return;

    if (!voc.addWord(*word)){
        printInCenter("The word is already added");
        getch();
    }
}

void Interface::lookUpWord(DatabaseTools &voc){
        printInCenter("Enter the word to look up");
        Command word = writeCommand();
        if (!word) return;
        werase(m_displayWin);
        std::optional<std::pair<int, bool>>
            row = voc.lookUpWord(*word);
        if (row){
            mvwprintw(m_displayWin, 3, 3, "%s %d %d",
                    word->c_str(),
                    row->first,
                    row->second
                    );
            wrefresh(m_displayWin);
        }
        else 
            printInCenter("There is no such word");

        getch();
}

void Interface::displayDefinitions(DatabaseTools &voc){
    printInCenter("Enter the word to see definitions");
    Command word = writeCommand();
    if (!word) return;

    werase(m_displayWin);
    std::array<std::string, 1> text{{"Here are the definitions"}};
    std::optional<std::vector<std::string>>
        definitions = voc.lookUpDefinitions(*word);
    if (definitions){
        for (std::size_t i = 0; i < definitions->size(); i++)
            mvwprintw(m_displayWin, i+3, 3, "%s",
                    (*definitions)[i].c_str());
        printInCenter(text, *definitions);
    }
    else 
        printInCenter("There is no such word");

    wrefresh(m_displayWin);
    getch();
}

void Interface::addDefinition(DatabaseTools &voc){
    printInCenter("Choose phrase to add definition to");
    Command word = writeCommand();
    if (!word) return;

    printInCenter("Type in the definition");
    Command definition = writeCommand();
    if (!definition) return;

    voc.addDefinition(*word, *definition);
}

void Interface::displaySentences(DatabaseTools &voc){
    printInCenter("Enter the word to see sentences");
    Command word = writeCommand();
    if (!word) return;

    std::array<std::string, 1> text{{"Here are the sentences"}};

    std::optional<std::vector<std::string>>
        definitions = voc.lookUpSentences(*word);
    if (definitions){
        for (std::size_t i = 0; i < definitions->size(); i++)
            mvwprintw(m_displayWin, i+3, 3, "%s",
                    (*definitions)[i].c_str());
        printInCenter(text, *definitions);
    }
    else
        printInCenter("There is no such word");

    getch();
}

void Interface::addSentence(DatabaseTools &voc){
    printInCenter("Choose phrase to add sentence to");
    Command word = writeCommand();
    if (!word) return;

    printInCenter("Type in the sentence");
    Command sentence = writeCommand();
    if (!sentence) return;

    voc.addSentence(*word, *sentence);
}

void Interface::openVocabulary(std::string &vocName){
    runVocabulary(vocName);
}

void Interface::openVocabulary(){
    Command vocName = initialVocabulary();
    if (!vocName){
        return;
    }

    runVocabulary(*vocName);
}

void Interface::runVocabulary(std::string &vocName){
    DatabaseTools vocabulary{vocName};
    defaultVocabularyDisplay();
    int ch;

    while ((ch = getch()) != ASCIICodes::ESC){
        switch(ch) {
            case ASCIICodes::a:
                addWord(vocabulary);
                break;
            case ASCIICodes::l:
                lookUpWord(vocabulary);
                break;
            case ASCIICodes::d:
                displayDefinitions(vocabulary);
                break;
            case ASCIICodes::f:
                addDefinition(vocabulary);
                break;
            case ASCIICodes::s:
                displaySentences(vocabulary);
                break;
            case ASCIICodes::j:
                addSentence(vocabulary);
                break;
            case ASCIICodes::r:
                randomWord(vocabulary);
                break;
            default:
                break;
        }
        defaultVocabularyDisplay();
    }
}

void Interface::randomWordInitialDisplay(std::string &word){
    std::array<std::string, 5> text ={{
        "The random word is " + word,
        "a - add sentence to the word",
        "<SPACE> - add sentence to the word and go to next word",
        "d - see definitions of the word",
        "m - flag the word as memorized, go to next word"
    }};

    printInCenter(text);
}

void Interface::randomWord(DatabaseTools &vocabulary){
    std::optional<std::string> random_word = vocabulary.getRandomFlaggedWord();

    if (!random_word){
        printInCenter("There are no words to memorize in the vocabulary");
        getch();
        return;
    }

    randomWordInitialDisplay(*random_word);

    int ch;

    while((ch = getch()) != ASCIICodes::ESC){
        switch (ch){
        case ASCIICodes::a:
            {
                printInCenter("Add a sentence with context for word " + *random_word);
                Command sentence = writeCommand();
                if (!sentence) break;

                vocabulary.addSentence(*random_word, *sentence);
                break;
            }
        case ASCIICodes::SPACE:
            {
                printInCenter("Add a sentence with context for word " + *random_word);
                Command sentence = writeCommand();
                if (!sentence) break;

                vocabulary.addSentence(*random_word, *sentence);
                random_word = vocabulary.getRandomFlaggedWord();
                break;
            }
        case ASCIICodes::d:
            {
                std::array<std::string, 1> text = {{
                    "The definition(s) of word " + *random_word
                }};

                std::optional<std::vector<std::string>>
                    definitions = vocabulary.lookUpDefinitions(*random_word);

                printInCenter(text, *definitions);
                getch();
                break;
            }
        case ASCIICodes::m:
            {
                vocabulary.unflagWord(*random_word);
                printInCenter("The word '" + *random_word + "' was unflagged");
                getch();
                random_word = vocabulary.getRandomFlaggedWord();
                break;
            }
        }

        if (!random_word){
            printInCenter("There are no words to memorize in the vocabulary");
            getch();
            return;
        }

        randomWordInitialDisplay(*random_word);
    }
}

void Interface::defaultVocabularyDisplay(){
    std::array<std::string, 7> text = {{
        "a - Add word or phrase to the vocabulary",
        "d - Look at definitions of word",
        "l - Look up a word",
        "f - Add definition to chosen word",
        "s - Look at sentences of chosen word",
        "j - Add sentence to chosen word",
        "r - Print random word"
    }};

    printInCenter(text);
}

Command Interface::initialVocabulary(){
    printInCenter("Enter name of the vocabulary to enter");

    Command vocName = writeCommand();
    if (!vocName) return std::nullopt;

    while(!DatabaseUtils::checkDatabaseExistence(*vocName)){
        std::array<std::string, 2> text = {{
            "Vocabulary with given name does not exist",
            "Enter name of the vocabulary to enter"
        }};

        printInCenter(text);

        vocName= writeCommand();
        if (!vocName) return std::nullopt;
    }

    return vocName;
}
