# Receiving_datas_over_Wi-Fi_with_b-u585i-iot02a

## Project Objective:

This project aims to modify a configuration stored in the non volatile memory of the evaluation kit (b-u585i-iot02a from STMicroelectronics). To do it the kit connects automaticaly to a Wi-Fi network. The client who wants to edit the config has to connect to the ip-address of the kit, then the kit sends a web page to edit the config. The user can edit it on the webpage and return it to the kit by filled in the form of the webpage.


## Project importation:

To import this project on your own b-u585i-iot02a, you need to:

**1)** *Download this project*

**2)** *Open it with STM32CubeIDE software (use File -> Import-> Existing projects into workspace to import the project in your workspace)*

**3)** *Run the project*


## Project Workflow:

![image](https://github.com/user-attachments/assets/2a666c92-d2de-4469-be39-977c83068f09)



## Non Volatile Memory informations

The area used to store the config (called "CONFIG") is defined in the liker file (STM32U585AIIXQ_FLASH.ld). This memrory area is defined at the end of the memory stack. The memory stack of the MPU used is the following :

![image](https://github.com/user-attachments/assets/b50f19e9-757f-4d32-94d7-46c3208fc088)
