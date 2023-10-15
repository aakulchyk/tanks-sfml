#pragma once

#include "MapCreator.h"

#include <map>


class GameObject;
class SpawnController;


class RandomMapCreator : public MapCreator
{
private:
    std::string size_string;
    int segments_w;
    int segments_h;

    void readSegment(std::string filepath, int mapStartIndex);
public:
    RandomMapCreator();
    int parseMapFile(std::string fileName) override;
};
