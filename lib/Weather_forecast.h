#pragma once
#include <iostream>
#include <algorithm>
#include <nlohmann/json.hpp>
#include <fstream>
#include <cpr/cpr.h>
#include <cstdint>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>6
#include "ftxui/component/captured_mouse.hpp"
#include "ftxui/component/component.hpp"
#include "ftxui/component/event.hpp"
#include "ftxui/component/mouse.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/dom/elements.hpp"
#include <map>
#include <vector>
#include <thread>


using json = nlohmann::json;

class Weather{

    enum WeatherType {
        unknown,
        Cloudy,
        Fog,
        Rain,
        Snow,
        Sunny
    };

    static std::string GetDayOfWeek(size_t year, size_t month, size_t day);

    struct Config {
        struct City {
            std::string city_;
            std::string country_;
            float longitude_ = 0;
            float latitude_ = 0;

            explicit City(const std::string& name_of_city);
            City() = default;
            ~City() = default;
        };

        std::vector<City> cities_;
        size_t freq_of_requests_;
        size_t forecast_days_;

        explicit Config(const std::string &file);

        Config() = default;
        ~Config() = default;
    };

    struct FinalData {
        struct SixHourData {
            uint8_t WeatherCode_ = 0;
            uint8_t MinWindSpeed_ = UINT8_MAX;
            uint8_t MaxWindSpeed_ = 0;
            uint16_t Pressure_ = 0;
            int8_t MinTemperature_ = INT8_MAX;
            int8_t MaxTemperature_ = INT8_MIN;
            uint8_t WindDirection_ = 0;
        };

        struct DayData {
            size_t day_ = 0;
            size_t month_ = 0;
            size_t year_ = 0;
            std::string day_of_week_;
            SixHourData parts_of_days_[4];

        };

        struct CityData {
            DayData weather_data_[15];
            std::string city_;
            std::string country_;

            CityData(std::string city, std::string country, const json &data);
        };

        std::vector<CityData> cities_weather_;
    };

    static FinalData GetWeather(const Config &config);

    static std::string DateFormatter(const FinalData::DayData& weather);

    static std::string& WeatherTypeToImage(WeatherType type, size_t row);

    static WeatherType GetWeatherType(const FinalData::DayData &weather, size_t part);

    static std::string GetWindDirection(size_t direction);

    bool terminate = false;
    bool reload = false;
    std::mutex mutex_;
    std::condition_variable cv_;
    size_t city_num = 0;
    FinalData final_data_;
    Config config_;

    static void Clear();

    void Restart();

    void Stop(std::thread& app, std::thread& request);

    static void PrintDay(const FinalData::DayData& weather, const std::string& city);
public:
    explicit Weather(const std::string& file);

    void Run();
};