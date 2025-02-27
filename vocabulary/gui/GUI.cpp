#include "GUI.h"
#include "DatabaseCreation.h"
#include "DatabaseTools.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <filesystem>
#include <optional>
#include <string>

#include <ncurses.h>

Interface::Interface(){
    startingScreen();
    getScreenSize();
    refresh();
    initializeDisplay();
    initializeCMDLine();
    m_functions.emplace([this]{return initialMenu();});
}

Interface::~Interface(){
    delwin(m_displayWin);
    delwin(m_CMDLine);
    endwin();
    curs_set(1);
}

void Interface::run(){
    while (!m_functions.empty())
        m_functions.top()();
}

void Interface::initializeCMDLine(){
    //setting first two to 0's defaults to filling the window till the end
    m_CMDLine = newwin(0, 0, LINES - 2, 0);
    wrefresh(m_CMDLine);
}

void Interface::initializeDisplay(){
    m_displayWin = newwin(LINES - 2, 0, 0, 0);
    m_centerRow = std::floor(static_cast<float>(getmaxy(m_displayWin))/2);
}

void Interface::initialDisplay(){
    const std::array<std::string, 4> text = {{
        "o - Open vocabulary",
        "c - Create new vocabulary",
        "d - Delete existing vocabulary",
        "r - Rename existing vocabulary"
    }};

    printInCenter(text);
}

void Interface::initialMenu(){
    initialDisplay();
    int ch;

    while ((ch =  getch()) != ASCIICodes::ESC){
        switch(ch) {
            case ASCIICodes::c:
                createVocabularyInterface();
                break;
            case ASCIICodes::o:
                openVocabulary();
                break;
            case ASCIICodes::d:
                deleteVocabulary();
                break;
            case ASCIICodes::r:
                renameVocabulary();
                break;
            default:
                break;
        }
        initialDisplay();
    }
    m_functions.pop();
}

void Interface::startingScreen(){
    initscr();
    cbreak();
    keypad(stdscr, TRUE);
    noecho();
    set_escdelay(25);
    curs_set(0);
}

void Interface::getScreenSize(){
    getmaxyx(stdscr, m_screenSize.first, m_screenSize.second);
}

void Interface::printInCenter(std::span<std::string const> span){
    werase(m_displayWin);
    int raiseBy = std::ceil((float)span.size() / 2);
    for (int i = 0; i < span.size(); i++){
        mvwprintw(m_displayWin, m_centerRow + i - raiseBy, 1, "%s", span[i].c_str());
    }
    wrefresh(m_displayWin);
}

void Interface::printInCenter(const std::string &text){
    wclear(m_displayWin);
    mvwprintw(m_displayWin, m_centerRow, 1, "%s", text.c_str());
    wrefresh(m_displayWin);
}

void Interface::printInCenter(
        std::span<std::string const> firstSpan,
        std::span<std::string const> secondSpan){
    werase(m_displayWin);
    int raiseBy = std::ceil((float)(firstSpan.size() + secondSpan.size()) / 2);
    for (int i = 0; i < firstSpan.size(); i++){
        mvwprintw(m_displayWin, m_centerRow + i - raiseBy, 1, "%s", firstSpan[i].c_str());
    }
    for (int i = 0; i < secondSpan.size(); i++){
        mvwprintw(m_displayWin, m_centerRow + i + firstSpan.size() - raiseBy, 1, "%s",
                secondSpan[i].c_str());
    }
    wrefresh(m_displayWin);
}

void Interface::printInCenter(const std::string &text, std::span<std::string const> span){
    werase(m_displayWin);
    int raiseBy = std::ceil(((float)(span.size() + 1) / 2));

    mvwprintw(m_displayWin, m_centerRow + - raiseBy, 1, "%s", text.c_str());

    for (int i = 0; i < span.size(); i++){
        mvwprintw(m_displayWin, m_centerRow + i + 1 - raiseBy, 1, "%s", span[i].c_str());
    }
    wrefresh(m_displayWin);
}

Command Interface::writeCommand(){
    curs_set(1);
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

    curs_set(0);

    return cmd;
}

void Interface::renameVocabulary(){
    Command oldVocName = selectVocabulary("Select a vocabulary to delete");

    if (!oldVocName) return;

    printInCenter("Type the new name of vocabulary " + *oldVocName);
    Command newVocName = writeCommand();

    if (!newVocName) return;

    DatabaseCreation::s_renameDatabase(*oldVocName, *newVocName);

    printInCenter("Vocabulary " + *oldVocName + "was renamed to " + *newVocName);
    getch();
}

