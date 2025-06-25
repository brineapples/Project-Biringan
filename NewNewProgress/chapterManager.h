#ifndef CHAPTER_MANAGER_H
#define CHAPTER_MANAGER_H

#include <SFML/Graphics.hpp>
#include <string>

class ChapterManager {
public:
    static void transitionToChapter(sf::RenderWindow& window, sf::Font& font, const std::string& chapterTitle);
};

#endif