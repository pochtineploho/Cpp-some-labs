#ifndef BOWEATHER_H_BOWEATHEROUTPUT_H
#define BOWEATHER_H_BOWEATHEROUTPUT_H

#include <ftxui/dom/elements.hpp>
#include <ftxui/component/component.hpp>
#include <iostream>
#include <iomanip>
#include <map>
#include <array>
#include <ctime>

using namespace ftxui;

namespace BoWeatherOutput {

    const int kErrorCode = 239;
    const int kReallyCold = -30;
    const int kReallyHot = 30;
    const int kSlowWind = 5;
    const int kNormalWind = 10;
    const int kFastWind = 15;
    const int kRapidWind = 20;
    const int kStormWind = 25;
    const int kWhoooshWind = 30;

    const std::array<std::string, 8> kWindDirections{"↓", "↙", "←", "↖", "↑", "↗", "→", "↘"};

    std::string GetWindDirection(int degree) {
        return kWindDirections[degree / 45];
    }

    const std::map<int, std::string> kWMOCodes{{0,          "Clear sky"},
                                               {1,          "Mainly clear"},
                                               {2,          "Partly cloudy"},
                                               {3,          "Overcast"},
                                               {45,         "Fog"},
                                               {48,         "Depositing rime fog"},
                                               {51,         "Light drizzle"},
                                               {53,         "Moderate drizzle"},
                                               {55,         "Dense drizzle"},
                                               {56,         "Light freezing drizzle"},
                                               {57,         "Dense freezing drizzle"},
                                               {61,         "Slight rain"},
                                               {63,         "Moderate rain"},
                                               {65,         "Dense rain"},
                                               {66,         "Light freezing rain"},
                                               {67,         "Heavy freezing rain"},
                                               {71,         "Slight snow fall"},
                                               {73,         "Moderate snow fall"},
                                               {75,         "Heavy snow fall"},
                                               {77,         "Snow grains"},
                                               {80,         "Slight rain shower"},
                                               {81,         "Moderate rain shower"},
                                               {82,         "Violent rain shower"},
                                               {85,         "Slight snow shower"},
                                               {86,         "Heavy snow shower"},
                                               {95,         "Thunderstorm"},
                                               {96,         "Thunderstorm with slight hail"},
                                               {99,         "Thunderstorm with heavy hail"},
                                               {kErrorCode, "Unknown conditions"}};

    int CheckWMO(int code) {
        if (kWMOCodes.find(code) != kWMOCodes.end()) {
            return code;
        }
        return kErrorCode;
    }

    Element kClearSky() {
        return vbox({
                            color(Color::Yellow, text("    \\   /    ")),
                            color(Color::Yellow, text("     .-.     ")),
                            color(Color::Yellow, text("  ― (   ) ―  ")),
                            color(Color::Yellow, text("     `-’     ")),
                            color(Color::Yellow, text("    /   \\    ")),
                    });
    }

    Element kMainlyClear() {
        return vbox({
                            color(Color::Yellow, text("   \\  /      ")),
                            hbox({
                                         color(Color::Yellow, text(" _ /\"\"")),
                                         text(".-.    ")
                                 }),
                            hbox({
                                         color(Color::Yellow, text("   \\_")),
                                         text("(   ).  ")
                                 }),
                            hbox({
                                         color(Color::Yellow, text("   /")),
                                         text("(___(__) ")
                                 }),
                            text("             ")
                    });
    }

    Element kPartlyCloudy() {
        return vbox({
                            color(Color::Yellow, text("   \\  /      ")),
                            hbox({
                                         color(Color::Yellow, text(" _ /\"")),
                                         text(".-.     ")
                                 }),
                            hbox({
                                         color(Color::Yellow, text("   \\")),
                                         text("(   ).   ")
                                 }),
                            hbox({
                                         color(Color::Yellow, text("   ")),
                                         text("(___(__)  ")
                                 }),
                            text("             ")
                    });
    }

