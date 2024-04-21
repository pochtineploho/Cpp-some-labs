#include "BoWeather.h"

int main(int argc, char** argv) {
    BoWeather weather;
    if (!weather.PrepareForOutput(argv[1])) {
        std::cerr << "Config must contain an \"api\" field and cities\n";

        return 0;
    }
    weather.Run();

    return 0;
}