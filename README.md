# Semimeander Crossing Number Estimator

## Overview
This program is designed to estimate the semimeander crossing number of knots, and developed to support the research presented in the preprint "[On the complexity of meander-like diagrams of knots](https://arxiv.org/abs/2312.05014)". It is important to note that this software is not a complete application. The library does not include a top-level application or API; it is up to the user to implement the integration into their projects.

## Prerequisites
The toolkit utilizes **ALGLIB Library**, specifically the `optimization.h` component, for solving linear optimization problems. Users will need to ensure that the librariy is properly installed. Visit [ALGLIB](http://www.alglib.net) to download and install the library.

## Structure and Examples
The main examples of how to utilize this toolkit can be found in `main.cpp`. This file includes the example of the calculation of the values of C_{n, 8} and D_{n, 8} for n < 10 (definitions of C_{n, 8} and D_{n, 8} can be found in the corresponding paper). The results of this calculationare stored in the directory "Files with numbers".