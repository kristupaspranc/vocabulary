#include <map>
#include <string>
#include <utility>
#include <span>

#include <ncurses.h>

#include "DatabaseCreation.h"
#include "DatabaseTools.h"
#include "DatabaseUtils.h"

using Command = std::optional<std::string>;

class Interface{
    WINDOW *m_CMDLine;
    WINDOW *m_displayWin;
    std::pair<int, int> m_screenSize;
    int m_centerRow;


    struct ASCIICodes{
        static constexpr int ENTER = 10;
        static constexpr int ESC = 27;
        static constexpr int SPACE = 32;
        static constexpr int Y = 89;
        static constexpr int a = 97;
        static constexpr int c = 99;
        static constexpr int d = 100;
        static constexpr int l = 108;
        static constexpr int m = 109;
        static constexpr int n = 110;
        static constexpr int o = 111;
        static constexpr int r = 114;
        static constexpr int s = 115;
        static constexpr int x = 120;
        static constexpr int y = 121;
        static constexpr int z = 122;
        static constexpr int BACKSPACE = 263;
    };

private:
    void initializeCMDLine();

    void initializeDisplay();

    void startingScreen();

    void getScreenSize();

    Command writeCommand();

    std::optional<std::string> createVocabulary();

    void openVocabulary();

    void openVocabulary(const std::string &vocName);

    inline void runVocabulary(const std::string &vocName);

    void defaultVocabularyDisplay();

    Command initialVocabulary();

    void randomWord(DatabaseTools &vocabulary);

    void randomWordInitialDisplay(const std::string &word);

    void initialDisplay();

    void printInCenter(const std::string &text);

    void printInCenter(const std::string &text, std::span<std::string const> span);

    void printInCenter(std::span<std::string const> span);

    void printInCenter(
            std::span<std::string const> firstSpan,
            std::span<std::string const> secondSpan);

    void addWord(DatabaseTools &voc);

    void lookUpWord(DatabaseTools &voc);

    void lookUpWord(DatabaseTools &voc, std::string &word);

    Command addDefinition(DatabaseTools &voc, const std::string &word);

    Command addSentence(DatabaseTools &voc, const std::string& word);

    void getMappedNames(std::map<char, std::string> &map);

    Command selectVocabulary(const std::string &msg);

    void deleteVocabulary();

    void displayLookUpWord(
            std::string &word,
            std::pair<int, bool> &row,
            std::span<std::string> definitions,
            std::span<std::string> sentences);

    bool switchFlag(DatabaseTools &voc, const std::string &word, bool flag);

    Command deleteDefinition(DatabaseTools &voc, const std::string &word);

    Command deleteSentence(DatabaseTools &voc, const std::string &word);

public:
    Interface();
    Interface(const Interface& inter) = delete;
    Interface& operator=(const Interface& inter) = delete;
    Interface(const Interface&& inter) = delete;
    Interface operator=(const Interface&& inter) = delete;
    ~Interface();

    void run();
};

