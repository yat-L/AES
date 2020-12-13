# Implementing AES
 * Implementing AES from [NIST](https://www.nist.gov/publications/advanced-encryption-standard-aes)
 * Only 128 bit variant is implemented
 * This goes without saying but I will still say it: **DO NOT USE ANY CODE IN THIS REPOSITORY FOR SECURITY PURPOSE**.
 * run "make" in this directory to compile the code. The main function will be run automatically.
 * run "make clean" in this directory to remove object files and executable
 * When running the code, enter the file name of the plaintext and key accordingly.
 * If no file name is provided and just pressing enter key, the default test case provided to us will be used.
 * If you want to use another sbox, or invSbox, change the new sbox and invSbox name to
 "aes_sbox.txt" and "aes_inv_sbox.txt" respectively.
 * If file name is wrong, it will terminate
 * "aeslib.h" contain the interface
