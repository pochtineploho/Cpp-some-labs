#include "BoWeather.h"
#include "BoWeatherOutput.h"

bool BoWeather::PrepareForOutput(const char* config_filename) {
    if (!ParseConfig(config_filename)) {
        return false;
    }
    if (!GetCities()) {
        return false;
    }
    switch (config_parameters.details) {
        case default_frequency:
            for (auto& city: cities) {
                if (city.second->stored) {
                    JoinDaysDefault(city.first);
                }
            }
            break;
        case daily:
            for (auto& city: cities) {
                if (city.second->stored) {
                    JoinDaysDaily(city.first);
                }
            }
            break;
        case hourly:
            for (auto& city: cities) {
                if (city.second->stored) {
                    JoinDaysHourly(city.first);
                }
            }
            break;
    }
    return true;
}

bool BoWeather::ParseConfig(const char* config_filename) {
    std::ifstream config(config_filename);
    nlohmann::json input;
    config >> input;
    if (input.contains("api")) {
        config_parameters.api = input["api"];
    } else {
        return false;
    }
    if (input.contains("frequency")) {
        if (input["details"] == "hourly") {
            config_parameters.details = hourly;
        } else if (input["details"] == "daily") {
            config_parameters.details = daily;
        }
    }
    if (input.contains("forecast_days")) {
        config_parameters.forecast_days = input["forecast_days"];
        if (config_parameters.forecast_days > kMaxForecastDays) {
            config_parameters.forecast_days = kMaxForecastDays;
        }
    }
    if (input.contains("past_days")) {
        config_parameters.past_days = input["past_days"];
        if (config_parameters.past_days > kMaxPastDays) {
            config_parameters.past_days = kMaxPastDays;
        }
    }
    if (input.contains("current_weather")) {
        config_parameters.current_weather = input["current_weather"];
    }
    if (input.contains("storage")) {
        config_parameters.storage = input["storage"];
    }
    if (input.contains("cities") || input.contains("city")) {
        for (auto& i: input["cities"]) {
            std::string city = i;
            City* tmp = new City;
            cities.insert({city, tmp});
        }
    } else {
        City* tmp = new City;
        cities.insert({"Saint Petersburg", tmp});
    }
    return true;
}

bool BoWeather::GetCities(bool first) {
    cpr::Response check = cpr::Get(cpr::Url{"http://www.google.com"}, cpr::Timeout{5000});
    if (check.status_code == kOkResponseCode) {
        something_stored = true;
        for (auto& city: cities) {
            GetWeather(city.first);
            if (cities.find(city.first) != cities.end()) {
                city.second->stored = true;
            }
        }
        if (cities.empty()) {
            std::cerr << "Your input does not contain existing cities\n";
            something_stored = false;

            return false;
        }
    } else {
        std::cerr << "Internet connection failed\n";
        bool something = false;
        for (auto& city: cities) {
            std::ifstream file;
            switch (config_parameters.details) {
                case daily:
                    file.open(config_parameters.storage + "/" + cities[city.first]->real_name + "_daily.json");
                    break;
                default:
                    file.open(config_parameters.storage + "/" + cities[city.first]->real_name + "_default.json");
                    break;
            }
            if (!file.is_open()) {
                std::cerr << "Storage doesn't contain " + city.first + "\n";
                city.second->stored = false;
            } else {
                file >> city.second->json;
                city.second->stored = true;
                something = true;
                something_stored = true;
                city_it = cities.find(city.first);
            }
        }
        if (!something) {
            if (!first) {
                return true;
            }
            something_stored = false;
            std::thread rewrite_thread(&BoWeather::RewriteData, this, false);
            rewrite_thread.detach();
            auto renderer = Renderer({[]() {
                return center(vbox(BoWeatherOutput::kWeatherPictures[BoWeatherOutput::kErrorCode]()));
            }});
            auto screen = ScreenInteractive::TerminalOutput();
            auto run = CatchEvent(renderer, [&](const Event& event) {
                if (event == Event::Character('r')) {
                    if (!GetCities(false)) {
                        screen.ExitLoopClosure()();
                        Render(screen, text("Your input does not contain existing cities"));
                        rewrite_thread.join();
                        return true;
                    }
                    if (something_stored) {
                        switch (config_parameters.details) {
                            case default_frequency:
                                for (auto& city: cities) {
                                    if (city.second->stored) {
                                        JoinDaysDefault(city.first);
                                    }
                                }
                                ChangeOutputDefault(city_it, output.size());
                                break;
                            case daily:
                                for (auto& city: cities) {
                                    if (city.second->stored) {
                                        JoinDaysDaily(city.first);
                                    }
                                }
                                ChangeOutputDaily(city_it);
                                break;
                            case hourly:
                                for (auto& city: cities) {
                                    if (city.second->stored) {
                                        JoinDaysHourly(city.first);
                                    }
                                    ChangeOutputHourly(city_it);
                                }
                                break;
                        }
                        rewrite_thread.join();
                        screen.ExitLoopClosure()();
                        Run();
                    }
                    return true;
                } else if (event == Event::Escape) {
                    screen.ExitLoopClosure()();
                    rewrite_thread.join();
                    return true;
                }
            });
            screen.Loop(run);
            Render(screen, text(""));
            if (cities.empty()) {
                rewrite_thread.join();
                return false;
            }
        }
    }
    return true;
}