void Interface::deleteVocabulary(){
    Command vocName = selectVocabulary("Select a vocabulary to delete");

    if (!vocName)
        return;

    printInCenter("Proceed with deletion of vocabulary " +
                *vocName +
                "? [Y/n]");

    int ch;
    while ((ch = getch()) != ASCIICodes::Y  && ch != ASCIICodes::y)
            if (ch == ASCIICodes::ESC || ch == ASCIICodes::n) return;

    DatabaseCreation::s_deleteDatabase(*vocName);

    printInCenter("Database " + *vocName + " deleted");
    getch();
}

std::optional<std::string> Interface::createVocabulary(){
    printInCenter("Enter name of the new vocabulary");

    Command vocName = writeCommand();
    if (!vocName) return std::nullopt; 

    while (DatabaseCreation::s_checkDatabaseExistence(*vocName)){
        printInCenter("Vocabulary with such name already exists, enter a new name");
        vocName = writeCommand();
        if (!vocName) return std::nullopt;
    }

    {
        DatabaseCreation vocCreator;
        vocCreator.createDatabaseWithTables(*vocName);
    }

    printInCenter("New vocabulary created");
    getch();

    return std::move(*vocName);
}

void Interface::createVocabularyInterface(){
    if (DatabaseCreation::s_getNumberOfDatabases() >= 7){
        printInCenter("Seven databases are supported, delete to add");
        getch();
        return;
    }

    Command vocName = createVocabulary();

    if (vocName)
        openVocabulary(*vocName);
}

void Interface::addWord(DatabaseTools &voc){
    printInCenter("Enter the word to add");
    Command word = writeCommand();
    if (!word) return;

    if (voc.checkWordExistance(*word)){
        printInCenter("The word was already added, thus, now flagged");
        getch();
        return;
    }

    printInCenter("Type in the definition for " + *word);
    Command definition = writeCommand();
    if (!definition) return;

    printInCenter("Type in the sentence for " + *word);
    Command sentence = writeCommand();
    if (!sentence) return;

    voc.addWord(*word);
    voc.addDefinition(*word, *definition);
    voc.addSentence(*word, *sentence);
}

void Interface::lookUpWord(DatabaseTools &voc){
    printInCenter("Enter the word to look up");
    Command word = writeCommand();
    if (!word) return;

    voc.incrementNumberOfLookups(*word);
    lookUpWord(voc, *word);
}

void Interface::lookUpWord(DatabaseTools &voc, std::string &word){
    std::optional<std::pair<int, bool>>
        row = voc.lookUpWord(word);

    if (!row){
        printInCenter("There is no such word");
        getch();
        return;
    }

    std::optional<std::vector<std::string>>
        definitions = voc.lookUpDefinitions(word);

    std::optional<std::vector<std::string>>
        sentences = voc.lookUpSentences(word);

    displayLookUpWord(word, *row, *definitions, *sentences);

    int ch;

    while ((ch=getch()) != ASCIICodes::ESC){
        switch (ch){
            case ASCIICodes::d:
                {
                    Command definition = addDefinition(voc, word);
                    if (definition) definitions->emplace_back(*definition);
                }
                break;
            case ASCIICodes::s:
                {
                    Command sentence = addSentence(voc, word);
                    if (sentence) sentences->emplace_back(*sentence);
                }
                break;
            case ASCIICodes::r:
                row->second = switchFlag(voc, word, row->second);
                break;
            case ASCIICodes::x:
                {
                    Command definition = deleteDefinition(voc, word);
                    if (definition)
                        definitions->erase(
                                std::find(
                                    definitions->begin(),
                                    definitions->end(),
                                    *definition));
                }
                break;
            case ASCIICodes::z:
                {
                    Command sentence = deleteSentence(voc, word);
                    if (sentence)
                        sentences->erase(
                                std::find(
                                    sentences->begin(),
                                    sentences->end(),
                                    *sentence));
                }
                break;
            default:
                break;
        }

        displayLookUpWord(word, *row, *definitions, *sentences);
    }
}

