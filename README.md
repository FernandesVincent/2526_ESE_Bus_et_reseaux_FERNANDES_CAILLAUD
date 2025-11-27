# 2526_ESE_Bus_et_reseaux_FERNANDES_CAILLAUD
TP de bus et réseaux

## TP1 - Bus I2C

### 2.1 Capteurs BMP280
[datasheet](https://cdn-shop.adafruit.com/datasheets/BST-BMP280-DS001-11.pdf)

1. Adresses I2C : 0x77 ou 0x76, en fonction de si l'on connecte SDO à GND (0x76) ou à V_DDIO (0x77)
    Les adresses I2C ne peuvent pas être laissées flotantes.
    On va utiliser l'adresse 0x77 car c'est cette dernière qui est écrite sur la board du BMP280.

2. Le register : "id" est à l'adresse 0xD0 et contient la valeur du numéro d'identification de la puce qui est 0x58.  

3. On peut mettre le capteur en mode normal en modifiant les deux derniers bits du registre "ctrl_meas", ie mode[1:0]. Si les deux bits sont à 0b11 on a le mode normal.

4. L'étalonnage est fait grâce aux registres de calibrations "calib25" à "calib00" des adresses 0xA1 à 0x88.

5. Pour gérer la température, on utilise 3 registres : "0xFA", "0xFB" et "0xFC".  
    "0xFA" contient la partie MSB de la mesure de température (ut[19:12])  
    "0xFB" contient la partie LSB de la mesure (ut[11:4])  
    "0xFC" contient la partie XLSB de la mesure de température (ut[3:0]). Son contenu dépend aussi de la résolution utilisée pour la pression.

6. Pour gérer la pression, on utilise aussi 3 registres : "0xF7", "0xF8" et "0xF9".  
    "0xF7" contient la partie MSB de la mesure de pression (ut[19:12])  
    "0xF8" contient la partie LSB de la mesure (ut[11:4])  
    "0xF9" contient la partie XLSB de la mesure de pression (ut[3:0]). Son contenu dépend aussi de la résolution utilisée pour la température.

7. Le code pour le calcul de la température et de la pression en format 32 bits est le suivant :
    
    Le "data type" BMP280_S32_t correspond à un entier signé de 32 bits,  
    Le "data type" BMP280_U32_t correspond à un entier non signé de 32 bits,
    
    ```c

    // Returns temperature in DegC, resolution is 0.01 DegC. Output value of “5123” equals 51.23 DegC.
    // t_fine carries fine temperature as global value
    BMP280_S32_t t_fine;
    BMP280_S32_t bmp280_compensate_T_int32(BMP280_S32_t adc_T)
    {
        BMP280_S32_t var1, var2, T;
        var1 = ((((adc_T>>3) - ((BMP280_S32_t)dig_T1<<1))) * ((BMP280_S32_t)dig_T2)) >> 11;
        var2 = (((((adc_T>>4) - ((BMP280_S32_t)dig_T1)) * ((adc_T>>4) - ((BMP280_S32_t)dig_T1))) >> 12) *
                ((BMP280_S32_t)dig_T3)) >> 14;
        t_fine = var1 + var2;
        T = (t_fine * 5 + 128) >> 8;
        return T;
    }

    // Returns pressure in Pa as unsigned 32 bit integer in Q24.8 format (24 integer bits and 8 fractional bits).
    // Output value of “24674867” represents 24674867/256 = 96386.2 Pa = 963.862 hPa
    BMP280_U32_t bmp280_compensate_P_int64(BMP280_S32_t adc_P)
    {
        BMP280_S32_t var1, var2;
        BMP280_U32_t p;
        var1 = (((BMP280_S32_t)t_fine)>>1) - (BMP280_S32_t)64000;
        var2 = (((var1>>2) * (var1>>2)) >> 11 ) * ((BMP280_S32_t)dig_P6);
        var2 = var2 + ((var1*(BMP280_S32_t)dig_P5)<<1);
        var2 = (var2>>2)+(((BMP280_S32_t)dig_P4)<<16);
        var1 = (((dig_P3 * ((var1>>2) * (var1>>2)) >> 13 )) >> 3) + ((((BMP280_S32_t)dig_P2) * var1)>>1)>>18;
        var1 = ((((32768+var1))*((BMP280_S32_t)dig_P1))>>15);

        if (var1 == 0)
        {
            return 0; // avoid exception caused by division by zero
        }

        p = (((BMP280_U32_t)(((BMP280_S32_t)1048576)-adc_P)-(var2>>12)))*3125;

        if (p < 0x80000000)
        {
            p = (p << 1) / ((BMP280_U32_t)var1);
        }
        else
        {
            p = (p / (BMP280_U32_t)var1) * 2;
        }

        var1 = (((BMP280_S32_t)dig_P9) * (((BMP280_S32_t)((p>>3) * (p>>3)))>>13))>>12;
        var2 = (((BMP280_S32_t)(p>>2)) * ((BMP280_S32_t)dig_P8))>>13;
        p = (BMP280_U32_t)((BMP280_S32_t)p + ((var1 + var2 + dig_P7) >> 4));

        return p;  
    }

    ```
### 2.3 Communication I2C

_BMP280 transmit Id register_
![](/photos/BMP280_transmit_IDreg.png)
On retrouve bien le 0xEE qui correspond au 0x77 qui a été décalé. On aussi bien l'ID du BMP280 qui est transmise. 

On veut la configuration suivante: mode normal, Pressure oversampling x16, Temperature oversampling x2.
On a donc bits 7,6,5 pour la température, 4,3,2 pour la pression et 1,0 pour le mode ce qui donne :
        -010 pour temp
        -101 pour press
        -11 pour le mode normal
Donc le registre "ctrl_meas" prend la valeur 0b01010111 et en hexadécimal : **0x57**


### 2.4 Interfacage de l'acceléromètre

## TP2 - Interfacage STM32 - Raspberry

[Raspberry Pi Imager](https://www.raspberrypi.com/software/)

On utilise l'image "Raspberry Pi OS (32-bit) Lite".  

_Connection de la Raspberry Pi 0_
![](/photos/Raspi_ssh.png)  

La Raspberry a obtenue son adresse IP grace au routeur lorsque cette dernière c'est connectée au wifi pour la première fois.  
Dans notre cas, c'est 192.168.4.218  

On installe ensuite minicom : sudo apt install minicom.  
On doit aussi utiliser la commande : minicom -D /dev/ttyS0 car la commande minicom -D /dev/ttyAMA0 ne fonctionne pas.

Le llopback fonctionne bien lorsque les pins Tx et Rx sont connectés.
![](/photos/minicom_init.png)  

