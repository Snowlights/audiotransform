/*
* 语音听写(iFly Auto Transform)技术能够实时地将语音转换成对应的文字。
*/

#include "iat_transform.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
    iat_login();
	const char* session_begin_params	=	"sub = iat, domain = iat, language = zh_ch, accent = mandarin, sample_rate = 44100, result_type = plain, result_encoding = utf8";

	char res[4096] = {0};
    int len = 0;

    FILE*			f_pcm						=	NULL;
	char*			p_pcm						=	NULL;
	long			pcm_size					=	0;
	long			read_size					=	0;

    const char*     session_id                  =   NULL;
    int error = 0;


	f_pcm = fopen("wav/iflytek01.wav", "rb");
	if (NULL == f_pcm){
        return -1;
	}

	fseek(f_pcm, 0, SEEK_END);
	pcm_size = ftell(f_pcm);
	fseek(f_pcm, 0, SEEK_SET);

	p_pcm = (char *)malloc(pcm_size);
	if (NULL == p_pcm){
        return -2;
    }

	read_size = fread((void *)p_pcm, 1, pcm_size, f_pcm); //读取音频文件内容
	if (read_size != pcm_size){
        return -3;
    }

    printf("size: %ld\n", read_size);

    session_id = iat_begin_session(session_begin_params, &error);
    printf("session: %d, id: %s\n", error, session_id);
    error = iat_transform(session_id, p_pcm, pcm_size, res, &len);
    printf("error: %d\n", error);
    iat_end_session(session_id);

    iat_logout();
    printf("res: %s\n", res);
	return 0;
}
