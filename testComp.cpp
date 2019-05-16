#define CATCH_CONFIG_MAIN
#include "cs221util/catch.hpp"
#include <vector>
#include <sys/stat.h>
#include <iostream>
#include "cs221util/PNG.h"
#include "cs221util/HSLAPixel.h"
#include "stats.h"
#include "twoDtree.h"

using namespace std;
using namespace cs221util;

TEST_CASE("twoDtree::Prune Nature1","[weight=1][part=twoDtree]"){
    PNG img;
    // 16 k huge ass image
    //img.readFromFile("images/nature1.png");
    img.readFromFile("images/rap6.png");
    // big image(weird 4k ish or something;)
    // img.readFromFile("images/ff2.png");
    // 1280x720
    //img.readFromFile("images/colors1.png");
    // 1920x1080
    //img.readFromFile("images/aenami.png");
    // 2560x1440
    //img.readFromFile("images/candh.png");
    std::cout << "read image" << '\n';
    std::cout << "width x height : "<<img.width() <<" x " << img.height() << '\n';

    twoDtree t1(img);
    std::cout << "generated tree" << '\n';

    PNG prePrune = t1.render();
    std::cout << "rendered pre prune" << '\n';

    t1.prune(.05);
    std::cout << "pruned" << '\n';
    PNG result = t1.render();
    std::cout << "rendered pruned" << '\n';
    result.writeToFile("images/rap6-pruned.png");
    std::cout << "wrote pruned image" << '\n';

    REQUIRE(true);
    std::cout << "Finished pruning" << '\n';
}
