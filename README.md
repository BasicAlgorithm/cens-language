# CENS language
CENS language (CEntral Nervous System Language) is a project aiming to be a programming language that simulates a neuron system environment. This is also a final project for the compiler course. This repository accepts pull requests.<br>
**Note:** This project is completed.

# Getting started
The following steps guide you to compile and run a demo code.

## 1. Compile CENS
On same directory of **main.cpp** execute:
```bash
  $ c++ -std=c++17 -O2 main.cpp -o cens.exe
  ```
## 2. Code your idea
You are free to code and always respect grammar (see language reference). There are 4 important parts that your code has to have.  There is a demo in the final part.

#### 2.1 Create neurons
A **neuron** keyword is imperative. Then, you have to choose a name and define if it shall be inhibitory or excitatory. i.e.

> neuron name_of_neuron1 (inhibitory);<br>
> neuron name_of_neuron2 (excitatory);

#### 2.2 Innervate neuron among them
A **>>** symbol  mean the left neuron innervate right neuron. The following example mean that my_neuron_1 innervate my_neuron_2. 

> my_neuron_1 >> my_neuron_2

And this that my_neuron_1 innervate my_neuron_2, and my_neuron_2 innervate my_neuron_3.

> my_neuron_1 >> my_neuron_2 >> > my_neuron_3
#### 2.3 Define which neurons you stimulate
You have to put as many neurons as parameters you need to be stimulated. i.e.

> SendCurrent(my_neuron_1, my_neuron_2,my_neuron_3);

#### 2.4 Run simulation
You must be sure that this instruction shall be the final instruction. i.e.

> RunSystem;

## 3. Run CENS
You have to follow this instructions.

####  Usage on terminal
```bash
Instructions:
        cens.exe <commands> <optional>
Commands:
        -i [input_file]          To define input file with CENS code.
        -o [output_file]         To define output file with tokens and lexemas.
        -s [quantity]            To define initial stimulus to simulation.
Optional:
        -graph                   Show Matlab graphics of output neurons.
        -debug                   Print couts of arrays.
```

Example using files of this repository
```bash
  $ ./cens.exe -i my_code.cens -o output_scanner.txt -s 10 -graph -debug
```

# Language Reference

## LL1 Attribute grammar:

| LL1 grammar  | Attribute Rules  | 
| :------------ |:---------------| 
|G => expr                            | $$ MakeNothing()
|expr => define ; expr'               | $$ MakeNeuronNode()
|expr => innervate ; expr'            | $$ MakeNothing()
|expr => input ; expr'                | $$ MakeSendCurrentNode()
|expr => start ;                      | $$ MakeNothing()
|expr' => define ; expr'              | $$ MakeNeuronNode()
|expr' => innervate ; expr'           | $$ MakeNothing()
|expr' => input ; expr'               | $$ MakeSendCurrentNode()
|expr' =>                             | $$ MakeNothing()
|define => neuron id ( param_d )      | $$ if(!ExistNeuron(id))
|                                     |&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;MakeNeuronIdNode(id)
|param_d => inhibitory param_d'       | $$ MakeInhibitoryNode()
|param_d => excitatory param_d'       | $$ MakeExcitatoryNode()
|param_d' => , num                    | $$ MakeIntensityNeuronNode(num) and
|                                     |&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;MakeInnervationNode()
|param_d' =>                          | $$ MakeInnervationNode()
|innervate => id0 >> id1 innervate'   | $$ if(ExistNeuron(id0) && ExistNeuron(id1))
|                                     |&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;MakeInnervationConnection(id0,id1)
|innervate' => >> id2 innervate'      | $$ if(ExistNeuron(id2))
|                                     |&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;MakeInnervationConnection(id1,id2)
|innervate' =>                        | $$ MakeNothing()
|input => SendCurrent ( id param_i )  | $$ if(ExistNeuron(id))
|                                     |&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;MakeSendCurrentConnection(id)
|param_i => , id param_i              | $$ if(ExistNeuron(id))
|                                     |&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;MakeSendCurrentConnection(id)
|param_i =>                           | $$ MakeNothing()
|start => RunSystem                   | $$ MakeRunSystemNode(AllSendCurentNodes)


## Lexical analysis

### Line structure

* Lines finish with a “;” like c++ language.
* Comments is possible 2 ways:
	* Multi line comment /* comments */
	* Single comment using // comment
* Since lines finish with “;”. Lines could be composed of more than 1 line.
* A logical line that contains only spaces, tabs and possibly a comment, is ignored.
* The whitespace characters space and tab can be used interchangeably to separate tokens.

###  Tokens

##### Identifiers
Identifiers are formed by the following regular expressions:
>letter  => [A-Za-z]<br>
digit    =>[0-9]<br>
identifier => (letter)(letter|digit)*

##### Keywords
The following identifiers are used as reserved words of the language, and cannot be used as ordinary identifiers. They must be spelled exactly as written here:
>"system", "run", "neuron", "glia", "glutamate", "acetylcholine", "norepinephrine", "gaba", "atp", "adenosine", "if", "else", "for" and "while".

##### Literals
Following the regular expression:
> ascii  => Any ASCII character except "\" or newline or the quote.<br>
literal  => ascii*

##### Operators
Available operators:
> ">>" mean innervate

##### Delimiters
Available delimiters:
>"{", "}", "(", ")" and ";"

###  Compound statements

1. class neuron statement
2. SendCurrent statement
3. RunSystem statement

# Demo

After compile CENS. You can execute this:
> ./cens.exe -i in_file.txt -o out_file.txt

or:

> ./cens.exe -default

The below image show graphically the demo:
![general_example](images/how_work_1.jpg)

# TODO

- Support to automatic millions neurons creation.
- Add neurotransmitter parameter.
- Parallelism on stimulation input.
