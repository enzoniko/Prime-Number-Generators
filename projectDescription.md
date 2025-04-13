### Individual Assignment on Prime Numbers



#### Introduction



Prime numbers are integers greater than one that have no positive divisors other than 1 and themselves. In computational security, prime numbers are used in various algorithms and protocols. To support this, it is necessary to either maintain a table of prime numbers (a precomputed list) or generate such numbers as needed.



Generating prime numbers for use in computational security systems is not a simple task. Typically, we are interested in large numbers, on the order of hundreds of digits. For example, in Brazil, signing electronic documents requires cryptographic keys generated from 2048-bit prime numbers.



One method to generate prime numbers is to first generate a large odd random number and then test whether it is prime. If it is not prime, another random number is generated until a prime is found.



Thus, this project involves two main tasks:



1. Generate random numbers;

2. Test whether the generated number is prime.



In this individual assignment, we will explore techniques for generating pseudo-random numbers and verifying the primality of these numbers.



**Note:** If deemed necessary, professors may invite students to present their work, and the grade will be based on both the submitted document and the presentation.



---



### 1) Regarding the Deliverable



You must submit a **single PDF file** on Moodle with the following requirements:



- A section on **Random Numbers** with a description and results obtained;

- A section on **Prime Numbers** with a description and results obtained;

- The codes must be **included in the PDF** (embedded within the document) and properly **documented** with comments.



Alternatively, you may include a link in the PDF to a Git repository, provided all commits are made before the submission deadline.



**Mandatory:** Include references and citations for each algorithm used (e.g., books, links, etc.).  

**Note:** Work without appropriate citations and bibliographical references will not be graded.



**Reminder:** Submit a single PDF document containing the full report for this individual assignment, including program codes, tables, outputs, and all other required elements.



---



### 2) Generating Pseudo-Random Numbers



#### Step 1 - Description and Implementation



You must choose **two (2)** of the following pseudo-random number generator algorithms, describe them, and implement them. In the description, explain in text how each algorithm works, followed by the bibliographical references used.



**List of Algorithms:**



- Blum Blum Shub

- Complementary-multiply-with-carry

- Inversive congruential generator

- ISAAC (cipher)

- Lagged Fibonacci generator

- Linear congruential generator

- Linear feedback shift register

- Maximal periodic reciprocals

- Mersenne twister

- Multiply-with-carry

- Naor-Reingold Pseudorandom Function

- Park–Miller random number generator

- Well Equidistributed Long-period Linear

- Xorshift



#### Step 2 - Experiment



For each chosen algorithm, you must:



- Generate **large pseudo-random numbers**, defined as numbers up to **4096 bits**. Experiment with generating numbers of the following sizes: **40, 56, 80, 128, 168, 224, 256, 512, 1024, 2048, 4096 bits**.

- If an algorithm cannot generate numbers of a certain size, provide a justification for why this is the case.

- The implemented code must be **documented**, preferably with comments within the code itself.

- Create a table detailing the algorithm, the size of the number, and the time required to generate it.



**Idea:** Generate a specific quantity of numbers for each size and calculate the **average generation time**.



**Example Table:**



| Algorithm      | Number Size | Time to Generate |

|----------------|-------------|------------------|

| Blum Blum Shub | 40 bits     | 10 ms            |

| ...            | 56 bits     | ...              |

| ...            | ...         | ...              |



#### Step 3 - Analysis



Compare the two chosen algorithms by:



- Discussing the **generation times** obtained for each;

- Performing a **complexity analysis** of the algorithms to justify the observed times.



---



### 3) Primality Testing



The **Miller-Rabin** method is a classic approach for verifying whether a number is prime. In this assignment, you must implement **Miller-Rabin** and **one additional method** (either from the list below or another of your choosing).



**Suggested Additional Primality Testing Methods (besides Miller-Rabin):**



- Fermat Primality Test

- Solovay-Strassen

- Frobenius



#### Step 1 - Description and Implementation



- Describe and implement the two algorithms.

- Justify your choice of the second method.

- In the description, explain in text how each algorithm works, followed by the bibliographical references used.

- The implemented code must be **documented**, preferably with comments within the code itself.



#### Step 2 - Experiment



- Generate a table containing prime numbers of sizes: **40, 56, 80, 128, 168, 224, 256, 512, 1024, 2048, and 4096 bits**.

- Use the random numbers generated in the previous section as input. If needed, generate additional numbers or increment the odd numbers generated to find primes.

- Write about the **difficulties encountered** and the **time required** on your computer to generate these numbers.

- Note that generating numbers of these sizes may be challenging; aim to generate as many as possible.



**Example Table:**



| Algorithm    | Number Size | Generated Prime Number | Time to Generate |

|--------------|-------------|------------------------|------------------|

| Miller Rabin | 40 bits     | ...                    | ...              |

|              | 56 bits     | ...                    | ...              |

| ...          | ...         | ...                    | ...              |



#### Step 3 - Analysis



Compare the two algorithms by:



- Discussing the **generation times** obtained;

- Performing a **complexity analysis** of the algorithms.



**Analysis Idea:** Investigate whether the two implemented algorithms return the same result for the same random input. Are there discrepancies (e.g., one identifies a number as prime while the other labels it composite)? There are **pseudo-prime numbers** that fail certain tests; test your algorithms with these cases.