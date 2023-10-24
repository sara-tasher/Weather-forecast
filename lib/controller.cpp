#include "Weather_forecast.h"

void Weather::Clear() {
    system("CLS");
}

void Weather::Restart() {
    {
        std::lock_guard<std::mutex> lk(mutex_);
        reload = true;
    }
    cv_.notify_all();
}

void Weather::Stop(std::thread& app, std::thread& request) {
    cv_.notify_all();
    {
        std::lock_guard<std::mutex> lk(mutex_);
        terminate = true;
    }
    cv_.notify_all();

    app.join();
    request.join();
}

void Weather::Run() {
    std::thread request([&] {
        std::unique_lock<std::mutex> lk(mutex_);
        while (true) {
            cv_.wait_for(lk, std::chrono::seconds(config_.freq_of_requests_), [&]{ return terminate; });
            if (terminate) break;
            final_data_ = GetWeather(config_);
        }
    });
    std::thread app([&] {
        std::unique_lock<std::mutex> lk(mutex_);
        while (true) {
            Clear();
            for (int i = 0; i < config_.forecast_days_; ++i) {
                PrintDay(final_data_.cities_weather_[city_num].weather_data_[i], final_data_.cities_weather_[city_num].city_);
            }
            std::cout << std::endl;
            cv_.wait_for(lk, std::chrono::seconds(config_.freq_of_requests_), [&]{ return terminate || reload; });
            if (terminate) break;
            reload = false;
        }
    });
    auto screen = ftxui::ScreenInteractive::TerminalOutput();
    auto renderer = ftxui::Renderer([] { return ftxui::text(""); });
    auto component = CatchEvent(renderer, [&](ftxui::Event event) {
        if (event == ftxui::Event::Character('q')) {
            screen.ExitLoopClosure()();
        } else if (event == ftxui::Event::Character('=')) {
            if(config_.forecast_days_ < 15) config_.forecast_days_ += 1;
            Restart();
        } else if (event == ftxui::Event::Character('-')) {
            if(config_.forecast_days_ > 1) config_.forecast_days_ -= 1;
            Restart();
        } else if (event == ftxui::Event::Character('n')) {
            city_num = (city_num + 1) % config_.cities_.size();
            Restart();
        } else if (event == ftxui::Event::Character('p')) {
            city_num = (city_num + config_.cities_.size() - 1) % config_.cities_.size();
            Restart();
        }
        return false;
    });

    screen.Loop(component);
    Stop(app, request);
}