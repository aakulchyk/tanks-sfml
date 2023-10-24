#include "RandomMapCreator.h"
#include "EagleController.h"
#include "GameObject.h"
#include "Controller.h"
#include "Shootable.h"
#include "Damageable.h"
#include "ObjectsPool.h"
#include "GlobalConst.h"
#include "SpriteRenderer.h"
#include "Utils.h"

#include <random>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

RandomMapCreator::RandomMapCreator() : MapCreator() {}

json data;

int RandomMapCreator::parseMapFile(std::string fileName)
{
    // file format:
    // line 1: width, height
    // next lines: map

    std::ifstream f(fileName);

    // parse file
    try {
        data = json::parse(f);
    }
    catch (json::parse_error& ex)
    {
        std::cerr << "parse error at byte " << ex.byte << std::endl;
        return -1; // error
    }


    // parse data

    assert(data["title"].is_string() == true);
    _currLevelProperies.name = data["title"];
    assert(_currLevelProperies.name.length() > 0 && _currLevelProperies.name.length() < 100);

    assert(data["win_condition"].is_string() == true);
    assert(data["win_param"].is_number() == true);
    std::string cond = data["win_condition"];
    int par = data["win_param"];
    assert(Level::winMeaningsMap.find(cond) != Level::winMeaningsMap.end());
    _currLevelProperies.win = Level::winMeaningsMap.at(cond);
    _currLevelProperies.winParam = par;

    assert(data["fail_condition"].is_string() == true);
    assert(data["fail_param"].is_number() == true);
    cond = data["fail_condition"];
    par = data["fail_param"];
    assert(Level::failMeaningsMap.find(cond) != Level::failMeaningsMap.end());
    _currLevelProperies.fail = Level::failMeaningsMap.at(cond);
    _currLevelProperies.failParam = par;


    json briefing_lines = data["briefing"];
    for (json::iterator it = briefing_lines.begin(); it != briefing_lines.end(); ++it) {
        json j = *it;
        assert(j.is_string() == true);
        _currLevelProperies.briefing.push_back(j);
    }

    assert(data["map"].is_object() == true);
    assert(data["map"]["width"].is_number() == true);
    assert(data["map"]["height"].is_number() == true);
    map_w = data["map"]["width"];
    map_h = data["map"]["height"];

    assert(map_w > 0 && map_w < globalConst::maxFieldWidth);
    assert(map_h > 0 && map_h < globalConst::maxFieldHeight);

    mapString.resize(map_w * map_h);

    // TODO: improve
    assert(data["map"]["segments"].is_object() == true);
    json segments = data["map"]["segments"];

    assert(segments["size"] == "7x7");
    segments_w = 7, segments_h = 7;
    type = segments["type"];

    const std::string segmentsFolderPath = "assets/maps/segments/" + type;
    int seg_count = 0;
    int segments_num_w = map_w / segments_w;
    int segments_num_h = map_h / segments_h;
    for (auto it = segments["variants"].begin(); it != segments["variants"].end(); ++it) {
        int col = seg_count % 3;
        int row = seg_count / 3;
        const std::string segment_name = it.key();
        const std::string segment_path = segmentsFolderPath + "/" + segment_name;
        namespace fs = std::filesystem;

        // read variants (1.txt, 2.txt, ...) in the segment folder
        const fs::path dir(segment_path);
        const char *segp = segment_path.c_str();
        assert(fs::exists(dir) == true);
        assert(fs::is_directory(dir) == true);
        std::vector<std::string> segmentVariants;
        for (const auto& entry : fs::directory_iterator(dir)) {
            if (!fs::is_regular_file(entry.status()))
                continue; // ignore directories
            std::string segFileName = entry.path().root_path().string() + entry.path().relative_path().string();
            const char *segp = segFileName.c_str();
            segmentVariants.push_back(segFileName);
        }

        assert(segmentVariants.size() > 0);

        // select random segment
        std::uniform_int_distribution<int> type_distr(0, segmentVariants.size()-1);
        int segmentIndex = type_distr(Utils::generator);
        assert(segmentIndex < segmentVariants.size());

        // read segment
        std::string segmentline;
        int mapStartIndex = row * map_w * segments_h + col * segments_w;
        readSegment(segmentVariants[segmentIndex], mapStartIndex);

        seg_count++;
    }

    //assert(data["enemies"].is_array() == true);
    json enemies = data["enemies"];
    for (json::iterator it = enemies.begin(); it != enemies.end(); ++it) {
        json e = *it;
        SpawnerData sd;
        sd.type = e["type"];
        sd.row = e["y"];
        sd.col = e["x"];
        sd.delay = e["delay"];
        sd.timeout = e["timeout"];
        sd.quantity = e["quantity"];

        assert(sd.type.empty() == false);
        assert(sd.row >= 0 && sd.row < globalConst::maxFieldWidth);
        assert(sd.col >= 0 && sd.col < globalConst::maxFieldHeight);
        assert(sd.delay >= 0 && sd.delay < 1000);
        assert(sd.timeout >= 0 && sd.timeout < 120);
        assert(sd.quantity > 0 && sd.quantity < 100);

        _spawners.push_back(sd);
    }


    return 0;
}

void RandomMapCreator::readSegment(std::string filepath, int mapStartIndex)
{
    std::string segmentline;
    std::ifstream fsegment(filepath);
    std::getline(fsegment, segmentline);
    std::string dummy;
    int seg_w, seg_h;
    std::istringstream isss(segmentline);
    isss >> dummy >> seg_w >> seg_h;
    assert(dummy == "segment");
    assert(seg_w == segments_w);
    assert(seg_h == segments_h);
    int seg_row = 0;
    while (std::getline(fsegment, segmentline)) {
        int mapRowIndex = mapStartIndex + seg_row*map_w;
        std::istringstream issss(segmentline);
        std::string seg;
        issss >> seg;

        for (int j=0; j<seg.length(); j++)
            mapString[mapRowIndex + j] = seg[j];

        seg_row++;
    }
}

