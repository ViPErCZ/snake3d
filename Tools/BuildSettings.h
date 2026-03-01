#ifndef SNAKE3_BUILDSETTINGS_H
#define SNAKE3_BUILDSETTINGS_H

namespace Build {
#ifdef IS_DEBUG
    inline constexpr bool isDebug = true;
#else
    inline constexpr bool isDebug = false;
#endif
}

#endif //SNAKE3_BUILDSETTINGS_H