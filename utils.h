#ifndef __UTILS_H__
#define __UTILS_H__

#include <string>

inline std::string directionToString(int direction) {
    return direction == 0 ? "OUTBOUND" : "INBOUND";
}

#endif // __UTILS_H__
