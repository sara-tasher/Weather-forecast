#include <iostream>
#include "lib/Weather_forecast.h"


int main(int argc, char* argv[]) {
    std::setlocale(LC_ALL, "en_US.UTF-8");
    if(argc < 2){
        std::cerr << "Config file path doesn't exist!";
        exit(-1);
    }
    Weather app(argv[1]);
    app.Run();
    return 0;
}
