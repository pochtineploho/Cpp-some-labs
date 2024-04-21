#ifndef LABWORK_10_POCHTINEPLOHO_BOWEATHER_H
#define LABWORK_10_POCHTINEPLOHO_BOWEATHER_H

#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/component/loop.hpp>
#include <ftxui/screen/screen.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <thread>

const double kError = 1000;
const int kOkResponseCode = 200;
const int kHoursPerDay = 24;
const int kMorning = 8;
const int kDay = 14;
const int kEvening = 20;
const int kNight = 26;
const int kMaxForecastDays = 7;
const int kMaxPastDays = 2;
const int kMinDays = 0;

const std::initializer_list<cpr::Parameter> kDefaultParameters{{"forecast_days", "8"},
                                                               {"past_days",     "2"},
                                                               {"timezone",      "auto"}};

const std::initializer_list<cpr::Parameter> kDailyParameters{{"daily", "temperature_2m_max"},
                                                             {"daily", "temperature_2m_min"},
                                                             {"daily", "weathercode"},
                                                             {"daily", "sunrise"},
                                                             {"daily", "sunset"},
                                                             {"daily", "precipitation_sum"},
                                                             {"daily", "precipitation_probability_mean"},
                                                             {"daily", "windspeed_10m_max"},
                                                             {"daily", "winddirection_10m_dominant"}};

const std::initializer_list<cpr::Parameter> kHourlyParameters{{"hourly", "temperature_2m"},
                                                              {"hourly", "apparent_temperature"},
                                                              {"hourly", "weathercode"},
                                                              {"hourly", "relativehumidity_2m"},
                                                              {"hourly", "surface_pressure"},
                                                              {"hourly", "precipitation"},
                                                              {"hourly", "precipitation_probability"},
                                                              {"hourly", "windspeed_10m"},
                                                              {"hourly", "winddirection_10m"}};
enum Frequency {
    daily, hourly, default_frequency
};

enum FindData {
    dly, hly, cur
};

const std::map<FindData, std::string> kDirectory {
        {dly, "daily"},
        {hly, "hourly"},
        {cur, "current_weather"}
};

struct ConfigParameters {
    std::string api;
    bool current_weather = false;
    Frequency details = default_frequency;
    uint32_t forecast_days = 3;
    uint32_t past_days = 0;
    uint32_t frequency = 10;
    std::string storage;
};

struct City {
    std::string real_name;
    nlohmann::json json;
    ftxui::Elements result;
    bool stored = true;
    bool need_coordinates = true;
};

class BoWeather {
public:
    bool PrepareForOutput(const char* config_filename);

    void Run();

    BoWeather() = default;

    ~BoWeather();

private:
    void RunDefault();

    void RunHourly();

    void RunDaily();

    bool GetCities(bool first = true);

    void GetWeather(const std::string& city);

    void GetWeather(const std::string& city, const std::pair<double, double>& coordinates);

    bool ParseConfig(const char* config_filename);

    void JoinDaysDefault(const std::string& city);

    void JoinDaysHourly(const std::string& city);

    void JoinDaysDaily(const std::string& city);

    void GetCoordinates(const std::string& city, std::pair<double, double>& coordinates);

    void ChangeOutputDefault(std::map<std::string, City*>::iterator city, unsigned long long days);

    void ChangeOutputDaily(std::map<std::string, City*>::iterator city);

    void ChangeOutputHourly(std::map<std::string, City*>::iterator city);

    ftxui::Element CreateDayDefault(const std::string& city, int day);

    ftxui::Element CreateDayDaily(const std::string& city, int day);

    ftxui::Element CreateDayHourly(const std::string& city, int day);

    ftxui::Element CreateDayHourlyHour(const std::string& city, int day, int hour);

    ftxui::Element CreateDayHourlyVbox(const std::string& city, int day, int hour);

    ftxui::Element CreateDayHourlyHbox(const std::string& city, int day);

    ftxui::Element GetLeftVbox(const std::string& city, int day, int day_time = 0, FindData find_data = hly);

    ftxui::Element GetRightVbox(const std::string& city, int day, int day_time = 0, FindData find_data = hly);

    ftxui::Element GetMainHbox(const std::string& city, int day, int day_time = 0, FindData find_data = hly);

    ftxui::Element GetTemperature(const std::string& city, int day, int day_time = 0, FindData  find_data= hly);

    ftxui::Element GetWind(const std::string& city, int day, int day_time = 0, FindData find_data = hly);

    ftxui::Element GetHumidityOrSunrise(const std::string& city, int day, int day_time = 0, FindData find_data = hly);

    ftxui::Element GetPrecipitation(const std::string& city, int day, int day_time = 0, FindData find_data = hly);

    ftxui::Element CurrentWeather(const std::string& city);

   void RewriteData(bool for_while);

private:
    std::map<std::string, City*> cities;
    ConfigParameters config_parameters;
    ftxui::Elements output;
    std::map<std::string, City*>::iterator city_it;
    bool something_stored = true;
    int day_hourly_show = 2;
};

#endif //LABWORK_10_POCHTINEPLOHO_BOWEATHER_H
