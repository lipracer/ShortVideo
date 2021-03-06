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
#include "src/LLDecodeAudio.h"


using namespace std;

#define SRC_FILE "./wangzhe.mp4"

int main(int argc, char* argv[])
{

	LLFormatCtx fmt_ctx(SRC_FILE);
	fmt_ctx.init_info();

	LLWindow window(fmt_ctx.width(), fmt_ctx.height());



	//LLDecodeAudio da(fmt_ctx);
	//thread th_audio([&da]() {
	//	da.decode_audio();
	//});
	//th_audio.detach();

	LLDecodeVideo dv(fmt_ctx);
	thread th_video([&window, &dv]() {
		dv.decode_video(&window);
	});
	th_video.detach();

	LLTimer::create_timer(chrono::milliseconds((long long)(5)), [&window, &fmt_ctx](long long int tick)->void {

		VideoInfo *info;
		if ((info = LLVideoMgr::get_instance().m_video_queue.front()))
		{
			if (tick > info->pts)
			{
				LLVideoMgr::get_instance().m_video_queue.pop_front(info);
				window.test(info->pixels);
				delete info;
			}

		}

	});

	window.init_window();
	system("pause");
	return 0;
}

