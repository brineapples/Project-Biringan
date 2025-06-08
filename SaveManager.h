#ifndef SAVEMANAGER_HPP
#define SAVEMANAGER_HPP

#include "GameProgress.h"

class SaveManager {
public:
    static bool saveProgress(const std::string& filename, const GameProgress& progress);
    static bool loadProgress(const std::string& filename, GameProgress& progress);
};

#endif