    Element kOvercast() {
        return vbox({
                            color(Color::GrayLight, text("             ")),
                            color(Color::GrayLight, text("     .--.    ")),
                            color(Color::GrayLight, text("  .-(    ).  ")),
                            color(Color::GrayLight, text(" (___.__)__) ")),
                            color(Color::GrayLight, text("             "))
                    });
    }

    Element kFog() {
        return vbox({
                            text("             "),
                            text(" _ - _ - _ - "),
                            text("  _ - _ - _  "),
                            text(" _ - _ - _ - "),
                            text("             ")
                    });
    }

    Element kDepositingRimeFog() {
        return vbox({
                            text("             "),
                            text(" * - _ - _ * "),
                            text("  _ - * - _  "),
                            text(" * - _ - * - "),
                            text("             ")
                    });
    }

    Element kLightDrizzle() {
        return vbox({
                            text("     .-.     "),
                            text("    (   ).   "),
                            text("   (___(__)  "),
                            color(Color::Blue, text("          ‘  ")),
                            color(Color::Blue, text("     ‘       ")),
                    });
    }

    Element kModerateDrizzle() {
        return vbox({
                            text("     .-.     "),
                            text("    (   ).   "),
                            text("   (___(__)  "),
                            color(Color::Blue, text("   ‘   ‘  ‘  ")),
                            color(Color::Blue, text("     ‘       ")),
                    });
    }

    Element kDenseDrizzle() {
        return vbox({
                            text("     .-.     "),
                            text("    (   ).   "),
                            text("   (___(__)  "),
                            color(Color::Blue, text("   ‘   ‘  ‘  ")),
                            color(Color::Blue, text("  ‘   ‘  ‘   ")),
                    });
    }

    Element kLightFreezingDrizzle() {
        return vbox({
                            text("     .-.     "),
                            text("    (   ).   "),
                            text("   (___(__)  "),
                            color(Color::Blue, text("          ‘  ")),
                            text("     *       "),
                    });
    }

    Element kDenseFreezingDrizzle() {
        return vbox({
                            text("     .-.     "),
                            text("    (   ).   "),
                            text("   (___(__)  "),
                            hbox({
                                         text("   *   "),
                                         color(Color::Blue, text("‘")),
                                         text("  *  ")
                                 }),
                            hbox({
                                         color(Color::Blue, text("  ‘   ")),
                                         text("*"),
                                         color(Color::Blue, text("  ‘   ")),
                                 })
                    });
    }

    Element kSlightRain() {
        return vbox({
                            text("     .-.     "),
                            text("    (   ).   "),
                            text("   (___(__)  "),
                            color(Color::Blue, text("    ‘ ‘ ‘ ‘  ")),
                            color(Color::Blue, text("   ‘ ‘ ‘ ‘   ")),
                    });
    }

    Element kModerateRain() {
        return vbox({
                            text("     .-.     "),
                            text("    (   ).   "),
                            text("   (___(__)  "),
                            color(Color::Blue, text("    ‘‚‘ ‘‚‘  ")),
                            color(Color::Blue, text("   ‘ ‘‚‘ ‘   ")),
                    });
    }

    Element kDenseRain() {
        return vbox({
                            text("     .-.     "),
                            text("    (   ).   "),
                            text("   (___(__)  "),
                            color(Color::Blue, text("  ‚‘‚‘‚‘‚‘   ")),
                            color(Color::Blue, text(" ‚’‚’‚’‚’   ")),
                    });
    }

    Element kLightFreezingRain() {
        return vbox({
                            text("     .-.     "),
                            text("    (   ).   "),
                            text("   (___(__)  "),
                            hbox({
                                         text("    * "),
                                         color(Color::Blue, text("‘")),
                                         text(" * "),
                                         color(Color::Blue, text("‘  ")),
                                 }),
                            hbox({
                                         color(Color::Blue, text("   ‘ ")),
                                         text("*"),
                                         color(Color::Blue, text(" ‘ ")),
                                         text("*   "),
                                 })
                    });
    }

