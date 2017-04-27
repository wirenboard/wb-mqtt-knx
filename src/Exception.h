#include <string>
#ifndef EXCEPTION_H
#define EXCEPTION_H
class TKnxException : public std::exception {
public:
    TKnxException(std::string message);
    const char* what() const throw();
    ~TKnxException() throw();

private:
    std::string Message;
};
#endif // EXCEPTION_H