void BoWeather::GetWeather(const std::string& city) {
    std::pair<double, double> coordinates;
    GetCoordinates(city, coordinates);
    if (coordinates.first == kError) {
        cities.erase(city);
        return;
    }
    GetWeather(city, coordinates);
}

void BoWeather::GetCoordinates(const std::string& city, std::pair<double, double>& coordinates) {
    if (cities[city]->need_coordinates) {
        cities[city]->need_coordinates = false;
        auto what_coordinates = cpr::Get(cpr::Url{"https://api.api-ninjas.com/v1/city"},
                                         cpr::Header{{"X-Api-Key", config_parameters.api}},
                                         cpr::Parameters{{"name", city}});
        if (what_coordinates.status_code == kOkResponseCode) {
            what_coordinates.text.pop_back();
            what_coordinates.text.erase(0, 1);
            if (what_coordinates.text.find_first_not_of(' ') != std::string::npos) {
                nlohmann::json data = nlohmann::json::parse(what_coordinates.text);
                coordinates.first = data["latitude"];
                coordinates.second = data["longitude"];
                cities[city]->real_name = data["name"];
            } else {
                coordinates = {kError, kError};
                cities.erase(city);
            }
        } else {
            coordinates = {kError, kError};
        }
    } else {
        coordinates = {cities[city]->json["latitude"], cities[city]->json["longitude"]};
    }
}


void BoWeather::GetWeather(const std::string& city, const std::pair<double, double>& coordinates) {
    cpr::Parameters parameters{{"latitude",  std::to_string(coordinates.first)},
                               {"longitude", std::to_string(coordinates.second)}};
    parameters.Add(kDefaultParameters);
    if (config_parameters.details == daily) {
        parameters.Add(kDailyParameters);
    } else {
        parameters.Add(kHourlyParameters);
    }
    if (config_parameters.current_weather) {
        parameters.Add({"current_weather", "true"});
    }
    auto what_weather = cpr::Get(cpr::Url{"https://api.open-meteo.com/v1/forecast"}, parameters);
    cities[city]->json = nlohmann::json::parse(what_weather.text);
}

ftxui::Element BoWeather::GetMainHbox(const std::string& city, int day, int day_time, FindData find_data) {
    return hbox({
                        GetLeftVbox(city, day, day_time, find_data),
                        filler(),
                        GetRightVbox(city, day, day_time, find_data),
                }) | flex;
}

ftxui::Element BoWeather::GetLeftVbox(const std::string& city, int day, int day_time, FindData find_data) {
    switch (find_data) {
        case cur:
            return BoWeatherOutput::kWeatherPictures.at(BoWeatherOutput::CheckWMO(
                    cities[city]->json[kDirectory.at(find_data)]["weathercode"]))();
        case hly:
            return BoWeatherOutput::kWeatherPictures.at(BoWeatherOutput::CheckWMO(
                    cities[city]->json[kDirectory.at(find_data)]["weathercode"][
                            day * kHoursPerDay + day_time]))();
        case dly:
            return BoWeatherOutput::kWeatherPictures.at(BoWeatherOutput::CheckWMO(
                    cities[city]->json[kDirectory.at(find_data)]["weathercode"][day]))();
    }
}