    Element kHeavyFreezingRain() {
        return vbox({
                            text("     .-.     "),
                            text("    (   ).   "),
                            text("   (___(__)  "),
                            hbox({
                                         text("    *"),
                                         color(Color::Blue, text("‚‘")),
                                         text("*"),
                                         color(Color::Blue, text("‚‘  ")),
                                 }),
                            hbox({
                                         color(Color::Blue, text("  ‚’")),
                                         text("*"),
                                         color(Color::Blue, text("‚’")),
                                         text("*      "),
                                 })
                    });
    }

    Element kSlightSnowFall() {
        return vbox({
                            text("     .-.     "),
                            text("    (   ).   "),
                            text("   (___(__)  "),
                            text("     *  *    "),
                            text("   *  *      ")
                    });
    }

    Element kModerateSnowFall() {
        return vbox({
                            text("     .-.     "),
                            text("    (   ).   "),
                            text("   (___(__)  "),
                            text("   *  *  *   "),
                            text("  *  *  *    ")
                    });
    }

    Element kHeavySnowFall() {
        return vbox({
                            text("     .-.     "),
                            text("    (   ).   "),
                            text("   (___(__)  "),
                            text("    * * * *  "),
                            text("   * * * *   ")
                    });
    }

    Element kSnowGrains() {
        return vbox({
                            text("     .-.     "),
                            text("    (   ).   "),
                            text("   (___(__)  "),
                            text("   ◦ ◦ ◦ ◦   "),
                            text("  ◦ ◦ ◦ ◦    ")
                    });
    }

    Element kSlightRainShower() {
        return vbox({
                            hbox({
                                         color(Color::Yellow, text(" _`/\"\"")),
                                         text(".-.    ")
                                 }),
                            hbox({
                                         color(Color::Yellow, text("  ,\\_")),
                                         text("(   ).  ")
                                 }),
                            hbox({
                                         color(Color::Yellow, text("   /")),
                                         text("(___(__) ")
                                 }),
                            color(Color::Blue, text("    ‘ ‘ ‘ ‘  ")),
                            color(Color::Blue, text("   ‘ ‘ ‘ ‘   ")),
                    });
    }

    Element kModerateRainShower() {
        return vbox({
                            hbox({
                                         color(Color::Yellow, text(" _`/\"\"")),
                                         text(".-.    ")
                                 }),
                            hbox({
                                         color(Color::Yellow, text("  ,\\_")),
                                         text("(   ).  ")
                                 }),
                            hbox({
                                         color(Color::Yellow, text("   /")),
                                         text("(___(__) ")
                                 }),
                            color(Color::Blue, text("    ‘‚‘ ‘‚‘  ")),
                            color(Color::Blue, text("   ‘ ‘‚‘ ‘   ")),
                    });
    }

    Element kViolentRainShower() {
        return vbox({
                            hbox({
                                         color(Color::Yellow, text(" _`/\"\"")),
                                         text(".-.    ")
                                 }),
                            hbox({
                                         color(Color::Yellow, text("  ,\\_")),
                                         text("(   ).  ")
                                 }),
                            hbox({
                                         color(Color::Yellow, text("   /")),
                                         text("(___(__) ")
                                 }),
                            color(Color::Blue, text("  ‚‘‚‘‚‘‚‘   ")),
                            color(Color::Blue, text(" ‚’‚’‚’‚’   ")),
                    });
    }

    Element kSlightSnowShower() {
        return vbox({
                            hbox({
                                         color(Color::Yellow, text(" _`/\"\"")),
                                         text(".-.    ")
                                 }),
                            hbox({
                                         color(Color::Yellow, text("  ,\\_")),
                                         text("(   ).  ")
                                 }),
                            hbox({
                                         color(Color::Yellow, text("   /")),
                                         text("(___(__) ")
                                 }),
                            text("   *  *  *   "),
                            text("  *  *  *    ")
                    });
    }

