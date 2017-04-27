#include <log4cpp/Category.hh>
#include <log4cpp/CategoryStream.hh>
#include <log4cpp/Priority.hh>

#define LOG(x) log4cpp::Category::getRoot() << log4cpp::Priority::x