void Interface::displayLookUpWord(
        std::string &word,
        std::pair<int, bool> &row,
        std::span<std::string> definitions,
        std::span<std::string> sentences
        ){
    werase(m_displayWin);

    int raiseBy = std::ceil((float)(definitions.size() + sentences.size()
                + 1 // isRepeatFlagged
                + 5) // help letters
                / 2);

    if (definitions.size()) ++raiseBy;
    if (sentences.size()) ++raiseBy;

    const std::string isRepeatFlagged = row.second ? "To be repeated." : "Memorized.";

    mvwprintw(m_displayWin, m_centerRow - raiseBy--, 1, "%s", word.c_str());
    mvwprintw(m_displayWin, m_centerRow - raiseBy--, 1, "%s %d. %s",
            "Times viewed:",
            row.first,
            isRepeatFlagged.c_str()
            );

    if (definitions.size()){
        mvwprintw(m_displayWin, m_centerRow - raiseBy--, 1, "%s", "Definitions:");

        for (std::size_t i = 0; i < definitions.size(); i++)
            mvwprintw(m_displayWin, m_centerRow - raiseBy--, 1, "%d. %s",
                    static_cast<int>(i+1),
                definitions[i].c_str());
    }

    if (sentences.size()){
        mvwprintw(m_displayWin,
                m_centerRow - raiseBy--,
                1, "%s", "Sentences:");

        for (std::size_t i = 0; i < sentences.size(); i++){
            mvwprintw(m_displayWin,
                    m_centerRow - raiseBy--,
                    1, "%d. %s",
                    static_cast<int>(i+1),
                    sentences[i].c_str());
        }
    }

    //do not forget to switch raiseBy when adding or deleting line
    raiseBy--; //for empty line
    mvwprintw(m_displayWin, m_centerRow - raiseBy--, 1, "%s", "d - add definition to the word");
    mvwprintw(m_displayWin, m_centerRow - raiseBy--, 1, "%s", "s - add sentence to the word");
    mvwprintw(m_displayWin, m_centerRow - raiseBy--, 1, "%s", "r - switch status of memorization");
    mvwprintw(m_displayWin, m_centerRow - raiseBy--, 1, "%s", "x - delete definition");
    mvwprintw(m_displayWin, m_centerRow - raiseBy--, 1, "%s", "z - delete sentence");

    wrefresh(m_displayWin);
}

Command Interface::deleteDefinition(DatabaseTools &voc, const std::string &word){
    std::optional<std::vector<std::string>>
        definitions = voc.lookUpDefinitions(word);

    if (!definitions){
        printInCenter("Word does not have any definitions");
        getch();
        return std::nullopt;
    }

    int raiseBy = std::ceil((float)(definitions->size() + 1) / 2);

    werase(m_displayWin);
    mvwprintw(m_displayWin, m_centerRow - raiseBy, 1, "%s", "Choose definition to delete");

    for (std::size_t i = 0; i < definitions->size(); i++)
        mvwprintw(m_displayWin, m_centerRow - raiseBy + 3 + i, 1, "%d. %s",
                static_cast<int>(i+1),
                (*definitions)[i].c_str());

    wrefresh(m_displayWin);

    int ch;
    while ((ch = getch()) != ASCIICodes::ESC){
        ch -= 49; //49 is the code of 1, aligning to integer
        if (ch < definitions->size()){
            std::string deletedDefinition = (*definitions)[ch];
            voc.deleteDefinition(deletedDefinition);

            return std::move(deletedDefinition);
        }
    }

    return std::nullopt;
}

Command Interface::deleteSentence(DatabaseTools &voc, const std::string &word){
    std::optional<std::vector<std::string>>
        sentences = voc.lookUpSentences(word);

    if (!sentences){
        printInCenter("Word does not have any sentences");
        getch();
        return std::nullopt;
    }

    int raiseBy = std::ceil((float)(sentences->size() + 1) / 2);

    werase(m_displayWin);
    mvwprintw(m_displayWin, m_centerRow - raiseBy, 1, "%s", "Choose sentence to delete");

    for (std::size_t i = 0; i < sentences->size(); i++)
        mvwprintw(m_displayWin, m_centerRow - raiseBy + 3 + i, 1, "%d. %s",
                static_cast<int>(i+1),
                (*sentences)[i].c_str());

    wrefresh(m_displayWin);

    int ch;
    while ((ch = getch()) != ASCIICodes::ESC){
        ch -= 49; //49 is the code of 1, aligning to integer
        if (ch < sentences->size()){
            std::string deletedSentence = (*sentences)[ch];
            voc.deleteSentence(deletedSentence);

            return std::move(deletedSentence);
        }
    }

    return std::nullopt;
}

bool Interface::switchFlag(DatabaseTools &voc, const std::string &word, bool flag){
    if (flag){
        voc.unflagWord(word);
        return false;
    }

    voc.flagWord(word);
    return true;
}

Command Interface::addDefinition(DatabaseTools &voc, const std::string &word){
    printInCenter("Type in the definition");
    Command definition = writeCommand();
    if (!definition) return std::nullopt;

    voc.addDefinition(word, *definition);
    return std::move(*definition);
}

Command Interface::addSentence(DatabaseTools &voc, const std::string& word){
    printInCenter("Type in the sentence");
    Command sentence = writeCommand();
    if (!sentence) return std::nullopt;

    voc.addSentence(word, *sentence);

    return std::move(*sentence);
}

void Interface::openVocabulary(const std::string &vocName){
    runVocabulary(vocName);
}

