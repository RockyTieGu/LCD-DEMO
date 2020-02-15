#ifndef _AVI_PLAYER_H_
#define _AVI_PLAYER_H_


/*-------------------------------------------------------------------------*
 *  MFL error code table                                                   *
 *-------------------------------------------------------------------------*/
/* general errors */
#define MFL_ERR_NO_MEMORY		0xFFFF8000	/* no memory */
#define MFL_ERR_HARDWARE		0xFFFF8002	/* hardware general error */
#define MFL_ERR_NO_CALLBACK		0xFFFF8004	/* must provide callback function */
#define MFL_ERR_AU_UNSUPPORT	0xFFFF8006	/* not supported audio type */
#define MFL_ERR_VID_UNSUPPORT	0xFFFF8008	/* not supported video type */
#define MFL_ERR_OP_UNSUPPORT	0xFFFF800C	/* unsupported operation */
#define MFL_ERR_PREV_UNSUPPORT	0xFFFF800E	/* preview of this media type was not supported or not enabled */
#define MFL_ERR_FUN_USAGE		0xFFFF8010	/* incorrect function call parameter */
#define MFL_ERR_RESOURCE_MEM	0xFFFF8012	/* memory is not enough to play/record a media file */
/* stream I/O errors */
#define MFL_ERR_FILE_OPEN		0xFFFF8020	/* cannot open file */
#define MFL_ERR_FILE_TEMP		0xFFFF8022	/* temporary file access failure */
#define MFL_ERR_STREAM_IO		0xFFFF8024	/* stream access error */
#define MFL_ERR_STREAM_INIT		0xFFFF8026	/* stream was not opened */
#define MFL_ERR_STREAM_EOF		0xFFFF8028	/* encounter EOF of file */
#define MFL_ERR_STREAM_SEEK		0xFFFF802A	/* stream seek error */
#define MFL_ERR_STREAM_TYPE		0xFFFF802C	/* incorrect stream type */
#define MFL_ERR_STREAM_METHOD	0xFFFF8030	/* missing stream method */
#define MFL_ERR_STREAM_MEMOUT	0xFFFF8032	/* recorded data has been over the application provided memory buffer */
#define MFL_INVALID_BITSTREAM	0xFFFF8034	/* invalid audio/video bitstream format */
/* AVI errors */
#define MFL_ERR_AVI_FILE		0xFFFF8080	/* Invalid AVI file format */
#define MFL_ERR_AVI_VID_CODEC	0xFFFF8081	/* AVI unsupported video codec type */
#define MFL_ERR_AVI_AU_CODEC	0xFFFF8082  /* AVI unsupported audio codec type */
#define MFL_ERR_AVI_CANNOT_SEEK	0xFFFF8083  /* The AVI file is not fast-seekable */
#define MFL_ERR_AVI_SIZE        0xFFFF8080  /* Exceed estimated size */
/* MP3 errors */
#define MFL_ERR_MP3_FORMAT		0xFFFF80D0	/* incorrect MP3 frame format */
#define MFL_ERR_MP3_DECODE		0xFFFF80D2	/* MP3 decode error */
/* hardware engine errors */
#define MFL_ERR_HW_NOT_READY	0xFFFF8100	/* the picture is the same as the last one */
#define MFL_ERR_SHORT_BUFF		0xFFFF8104	/* buffer size is not enough */
#define MFL_ERR_VID_DEC_ERR		0xFFFF8106	/* video decode error */
#define MFL_ERR_VID_DEC_BUSY	0xFFFF8108	/* video decoder is busy */
#define MFL_ERR_VID_ENC_ERR		0xFFFF810A	/* video encode error */
/* other errors */
#define MFL_ERR_UNKNOWN_MEDIA	0xFFFF81E2	/* unknow media type */


typedef enum jv_mode_e
{
	DIRECT_RGB555,				// JPEG output RGB555 directly to VPOST RGB555 frame buffer
	DIRECT_RGB565,				// JPEG output RGB565 directly to VPOST RGB565 frame buffer
	DIRECT_RGB888,				// JPEG output RGB888 directly to VPOST RGB888 frame buffer
	DIRECT_YUV422				// JPEG output YUV422 directly to VPOST YUV422 frame buffer	
			
	//SW_YUV422_TO_RGB565,		// JPEG output packet YUV422, software convert to RGB565 for VPOST
	//OVERLAY_YUV422_RGB555		// JPEG output packet YUV422 buffer to overlay on a RGB555 buffer
	                            //   finally be output to VPOST YUV buffer
}  JV_MODE_E;


typedef enum au_type_e
{
	AU_CODEC_UNKNOWN,
	AU_CODEC_PCM,
	AU_CODEC_IMA_ADPCM,
	AU_CODEC_MP3
}  AU_TYPE_E;


typedef struct avi_info_t
{
	UINT32		uMovieLength;		/* in 1/100 seconds */
	UINT32		uPlayCurTimePos;	/* for playback, the play time position, in 1/100 seconds */
	
	/* audio */
	AU_TYPE_E   eAuCodec;         
	INT			nAuPlayChnNum;		/* 1:Mono, 2:Stero */
	INT 		nAuPlaySRate;		/* audio playback sampling rate */

	/* video */
	UINT32		uVideoFrameRate;	/* only available in MP4/3GP/ASF/AVI files */
	UINT16		usImageWidth;
	UINT16		usImageHeight;
	UINT32		uVidTotalFrames;	/* For playback, it's the total number of video frames. For recording, it's the currently recorded frame number. */
	UINT32		uVidFramesPlayed;	/* Indicate how many video frames have been played */
	UINT32		uVidFramesSkipped;	/* For audio/video sync, some video frames may be dropped. */
}	AVI_INFO_T;


typedef VOID (AVI_CB)(AVI_INFO_T *);

extern void aviStopPlayFile(void);
extern int aviPlayFile(char *suFileName, int x, int y, JV_MODE_E mode, AVI_CB *cb);
extern int aviGetFileInfo(char *suFileName, AVI_INFO_T *ptAviInfo);
extern void aviSetPlayVolume(int vol);
extern void aviSetRightChannelVolume(int vol);
extern void aviSetLeftChannelVolume(int vol);
extern char * mfl_get_last_buf(void);


#endif   // _AVI_PLAYER_H_
