# Receiving_datas_over_Wi-Fi_with_b-u585i-iot02a

## Project Objective:

This project aims to modify a configuration stored in the non volatile memory of the evaluation kit. To do it the kit connects automaticly to a Wi-Fi network using credentials entered in the code. When the kit is connected to the network, a green LED switchs on then the user has to connect to the ip-address of the kit. The kit sends a web page to edit the config using a form to fill in on the webpage.

If something went wrong, a red LED would switched on and value corresponding to the error is returned from the application fonction. 

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
