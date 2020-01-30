#ifndef TOOLS_H
#define TOOLS_H

#include <vector>
#include <string>

class Tools
{
public:
    Tools() = delete;

    static std::vector<std::string> split(const std::string& buffer, char delimiter);

};

#endif // TOOLS_H
