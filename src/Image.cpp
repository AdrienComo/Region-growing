#include "Image.h"

Image::Image(const uif16 _seed, const uif8 _blur, const uif8 _threshold, const uif8 _threshold2, const uif8 _fusionThreshold, const uif8 _min, const uif8 _max, const std::string _path, const std::string _output) : seed(_seed), 
blur(_blur), threshold(_threshold), threshold2(_threshold2), fusionThreshold(_fusionThreshold), min(_min), max(_max), path(_path), output(_output) {}

void Image::init() {
    img = cv::imread(path, 1);
    if(!img.empty()){
        imgS = img.size();
        imgS2 = cv::Size(imgS.width - 1, imgS.height - 1);
        imgS3 = cv::Size(imgS.width + 2, imgS.height + 2);
        img2 = cv::Mat(imgS.height, imgS.width, CV_8UC3, cv::Scalar(0,0,0));
        if(blur != 0)
            cv::GaussianBlur(img, img, cv::Size(blur, blur), 0);

        std::srand(time(nullptr));
        static unsigned int g_seed = rand() % 255 + 1;

        //Initialize random seeds over image
        std::set<uif32> t;
        t.insert(1);
        for(uif16 i=0;i<seed;++i){
            g_seed = (214013*g_seed+2531011);
            seedsCoord.push_back((((g_seed>>16)&0x7FFF) % imgS.height)+1);

            g_seed = (214013*g_seed+2531011);
            seedsCoord.push_back((((g_seed>>16)&0x7FFF) % imgS.width)+1);
            color.push_back(t);
        }

        // Initialize 2d array flag
        flag = new uif32 * [imgS3.height];
        for(uif16 i=0;i<imgS3.height;++i){
            flag[i] = new uif32[imgS3.width];
            std::memset(flag[i], 0, imgS3.width * sizeof(uif32));
        }

        // Initialize border of 2d array flag
        for(uif16 i=0;i<imgS3.height;++i){
            flag[i][0] = 1;
            flag[i][imgS.width+1] = 1;
        }

        for(uif16 i=0;i<imgS3.width;++i){
            flag[0][i] = 1;
            flag[imgS.height+1][i] = 1;
        }

        // Get color of seeds
        for(uif16 i=0;i<seedsCoord.size();i+=2){
            cv::Vec3b* ptr = img.ptr<cv::Vec3b>(seedsCoord.at(i));
            col.push_back(ptr[seedsCoord.at(i)+1][0]);
            col.push_back(ptr[seedsCoord.at(i)+1][1]);
            col.push_back(ptr[seedsCoord.at(i)+1][2]);
        }
    }else{
        std::cout<<"Unavailable image."<<std::endl;
        std::exit(EXIT_FAILURE);
    }
}

// Iterates over all seeds and grows
void Image::kernel() {
    for(uif16 i=0;i<seed;++i) {
        process.push_back(seedsCoord.at(0));
        seedsCoord.pop_front();
        process.push_back(seedsCoord.at(0));
        seedsCoord.pop_front();

        if(flag[process.at(0)][process.at(1)] == 0)
            growConn4(i);
        else
            process.clear();
    }
}

// Growing algorithm
bool Image::growConn4(const uif16 region) {
    c = region * 3;
    rI = region + 2;

    r = col.at(c);
    g = col.at(c+1);
    b = col.at(c+2);

    bool full = true;
    while(full) {

        uif16 cY = process.front();
        process.pop_front();
        uif16 cX = process.front();
        process.pop_front();

        if(flag[cY][cX] == 0 && simpleAbsDist(cY, cX))
            flag[cY][cX] = rI;

        uif16 bX = cX - 1;
        if (flag[cY][bX] == 0) {
            if (simpleAbsDist(cY-1, bX-1)) {
                flag[cY][bX] = rI;
                process.push_back(cY);
                process.push_back(bX);
            }
        }else{
            color.back().insert(flag[cY][bX]);
        }
        uif16 aX = cX + 1;
        if (flag[cY][aX] == 0){
            if (simpleAbsDist(cY-1, aX-1)) {
                flag[cY][aX] = rI;
                process.push_back(cY);
                process.push_back(aX);
            }
        }else{
            color.back().insert(flag[cY][aX]);
        }
        uif16 bY = cY - 1;
        if (flag[bY][cX] == 0) {
            if (simpleAbsDist(bY-1, cX-1)) {
                flag[bY][cX] = rI;
                process.push_back(bY);
                process.push_back(cX);
            }
        }else{
            color.back().insert(flag[bY][cX]);
        }
        uif16 aY = cY + 1;
        if (flag[aY][cX] == 0){
            if(simpleAbsDist(aY-1, cX-1)) {
                flag[aY][cX] = rI;
                process.push_back(aY);
                process.push_back(cX);
            }
        }else{
            color.back().insert(flag[aY][cX]);
        }
        if(process.empty())
            full = false;
    }
}