ftxui::Element BoWeather::GetRightVbox(const std::string& city, int day, int day_time, FindData find_data) {
    Element WMO;
    if (find_data == cur) {
        WMO = text(BoWeatherOutput::kWMOCodes.at(
                BoWeatherOutput::CheckWMO(
                        cities[city]->json[kDirectory.at(find_data)]["weathercode"])));
    } else if (find_data == hly) {
        WMO = text(BoWeatherOutput::kWMOCodes.at(
                BoWeatherOutput::CheckWMO(
                        cities[city]->json[kDirectory.at(find_data)]["weathercode"][
                                day * kHoursPerDay + day_time])));
    } else if (find_data == dly) {
        WMO = text(BoWeatherOutput::kWMOCodes.at(
                BoWeatherOutput::CheckWMO(
                        cities[city]->json[kDirectory.at(find_data)]["weathercode"][day])));
    }

    return vbox({
                        WMO,
                        GetTemperature(city, day, day_time, find_data),
                        GetWind(city, day, day_time, find_data),
                        GetHumidityOrSunrise(city, day, day_time, find_data),
                        GetPrecipitation(city, day, day_time, find_data)
                });
}

ftxui::Element BoWeather::GetTemperature(const std::string& city, int day, int day_time, FindData find_data) {
    int temperature;
    int apparent_temperature;
    switch (find_data) {
        case dly:
            temperature = cities[city]->json[kDirectory.at(find_data)]["temperature_2m_min"][day];
            apparent_temperature = cities[city]->json[kDirectory.at(find_data)]["temperature_2m_max"][day];
            return hbox({
                                color(BoWeatherOutput::GetTemperatureColor(temperature),
                                      text(BoWeatherOutput::GetStringTemperature(temperature))),
                                text("-"),
                                color(BoWeatherOutput::GetTemperatureColor(apparent_temperature),
                                      text(BoWeatherOutput::GetStringTemperature(apparent_temperature))),
                                text(" °C      ")
                        });
        case cur:
            temperature = cities[city]->json[kDirectory.at(find_data)]["temperature"];
            return hbox({
                                color(BoWeatherOutput::GetTemperatureColor(temperature),
                                      text(BoWeatherOutput::GetStringTemperature(temperature))),
                                text(" °C      ")
                        });
        case hly:
            temperature = cities[city]->json[kDirectory.at(find_data)]["temperature_2m"][day * kHoursPerDay + day_time];
            apparent_temperature = cities[city]->json[kDirectory.at(find_data)]["apparent_temperature"][
                    day * kHoursPerDay + day_time];
            return hbox({
                                color(BoWeatherOutput::GetTemperatureColor(temperature),
                                      text(BoWeatherOutput::GetStringTemperature(temperature))),
                                text("("),
                                color(BoWeatherOutput::GetTemperatureColor(apparent_temperature),
                                      text(BoWeatherOutput::GetStringTemperature(apparent_temperature))),
                                text(") °C      ")
                        });
    }
}