void Interface::openVocabulary(){
    Command vocName = selectVocabulary("Select a vocabulary to enter");

    if (vocName)
        runVocabulary(*vocName);
}

Command Interface::selectVocabulary(const std::string &msg){
    std::map<char, std::string> bindedNames;

    getMappedNames(bindedNames);

    if (!bindedNames.size()){
        printInCenter("No vocabularies exist");
        getch();
        return std::nullopt;
    }

    std::vector<std::string> text;
    text.reserve(bindedNames.size());

    for (const auto &pair: bindedNames)
        text.emplace_back(std::string{pair.first} + " - " + pair.second);

    printInCenter(msg, text);

    int ch;
    while ((ch = getch()) != ASCIICodes::ESC){
        if (char key = static_cast<char>(ch); bindedNames.contains(key)){
            return bindedNames[key];
        }
        printInCenter(msg, text);
    }

    return std::nullopt;
}

void Interface::getMappedNames(std::map<char, std::string> &map){
    const std::array<char, 7> bindsToVocabularyNames{{
        'a', 's', 'd', 'f', 'j', 'k', 'l'
    }};

    std::filesystem::directory_iterator dirIter;

    try{
        dirIter = std::filesystem::directory_iterator(
                std::filesystem::current_path() / "Databases"
                );
    }
    catch (const std::filesystem::filesystem_error& e){
        return;
    }

    for (const std::filesystem::directory_entry &dirEntry: dirIter){
        std::string path = dirEntry.path();

        if (path.size() > 3)
            if (path.ends_with(".db")){
                std::string::size_type nextLastSlashPos = path.rfind("/", std::string::npos) + 1;

                map.emplace(std::make_pair(
                            bindsToVocabularyNames[map.size()],
                            path.substr(
                                nextLastSlashPos,
                                path.size() - nextLastSlashPos - 3)
                            ));
            }
    }
}

void Interface::runVocabulary(const std::string &vocName){
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
            case ASCIICodes::r:
                randomWord(vocabulary);
                break;
            default:
                break;
        }
        defaultVocabularyDisplay();
    }
}

void Interface::defaultVocabularyDisplay(){
    const std::array<std::string, 7> text = {{
        "a - Add word or phrase to the vocabulary",
        "l - Look up a word",
        "r - Print random word"
    }};

    printInCenter(text);
}

void Interface::randomWordInitialDisplay(const std::string &word){
    const std::array<std::string, 8> text ={{
        word,
        "",
        "a - add sentence to the word",
        "<SPACE> - add sentence to the word and go to next word",
        "d - see definitions of the word",
        "s - see sentences of the word",
        "m - flag the word as memorized, go to next word",
        "l - look up word"
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
            (void)addSentence(vocabulary, *random_word);
            break;
        case ASCIICodes::SPACE:
            (void)addSentence(vocabulary, *random_word);
            random_word = vocabulary.getRandomFlaggedWord();
            vocabulary.incrementNumberOfLookups(*random_word);
            break;
        case ASCIICodes::d:
            {
                std::optional<std::vector<std::string>>
                    definitions = vocabulary.lookUpDefinitions(*random_word);

                if (definitions)
                    printInCenter("The definition(s) of word " + *random_word,
                        *definitions);
                else printInCenter("No definitions for word " + *random_word);

                getch();
            }
            break;
        case ASCIICodes::m:
            vocabulary.unflagWord(*random_word);
            printInCenter("The word '" + *random_word + "' was unflagged");
            getch();
            random_word = vocabulary.getRandomFlaggedWord();
            vocabulary.incrementNumberOfLookups(*random_word);
            break;
        case ASCIICodes::l:
            lookUpWord(vocabulary, *random_word);
            break;
        case ASCIICodes::s:
            {
                std::optional<std::vector<std::string>>
                    sentences = vocabulary.lookUpSentences(*random_word);

                if (sentences)
                    printInCenter("The sentence(s) of word " + *random_word,
                        *sentences);

                else printInCenter("No sentences for word " + *random_word);
                getch();
            }
            break;
        default:
            break;
        }

        if (!random_word){
            printInCenter("There are no words to memorize in the vocabulary");
            getch();
            return;
        }

        randomWordInitialDisplay(*random_word);
    }
}

Command Interface::initialVocabulary(){
    printInCenter("Enter name of the vocabulary to enter");

    Command vocName = writeCommand();
    if (!vocName) return std::nullopt;

    while(!DatabaseCreation::s_checkDatabaseExistence(*vocName)){
        const std::array<std::string, 2> text = {{
            "Vocabulary with given name does not exist",
            "Enter name of the vocabulary to enter"
        }};

        printInCenter(text);

        vocName= writeCommand();
        if (!vocName) return std::nullopt;
    }

    return vocName;
}

