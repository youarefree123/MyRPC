#include "log.h"

int main() {
    set_log(PutType::File, LogType::Info);
    INFO( "Hello" );
}