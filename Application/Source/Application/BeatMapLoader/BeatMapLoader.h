#pragma once

#include <Application/BeatMapLoader/BeatMapData.h>

#include <string>
#include <future>
#include <vector>
#include <cstdint>

#include <json.hpp>


class BeatMapLoader
{
public:

    static BeatMapLoader* GetInstance();

    std::future<bool> LoadBeatMap(const std::string& _beatMapPath);

    BeatMapData GetLoadedBeatMapData() const { return loadedBeatMapdata_; }

    bool IsLoading() const { return isLoading_; }

    bool IsLoadingSuccess() const { return isLoadingSuccess_; }

    std::string GetErrorMessage() const { return errorMessage_; }


private:

    BeatMapData ParseJsonToBeatMap(const nlohmann::json& _jsonData);

    bool isLoading_ = false;
    std::string errorMessage_;
    bool isLoadingSuccess_ = false;
    BeatMapData loadedBeatMapdata_;

    BeatMapLoader();
    ~BeatMapLoader();
    BeatMapLoader(const BeatMapLoader&) = delete;
    BeatMapLoader& operator=(const BeatMapLoader&) = delete;

};