    Element kHeavySnowShower() {
        return vbox({
                            hbox({
                                         color(Color::Yellow, text(" _`/\"\"")),
                                         text(".-.    ")
                                 }),
                            hbox({
                                         color(Color::Yellow, text("  ,\\_")),
                                         text("(   ).  ")
                                 }),
                            hbox({
                                         color(Color::Yellow, text("   /")),
                                         text("(___(__) ")
                                 }),
                            text("    * * * *  "),
                            text("   * * * *   ")
                    });
    }


    Element kThunderstorm() {
        return vbox({
                            color(Color::GrayLight, text("     .-.     ")),
                            color(Color::GrayLight, text("    (   ).   ")),
                            color(Color::GrayLight, text("   (___(__)  ")),
                            hbox({
                                         color(Color::Blue, text("  ‚‘‚")),
                                         color(Color::Yellow, text("ϟ")),
                                         color(Color::Blue, text("‚‘‚")),
                                         color(Color::Yellow, text("ϟ   "))
                                 }),
                            hbox({
                                         color(Color::Yellow, text(" ϟ")),
                                         color(Color::Blue, text("‚’‚")),
                                         color(Color::Yellow, text("ϟ")),
                                         color(Color::Blue, text("‚’,    "))
                                 }),
                    });
    }

    Element kThunderstormWithSlightHail() {
        return vbox({
                            color(Color::GrayLight, text("     .-.     ")),
                            color(Color::GrayLight, text("    (   ).   ")),
                            color(Color::GrayLight, text("   (___(__)  ")),
                            hbox({
                                         color(Color::Blue, text("  ‚")),
                                         text("◦"),
                                         color(Color::Blue, text("‚")),
                                         color(Color::Yellow, text("ϟ")),
                                         color(Color::Blue, text("‚")),
                                         text("◦"),
                                         color(Color::Blue, text("‚")),
                                         color(Color::Yellow, text("ϟ   "))
                                 }),
                            hbox({
                                         color(Color::Yellow, text(" ϟ")),
                                         color(Color::Blue, text("‚")),
                                         text("◦"),
                                         color(Color::Blue, text("‚")),
                                         color(Color::Yellow, text("ϟ")),
                                         color(Color::Blue, text("‚")),
                                         text("◦"),
                                         color(Color::Blue, text("‚    ")),
                                 }),
                    });
    }

    Element kThunderstormWithHeavyHail() {
        return vbox({
                            color(Color::GrayLight, text("     .-.     ")),
                            color(Color::GrayLight, text("    (   ).   ")),
                            color(Color::GrayLight, text("   (___(__)  ")),
                            hbox({
                                         color(Color::Blue, text("  ‚")),
                                         text("॰"),
                                         color(Color::Blue, text("‚")),
                                         color(Color::Yellow, text("ϟ")),
                                         color(Color::Blue, text("‚")),
                                         text("॰"),
                                         color(Color::Blue, text("‚")),
                                         color(Color::Yellow, text("ϟ   "))
                                 }),
                            hbox({
                                         color(Color::Yellow, text(" ϟ")),
                                         color(Color::Blue, text("‚")),
                                         text("॰"),
                                         color(Color::Blue, text("‚")),
                                         color(Color::Yellow, text("ϟ")),
                                         color(Color::Blue, text("‚")),
                                         text("॰"),
                                         color(Color::Blue, text("‚    ")),
                                 }),
                    });
    }

    Element kUnknownConditions() {
        return vbox({
                            color(Color::Purple, text("     .-.     ")),
                            color(Color::Purple, text("    ( ? ).   ")),
                            color(Color::Purple, text("   (_?_(_?)  ")),
                            color(Color::Pink1, text("   ? ? ? ?   ")),
                            color(Color::Pink1, text("  ? ? ? ?    "))
                    });
    }

