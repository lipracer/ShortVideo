#include <cstdio>
#include <iostream>
#include <thread>
#include <chrono>
#include "src/DecodeVideo.h"

using namespace std;

DecodeVideo::DecodeVideo(FormatCtx& fmt_ctx) : m_fmt_ctx(fmt_ctx)
{
    
}



int DecodeVideo::decode_video(LLWindow* window)
{
    
    AVFrame *pFrame = av_frame_alloc();
    if (NULL == pFrame)
    {
        return -1;
    }
    
    AVFrame *pFrameRGB = av_frame_alloc();
    if (NULL == pFrameRGB)
    {
        return -1;
    }
    
    int numBytes = avpicture_get_size(AV_PIX_FMT_RGB24, m_fmt_ctx.width(),
                                  m_fmt_ctx.height());
    
    unsigned char *buffer = (uint8_t*)av_malloc(numBytes * sizeof(uint8_t));
    avpicture_fill((AVPicture*)pFrameRGB, buffer, AV_PIX_FMT_RGB24, m_fmt_ctx.width(), m_fmt_ctx.height());
    
    AVPacket packet;
    int frameFinished;
    
    window->m_video_start_tp = chrono::steady_clock::now();
    while (av_read_frame(m_fmt_ctx.m_pformat_ctx, &packet) >= 0)
    {
        if (m_fmt_ctx.m_video_strem == packet.stream_index)
        {
            avcodec_decode_video2(m_fmt_ctx.m_pcodec_ctx, pFrame, &frameFinished, &packet);
            if (frameFinished)
            {
                struct SwsContext *img_convert_ctx = NULL;
                img_convert_ctx = sws_getCachedContext(img_convert_ctx,
                                                       m_fmt_ctx.width(), m_fmt_ctx.height(),
                                                       m_fmt_ctx.m_pcodec_ctx->pix_fmt, m_fmt_ctx.width(), m_fmt_ctx.height(),
                                                       AV_PIX_FMT_BGR24, SWS_BICUBIC, NULL, NULL, NULL);
                
                if (!img_convert_ctx)
                {
                    exit(1);
                }
                
                sws_scale(img_convert_ctx, pFrame->data, pFrame->linesize, 0, m_fmt_ctx.height(),
                          pFrameRGB->data, pFrameRGB->linesize);
                
//                long long need_draw_pt = pFrame->pts * av_q2d(m_fmt_ctx.m_pstream->time_base) * 1000;
//                long long int duration_ = 0;
//                do
//                {
//                    auto cur_point = chrono::steady_clock::now();
//                    duration_ = (chrono::duration_cast<chrono::milliseconds>(cur_point - m_start_time)).count();
//                    //this_thread::sleep_for(chrono::milliseconds(5));
//
//                } while (duration_ < need_draw_pt);
                window->wait_times(pFrame->pts * av_q2d(m_fmt_ctx.m_pstream->time_base) * 1000);
                window->test((char*)pFrameRGB->data[0]);
                
            }
        }
        av_free_packet(&packet);
    }
    
    av_free(buffer);
    av_free(pFrameRGB);
    av_free(pFrame);
    return 0;

}

DecodeVideo::~DecodeVideo()
{
    
}
