#pragma once

// bug problem of BOOST: http://stackoverflow.com/questions/15634114/c#17988317
// #define BOOST_NO_CXX11_SCOPED_ENUMS
// #include <boost/filesystem.hpp>
// #undef BOOST_NO_CXX11_SCOPED_ENUMS

#include <string>
#include <filesystem>
namespace fs = std::filesystem;


#define NS_FILESYSTEM_BEGIN namespace filesystem{
#define NS_FILESYSTEM_END }

NS_FILESYSTEM_BEGIN

std::vector<std::string> folder_list(const fs::path &folder_path);
std::vector<std::string> file_list(const fs::path &folder_path);
void ensure_folder(const fs::path &filepath);
bool exists(const fs::path &filepath);
std::string read_file(const fs::path &filepath);
void write_file(const fs::path &filepath, const std::string &text);
void copy_file(const fs::path &from,const fs::path &to);
void copy_folder(const fs::path &from,const fs::path &to,const bool is_merged=true);

NS_FILESYSTEM_END
