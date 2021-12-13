#ifndef CENS_LL1_HPP
#define CENS_LL1_HPP

#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <iterator>
#include <list>
#include <string>
#include <vector>

namespace CENS {

bool print_info = false;
bool print_matlab_graph = false;

const std::vector<std::string> CENS_keywords{
    "SendCurrent",   "RunSystem",      "neuron", "glia", "glutamate",
    "acetylcholine", "norepinephrine", "gaba",   "atp",  "adenosine",
    "inhibitory",    "excitatory",     "if",     "else", "for",
    "while"};

const std::vector<std::string> CENS_operators{
    "<<", ">>", "<", ">", "<=", ">=", "*", "+", "-", "=", "=="};

const std::vector<std::string> CENS_delimiters{"{", ",", "}", "(", ")", ";"};

const std::vector<std::vector<std::string>> gramaticaLL1{
    /*0*/ {"goal", "expr"},
    /*1*/ {"expr", "define", ";", "expr+"},
    /*2*/ {"expr", "innervate", ";", "expr+"},
    /*3*/ {"expr", "input", ";", "expr+"},
    /*4*/ {"expr", "start", ";"},
    /*5*/ {"expr+", "define", ";", "expr+"},
    /*6*/ {"expr+", "innervate", ";", "expr+"},
    /*7*/ {"expr+", "input", ";", "expr+"},
    /*8*/ {"expr+"},
    /*9*/ {"define", "neuron", "id", "(", "param_d", ")"},
    /*10*/ {"param_d", "inhibitory", "param_d+"},
    /*11*/ {"param_d", "excitatory", "param_d+"},
    /*12*/ {"param_d+", ",", "num"},
    /*13*/ {"param_d+"},
    /*14*/ {"innervate", "id", ">>", "id", "innervate+"},
    /*15*/ {"innervate+", ">>", "id", "innervate+"},
    /*16*/ {"innervate+"},
    /*17*/ {"input", "SendCurrent", "(", "id", "param_i", ")"},
    /*18*/ {"param_i", ",", "id", "param_i"},
    /*19*/ {"param_i"},
    /*20*/ {"start", "RunSystem"},
};

const std::vector<std::vector<std::string>> tablaLL1{
    {" ", ";", "neuron", "id", "(", ")", "inhibitory", "excitatory", ",", "num",
     ">>", "SendCurrent", "RunSystem", "$"},
    {"goal", " ", "0", "0", " ", " ", " ", " ", " ", " ", " ", "0", "0", " "},
    {"expr", " ", "1", "2", " ", " ", " ", " ", " ", " ", " ", "3", "4", " "},
    {"expr+", " ", "5", "6", " ", " ", " ", " ", " ", " ", " ", "7", " ", "8"},
    {"define", " ", "9", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " "},
    {"param_d", " ", " ", " ", " ", " ", "10", "11", " ", " ", " ", " ", " ",
     " "},
    {"param_d+", " ", " ", " ", " ", "13", " ", " ", "12", " ", " ", " ", " ",
     " "},
    {"innervate", " ", " ", "14", " ", " ", " ", " ", " ", " ", " ", " ", " ",
     " "},
    {"innervate+", "16", " ", " ", " ", " ", " ", " ", " ", " ", "15", " ", " ",
     " "},
    {"input", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", "17", " ", " "},
    {"param_i", " ", " ", " ", " ", "19", " ", " ", "18", " ", " ", " ", " ",
     " "},
    {"start", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", "20", " "},

};

std::string parte_1 = "C = 1; % uF/cm^2\n"
    "% constants; values based on Table 1\n"
    "E_Na = 115 + Vrest; % mV\n"
    "E_K = -6 + Vrest; %mV\n"
    "E_Leak = 10.6 + Vrest; % mV\n"
    "g_Na = 120; % mS/cm^2\n"
    "g_K = 36; % mS/cm^2\n"
    "g_Leak = 0.3; % mS/cm^2\n"
    "% Vector of timesteps\n"
    "t = [0:dt:totalTime];\n"
    "% Current input −− change this to see how different inputs affect the "
    "neuron\n"
    "I_current = ones(1,length(t))*0.0;\n"
    "I_current(50/dt:end) = 3; \n"
    "% Input of 3 microA/cm2 beginning at 50 ms and steady until end of time "
    "period.\n"
    "% initializing values\n"
    "V(1) = Vrest; % membrane potential is starting at its resting state\n"
    "% separate functions to get the alpha and beta values\n"
    "[alphaM, betaM] = m_equations(V(1), Vrest);\n"
    "[alphaN, betaN] = n_equations(V(1), Vrest);\n"
    "[alphaH, betaH] = h_equations(V(1), Vrest);\n"
    "% initializing gating variables to the asymptotic values when membrane "
    "potential\n"
    "% is set to the membrane resting value based on equation 13\n"
    "m(1) = (alphaM / (alphaM + betaM));\n"
    "n(1) = (alphaN / (alphaN + betaN));\n"
    "h(1) = (alphaH / (alphaH + betaH));\n"
    "% repeat for time determined in totalTime , by each dt\n"
    "for i = 1:length(t)\n"
    "    % calculate new alpha and beta based on last known membrane "
    "potenatial\n"
    "    [alphaN, betaN] = n_equations(V(i), Vrest);\n"
    "    [alphaM, betaM] = m_equations(V(i), Vrest);\n"
    "    [alphaH, betaH] = h_equations(V(i), Vrest);\n"
    "    % conductance variables − computed separately to show how this\n"
    "    % changes with membrane potential in one of the graphs\n"
    "    conductance_K(i) = g_K*(n(i)^4);\n"
    "    conductance_Na(i)=g_Na*(m(i)^3)*h(i);\n"
    "    % retrieving ionic currents\n"
    "    I_Na(i) = conductance_Na(i)*(V(i)-E_Na);\n"
    "    I_K(i) = conductance_K(i)*(V(i)-E_K);\n"
    "    I_Leak(i) = g_Leak*(V(i)-E_Leak);\n"
    "    % Calculating the input\n"
    "    Input = I_current(i) - (I_Na(i) + I_K(i) + I_Leak(i));\n"
    "    % Calculating the new membrane potential\n"
    "    V(i+1) = V(i) + Input* dt*(1/C);\n"
    "    % getting new values for the gating variables\n"
    "    m(i+1) = m(i) + (alphaM *(1-m(i)) - betaM * m(i))*dt;\n"
    "    n(i+1) = n(i) + (alphaN *(1-n(i)) - betaN * n(i))*dt;\n"
    "    h(i+1) = h(i) + (alphaH *(1-h(i)) - betaH * h(i))*dt;\n"
    "end\n";
    std::string graphic = "subplot(2,1,1)\n"
    "plot(t(45/dt:end),V(45/dt:end-1), 'LineWidth', 2)\n"
    "xlabel('Time (ms)')\n"
    "ylabel('Voltage (mV)')\n"
    "title('Action Potential')\n"
    "subplot(2,1,2)\n"
    "plot(t(45/dt:end),I_current(45/dt:end), 'r', 'LineWidth', 2)\n"
    "xlabel('Time (ms)')\n"
    "ylabel('Voltage (mV)')\n"
    "title('Input')\n";
    std::string parte_2 = "end\n% calculate alpha m and beta m based on Table 2\n"
    "function [alpha_m, beta_m] = m_equations(V, Vrest)\n"
    "alpha_m = (2.5-0.1*(V-Vrest))/(exp(2.5-0.1*(V-Vrest))-1);\n"
    "beta_m = 4*exp((Vrest-V)/18);\n"
    "end\n"
    "% calculate alpha n and beta n based on Table 2\n"
    "function [alpha_n, beta_n] = n_equations(V, Vrest)\n"
    "alpha_n = (0.1-0.01*(V-Vrest))/(exp(1-0.1*(V-Vrest))-1);\n"
    "beta_n = 0.125*exp((Vrest-V)/80);\n"
    "end\n"
    "% calculate alpha h and beta h based on Table 2\n"
    "function [alpha_h, beta_h] = h_equations(V, Vrest)\n"
    "alpha_h = 0.07*exp((Vrest-V)/20);\n"
    "beta_h = 1/(1+exp(3-0.1*(V-Vrest)));\n"
    "end";



}  // namespace CENS

#endif  // CENS_LL1_HPP