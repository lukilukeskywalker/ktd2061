#include <stdbool.h>
#include <stdlib.h>
#include "KTD2061.h"

#define BITMASK_SET(x, mask, value) (((x) & ~(mask)) | ((value) & (mask)))
#define BITSHIFT_LEFT(x, n) ((x) = (x) << (n))

static inline void read_Register(const ktd2061_t *ktd, uint8_t reg, uint8_t *value){
    ktd->read(ktd->handle, reg, value, 1);
}
void ktd2061_init(const ktd2061_t *ktd){
    //This function sets for the first time the calback for I2C operations
    //ktd->write(ktd->handle, KTD2061_I2C_BASE_MODULE_ADDRESS, KTD2061_REG_CONTROL, KTD2061_CONTROL_RESET, 1);
    uint8_t value = KTD2061_CONTROL_RESET;
    ktd->write(ktd->handle, KTD2061_REG_CONTROL, &value, 1);

}
/*
* @param const ktd2061_t *ktd Struct pointer to configuration
* @param mode_t mode    Mode of Driver
* @param uint8_t fade   Fade time
*/
void ktd2061_mode(const ktd2061_t *ktd, ktd_mode_t mode, uint8_t fade){
    uint8_t *value = malloc(1);
    ktd->read(ktd->handle, KTD2061_REG_CONTROL, value, 1);
    *value = BITMASK_SET(*value, KTD2061_CONTROL_RESET | KTD2061_CONTROL_FADE_RATE_4S, mode | fade);
    ktd->write(ktd->handle, KTD2061_REG_CONTROL, value, 1);
    free(value);
}
void ktd2061_Temp_Setting(const ktd2061_t *ktd, ktd_temp_t temperature){
    uint8_t *value = malloc(1);
    ktd->read(ktd->handle, KTD2061_REG_CONTROL, value, 1);
    *value = BITMASK_SET(*value, KTD2061_CONTROL_CE_TEMP_90, temperature);
    ktd->write(ktd->handle, KTD2061_REG_CONTROL, value, 1);
    free(value);
}
void ktd2061_Bright_Extend_Setting(const ktd2061_t *ktd, bool enable){
    uint8_t *value = malloc(1);
    ktd->read(ktd->handle, KTD2061_REG_CONTROL, value, 1);
    if(enable){
        *value = BITMASK_SET(*value, KTD2061_CONTROL_BE_EN, KTD2061_CONTROL_BE_EN);
    }else{
        *value = BITMASK_SET(*value, KTD2061_CONTROL_BE_EN, 0);
    }
    ktd->write(ktd->handle, KTD2061_REG_CONTROL, value, 1);
    free(value);
}
void ktd2061_write_Color0(const ktd2061_t *ktd, RgbColor_t *Color){
    ktd->write(ktd->handle, KTD2061_REG_IRED0, Color, sizeof(RgbColor_t));
}
void ktd2061_write_Color1(const ktd2061_t *ktd, RgbColor_t *Color){
    ktd->write(ktd->handle, KTD2061_REG_IRED1, Color, sizeof(RgbColor_t));

}
void ktd2061_setChannelOn(const ktd2061_t *ktd, uint16_t channels){
    uint8_t *value = malloc(sizeof(uint8_t)*6); //i KNOW. uint8_t size is one
    ktd->read(ktd->handle, KTD2061_REG_ISELA12, value, 6);
    // Some times the best way is just not to think what you are doing. Just do it
    for(int i=0; i<6; i++){
        
        value[i] |= (0x80&(channels >> 4)) | (0x08&(channels >> 7)); 
        BITSHIFT_LEFT(channels, 2);
    }
    ktd->write(ktd->handle, KTD2061_REG_ISELA12, value, 6);
    free(value);
}
void ktd2061_setChannelOff(const ktd2061_t *ktd, uint16_t channels){
    uint8_t *value = malloc(sizeof(uint8_t)*6); //i KNOW. uint8_t size is one
    ktd->read(ktd->handle, KTD2061_REG_ISELA12, value, 6);
    // Some times the best way is just not to think what you are doing. Just do it
    for(int i=0; i<6; i++){
        value[i] &= ~((0x80&(channels >> 4)) | (0x08&(channels >> 7))); 
        BITSHIFT_LEFT(channels, 2);
    }
    ktd->write(ktd->handle, KTD2061_REG_ISELA12, value, 6);
    free(value);
}
void ktd2061_selectColorRegister(const ktd2061_t *ktd, uint16_t channels, uint8_t setting){
    /* You pass to channels the pins to be manipulated, and in setting a 3 bit setting that selects 
     * what register to select from the 2 colors
     */
    uint8_t *value = malloc(sizeof(uint8_t)*6);
    ktd->read(ktd->handle, KTD2061_REG_ISELA12, value, 6);
    //setting = BITMASK_SET(setting, 0x7, 0x0);   //Filtro para que no se pueda meter un bit que pueda joder la conf de on o off
    setting &= 0x7; 
    for(int i=0; i<6; i++){ //Compute which ones have to be changed
        value[i] =  (value[i] & 0x88) |
                    (0x70 &(((channels >> 11)&0x01)!= 0 ? (setting << 4) : value[i])) |    //Ahora si los bits no deben ser clereados contendran el valor original
                    (0x07 &(((channels >> 10)&0x01)!= 0 ? setting : value[i]));
        //So first we evaluate if the setting applies for this led or not, if it applies then we write the setting
        //value[i] = BITMASK_SET(value[i], ~pin_conf, pin_conf);  //Asi que ahora puedo clerear siempre, que voy a reescribir el valor que debe de tener
        BITSHIFT_LEFT(channels, 2);
    }
    ktd->write(ktd->handle, KTD2061_REG_ISELA12, value, 6);
    free(value);

    //OLD
    /*
    uint8_t *value = malloc(sizeof(uint8_t)*6);
    ktd->read(ktd->handle, KTD2061_REG_ISELA12, value, 6);
    for(int i=0; i<6; i++){ //Compute which ones have to be changed
        uint8_t pin_conf =  (0x70 & (((channels >> 11)&0x01)!= 0 ? (setting << 4) : 0)) |    
                            (0x07 & (((channels >> 10)&0x01)!= 0 ? setting : 0));
        //So first we evaluate if the setting applies for this led or not, if it applies then we write the setting
        value[i] = BITMASK_SET(value[i], pin_conf, pin_conf);
        BITSHIFT_LEFT(channels, 2);
    }
    ktd->write(ktd->handle, KTD2061_REG_ISELA12, value, 6);
    free(value);
    */
}







