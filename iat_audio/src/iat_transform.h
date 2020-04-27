/*
* 语音听写(iFly Auto Transform)技术能够实时地将语音转换成对应的文字。
*/

#ifndef IAT_TRANSFORM_H
#define IAT_TRANSFORM_H


extern const char* iat_begin_session(const char* session_begin_params, int* error);
extern void iat_end_session(const char* session_id);

extern int iat_transform(const char* session_begin_params, char* p_pcm, int pcm_size, char* ret, int* length);
extern int iat_login(void);
extern int iat_logout(void);

#endif
