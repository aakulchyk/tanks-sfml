#pragma once

#include <fstream>
#include <nlohmann/json.hpp>

#include <map>


using json = nlohmann::json;


class GameObject;
class SpawnController;

namespace Level
{
    enum WinCondition
    {
        SurviveTime,
        KillEmAll,
        ObtainFlag
    };

    enum FailCondition
    {
        LoseBase,
        ExpireTime
    };

    struct Properties
    {
        std::string name;
        std::vector<std::string> briefing;

        WinCondition win;
        int winParam; // meaning depends on goal
        FailCondition fail;
        int failParam; // meaning depends on goal
        bool failedToLoad = false;
    };

    const std::map<std::string, WinCondition> winMeaningsMap = {
        { "SURVIVE", SurviveTime },
        { "KILLEMALL", KillEmAll },
        { "OBTAIN", ObtainFlag }
    };

    const std::map<std::string, FailCondition> failMeaningsMap = {
        { "LOSE_BASE", LoseBase },
        { "EXPIRE", ExpireTime }
    };

    // TODO: why it doesn't work with const map???
    extern std::map<WinCondition, const char *> winDescriptionsMap;

    extern std::map<FailCondition, const char *> failDescriptionsMap;
}



class MapCreator
{
protected:
    int map_w, map_h;

    struct SpawnerData
    {
        std::string type;
        int row, col;
        int delay;
        int timeout;
        int quantity;
    };

    Level::Properties _currLevelProperies;
    std::vector<SpawnerData> _spawners;
    std::string mapString;
    std::map<char, std::string> charMap;

    char charFromMap(int x, int y) const { return mapString[y*map_w + x]; }
public:
    MapCreator();
    virtual int parseMapFile(std::string fileName);
    Level::Properties buildMapFromData();
    Level::Properties levelProperties() const { return _currLevelProperies; };
    int placeSpawnerObjects();
    int mapWidth() const { return map_w; }
    int mapHeight() const { return map_h; }

public:
    static GameObject *buildObject(std::string type);
    static GameObject *createSpawnerObject(const SpawnerData&);

    static void placeBrickWall(int x, int y);
    static void placeStaticCars(int x, int y, bool vert);
    void placeWater(int x, int y);
    void placeRoad(int x, int y);
    void placeBridge(int x, int y);

    void setupScreenBordersBasedOnMapSize();

};