ftxui::Element BoWeather::GetWind(const std::string& city, int day, int day_time, FindData find_data) {
    int min_wind_speed;
    int max_wind_speed;
    switch (find_data) {
        case hly:
            if (config_parameters.details == default_frequency) {
                min_wind_speed = std::min(
                        {cities[city]->json[kDirectory.at(find_data)]["windspeed_10m"][day * kHoursPerDay + day_time -
                                                                                       3],
                         cities[city]->json[kDirectory.at(find_data)]["windspeed_10m"][day * kHoursPerDay + day_time -
                                                                                       2],
                         cities[city]->json[kDirectory.at(find_data)]["windspeed_10m"][day * kHoursPerDay + day_time -
                                                                                       1],
                         cities[city]->json[kDirectory.at(find_data)]["windspeed_10m"][day * kHoursPerDay + day_time],
                         cities[city]->json[kDirectory.at(find_data)]["windspeed_10m"][day * kHoursPerDay + day_time +
                                                                                       1],
                         cities[city]->json[kDirectory.at(find_data)]["windspeed_10m"][day * kHoursPerDay + day_time +
                                                                                       2]});
                max_wind_speed = std::max(
                        {cities[city]->json[kDirectory.at(find_data)]["windspeed_10m"][day * kHoursPerDay + day_time -
                                                                                       3],
                         cities[city]->json[kDirectory.at(find_data)]["windspeed_10m"][day * kHoursPerDay + day_time -
                                                                                       2],
                         cities[city]->json[kDirectory.at(find_data)]["windspeed_10m"][day * kHoursPerDay + day_time -
                                                                                       1],
                         cities[city]->json[kDirectory.at(find_data)]["windspeed_10m"][day * kHoursPerDay + day_time],
                         cities[city]->json[kDirectory.at(find_data)]["windspeed_10m"][day * kHoursPerDay + day_time +
                                                                                       1],
                         cities[city]->json[kDirectory.at(find_data)]["windspeed_10m"][day * kHoursPerDay + day_time +
                                                                                       2]});
                return hbox({
                                    color(BoWeatherOutput::GetWindColor(min_wind_speed),
                                          text(std::to_string(min_wind_speed))),
                                    text("-"),
                                    color(BoWeatherOutput::GetWindColor(max_wind_speed),
                                          text(std::to_string(max_wind_speed))),
                                    text(" km/h "),
                                    text(BoWeatherOutput::GetWindDirection(
                                            cities[city]->json[kDirectory.at(find_data)]["winddirection_10m"][
                                                    day * kHoursPerDay +
                                                    day_time]))
                            });
            } else {
                max_wind_speed = cities[city]->json[kDirectory.at(find_data)]["windspeed_10m"][day * kHoursPerDay +
                                                                                               day_time];
                return hbox({
                                    color(BoWeatherOutput::GetWindColor(max_wind_speed),
                                          text(std::to_string(max_wind_speed))),
                                    text(" km/h "),
                                    text(BoWeatherOutput::GetWindDirection(
                                            cities[city]->json[kDirectory.at(find_data)]["winddirection_10m"][
                                                    day * kHoursPerDay +
                                                    day_time]))
                            });
            }

        case cur:
            max_wind_speed = cities[city]->json[kDirectory.at(find_data)]["windspeed"];

            return hbox({
                                color(BoWeatherOutput::GetWindColor(max_wind_speed),
                                      text(std::to_string(max_wind_speed))),
                                text(" km/h "),
                                text(BoWeatherOutput::GetWindDirection(
                                        cities[city]->json[kDirectory.at(find_data)]["winddirection"]))
                        });
        case dly:
            max_wind_speed = cities[city]->json[kDirectory.at(find_data)]["windspeed_10m_max"][day];

            return hbox({
                                color(BoWeatherOutput::GetWindColor(max_wind_speed),
                                      text(std::to_string(max_wind_speed))),
                                text(" km/h "),
                                text(BoWeatherOutput::GetWindDirection(
                                        cities[city]->json[kDirectory.at(
                                                find_data)]["winddirection_10m_dominant"][day]))
                        });
    }
}

ftxui::Element BoWeather::GetHumidityOrSunrise(const std::string& city, int day, int day_time, FindData find_data) {
    int surface_pressure;
    int relativehumidity_2m;
    std::string sunrise;
    std::string sunset;
    switch (find_data) {
        case dly:
            sunrise = cities[city]->json[kDirectory.at(find_data)]["sunrise"][day];
            sunset = cities[city]->json[kDirectory.at(find_data)]["sunset"][day];
            return hbox({
                                text(BoWeatherOutput::ParseTime(sunrise)),
                                text(" | "),
                                text(BoWeatherOutput::ParseTime(sunset)),
                        });
        case hly:
            surface_pressure = cities[city]->json[kDirectory.at(find_data)]["surface_pressure"][day * kHoursPerDay +
                                                                                                day_time];
            relativehumidity_2m = cities[city]->json[kDirectory.at(find_data)]["relativehumidity_2m"][
                    day * kHoursPerDay + day_time];
            return hbox({
                                text(std::to_string(surface_pressure) + " hPa | "),
                                text(std::to_string(relativehumidity_2m) + "% "),
                        });

        case cur:
            return text("");
    }
}

