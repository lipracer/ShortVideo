//
//  main.cpp
//  Player
//
//  Created by 陈龙 on 2018/10/29.
//  Copyright © 2018 陈龙. All rights reserved.
//

#include <iostream>
#include <thread>
#include <chrono>
#include "src/LLWindow.h"
#include "src/LLDecodeVideo.h"
#include "src/LLTimer.hpp"
#include "src/LLVideoMgr.hpp"
#include "src/LLPool.hpp"


using namespace std;

#define SRC_FILE "../wangzhe.mp4"

int main(int argc, char* argv[])
{    
    LLFormatCtx fmt_ctx(SRC_FILE);
    fmt_ctx.init_info();
    
    LLWindow window(fmt_ctx.width(), fmt_ctx.height());
    
    LLDecodeVideo dv(fmt_ctx);
    thread th([&window, &dv](){
        dv.decode_video(&window);
    });
    th.detach();
    
    LLTimer::create_timer(chrono::milliseconds(5ll), [&window, &fmt_ctx](long long int tick)->void{
        
        VideoInfo *info;
        if((info = LLVideoMgr::get_instance().m_video_queue.front()))
        {
            if(tick > info->pts)
            {
                LLVideoMgr::get_instance().m_video_queue.pop_front(info);
                window.test(info->pixels);
                delete info;
            }

        }

    });
    
    window.init_window();
    return 0;
}

