#ifndef RFC_MANAGER_H
#define RFC_MANAGER_H

#include <string>
#include <list>
#include <exception>


/* Manages local copies of RFCs */
class RFCManager {

private:
    std::string path;

    /* Verifies path is valid.
     * If it does not exist, calls create_path. */
    bool verify_path();

    /* Tries to create a directory at path. */
    bool create_path();

    /* Parses an RFC number from a filename */
    int parse_filename(std::string filename);

public:

    /* Create a new manager, using the dir at path to store and manage RFCs */
    RFCManager(std::string path);

    /* List all the RFCs at path */
    std::list<int> get_rfcs();

    /* Add an RFC to the dir at path */
    bool add_rfc(int number, std::string &content);

    /* Get the content of an RFC. Empty string if not found. */
    std::string get_content(int number);

};


class InvalidPathException : public std::exception {
    virtual const char* what() const noexcept;
};


class PathReadException : public std::exception {
    virtual const char* what() const noexcept;
};


class FilenameParseException : public std::exception {
    virtual const char* what() const noexcept;
};


class FileReadException : public std::exception {
    virtual const char* what() const noexcept;
};


#endif