ftxui::Element BoWeather::GetPrecipitation(const std::string& city, int day, int day_time, FindData find_data) {
    if (find_data == hly && config_parameters.details == default_frequency) {
        std::array<float, 6> precipitation(
                {cities[city]->json[kDirectory.at(find_data)]["precipitation"][day * kHoursPerDay + day_time - 3],
                 cities[city]->json[kDirectory.at(find_data)]["precipitation"][day * kHoursPerDay + day_time - 2],
                 cities[city]->json[kDirectory.at(find_data)]["precipitation"][day * kHoursPerDay + day_time - 1],
                 cities[city]->json[kDirectory.at(find_data)]["precipitation"][day * kHoursPerDay + day_time],
                 cities[city]->json[kDirectory.at(find_data)]["precipitation"][day * kHoursPerDay + day_time + 1],
                 cities[city]->json[kDirectory.at(find_data)]["precipitation"][day * kHoursPerDay + day_time + 2]});
        float precipitation_mean = 0;
        for (auto number: precipitation) {
            precipitation_mean += number;
        }
        precipitation_mean /= precipitation.size();
        std::ostringstream tmp;
        tmp << std::fixed << std::setprecision(1) << precipitation_mean;
        int precipitation_probability = 0;
        precipitation = {
                cities[city]->json[kDirectory.at(find_data)]["precipitation_probability"][day * kHoursPerDay +
                                                                                          day_time -
                                                                                          3],
                cities[city]->json[kDirectory.at(find_data)]["precipitation_probability"][day * kHoursPerDay +
                                                                                          day_time -
                                                                                          2],
                cities[city]->json[kDirectory.at(find_data)]["precipitation_probability"][day * kHoursPerDay +
                                                                                          day_time -
                                                                                          1],
                cities[city]->json[kDirectory.at(find_data)]["precipitation_probability"][day * kHoursPerDay +
                                                                                          day_time],
                cities[city]->json[kDirectory.at(find_data)]["precipitation_probability"][day * kHoursPerDay +
                                                                                          day_time +
                                                                                          1],
                cities[city]->json[kDirectory.at(find_data)]["precipitation_probability"][day * kHoursPerDay +
                                                                                          day_time +
                                                                                          2]};
        for (auto number: precipitation) {
            precipitation_probability += number;
        }
        precipitation_probability /= precipitation.size();

        return text(tmp.str() + " mm | " + std::to_string(precipitation_probability) + "%");
    } else if (find_data == hly && config_parameters.details == hourly) {
        int precipitation_probability = cities[city]->json[kDirectory.at(find_data)]["precipitation_probability"][
                day * kHoursPerDay +
                day_time];
        float precipitation = cities[city]->json[kDirectory.at(find_data)]["precipitation"][day * kHoursPerDay +
                                                                                            day_time];
        std::ostringstream tmp;
        tmp << std::fixed << std::setprecision(1) << precipitation;

        return text(tmp.str() + " mm | " + std::to_string(precipitation_probability) + "%");
    } else if (find_data == cur) {
        if (cities[city]->json[kDirectory.at(find_data)]["is_day"] != 0) {
            return text("Have a good day!");
        }
        return text("Good night!");
    } else {
        float precipitation = cities[city]->json[kDirectory.at(find_data)]["precipitation_sum"][day];
        int precipitation_probability = cities[city]->json[kDirectory.at(
                find_data)]["precipitation_probability_mean"][day];
        std::ostringstream tmp;
        tmp << std::fixed << std::setprecision(1) << precipitation;

        return text(tmp.str() + " mm | " + std::to_string(precipitation_probability) + "%");
    }
}

void BoWeather::JoinDaysDefault(const std::string& city) {
    Elements elements;
    for (int day = 0; day < kMaxPastDays; ++day) {
        elements.push_back(CreateDayDefault(city, day));
    }
    for (int day = kMaxPastDays; day < kMaxPastDays + kMaxForecastDays; ++day) {
        elements.push_back(CreateDayDefault(city, day));
    }
    cities[city]->result = elements;

    std::ofstream file(config_parameters.storage + "/" + cities[city]->real_name + "_default.json");
    file << cities[city]->json;
}

ftxui::Element BoWeather::CreateDayDefault(const std::string& city, int day) {
    auto morning_vbox =
            vbox({
                         center(text("Morning")),
                         separator(),
                         GetMainHbox(city, day, kMorning)
                 }) | flex;
    auto day_vbox = vbox({
                                 center(text("Day")),
                                 separator(),
                                 GetMainHbox(city, day, kDay)
                         }) | flex;
    auto evening_vbox = vbox({
                                     center(text("Evening")),
                                     separator(),
                                     GetMainHbox(city, day, kEvening)
                             }) | flex;
    auto night_vbox = vbox({
                                   center(text("Night")),
                                   separator(),
                                   GetMainHbox(city, day, kNight)
                           }) | flex;

    auto table = window(center(
                                text(BoWeatherOutput::ParseDate(cities[city]->json["hourly"]["time"][day * kHoursPerDay]))),
                        hbox({
                                     morning_vbox,
                                     separator(),
                                     day_vbox,
                                     separator(),
                                     evening_vbox,
                                     separator(),
                                     night_vbox,
                             }) | flex
    ) | flex;
    return table;
}

