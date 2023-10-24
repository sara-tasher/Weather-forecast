#include "Weather_forecast.h"


std::string Weather::DateFormatter(const FinalData::DayData& weather){
    std::string ans;
    if(weather.day_ / 10 == 0) ans += "0" + std::to_string(weather.day_) + ".";
    else ans += std::to_string(weather.day_) + ".";
    if(weather.month_ / 10 == 0) ans += "0" + std::to_string(weather.month_);
    else ans += std::to_string(weather.month_);

    ans = weather.day_of_week_ + " " + ans;
    return ans;
}


std::string& Weather::WeatherTypeToImage(Weather::WeatherType type, size_t row) {
    static std::map<WeatherType, std::vector<std::string>> pictures = {
            {
                    WeatherType::unknown,
                    {
                            "    .-.      ",
                            "     __)     ",
                            "    (        ",
                            "     `-᾿     ",
                            "      •      "
                    }
            },
            {
                    WeatherType::Cloudy,
                    {
                            "             ",
                            "\033[38;5;250m     .--.    \033[0m",
                            "\033[38;5;250m  .-(    ).  \033[0m",
                            "\033[38;5;250m (___.__)__) \033[0m",
                            "             ",
                    }
            },
            {
                    WeatherType::Fog,
                    {
                            "             ",
                            "\033[38;5;251m _ - _ - _ - \033[0m",
                            "\033[38;5;251m  _ - _ - _  \033[0m",
                            "\033[38;5;251m _ - _ - _ - \033[0m",
                            "             ",
                    }
            },
            {
                    WeatherType::Rain,
                    {
                            "\033[38;5;240;1m     .-.     \033[0m",
                            "\033[38;5;240;1m    (   ).   \033[0m",
                            "\033[38;5;240;1m   (___(__)  \033[0m",
                            "\033[38;5;21;1m  ‚ʻ‚ʻ‚ʻ‚ʻ   \033[0m",
                            "\033[38;5;21;1m  ‚ʻ‚ʻ‚ʻ‚ʻ   \033[0m",
                    }
            },
            {
                    WeatherType::Snow,
                    {
                            "\033[38;5;240;1m     .-.     \033[0m",
                            "\033[38;5;240;1m    (   ).   \033[0m",
                            "\033[38;5;240;1m   (___(__)  \033[0m",
                            "\033[38;5;255;1m   * * * *   \033[0m",
                            "\033[38;5;255;1m  * * * *    \033[0m",
                    }
            },
            {
                    WeatherType::Sunny,
                    {
                            "\033[38;5;226m    \\   /    \033[0m",
                            "\033[38;5;226m     .-.     \033[0m",
                            "\033[38;5;226m  ‒ (   ) ‒  \033[0m",
                            "\033[38;5;226m     `-᾿     \033[0m",
                            "\033[38;5;226m    /   \\    \033[0m",
                    }
            }
    };

    return pictures[type][row];
}


Weather::WeatherType Weather::GetWeatherType(const Weather::FinalData::DayData &weather, size_t part) {
    size_t code = weather.parts_of_days_[part].WeatherCode_;
    if (code < 4 && 2 <= code) {
        return WeatherType::Cloudy;
    } else if ((10 <= code && code <= 12) || code == 28 || (40 <= code && code <= 59)) {
        return WeatherType::Fog;
    } else if ((13 <= code && code <= 19) || code == 21 || code == 24 || code == 25 || code == 29 ||
               (60 <= code && code <= 99)) {
        return WeatherType::Rain;
    } else if (code == 20 || code == 26 || code == 27) {
        return WeatherType::Snow;
    } else if (code < 10 || (code <= 39)) {
        return WeatherType::Sunny;
    } else {
        return WeatherType::unknown;
    }
}

std::string Weather::GetWindDirection(size_t direction) {
    switch (direction) {
        case 0:
            return "↓";
        case 1:
            return "↙";
        case 2:
            return "←";
        case 3:
            return "↖";
        case 4:
            return "↑";
        case 5:
            return "↗";
        case 6:
            return "→";
        case 7:
            return "↘";
        default:
            return "?";
    }
}

void Weather::PrintDay(const FinalData::DayData& weather, const std::string& city) {
    std::string date = std::move(DateFormatter(weather));

    size_t size_of_line = 106;
    std::cout << "├";
    for(size_t i = 0; i < (size_of_line - city.size() - 2) / 2; ++i) std::cout << "─";
    std::cout << city;
    for(size_t i = 0; i < (size_of_line - city.size() - 2) / 2; ++i) std::cout << "─";
    std::cout << "┤\n";

    std::cout << "                                             ┌─────────────┐                                   \n";
    std::cout << "┌─────────────────────────┬──────────────────┤  " << date << "  ├──────────────────┬─────────────────────────┐\n";
    std::cout << "│         Morning         │       Afternoon  └──────┬──────┘   Evening        │          Night          │\n";
    std::cout << "├─────────────────────────┼─────────────────────────┼─────────────────────────┼─────────────────────────┤\n";

    std::vector<std::vector<std::string>> day_data_;
    for (size_t stamp = 0; stamp < 4; stamp++) {
        std::string temperature = std::to_string(weather.parts_of_days_[stamp].MaxTemperature_) + "(" + std::to_string(weather.parts_of_days_[stamp].MinTemperature_) + ")°C";
        for(auto i = temperature.size(); i < 11; i++){
            temperature += " ";
        }
        temperature += "  │";
        std::string wind_speed = std::to_string(weather.parts_of_days_[stamp].MaxWindSpeed_);
        std::string wind_dir = GetWindDirection(weather.parts_of_days_[stamp].WindDirection_);
        if(wind_speed.size() == 2) wind_speed += " m/s     │";
        else wind_speed += " m/s     │";

        std::string pressure = std::to_string(weather.parts_of_days_[stamp].Pressure_) + " mm      │";
        day_data_.push_back({
                                    "            │",
                                    temperature,
                                    pressure,
                                    wind_dir + " " + wind_speed,
                                    "            │"
                            });
    }

    for (size_t row = 0; row < 5; ++row) {
        std::cout << "│";
        for (size_t i = 0; i < 4; ++i) {
            std::cout << WeatherTypeToImage(GetWeatherType(weather, i), row) << day_data_[i][row];
        }

        std::cout << '\n';

    }


    std::cout << "└─────────────────────────┴─────────────────────────┴─────────────────────────┴─────────────────────────┘\n";
}