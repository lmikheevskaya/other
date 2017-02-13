#include "SDL.h"

extern "C"
{
	#include <libavcodec/avcodec.h>
	#include <libavformat/avformat.h>
	#include <libswscale/swscale.h>
	#include <libavutil/avconfig.h>

	#include <libavformat/avio.h>
	#include <libavutil/dict.h>
	#include <libavutil/frame.h>
}

# pragma comment (lib, "avformat.lib")
# pragma comment (lib, "avcodec.lib")
# pragma comment (lib, "avutil.lib")
# pragma comment (lib, "swscale.lib")


int main( int argc, char* args[] )
{
	// Register all available file formats and codecs
	av_register_all();

    	//Start SDL
    	int err = SDL_Init( SDL_INIT_EVERYTHING );
    
	if(err < 0){
		return -1;
	}

	// Open video file
	const char* filename = "C:\\test.avi";
	AVFormatContext* format_context = NULL;
	err = avformat_open_input(&format_context, filename, NULL, NULL);
	if (err < 0) {
		return -1;
	}

	// Retrieve stream information
    	err = avformat_find_stream_info(format_context, NULL);
    	if (err < 0) {
       	 	return -1;
    	}

	// Dump information about file onto standard error
    	av_dump_format(format_context, 0, filename, 0);

	// Find the first video stream
	int video_stream;
    	for (video_stream = 0; video_stream < format_context->nb_streams; ++video_stream) {
		if (format_context->streams[video_stream]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
			break;
		}
	}
	if (video_stream == format_context->nb_streams) {
       	 	return -1;
    	}

    	// Get a pointer to the codec context for the video stream   
    	AVCodecContext* codec_context = format_context->streams[video_stream]->codec;

	// Find the decoder for the video stream
    	AVCodec* codec = avcodec_find_decoder(codec_context->codec_id);

	if(!codec)
		return -1;

    	err = avcodec_open2(codec_context, codec, NULL);
    	if (err < 0) {
       	 	return -1;
    	}
       
        SDL_Surface* screen = SDL_SetVideoMode(codec_context->width, codec_context->height, 0, 0);
        if (!screen) {
                return -1;
        }
       
        SDL_Overlay* bmp = SDL_CreateYUVOverlay(codec_context->width, codec_context->height, SDL_YV12_OVERLAY, screen);
 
	struct SwsContext* img_convert_context = nullptr;
        img_convert_context = sws_getContext(codec_context->width, codec_context->height,
                              codec_context->pix_fmt,
                              codec_context->width, codec_context->height,
                              AV_PIX_FMT_YUV420P, SWS_BICUBIC,
                              NULL, NULL, NULL);

        if (img_convert_context == NULL) {
                return -1;
        }
 
	// Allocate video frame
	AVFrame* frame = av_frame_alloc();
        AVPacket packet;
        while (av_read_frame(format_context, &packet) >= 0) {
                if (packet.stream_index == video_stream) {
                        // Video stream packet
                        int frame_finished;
                        avcodec_decode_video2(codec_context, frame, &frame_finished, &packet);
                       
                        if (frame_finished) {
                                SDL_LockYUVOverlay(bmp);
                               
                                // Convert frame to YV12 pixel format for display in SDL overlay
                               
                                AVPicture pict;
                                pict.data[0] = bmp->pixels[0];
                                pict.data[1] = bmp->pixels[2];  // it's because YV12
                                pict.data[2] = bmp->pixels[1];
                               
                                pict.linesize[0] = bmp->pitches[0];
                                pict.linesize[1] = bmp->pitches[2];
                                pict.linesize[2] = bmp->pitches[1];
                               
                                sws_scale(img_convert_context,
                                                        frame->data, frame->linesize,
                                                        0, codec_context->height,
                                                        pict.data, pict.linesize);
                               
                                SDL_UnlockYUVOverlay(bmp);
                               
                                SDL_Rect rect;
                                rect.x = 0;
                                rect.y = 0;
                                rect.w = codec_context->width;
                                rect.h = codec_context->height;
                                SDL_DisplayYUVOverlay(bmp, &rect);
                        }
                }
               
                // Free the packet that was allocated by av_read_frame
                av_free_packet(&packet);
       
                // Handling SDL events there
                SDL_Event event;
                if (SDL_PollEvent(&event)) {
                        if (event.type == SDL_QUIT) {
                                break;
                        }
                }
        }
       
	sws_freeContext(img_convert_context);
       
	// Free the YUV frame
	av_free(frame);
    
	// Close the codec
	avcodec_close(codec_context);
       
	// Close the video file
	avformat_close_input(&format_context);
	
    //Quit SDL
    SDL_Quit();  
}
