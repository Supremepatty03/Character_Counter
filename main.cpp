#include "application/ConsoleApplication.h"

#ifdef _WIN32
#include <fcntl.h>
#include <io.h>
#endif

int main()
{
#ifdef _WIN32
    _setmode(_fileno(stdin), _O_U16TEXT);
    _setmode(_fileno(stdout), _O_U16TEXT);
#endif

    ConsoleApplication application;
    application.Run();

    return 0;
}
