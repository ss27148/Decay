# Decay
Program for simulation of radioactive decay.

When the program is started, a radioactive isotope is randomly selected from the database. 
The chemical symbol of the selected isotope is displayed to the student. The objective is to find the half-life, 
decay constant, lifetime of a radioactive isotope. Then, using the found half-life, identify a specific isotope of a given element.

The program operates in one of two modes: `Half-Life` and `Decay Constant`.

In `Half-Life` mode, simulation continues until the number of decayed atoms reaches half of the initial number of atoms.

In `Decay Constant` mode, the simulation continues until the decay time reaches the value set by the user. 
The decay time can be set between 1 and 10 seconds using the spinner control. You can also enter an arbitrary value in the textbox; 
to set infinite time leave the textbox blank.

To start the simulation, press the `Start` button. During the simulation, the atom states are displayed in the viewport; 
the number of undecayed (remaining) and decayed atoms and the current time are displayed on the corresponding indicators.

To reset all atoms to active (undecayed) state, use the `Reset` button. It is not necessary to press the `Reset` button 
before each start of the simulation: the program performs reset automatically.

To determine the half-life, select `Half-Life` mode and run the simulation. At the end of the simulation 
(when half of all nuclei have decayed), the time displayed by the program will be the half-life. 
Repeat the experiment 10 times. Note that the half-life in each experiment will be slightly different due to 
the stochastic nature of the process. Find the average half-life (_T_<sub>1/2</sub>).

To determine the decay constant, set mode 2 and decay time to 1 s. Run the experiment and record the number of undecayed atoms 
_N_, as well as the natural logarithm of this value ln _N_. Repeat the experiment for decay times of 2, 3, ... 10 s. 
The slope of the dependence (ln _N_(t)) will be equal to the decay constant (_lambda_). Find the average lifetime (_tau_=1/_lambda_) 
and the half-life (_T_<sub>1/2</sub>=ln2/_lambda_).

To identify an isotope, open `Table of Isotopes` and enter the symbol of the element under study in the search bar. 
Then scroll down to the isotope that has the closest half-life to that determined from the simulation.

## Localization
The program has multilingual support. Translation into other languages can be done by creating additional resource-only DLLs. 
The package contains such DLL for Ukrainian language. The procedure how to create localized satellite DLL is described [here]( https://docs.microsoft.com/en-us/cpp/build/localized-resources-in-mfc-applications-satellite-dlls?view=msvc-170).
To switch language use command line parameter `/lang XXX`, where `XXX` is three letter code of language. 
For example, to set Ukrainian language launch program like this: `Decay.exe /lang UKR`.

## Building
To build the application a Visual Studio 2019 solition is provided.
