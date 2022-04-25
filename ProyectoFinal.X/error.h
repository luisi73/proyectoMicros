#ifndef error_h
#define error_h

#ifdef __cplusplus
extern "C"
{
#endif

    void setErrorCounter(int counter);
    void plusErrorCounter(int add_num);
    int getErrorCounter(void);

#ifdef __cplusplus
}
#endif

#endif
