#include "Weather_forecast.h"


Weather::Weather(const std::string& file){
    config_ = Config(file);
    final_data_ = std::move(GetWeather(config_));
}

std::string Weather::GetDayOfWeek(size_t year, size_t month, size_t day) {
    static std::array<int, 12> day_trick = { 0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4 };
    year -= month < 3;
    size_t day_of_week = ( year + year / 4 - year / 100 + year / 400 + day_trick[month - 1] + day) % 7;
    static std::map<size_t, std::string> day_to_string = {
            {0, "Sun"},
            {1, "Mon"},
            {2, "Tue"},
            {3, "Wed"},
            {4, "Thu"},
            {5, "Fri"},
            {6, "Sat"},
    };

    return day_to_string[day_of_week];
}


Weather::Config::City::City(const std::string& name_of_city){
    std::string url = "https://api.api-ninjas.com/v1/city";
    cpr::Response coordinates = cpr::Get(cpr::Url{url},
                                         cpr::Parameters{{"name", name_of_city}},
                                         cpr::Header{
                                                 {"X-Api-Key", "CBsa/y2m/Qyi10AVV6QnWA==RhDai3Ig4F9c8d8P"}});
    json City_data = json::parse(coordinates.text);
    if (!City_data.empty()) {
        longitude_ = City_data[0]["longitude"].get<float>();
        latitude_ = City_data[0]["latitude"].get<float>();
        country_ = City_data[0]["country"].get<std::string>();
        city_ = City_data[0]["name"].get<std::string>();
    } else {
        city_ = "Error";
        country_ = "Error";
        longitude_ = 0;
        latitude_ = 0;
    }
}


Weather::Config::Config(const std::string &file) {
    std::ifstream f(file);
    if (!f.is_open()) throw std::invalid_argument("couldn't read configuration file");

    json data = json::parse(f);

    forecast_days_ = data["forecast_days"].get<size_t>();
    freq_of_requests_ = data["frequency"].get<size_t>();

    for (const auto& name: data["cities"]) {
        cities_.emplace_back(name.get<std::string>());
        if (cities_.back().city_ == "Error") cities_.pop_back();
    }
}


Weather::FinalData::CityData::CityData(std::string city, std::string country, const json &data) {
    city_ = std::move(city);
    country_ = std::move(country);
    for (int day = 0; day < 15; ++day) {
        for (int part = 0; part < 4; ++part) {
            for (int hour = 6; hour < 12; ++hour) {
                weather_data_[day].parts_of_days_[part].MaxTemperature_ = std::max(
                        weather_data_[day].parts_of_days_[part].MaxTemperature_,
                        (int8_t) data["hourly"]["temperature_2m"][day * 24 + part * 6 + hour]);
                weather_data_[day].parts_of_days_[part].MinTemperature_ = std::min(
                        weather_data_[day].parts_of_days_[part].MinTemperature_,
                        (int8_t) data["hourly"]["temperature_2m"][day * 24 + part * 6 + hour]);
                weather_data_[day].parts_of_days_[part].Pressure_ = std::max(
                        weather_data_[day].parts_of_days_[part].Pressure_,
                        uint16_t(data["hourly"]["surface_pressure"][day * 24 + part * 6 + hour].get<int>() * 3 / 4));
                weather_data_[day].parts_of_days_[part].WindDirection_ = (((int(
                        data["hourly"]["winddirection_10m"][day * 24 + part * 6 + hour]) + 28) % 360) / 45);
                weather_data_[day].parts_of_days_[part].WeatherCode_ = std::max(
                        weather_data_[day].parts_of_days_[part].WeatherCode_,
                        (uint8_t) data["hourly"]["weathercode"][day * 24 + part * 6 + hour]);
                weather_data_[day].parts_of_days_[part].MaxWindSpeed_ = std::max(
                        weather_data_[day].parts_of_days_[part].MaxWindSpeed_,
                        (uint8_t) data["hourly"]["windspeed_10m"][day * 24 + part * 6 + hour]);
                weather_data_[day].parts_of_days_[part].MinWindSpeed_ = std::min(
                        weather_data_[day].parts_of_days_[part].MinWindSpeed_,
                        (uint8_t) data["hourly"]["windspeed_10m"][day * 24 + part * 6 + hour]);
            }
        }
        std::string date = data["hourly"]["time"][day * 24 + 1];
        weather_data_[day].day_ = (date[8] - '0') * 10 + date[9] - '0';
        weather_data_[day].month_ = (date[5] - '0') * 10 + date[6] - '0';
        weather_data_[day].year_ = (date[0] - '0') * 1000 + (date[1] - '0') * 100 + (date[2] - '0') * 10 + date[3] - '0';
        weather_data_[day].day_of_week_ = std::move(GetDayOfWeek(weather_data_[day].year_, weather_data_[day].month_, weather_data_[day].day_));
    }
}


Weather::FinalData Weather::GetWeather(const Config &config) {
    std::string url = "https://api.open-meteo.com/v1/forecast";
    FinalData Weather;
    for (const auto &city: config.cities_) {
        cpr::Response origin_data = cpr::Get(cpr::Url{url},
                                             cpr::Parameters{{"latitude",       std::to_string(city.latitude_)},
                                                             {"longitude",      std::to_string(city.longitude_)},
                                                             {"hourly",         "temperature_2m,weathercode,surface_pressure,windspeed_10m,winddirection_10m"},
                                                             {"windspeed_unit", "ms"},
                                                             {"forecast_days",  "16"},
                                                             {"timezone",       "auto"}});
        json data = json::parse(origin_data.text);
        Weather.cities_weather_.emplace_back(city.city_, city.country_, data);
    }
    return Weather;
}
