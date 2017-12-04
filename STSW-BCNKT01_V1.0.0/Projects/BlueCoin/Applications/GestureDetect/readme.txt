/**
  @page Gesture Detection application for BlueCoin
  
  @verbatim
  ******************** (C) COPYRIGHT 2017 STMicroelectronics *******************
  * @file    readme.txt  
  * @version V1.0.0
  * @date    26-Jun-2017
  * @brief   This application contains an example which shows how to configure 
  *          the BlueCoin to use Time of Flight sensors.
  ******************************************************************************
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  @endverbatim

@par Application Description 

The Gesture Recognition application exploits the Time of Flight sensors to provide an example of a very long and accurate range sensing, directional swipe and tap detection.

After resetting, the firmware:
- configures HAL and clocks
- initializes the LEDs and push buttons
- initializes the Time of Filght sensors
- initializes Gesture Detection Middleware
	- Tap detection
	- Directional swipe detection
- Main loop:
	- Push SW2 button to switch between two demo:
		- Range sensising with LEDs interface
		- Gesture Detection
	- Push PWR button to switch off the system (in case the battery is connected)


@par Hardware and Software environment

  - This application runs on BlueCoin platform (STEVAL-BCNCS01V1) plugged on CoinStation expansion (STEVAL-BCNST01V1).
    
@par How to use it? 

This package contains projects for 3 IDEs viz. IAR, �Vision and System Workbench. In order to make the 
program work, you must do the following:
 - WARNING: before opening the project with any toolchain be sure your folder
   installation path is not too in-depth since the toolchain may report errors
   after building.

For IAR:
 - Open IAR toolchain (this firmware has been successfully tested with
   Embedded Workbench V7.70).
 - Open the IAR project file EWARM\Project.eww
 - Rebuild all files and load your image into target memory.
 - Run the example.

For �Vision:
 - Open �Vision 5 toolchain (this firmware has been successfully tested with MDK-ARM Professional Version: 5.20).
 - Open the �Vision project file MDK-ARM\Project.uvprojx
 - Rebuild all files and load your image into target memory.
 - Run the example.

For System Workbench:
 - Open System Workbench for STM32.
 - Set the default workspace proposed by the IDE (please be sure that there are not spaces in the workspace path).
 - Press "File" -> "Import" -> "Existing Projects into Workspace"; press "Browse" in the "Select root directory" and choose the path where the System
   Workbench project is located (SW4STM32)
 - Rebuild all files and load your image into target memory.
 - Run the example.


 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