void BoWeather::JoinDaysDaily(const std::string& city) {
    Elements elements;
    auto table = hbox({
                              vbox({
                                           CreateDayDaily(city, kMaxPastDays - config_parameters.past_days),
                                           CreateDayDaily(city, kMaxPastDays - config_parameters.past_days + 4),
                                   }) | flex,
                              vbox({
                                           CreateDayDaily(city, kMaxPastDays - config_parameters.past_days + 1),
                                           CreateDayDaily(city, kMaxPastDays - config_parameters.past_days + 5),
                                   }) | flex,
                              vbox({
                                           CreateDayDaily(city, kMaxPastDays - config_parameters.past_days + 2),
                                           CreateDayDaily(city, kMaxPastDays - config_parameters.past_days + 6),
                                   }) | flex,
                              vbox({
                                           CreateDayDaily(city, kMaxPastDays - config_parameters.past_days + 3),
                                           CreateDayDaily(city, kMaxPastDays - config_parameters.past_days + 7),
                                   }) | flex
                      });
    elements.push_back(table);
    cities[city]->result = elements;

    std::ofstream file(config_parameters.storage + "/" + cities[city]->real_name + "_daily.json");
    file << cities[city]->json;;
}

ftxui::Element BoWeather::CreateDayDaily(const std::string& city, int day) {
    return window(center(text(BoWeatherOutput::ParseDate(
                          cities[city]->json["daily"]["sunrise"][day]))),
                  GetMainHbox(city, day, 0, dly));
}

void BoWeather::JoinDaysHourly(const std::string& city) {
    Elements elements;
    for (int day = 0; day < kMaxPastDays; ++day) {
        elements.push_back(CreateDayHourly(city, day));
    }
    for (int day = kMaxPastDays; day < kMaxPastDays + kMaxForecastDays; ++day) {
        elements.push_back(CreateDayHourly(city, day));
    }
    cities[city]->result = elements;

    std::ofstream file(config_parameters.storage + "/" + cities[city]->real_name + "_default.json");
    file << cities[city]->json;
}

ftxui::Element BoWeather::CreateDayHourly(const std::string& city, int day) {
    auto table = window(center(
                                text(BoWeatherOutput::ParseDate(cities[city]->json["hourly"]["time"][day * kHoursPerDay]))),
                        CreateDayHourlyHbox(city, day)
    ) | flex;
    return table;
}

ftxui::Element BoWeather::CreateDayHourlyHbox(const std::string& city, int day) {
    return hbox({
                        CreateDayHourlyVbox(city, day, 0) | flex,
                        separator(),
                        CreateDayHourlyVbox(city, day, 6) | flex,
                        separator(),
                        CreateDayHourlyVbox(city, day, 12) | flex,
                        separator(),
                        CreateDayHourlyVbox(city, day, 18) | flex
                });
}

ftxui::Element BoWeather::CreateDayHourlyVbox(const std::string& city, int day, int hour) {
    return vbox({
                        CreateDayHourlyHour(city, day, hour),
                        separator(),
                        CreateDayHourlyHour(city, day, hour + 1),
                        separator(),
                        CreateDayHourlyHour(city, day, hour + 2),
                        separator(),
                        CreateDayHourlyHour(city, day, hour + 3),
                        separator(),
                        CreateDayHourlyHour(city, day, hour + 4),
                        separator(),
                        CreateDayHourlyHour(city, day, hour + 5)
                });
}

ftxui::Element BoWeather::CreateDayHourlyHour(const std::string& city, int day, int hour) {
    return vbox({
                        center(text(BoWeatherOutput::ParseTime(
                                cities[city]->json["hourly"]["time"][day * kHoursPerDay + hour]))),
                        separator(),
                        GetMainHbox(city, day, hour)
                });
}

void BoWeather::Run() {
    std::thread rewrite_thread(&BoWeather::RewriteData, this, true);
    rewrite_thread.detach();
    switch (config_parameters.details) {
        case default_frequency:
            RunDefault();
            break;
        case hourly:
            RunHourly();
            break;
        case daily:
            RunDaily();
            break;
    }
    rewrite_thread.join();
}

