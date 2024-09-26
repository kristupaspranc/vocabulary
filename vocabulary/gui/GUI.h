#include <string>
#include <utility>

#include <ncurses.h>

#include "DatabaseCreation.h"
#include "DatabaseTools.h"
#include "DatabaseUtils.h"

class Interface{
    WINDOW *CMDLine;
    WINDOW *displayWin;
    std::pair<int, int> screenSize;
    std::string command;

    void initializeCMDLine();

    void initializeDisplay();

    void startingScreen();

    void getScreenSize();

    void writeCommand();

    void createVocabulary();

    void openVocabulary();

    void defaultVocabularyDisplay();

    void initialVocabulary();

public:
    Interface();
    ~Interface();
};

