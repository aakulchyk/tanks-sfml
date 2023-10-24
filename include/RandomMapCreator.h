#pragma once

#include "MapCreator.h"

#include <map>
#include <vector>


class GameObject;
class SpawnController;


class RandomMapCreator : public MapCreator
{
private:
    std::string type;
    int segments_w;
    int segments_h;

    std::map<std::string, std::vector<int>> allowedSegmentVariants;

    void readSegment(std::string filepath, int mapStartIndex);
public:
    RandomMapCreator();
    int parseMapFile(std::string fileName) override;
};
