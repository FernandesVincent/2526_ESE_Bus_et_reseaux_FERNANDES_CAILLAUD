# 2526_ESE_Bus_et_reseaux_FERNANDES_CAILLAUD
TP de bus et réseaux

## TP1 - Bus I2C

### 2.1 Capteurs BMP280
[datasheet](https://cdn-shop.adafruit.com/datasheets/BST-BMP280-DS001-11.pdf)

1. Adresses I2C : 0x77 ou 0x76, en fonction de si l'on connecte SDO à GND (0x76) ou à V_DDIO (0x77)
    Les adresses I2C ne peuvent pas être laissées flotantes.

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

7. Le "data type" BMP280_S32_t correspond à un entier signé de 32 bits,
    Le "data type" BMP280_U32_t correspond à un entier non signé de 32 bits,
    
    ![](C:\Users\pcail\OneDrive\Documents\ENSEA\S9\Bus\Tp\Calcul_temp_press.jpg"Calcul de la température et de la pression")

