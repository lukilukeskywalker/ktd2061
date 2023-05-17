/*
*   Library for the KTD2061 32 Channel Led Driver.
*   Version: 0.1
*   Author: lukilukeskywaker
*
*
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
*/
#ifndef __KTD2061_H__
#define __KTD2061_H__

typedef struct{
    int32_t (* read)(void *handle, uint8_t reg, void *in_data, size_t in_size);
    int32_t (* write)(void *handle, uint8_t reg, const void *out_data, size_t out_size);
    void *handle;
}ktd2061_t;
typedef struct{
    uint8_t R;
    uint8_t G;
    uint8_t B;
}RgbColor_t;

/* Delete!  Won't be necesary, if the thing I am trying out works out
// Library to Interface. The Interface should be able to recognize this commands, for doing the task that the library expects
#define KTD2061_INTERFACE_MSG_BYTE_INIT                 20  // Send once during the init phase (I guess to set up I2C emulator or hardware)
#define KTD2061_INTERFACE_MSG_BYTE_SEND_CMD             21  // Deprecated?
#define KTD2061_INTERFACE_MSG_BYTE_SEND_ARG             22  // Deprecated?
#define KTD2061_INTERFACE_MSG_BYTE_SEND                 23  //Send one or more bytes, located at arg_ptr, arg_int contains the number of bytes.
#define KTD2061_INTERFACE_MSG_BYTE_START_TRANSFER       24  //Select device 
#define KTD2061_INTERFACE_MSG_BYTE_END_TRANSFER         25  //Unselect device
#define KTD2061_INTERFACE_MSG_BYTE_SET_DC               32  //Set the level of the data/command pin. arg_int contains expected output level.
    // The reason for this strange numbers is because the Idea is based on olikraus u8x8 library, that has the same kind of flexibility.
    // In this library there are already some implemented software and hardware callbacks for other arquitectures. It would be nice, 
    // that this callbacks worked just out of the box with this library. No need to implement the same stuff over and over again by dif people

*/

/*------------------------------------------------DEVICE SPECIFIC------------------------------------------*/
// DEVICE SPECIFIC DEFINES
#define KTD2061_I2C_BASE_MODULE_ADDRESS     0x68
#define KTD2061_I2C_

// Register Addresses from datasheet
#define KTD2061_REG_ID                      0x00
#define KTD2061_REG_MONITOR                 0x01
#define KTD2061_REG_CONTROL                 0x02
#define KTD2061_REG_IRED0                   0x03
#define KTD2061_REG_IGRN0                   0x04
#define KTD2061_REG_IBLU0                   0x05
#define KTD2061_REG_IRED1                   0x06
#define KTD2061_REG_IGRN1                   0x07
#define KTD2061_REG_IBLU1                   0x08
#define KTD2061_REG_ISELA12                 0x09
#define KTD2061_REG_ISELA34                 0x0A
#define KTD2061_REG_ISELB12                 0x0B
#define KTD2061_REG_ISELB34                 0x0C
#define KTD2061_REG_ISELC12                 0x0D
#define KTD2061_REG_ISELC34                 0x0E

/*--------------------------- Device functionality --------------------------*/
// CONTROL REGISTER
#define KTD2061_CONTROL_RESET               (0x03 << 6)
#define KTD2061_CONTROL_NORMAL_M            (0x02 << 6)
#define KTD2061_CONTROL_NIGHT_M             (0x01 << 6)
#define KTD2061_CONTROL_OFF_M               (0x00 << 6)

#define KTD2061_CONTROL_BE_EN               0x20    //Enable BrightExtend

#define KTD2061_CONTROL_CE_TEMP_90          0x18    //Throttle Luminosity to control temperature of chip
#define KTD2061_CONTROL_CE_TEMP_105         0x10
#define KTD2061_CONTROL_CE_TEMP_120         0x08
#define KTD2061_CONTROL_CE_TEMP_135         0x00

#define KTD2061_CONTROL_FADE_RATE_31mS      0x00    //Fade rate of leds
#define KTD2061_CONTROL_FADE_RATE_63mS      0x01
#define KTD2061_CONTROL_FADE_RATE_125mS     0x02
#define KTD2061_CONTROL_FADE_RATE_250mS     0x03
#define KTD2061_CONTROL_FADE_RATE_500mS     0x04   
#define KTD2061_CONTROL_FADE_RATE_1S        0x05
#define KTD2061_CONTROL_FADE_RATE_2S        0x06
#define KTD2061_CONTROL_FADE_RATE_4S        0x07

// CHANNEL SELECTION
// To select a specific pin, you take the enable number. pass it through the macro of the channel
#define EN1                                0x8
#define EN2                                0x4
#define EN3                                0x2
#define EN4                                0x1
#define ENA(x)                             ((x) << 8)        
#define ENB(x)                             ((x) << 4)    
#define ENC(x)                             ((x))        
// COLOR SELECTION
#define SETTING_R                           ((x) << 2)
#define SETTING_G                           ((x) << 1)
#define SETTING_B                           ((x) << 0)
#define C_REG_0                              0
#define C_REG_1                              1




typedef enum{      
    NORMAL = KTD2061_CONTROL_NORMAL_M,
    NIGHT  = KTD2061_CONTROL_NIGHT_M,
    OFF = KTD2061_CONTROL_OFF_M,
    RESET = KTD2061_CONTROL_RESET,
    } ktd_mode_t;
typedef enum{      
    TEMP_90 = KTD2061_CONTROL_CE_TEMP_90,
    TEMP_105 = KTD2061_CONTROL_CE_TEMP_105,
    TEMP_120 = KTD2061_CONTROL_CE_TEMP_120,
    TEMP_135 = KTD2061_CONTROL_CE_TEMP_135,
    } ktd_temp_t;
     
void ktd2061_init(const ktd2061_t *ktd);
void ktd2061_mode(const ktd2061_t *ktd, ktd_mode_t mode, uint8_t fade);
void ktd2061_Temp_Setting(const ktd2061_t *ktd, ktd_temp_t temperature);
void ktd2061_Bright_Extend_Setting(const ktd2061_t *ktd, bool enable);
void ktd2061_write_Color0(const ktd2061_t *ktd, RgbColor_t *Color);
void ktd2061_write_Color1(const ktd2061_t *ktd, RgbColor_t *Color);
void ktd2061_setChannelOn(const ktd2061_t *ktd, uint16_t channels);
void ktd2061_setChannelOff(const ktd2061_t *ktd, uint16_t channels);
void ktd2061_selectColorRegister(const ktd2061_t *ktd, uint16_t channels, uint8_t setting);
#endif