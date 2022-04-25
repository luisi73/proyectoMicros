#ifndef menu_h
#define menu_h

#ifdef __cplusplus
extern "C"
{
#endif

    void menuIntro(void);
    void selectOption(char s[]);
    void mostarUsuarios(void);
    void changePin(char s[]);
    void selectUser(char s[]);
    void modifyPin(void);
    int charToInt(char c);

#ifdef __cplusplus
}
#endif

#endif /* menu_h */