void BoWeather::RunDaily() {
    if (something_stored) {
        bool everything = true;
        for (auto& city: cities) {
            if (!city.second->stored) {
                everything = false;
                break;
            }
        }
        if (everything) {
            city_it = cities.begin();
        }
        output = city_it->second->result;
    }
    auto renderer = Renderer({[this]() {
        if (something_stored) {
            return vbox({
                                text(city_it->second->real_name + " weather forecast") | bold | center,
                                text("\n"),
                                center(CurrentWeather(city_it->first)),
                                vbox({output})
                        });
        }
        return text("");
    }});
    auto screen = ScreenInteractive::TerminalOutput();
    auto run = CatchEvent(renderer, [&](const Event& event) {
        if (event == Event::Character('p')) {
            if (something_stored) {
                if (city_it != cities.begin()) {
                    city_it--;
                } else {
                    city_it = --cities.end();
                }
                while (!city_it->second->stored) {
                    if (city_it != cities.begin()) {
                        city_it--;
                    } else {
                        city_it = --cities.end();
                    }
                }
                ChangeOutputDaily(city_it);
            }

            return true;
        } else if (event == Event::Character('n')) {
            if (something_stored) {
                if (++city_it == cities.end()) {
                    city_it = cities.begin();
                }
                while (!city_it->second->stored) {
                    if (++city_it == cities.end()) {
                        city_it = cities.begin();
                    } else {
                        ++city_it;
                    }
                }
            }
            ChangeOutputDaily(city_it);

            return true;
        } else if (event == Event::Escape) {
            screen.ExitLoopClosure()();
            return true;
        } else if (event == Event::Character('r')) {
            GetCities();
            for (auto& city: cities) {
                if (city.second->stored) {
                    JoinDaysDaily(city.first);
                }
            }
            if (something_stored) {
                ChangeOutputDaily(city_it);
            }
            return true;
        }
    });
    screen.Loop(run);
}

void BoWeather::RunDefault() {
    if (something_stored) {
        bool everything = true;
        for (auto& city: cities) {
            if (!city.second->stored) {
                everything = false;
                break;
            }
        }
        if (everything) {
            city_it = cities.begin();
        }
        for (uint16_t day = kMaxPastDays - config_parameters.past_days; day < kMaxPastDays; ++day) {
            output.push_back(city_it->second->result[day]);
        }
        for (int day = kMaxPastDays; day < kMaxPastDays + config_parameters.forecast_days; ++day) {
            output.push_back(city_it->second->result[day]);
        }
    }
    unsigned long long days = output.size();
    auto renderer = Renderer({[this]() {
        if (something_stored) {
            return vbox({
                                text(city_it->second->real_name + " weather forecast") | bold | center,
                                text("\n"),
                                center(CurrentWeather(city_it->first)),
                                vbox({output})
                        });
        }
        return text("");
    }});
    auto screen = ScreenInteractive::TerminalOutput();
    auto run = CatchEvent(renderer, [&](const Event& event) {
        if (event == Event::Character('p')) {
            if (something_stored) {
                if (city_it != cities.begin()) {
                    city_it--;
                } else {
                    city_it = --cities.end();
                }
                while (!city_it->second->stored) {
                    if (city_it != cities.begin()) {
                        city_it--;
                    } else {
                        city_it = --cities.end();
                    }
                }
                ChangeOutputDefault(city_it, days);
            }

            return true;
        } else if (event == Event::Character('n')) {
            if (something_stored) {
                if (++city_it == cities.end()) {
                    city_it = cities.begin();
                }
                while (!city_it->second->stored) {
                    if (++city_it == cities.end()) {
                        city_it = cities.begin();
                    } else {
                        ++city_it;
                    }
                }
            }
            ChangeOutputDefault(city_it, days);

            return true;
        } else if (event == Event::Escape) {
            screen.ExitLoopClosure()();

            return true;
        } else if (event == Event::Character('+')) {
            if (something_stored) {
                if (days < kMaxForecastDays) {
                    days++;
                    ChangeOutputDefault(city_it, days);
                }
            }
            return true;
        } else if (event == Event::Character('-')) {
            if (something_stored) {
                if (days > kMinDays) {
                    days--;
                    ChangeOutputDefault(city_it, days);
                }
            }
            return true;
        } else if (event == Event::Character('r')) {
            GetCities();
            for (auto& city: cities) {
                if (city.second->stored) {
                    JoinDaysDefault(city.first);
                }
            }
            if (something_stored) {
                ChangeOutputDefault(city_it, days);
            }
            return true;
        }
    });
    screen.Loop(run);
}

