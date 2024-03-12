#ifndef __BEEP_H__
#define __BEEP_H__

#ifdef __cplusplus
extern "C" {
#endif
int open_bee();
int beep_say(float x);
void close_bee();
#ifdef __cplusplus
}
#endif

#endif /* BEEP_H */