void Image::growShadows() {
    uif32 inR = seed;
    threshold = threshold * threshold2;
    std::set<uif32> t;
    t.insert(1);

    for(uif16 i=1;i<imgS.height+1;++i){
        for(uif16 j=1;j<imgS.width+1;++j){
            uif32 index = flag[i][j];
            if(index == 0){
                cv::Vec3b cc = img.at<cv::Vec3b>(cv::Point(j-1, i-1));
                col.push_back(cc[0]);
                col.push_back(cc[1]);
                col.push_back(cc[2]);
                color.push_back(t);
                process.push_back(i);
                process.push_back(j);
                growConn4(inR);
                ++inR;
            }
        }
    }
    color.push_back(t);
}

bool Image::simpleAbsDist(const uif16 y, const uif16 x) {
    cv::Vec3b* ptr = img.ptr<cv::Vec3b>(y);
    if(std::abs(ptr[x][0] - r) <= threshold && std::abs(ptr[x][1] - g) <= threshold && std::abs(ptr[x][2] - b) <= threshold)
        return true;
    return false;
}

void Image::draw() {
    uif32 index = 0;
    for(uif16 i=1;i<imgS.height+1;++i){
        for(uif16 j=1;j<imgS.width+1;++j){
            index = flag[i][j];
            if(index != 0 && index != 1){
                cv::Vec3b c = img.at<cv::Vec3b>(cv::Point(j-1, i-1));
                c[0] = col.at(index*3-6);
                c[1] = col.at(index*3-5);
                c[2] = col.at(index*3-4);
                img.at<cv::Vec3b>(cv::Point(j-1, i-1)) = c;
            }
        }
    }
}

void Image::fusionSystem1() {
    uif8 r,g,b;
    static unsigned int g_seed = rand() % 255 + 1;
    for(uif32 i=0;i<color.size();++i){
        g_seed = (214013*g_seed+2531011);
        r =  (((g_seed>>16)&0x7FFF) % 255);
        g_seed = (214013*g_seed+2531011);
        g  = (((g_seed>>16)&0x7FFF) % 255);
        g_seed = (214013*g_seed+2531011);
        b = (((g_seed>>16)&0x7FFF) % 255);
        for (auto it=color.at(i).begin(); it!=color.at(i).end(); ++it){
            if(*it != 1){
                col.at(i*3-6) = r;
                col.at(i*3-5) = g;
                col.at(i*3-4) = b;
                if(std::abs(col.at(i*3-6) - col.at(*it*3-6)) <= fusionThreshold && std::abs(col.at(i*3-5)- col.at(*it*3-5)) <= fusionThreshold && std::abs(col.at(i*3-4)- col.at(*it*3-4)) <= fusionThreshold){
                    col.at(*it*3-6) = r;
                    col.at(*it*3-5) = g;
                    col.at(*it*3-4) = b;
                }
            }
        }
    }
}

void Image::fusionSystem2() {
    int t = 10;
    for(uif32 i=0;i<color.size();++i){
        for (auto it=color.at(i).begin(); it!=color.at(i).end(); ++it){
            if(*it != 1){
                if(std::abs(col.at(i*3-6) - col.at(*it*3-6)) <= t && std::abs(col.at(i*3-5)- col.at(*it*3-5)) <= t && std::abs(col.at(i*3-4)- col.at(*it*3-4)) <= t){
                    col.at(i*3-6) = col.at(*it*3-6);
                    col.at(i*3-5) = col.at(*it*3-5);
                    col.at(i*3-4) = col.at(*it*3-4);
                }
            }
        }
    }
}


void Image::contour() {
    cv::Canny(img, img2, min, max);
}

void Image::write() const{
    cv::imwrite(output + ".png", img);
    cv::imwrite(output + "2.png", img2);
}

void Image::clear() {
    img.release();
    cv::destroyAllWindows();

    col.clear();
    color.clear();
    process.clear();
    seedsCoord.clear();

    for (uif16 i = 0;i<imgS3.height;++i)
        delete [] flag[i];
    delete [] flag;
    flag = nullptr;
}