#ifndef INFINITYKINSTALLER_H_
#define INFINITYKINSTALLER_H_

#ifdef __cplusplus
extern "C" {
#endif

unsigned int getBaryon(void);
int getTrueModel(void);
int setHomePopup(int value);

void boot_infinity(void);

#define MODEL(x) (1 << x)
#define ALL_MODELS (0xFFFFFFFF)

#ifdef __cplusplus
}
#endif

#endif // INFINITYKINSTALLER_H_