void BoWeather::RunHourly() {
    if (something_stored) {
        bool everything = true;
        for (auto& city: cities) {
            if (!city.second->stored) {
                everything = false;
                break;
            }
        }
        if (everything) {
            city_it = cities.begin();
        }
        output.push_back(city_it->second->result[kMaxPastDays - config_parameters.past_days]);
    }
    day_hourly_show = kMaxPastDays - config_parameters.past_days;
    auto renderer = Renderer({[this]() {
        if (something_stored) {
            return vbox({
                                text(city_it->second->real_name + " weather forecast") | bold | center,
                                text("\n"),
                                // center(CurrentWeather(city_it->first)),
                                vbox({output})
                        });
        }
        return text("");
    }});
    auto screen = ScreenInteractive::TerminalOutput();
    auto run = CatchEvent(renderer, [&](const Event& event) {
        if (event == Event::Character('p')) {
            if (something_stored) {
                if (city_it != cities.begin()) {
                    city_it--;
                } else {
                    city_it = --cities.end();
                }
                while (!city_it->second->stored) {
                    if (city_it != cities.begin()) {
                        city_it--;
                    } else {
                        city_it = --cities.end();
                    }
                }
                ChangeOutputHourly(city_it);
            }

            return true;
        } else if (event == Event::Character('n')) {
            if (something_stored) {
                if (++city_it == cities.end()) {
                    city_it = cities.begin();
                }
                while (!city_it->second->stored) {
                    if (++city_it == cities.end()) {
                        city_it = cities.begin();
                    } else {
                        ++city_it;
                    }
                }
            }
            ChangeOutputHourly(city_it);

            return true;
        } else if (event == Event::Escape) {
            screen.ExitLoopClosure()();
            return true;
        } else if (event == Event::Character('+')) {
            if (something_stored) {
                if (day_hourly_show + 1 < kMaxForecastDays + kMaxPastDays) {
                    day_hourly_show++;
                    ChangeOutputHourly(city_it);
                }
            }
            return true;
        } else if (event == Event::Character('-')) {
            if (something_stored) {
                if (day_hourly_show > kMinDays) {
                    day_hourly_show--;
                    ChangeOutputHourly(city_it);
                }
            }
            return true;
        } else if (event == Event::Character('r')) {
            GetCities();
            for (auto& city: cities) {
                if (city.second->stored) {
                    JoinDaysHourly(city.first);
                }
            }
            if (something_stored) {
                ChangeOutputHourly(city_it);
            }
            return true;
        }
    });
    screen.Loop(run);
}

void BoWeather::ChangeOutputDefault(std::map<std::string, City*>::iterator city, unsigned long long days) {
    if (days < output.size()) {
        output.pop_back();
    } else if (days > output.size()) {
        output.push_back(city->second->result[kMaxPastDays - config_parameters.past_days + days - 1]);
    } else {
        output.clear();
        for (uint16_t day = kMaxPastDays - config_parameters.past_days; day < kMaxPastDays && day < days; ++day) {
            output.push_back(city->second->result[day]);
        }
        for (int day = kMaxPastDays; day < days + kMaxPastDays - config_parameters.past_days; ++day) {
            output.push_back(city->second->result[day]);
        }
    }
}

void BoWeather::ChangeOutputDaily(std::map<std::string, City*>::iterator city) {
    output = city_it->second->result;
}

void BoWeather::ChangeOutputHourly(std::map<std::string, City*>::iterator city) {
    output.clear();
    output.push_back(city_it->second->result[day_hourly_show]);
}

ftxui::Element BoWeather::CurrentWeather(const std::string& city) {
    if (config_parameters.current_weather) {
        return window(center(text("Current weather")), GetMainHbox(city, 0, 0, cur));
    }
    return text("");
}

void BoWeather::RewriteData(bool for_while) {
    while (for_while || !something_stored) {
        std::this_thread::sleep_for(std::chrono::minutes(config_parameters.frequency));
        GetCities();
        switch (config_parameters.details) {
            case default_frequency:
                for (auto& city: cities) {
                    if (city.second->stored) {
                        JoinDaysDefault(city.first);
                    }
                }
                if (something_stored) {
                    ChangeOutputDefault(city_it, output.size());
                }
                break;
            case daily:
                for (auto& city: cities) {
                    if (city.second->stored) {
                        JoinDaysDaily(city.first);
                    }
                }
                if (something_stored) {
                    ChangeOutputDaily(city_it);
                }
                break;
            case hourly:
                for (auto& city: cities) {
                    if (city.second->stored) {
                        JoinDaysHourly(city.first);
                    }
                }
                if (something_stored) {
                    ChangeOutputHourly(city_it);
                }
                break;
        }
    }
}

BoWeather::~BoWeather() {
    for (auto& i: cities) {
        delete i.second;
    }
}


