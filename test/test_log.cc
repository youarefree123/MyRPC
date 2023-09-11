#include "log.h"

int main() {
    LogInit(PutType::Console, LogType::Trace);
    

    for( int i = 0; i < 5; i++ ) {
        if( i == 0 ) {
            TRACE("trace{}",i);
            DEBUG("debug{}",i);
            INFO("info{}",i);
            WARN("warn{}",i);
            ERROR("error{}",i);
        } 

        if( i == 1 ) {
            LOG_LEVEL_DEBUG;
            TRACE("trace{}",i);
            DEBUG("debug{}",i);
            INFO("info{}",i);
            WARN("warn{}",i);
            ERROR("error{}",i);
        }

        if( i == 2 ) {
            LOG_LEVEL_WARN;
            TRACE("trace{}",i);
            DEBUG("debug{}",i);
            INFO("info{}",i);
            WARN("warn{}",i);
            ERROR("error{}",i);
        }
        if( i == 3 ) {
            LOG_LEVEL_ERROR;
            TRACE("trace{}",i);
            DEBUG("debug{}",i);
            INFO("info{}",i);
            WARN("warn{}",i);
            ERROR("error{}",i);
        }
        
        
        
    }
    CRITICAL( "crittcal{}" );
    INFO( "crittcal not active" );
  
  return 0;
}