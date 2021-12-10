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

}  // namespace CENS

#endif  // CENS_LL1_HPP