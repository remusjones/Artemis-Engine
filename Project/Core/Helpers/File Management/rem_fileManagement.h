//
// Created by Remus on 9/12/2022.
//


#define SHADER_SUB_DIRECTORY "\\Shaders\\"
#define SHADER_VERTEX_SUFFIX "_v"
#define SHADER_FRAGMENT_SUFFIX "_f"
#define SHADER_FILE_EXTENSION ".spv"
#define SHADER_DIRECTORY "\\Shaders\\"

#include <vector>
#include <string>

class rem_fileManagement {
public:
    static std::vector<char> GetShaderFileData(const std::string& filename);
    static std::string GetCurrentDirectory();
};


