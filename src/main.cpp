#include <chrono>
#include <string>
#include <iostream>

#include "Image.h"

using namespace std::chrono;

using uil8 = uint_least8_t;

// Number of default parameters + 1
const uil8 PARAMETERS = 11;

// Default parameters
uif16 seed   = 10;
uif8 blur    = 0;
uif8 options = 1;
uif8 threshold = 10;
uif8 threshold2 = 3;
uif8 min = 10;
uif8 max = 100;
uif8 fusionThreshold = 10;
std::string path   = "../img/image3.jpg";
std::string output = "../output";

void getElements(int argc, char** argv){
    if(argc == PARAMETERS){
        path      = argv[1];
        output    = argv[2];
        seed      = std::stoi(argv[3]);
        if(seed < 2){
            std::cout<<"Number of seeds equal to 0 or 1 not authorized."<<std::endl;
            std::exit(EXIT_FAILURE);
        }
        blur      = std::stoi(argv[4]);
        if(blur < 0 ){
            std::cout<<"Blur must be at least 0."<<std::endl;
            std::exit(EXIT_FAILURE);
        }
        options   = std::stoi(argv[5]);
        if(options != 1 && options != 2){
            std::cout<<"Options settings must be one or two."<<std::endl;
            std::exit(EXIT_FAILURE);
        }
        threshold = std::stoi(argv[6]);
        if(threshold <= 0 || threshold > 255){
            std::cout<<"Threshold cannot be equal to zero or superior to 255."<<std::endl;
            std::exit(EXIT_FAILURE);
        }
        threshold2 = std::stoi(argv[7]);
        if(threshold2 <= 0 || threshold > 255){
            std::cout<<"Threshold cannot be equal to zero or superior to 255."<<std::endl;
            std::exit(EXIT_FAILURE);
        }
        fusionThreshold = std::stoi(argv[8]);
        if(fusionThreshold <= 0 || fusionThreshold > 255){
            std::cout<<"fusionThreshold cannot be equal to zero or superior to 255."<<std::endl;
            std::exit(EXIT_FAILURE);
        }
        min = std::stoi(argv[9]);
        if(min < 0){
            std::cout<<"min must be superior to 0."<<std::endl;
            std::exit(EXIT_FAILURE);
        }
        max = std::stoi(argv[10]);
        if(max < min){
            std::cout<<"max must be superior to min."<<std::endl;
            std::exit(EXIT_FAILURE);
        }
    }else{
        std::cout<<"Using default parameters."<<std::endl;
    }
}

int main(int argc, char** argv) {

    auto B = high_resolution_clock::now();
    getElements(argc, argv);

    Image im(seed, blur, threshold, threshold2, fusionThreshold, min, max, path, output);
    im.init();

    auto b = high_resolution_clock::now();
    im.kernel();
    im.growShadows();
    auto e = high_resolution_clock::now();
    
    if(options == 1)
        im.fusionSystem1();
    else
        im.fusionSystem2();

    im.draw();
    im.contour();

    im.write();
    im.clear();
    
    auto E = high_resolution_clock::now();
    auto t = duration_cast<milliseconds>(e - b);
    auto T = duration_cast<milliseconds>(E - B);
    std::cout << "Kernel time : " << t.count() << " ms"<<std::endl;
    std::cout << "Total time : " << T.count() << " ms"<<std::endl;
    return 0;
}