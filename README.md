# Vocabulary
Vocabulary - minimalistic command line program to write down words which a
person would like to memorize and begin using on day-to-day basis. The approach
of learning new language or improving current skills is based on learning
desired words via repetition and use in context.

## Philosophy
The mindset applied here is that all the work must be done by a user, that is
whenever a new word is added, the learner must type in the definition, which in
ideal case will be user's own words, and also the sentence, in which the word
may be used providing context.<br>
Furthermore, there are two options to be used to interact with the words that
are in the database.<br>
- The first option is to search for the added word to look up its definition and
sentences it was used in.<br>
- The second option randomly suggests added words and the user must respond
whether the word is known and used up to learner's standards and add a another
sentence with that word contextualised. There are two pools from which the
words may be selected: flagged words and unflagged words. If a word is flagged,
it means that the learner is not comfortable using it yet and has to practice
it further, whereas unflagged are well-known and being used.

## Interaction
To begin with, all the controls are keyboard based to save time. The hotkey to
come back to the previous screen or exit the program is Escape.<br>
Here are the stages of the program:
1. The beginning Menu allows to choose or create vocabulary, for instance,
vocabularies could be English, French or Lithuanian.
2. With the selection of the vocabulary the database of words is selected. Next
the learner may opt to add new words, look up or randomly practise already added
words.

## Technical Details
The program is developed and tested on Arch Linux with Alacritty terminal
emulator, though, could be compiled in other environments. To install check
installation branch. The program is written in C++ using ncurses and SQLite3
libraries.

