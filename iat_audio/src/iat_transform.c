/*
* 语音听写(iFly Auto Transform)技术能够实时地将语音转换成对应的文字。
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>

#include "qisr.h"
#include "msp_cmn.h"
#include "msp_errors.h"

#define	BUFFER_SIZE	4096
#define FRAME_LEN	640
#define HINTS_SIZE  100


#define SESSION_ERROR         -1
#define AUDIO_WRITE_ERROR     -2
#define TRANSFORM_ERROR       -3
#define BUF_NOT_ENOUGH_ERROR  -4


const char* iat_begin_session(const char* session_begin_params, int* error){

	int				errcode						=	MSP_SUCCESS ;
    const char*     session_id                  =   NULL;

    //听写不需要语法，第一个参数为NULL
	session_id = QISRSessionBegin(NULL, session_begin_params, &errcode);
	if (MSP_SUCCESS != errcode){
        *error = -1;
        return NULL;
	}
    *error = 0;
    return session_id;
}

void iat_end_session(const char* session_id){

    //hints为结束本次会话的原因描述，由用户自定义
	char hints[HINTS_SIZE] =	{0};
	QISRSessionEnd(session_id, hints);
}



int iat_transform(const char* session_id, char* p_pcm, int pcm_size, char* rec_result, int* rec_size){

	unsigned int	total_len					=	0;
	int				aud_stat					=	MSP_AUDIO_SAMPLE_CONTINUE ;		//音频状态
	int				ep_stat						=	MSP_EP_LOOKING_FOR_SPEECH;		//端点检测
	int				rec_stat					=	MSP_REC_STATUS_SUCCESS ;			//识别状态
	int				errcode						=	MSP_SUCCESS ;

	long			pcm_count					=	0;

	while (1)
	{
		//每次写入2s音频(16k，16bit)：1帧音频20ms，100帧=2s
        //16k采样率的16位音频，一帧的大小为640Byte
		unsigned int len = 100 * FRAME_LEN;
		int ret = 0;

		if (pcm_size < 2 * len)
			len = pcm_size;
		if (len <= 0)
			break;

		aud_stat = MSP_AUDIO_SAMPLE_CONTINUE;
		if (0 == pcm_count)
			aud_stat = MSP_AUDIO_SAMPLE_FIRST;

		ret = QISRAudioWrite(session_id, (const void *)&p_pcm[pcm_count], len, aud_stat, &ep_stat, &rec_stat);
		if (MSP_SUCCESS != ret){
            return AUDIO_WRITE_ERROR;
		}

		pcm_count += (long)len;
		pcm_size  -= (long)len;

		//已经有部分听写结果
		if (MSP_REC_STATUS_SUCCESS == rec_stat){
			const char *rslt = QISRGetResult(session_id, &rec_stat, 0, &errcode);
			if (MSP_SUCCESS != errcode){
                return TRANSFORM_ERROR;
			}

			if (NULL != rslt){
				unsigned int rslt_len = strlen(rslt);
				total_len += rslt_len;
				if (total_len >= BUFFER_SIZE){
                    return BUF_NOT_ENOUGH_ERROR;
				}
				strncat(rec_result, rslt, rslt_len);
			}
		}

		if (MSP_EP_AFTER_SPEECH == ep_stat)
			break;
		//usleep(10*1000); //模拟人说话时间间隙。200ms对应10帧的音频
	}

    // 最后一次写入
	errcode = QISRAudioWrite(session_id, NULL, 0, MSP_AUDIO_SAMPLE_LAST, &ep_stat, &rec_stat);
	if (MSP_SUCCESS != errcode){
        return AUDIO_WRITE_ERROR;
	}

	while (MSP_REC_STATUS_COMPLETE != rec_stat) {
		const char *rslt = QISRGetResult(session_id, &rec_stat, 0, &errcode);
		if (MSP_SUCCESS != errcode){
            return TRANSFORM_ERROR;
		}

		if (NULL != rslt){
			unsigned int rslt_len = strlen(rslt);
			total_len += rslt_len;
			if (total_len >= BUFFER_SIZE){
                return TRANSFORM_ERROR;
			}
			strncat(rec_result, rslt, rslt_len);
		}

		//防止频繁占用CPU
		usleep(5*1000);
	}

    *rec_size = strlen(rec_result);
    return 0;
}


int iat_login() {
    int			ret						=	MSP_SUCCESS;
	const char* login_params			=	"appid = 5e1c39a3, work_dir = .";
	ret = MSPLogin(NULL, NULL, login_params);
	if (MSP_SUCCESS != ret)
	{
        return -1;
	}

    return 0;
}

void iat_logout() {
	MSPLogout();
}