    std::map<int, std::function<Element()>> kWeatherPictures{{0,          &kClearSky},
                                                             {1,          &kMainlyClear},
                                                             {2,          &kPartlyCloudy},
                                                             {3,          &kOvercast},
                                                             {45,         &kFog},
                                                             {48,         &kDepositingRimeFog},
                                                             {51,         &kLightDrizzle},
                                                             {53,         &kModerateDrizzle},
                                                             {55,         &kDenseDrizzle},
                                                             {56,         &kLightFreezingDrizzle},
                                                             {57,         &kDenseFreezingDrizzle},
                                                             {61,         &kSlightRain},
                                                             {63,         &kModerateRain},
                                                             {65,         &kDenseRain},
                                                             {66,         &kLightFreezingRain},
                                                             {67,         &kHeavyFreezingRain},
                                                             {71,         &kSlightSnowFall},
                                                             {73,         &kModerateSnowFall},
                                                             {75,         &kHeavySnowFall},
                                                             {77,         &kSnowGrains},
                                                             {80,         &kSlightRainShower},
                                                             {81,         &kModerateRainShower},
                                                             {82,         &kViolentRainShower},
                                                             {85,         &kSlightSnowShower},
                                                             {86,         &kHeavySnowShower},
                                                             {95,         &kThunderstorm},
                                                             {96,         &kThunderstormWithSlightHail},
                                                             {99,         &kThunderstormWithHeavyHail},
                                                             {kErrorCode, &kUnknownConditions}};

    const std::array<std::string, 12> kMonths = {
            "January", "February", "March", "April", "May", "June", "July", "August",
            "September", "October", "November", "December"
    };

    std::string ParseDate(const std::string& date_time) {
        std::istringstream ss(date_time);
        int year, month, day, hour, minute, second;
        char delimiter;
        ss >> year >> delimiter >> month >> delimiter >> day >> delimiter
           >> hour >> delimiter >> minute >> delimiter >> second;
        std::string suffix;
        if (day % 10 == 1 && day != 11) {
            suffix = "st";
        } else if (day % 10 == 2 && day != 12) {
            suffix = "nd";
        } else if (day % 10 == 3 && day != 13) {
            suffix = "rd";
        } else {
            suffix = "th";
        }
        std::ostringstream result;
        result << day << suffix << " " << kMonths[month];

        return result.str();
    }

    std::string ParseTime(const std::string& date_time) {
        std::istringstream ss(date_time);
        int year, month, day, hour, minute, second;
        char delimiter;
        ss >> year >> delimiter >> month >> delimiter >> day >> delimiter
           >> hour >> delimiter >> minute >> delimiter >> second;
        std::stringstream hh_mm;
        hh_mm << std::setfill('0') << std::setw(2) << hour << ":" << std::setw(2) << minute;

        return hh_mm.str();
    }

    std::string GetStringTemperature(int temperature) {
        if (temperature > 0) {
            return "+" + std::to_string(temperature);
        }
        return std::to_string(temperature);
    }

    Color GetTemperatureColor(int temperature) {
        if (temperature < kReallyCold) {
            return Color::LightSkyBlue1;
        } else if (temperature > kReallyHot) {
            return Color::RedLight;
        }
        return Color::White;
    }

    Color GetWindColor(int speed) {
        if (speed < kSlowWind) {
            return Color::White;
        } else if (speed < kNormalWind) {
            return Color::GreenLight;
        } else if (speed < kFastWind) {
            return Color::YellowLight;
        } else if (speed < kRapidWind) {
            return Color::Yellow;
        } else if (speed < kStormWind) {
            return Color::Orange1;
        } else if (speed < kWhoooshWind) {
            return Color::RedLight;
        }
        return Color::Red;
    }
}
#endif // BOWEATHER_H_BOWEATHEROUTPUT_H
