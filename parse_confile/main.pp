#include <string>
#include <iostream>
#include "config_file.hpp"

int main(int argc, char *argv[])
{
    parse_config start;

    start.before_start_parsing(argc, argv);
    start.read_server();
}
