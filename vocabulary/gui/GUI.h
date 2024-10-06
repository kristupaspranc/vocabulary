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
    WINDOW *m_CMDLine;
    WINDOW *m_displayWin;
    std::pair<int, int> m_screenSize;
    int m_centerRow;


    struct ASCIICodes{
        static constexpr int ENTER = 10;
        static constexpr int ESC = 27;
        static constexpr int SPACE = 32;
        static constexpr int a = 97;
        static constexpr int c = 99;
        static constexpr int d = 100;
        static constexpr int f = 102;
        static constexpr int j = 106;
        static constexpr int l = 108;
        static constexpr int m = 109;
        static constexpr int o = 111;
        static constexpr int r = 114;
        static constexpr int s = 115;
        static constexpr int BACKSPACE = 263;
    };

private:
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

