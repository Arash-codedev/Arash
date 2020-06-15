#include "filesystem.hpp"

#include <fstream>
#include <sstream>

NS_FILESYSTEM_BEGIN

std::vector<std::string> folder_list(const fs::path &folder_path)
{
    std::vector<std::string> flist;
    fs::directory_iterator end_iter;
    if(fs::exists(folder_path)&&fs::is_directory(folder_path))
        for(fs::directory_iterator dir_iter(folder_path);dir_iter!=end_iter;++dir_iter)
            if(fs::is_directory(dir_iter->status()))
                flist.push_back(dir_iter->path().filename().string());
    return flist;
}

std::vector<std::string> file_list(const fs::path &folder_path)
{
    std::vector<std::string> flist;
    fs::directory_iterator end_iter;
    if(fs::exists(folder_path)&&fs::is_directory(folder_path))
        for(fs::directory_iterator dir_iter(folder_path);dir_iter!=end_iter;++dir_iter)
            if(!fs::is_directory(dir_iter->status()))
                flist.push_back(dir_iter->path().filename().string());
    return flist;
}

void ensure_folder(const fs::path &filepath)
{
    if(!fs::exists(filepath))
        fs::create_directories(filepath);
}

bool exists(const fs::path &filepath)
{
    return fs::exists(filepath);
}

std::string read_file(const fs::path &filepath)
{
    if(!fs::exists(filepath))
        throw std::runtime_error(std::string("file \"")+filepath.string()+"\" does not exist!");
    std::string data;
    std::ifstream in(filepath.string().c_str());
    std::getline(in, data, std::string::traits_type::to_char_type(
                      std::string::traits_type::eof()));
    return data;
}

void write_file(const fs::path &filepath, const std::string &text)
{
    std::ofstream out(filepath.string().c_str());
    out << text;
    out.close();
}

void copy_file(const fs::path &from,const fs::path &to)
{
    std::ifstream stream_from(from.string(), std::ios::binary);
    std::ofstream stream_to(to.string(), std::ios::binary);
    stream_to << stream_from.rdbuf();
}

void copy_folder(const fs::path &from,const fs::path &to,const bool is_merged/*=true*/)
{
    if(!fs::exists(from))
        throw std::runtime_error(std::string("Folder ")+from.string()+" does not exist.");
    if(!fs::is_directory(from) )
        throw std::runtime_error(std::string("")+from.string()+" is not a folder.");
    if(!is_merged && fs::exists(to))
        throw std::runtime_error(std::string("")+to.string()+" already does exist.");
    if(!fs::exists(to) && !fs::create_directory(to))
        throw std::runtime_error(std::string("Cannot create ")+to.string()+" .");

    for(fs::directory_iterator file_it(from);file_it != fs::directory_iterator(); ++file_it)
        if(fs::is_directory(file_it->path()))
            copy_folder(file_it->path(),to/file_it->path().filename(),is_merged);
        else
        {
            // there is a bug in fs 
            // causing this error: undefined reference to `fs::detail::copy_file ...
            // fs::copy_file(file_it->path(),to/file_it->path().filename());

            filesystem::copy_file(file_it->path(),to/file_it->path().filename());
        }

}

NS_FILESYSTEM_END

