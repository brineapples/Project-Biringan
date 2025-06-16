#include "SaveManager.h"
#include <iostream>
#include <fstream>

bool SaveManager::saveProgress(const std::string& filename, const GameProgress& progress) {
    std::ofstream file(filename);
    if (!file) return false;

    file << progress.backgroundImage << '\n';
    file << progress.speaker << '\n';
    file << progress.dialogue << '\n';

    return true;
}

bool SaveManager::loadProgress(const std::string& filename, GameProgress& progress) {
    std::ifstream file(filename);
    if (!file) return false;

    std::getline(file, progress.backgroundImage);
    std::getline(file, progress.speaker);
    std::getline(file, progress.dialogue);

    return true;
}