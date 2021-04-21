#include "rfc_manager.hpp"

#include <list>
using std::list;
#include <string>
using std::string;
#include <iostream>
#include <regex>
#include <charconv>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>


bool RFCManager::verify_path() {
    struct stat stat_buf;
    if (stat(path.c_str(), &stat_buf) < 0) {
        std::cout << path << " does not exist" << std::endl;
        return create_path();
    } else if (!S_ISDIR(stat_buf.st_mode)) {
        std::cout << path << " is not a directory" << std::endl;
        return false;
    }
    return true;
}


bool RFCManager::create_path() {
    if (mkdir(path.c_str(), 0755) < 0) {
        std::cout << "Unable to create directory at " << path << std::endl;
        return false;
    }
    return true;
}


int RFCManager::parse_filename(string filename) {
    std::regex re("^rfc(\\d+).txt$");
    std::cmatch cm;

    std::regex_match(filename.c_str(), cm, re, std::regex_constants::match_default);
    if (cm.size() != 2)
        throw FilenameParseException();

    return std::stoi(cm[1]);
}


RFCManager::RFCManager(string path) {
    this->path = path;
    if (!verify_path())
        throw InvalidPathException();
}


list<int> RFCManager::get_rfcs() {

    list<int> l;

    DIR *dir = opendir(path.c_str());
    if (dir == nullptr)
        throw PathReadException();

    struct dirent *item;
    while ((item = readdir(dir)) != nullptr) {
        std::string item_path(item->d_name);
        if (item_path == std::string("..") || item_path == std::string("."))
            continue;
        l.push_back(parse_filename(item_path));
    }

    closedir(dir);
    return l;

}


bool RFCManager::add_rfc(int number, string &content) {
    std::stringstream filename;
    filename << "rfc" << number << ".txt";
    FILE *fp = fopen((path + "/" + filename.str()).c_str(), "w");
    if (fp == nullptr)
        return false;
    fprintf(fp, "%s", content.c_str());
    fclose(fp);
    return true;
}


string RFCManager::get_content(int number) {
    std::stringstream filename;
    filename << "rfc" << number << ".txt";
    FILE *fp = fopen((path + "/" + filename.str()).c_str(), "r");
    if (fp == nullptr)
        throw FileReadException();
    std::stringstream content;
    char c;
    while ((c = fgetc(fp)) != EOF)
        content << c;
    return content.str();
}


const char* InvalidPathException::what() const noexcept {
    return "path is invalid and could not be created";
}


const char* PathReadException::what() const noexcept {
    return "unable to open dir at path";
}


const char* FilenameParseException::what() const noexcept {
    return "unable to parse a filename";
}


const char* FileReadException::what() const noexcept {
    return "unable to read a file";
}

