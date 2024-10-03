#include <string>
#include <utility>

#include <ncurses.h>

#include "DatabaseCreation.h"
#include "DatabaseTools.h"
#include "DatabaseUtils.h"

template <typename T>
concept stringArray = std::same_as<T, std::string>;

class Interface{
    WINDOW *CMDLine;
    WINDOW *displayWin;
    std::pair<int, int> screenSize;
    int centerRow;
    std::optional<std::string> command = "";

    void initializeCMDLine();

    void initializeDisplay();

    void startingScreen();

    void getScreenSize();

    void writeCommand();

    void createVocabulary();

    void openVocabulary();

    void defaultVocabularyDisplay();

    void initialVocabulary();

    void randomWord(DatabaseTools &vocabulary);

    void randomWordInitialDisplay(std::string &word);

    void initialDisplay();

    void printInCenter(const std::string &text);

    template <stringArray T, std::size_t N>
    void printInCenter(const std::array<T,N> &array);

    template <stringArray T, std::size_t N>
    void printInCenter(const std::array<T,N> &array, const std::vector<T> &vec);

public:
    Interface();
    ~Interface();
};

