\# Dual Voltage Monitor



\## Project Overview



Dual Voltage Monitor is an embedded monitoring system based on the

STM32F103C8T6 microcontroller.



The main purpose of this project is to measure, monitor and record

input and output voltages in a power control system.



The project is designed with a clean modular architecture to make the

software easy to understand, maintain and expand.





\---



\# Hardware Platform



\## Microcontroller



\- STM32F103C8T6 (Blue Pill)



\## Development Environment



\- STM32CubeIDE

\- STM32CubeMX

\- HAL Library



\## Main Hardware Components



\- 20x4 Character LCD with I2C interface

\- Push buttons for user interface

\- Buzzer for alarm notification

\- ADC voltage measurement inputs

\- UART communication interface





\---



\# Software Architecture



The project follows a modular embedded software architecture.



Main software layers:









Core

|

+-- App

| |

| +-- Application Modules

| |

| +-- Configuration Files

| |

| +-- Common Definitions

|

+-- HAL

|

+-- CMSIS











The application layer is separated into independent modules.



Each module should contain:





module\_name.h

module\_name.c





The header file contains:

\- Public definitions

\- Data types

\- Function declarations



The source file contains:

\- Function implementations

\- Internal logic





\---



\# Coding Rules



The following rules are applied during development:



1\. Each module has a single responsibility.

2\. No unnecessary files are created.

3\. Every file must have a clear purpose.

4\. Comments must explain both:

&#x20;  - What the code does

&#x20;  - Why the code exists

5\. Magic numbers should be avoided.

6\. Configuration values should be separated from logic.

7\. Code must remain readable and maintainable.





\---



\# Git Development Rules



The project uses Git for version control.



Important development milestones are committed separately.



Commit messages should describe the purpose of each change.





Example:





Add application status definitions



Refine common application data types



Remove unused project information module













\---



\# Project Development History



\## Version 1.0.0 - Initial Architecture



Completed:



\- STM32CubeMX project creation

\- Clock configuration

\- GPIO configuration

\- ADC1 configuration

\- I2C1 configuration

\- Application folder structure

\- Common application headers



Created files:







project\_config.h

app\_status.h

app\_defines.h

app\_types.h







\---



\# Future Development



Planned modules:



\- Button Manager

\- Buzzer Driver

\- ADC Voltage Measurement

\- LCD Interface

\- Menu System

\- UART Communication

\- Data Logging

\- Flash Configuration Storage





\---



\# Documentation



This project is developed together with a learning document.



The purpose of the documentation is to explain:



\- Design decisions

\- Software architecture

\- Embedded C concepts

\- Development process

\- Problems and solutions









