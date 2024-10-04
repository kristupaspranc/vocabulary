#include <string>
#include <utility>

#include <ncurses.h>

#include "DatabaseCreation.h"
#include "DatabaseTools.h"
#include "DatabaseUtils.h"

using Command = std::optional<std::string>;

template <typename T>
concept stringArray = std::same_as<T, std::string>;

class Interface{
    WINDOW *CMDLine;
    WINDOW *displayWin;
    std::pair<int, int> screenSize;
    int centerRow;

    static constexpr int ESC = 27;

    void initializeCMDLine();

    void initializeDisplay();

    void startingScreen();

    void getScreenSize();

    Command writeCommand();

    void createVocabulary();

    void openVocabulary();

    void defaultVocabularyDisplay();

    Command initialVocabulary();

    void randomWord(DatabaseTools &vocabulary);

    void randomWordInitialDisplay(std::string &word);

    void initialDisplay();

    void printInCenter(const std::string &text);

    template <stringArray T, std::size_t N>
    void printInCenter(const std::array<T,N> &array);

    template <stringArray T, std::size_t N>
    void printInCenter(const std::array<T,N> &array, const std::vector<T> &vec);

    void addWord(DatabaseTools &voc);

    void lookUpWord(DatabaseTools &voc);

    void displayDefinitions(DatabaseTools &voc);

    void addDefinition(DatabaseTools &voc);

    void displaySentences(DatabaseTools &voc);

    void addSentence(DatabaseTools &voc);

public:
    Interface();
    ~Interface();

    void run();
};

