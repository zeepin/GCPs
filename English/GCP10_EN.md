# GCP-10 Smart Contract Protocol

Zeepin Token GCP10

## Directory
 - [Simple Summary](#Simple-Summary)
 - [Abstract](#Abstract)
 - [Motivation](#Motivation)
 - [Specification](#Specification)
    * [Basic Interpretations](#Basic-Interpretations)
    * [Methods Analysis](#Methods-Analysis)
## Simple Summary
GCP-10(Galaxy Consensus Proposal - 10) is a standard interface for fungible tokens, which is similar to ERC-20 based on Ethereum.
## Abstract
The following standard written by C language allows for the implementation of a standard API for tokens within smart contracts deployed on Zeepin Chain. This standard provides basic functionality to transfer tokens, as well as allow tokens to be approved so they can be spent by another on-chain third party. In addtion, it provides authority for CEO address and administrator address to control the deployed contract.
## Motivation
A standard interface allows any tokens on Zeepin Chain to be re-used by other applications: from wallets to decentralized exchanges.
## Specification
### Basic Interpretations
There are three vital interfaces correlated to our database.
```c
void ZPT_Storage_Put(char * key,char * value); //Add a key-value pair into the database.
char * ZPT_Storage_Get(char * key); //Get the value by a given key.
void ZPT_Storage_Delete(char * key); //Delete a key-value pair from the database by a given key.
```
Our database can only save `char` data type. Do not use any other input parameter types for the above interfaces. We have offered "array to int" and "int to array" interfaces to help transform the data types between `char` and `int`.
```c
int Atoi(char * s); //array to int
char * Itoa(int a); //int to array
```
We offered two brief and useful functions to help check if an account is stored in the database, and check if an account approves another one.
```c
int isStored(char * key){
	if (arrayLen(ZPT_Storage_Get(key)) == 0)
		return 0;
	return 1;
}
```
```c
int isApproved(char * owner, char * spender){
	char * allowedKey = concat(owner, spender);
	if (arrayLen(ZPT_Storage_Get(allowedKey)) == 0)
		return 0;
	return 1;
}
```
### Methods Analysis
#### init
Initializes the first deployed contract and determines the `totalSupply` of the token.
Once a contract has been initialized, the `init` method can not be invoked again.
```c
char * init(char * totalSupply, char * adminBalance){
	if (arrayLen(ZPT_Storage_Get("totalSupply")) != 0)
		return "Already initialized.";
	if (Atoi(totalSupply) <= 0)
		return "The totalsupply can not be less than or equal to 0.";
	ZPT_Storage_Put("totalSupply", totalSupply);
    return "Init success!";
}
```
#### totalSupply
Returns the total token supply. It will remind you if the contract has not been initialized yet.
```c
char * totalSupply(){
	char * total_supply = ZPT_Storage_Get("totalSupply");
    if (arrayLen(total_supply) == 0)
    	return "Please init first.";
    return total_supply;
}
```
#### balanceOf
Returns the balance of an account address if it is stored in the database.
```c
char * balanceOf(char * address){
	char * balance = ZPT_Storage_Get(address);
	if (arrayLen(balance) == 0)
		return "Address is not in our database.";
	return balance;
}
```
#### transfer
Transfer `amountChar` amount from `fromAddr` to `toAddr`. Do not worry `amountChar` is a `char` data type, you can transform it to `int` type by interface `Atoi`.
The function SHOULD `return` directly if:
- The `fromAddr` is inconsistent with the caller address, and you can use the interface `ZPT_Runtime_CheckWitness` to check this. 
- The `fromAddr` account balance does not have enough tokens to spend.
Transfers of 0 values are prohibited for the consideration of our chain utilization.
```c
char * transfer(char * fromAddr, char * toAddr, char * amountChar){
	if (arrayLen(ZPT_Storage_Get("totalSupply")) == 0)
		return "Please init first.";
	if (ZPT_Runtime_CheckWitness(fromAddr) == 0)
    	return "Inconsistent address.";
	if (isStored(fromAddr) == 0)
		return "Sender address is not in our database.";
	int amount = Atoi(amountChar);
    if (amount <= 0)
    	return "Transfer amount cannot be less than or equal to 0.";
	int balance_from = Atoi(BalanceOf(fromAddr));
	if (balance_from < amount)
    	return "No sufficient balance.";
    
    balance_from -= amount;
    if (balance_from == 0)
    	ZPT_Storage_Delete(fromAddr);
    else
    	ZPT_Storage_Put(fromAddr,Itoa(balance_from));
    
    if (IsStored(toAddr) == 0)
    	ZPT_Storage_Put(toAddr, amountChar);
	else {
		int balance_to = Atoi(BalanceOf(toAddr));
    	balance_to += amount;
    	ZPT_Storage_Put(toAddr,Itoa(balance_to));
	}
    return "Transfer success.";
}
```
#### transferFrom
The `transferFrom` method allows the contract to transfer tokens on someone's behalf.
Firstly the `fromAddr` approves `spenderAddr` a certain amount of allowance, and then `spenderAddr` can spend tokens of `fromAddr` within this amount on behalf of the `fromAddr`.
The function SHOULD `return` directly if:
- The `spenderAddr` is inconsistent with the caller address;
- The `fromAddr` does not approve `spenderAddr`;
- The allowance is insufficient to spend.
- The balance of `fromAddr` is insufficient to spend.
Transfers of 0 values are prohibited for the consideration of our chain utilization.
```c
char * transferFrom(char *fromAddr, char *spenderAddr, char *toAddr, char *amountChar){
	if (arrayLen(ZPT_Storage_Get("totalSupply")) == 0)
		return "Please init first.";
	if (ZPT_Runtime_CheckWitness(spenderAddr) == 0)
    	return "Inconsistent address.";
	if (isStored(fromAddr) == 0)
		return "Sender address is not in our database.";
	if (isApproved(fromAddr, spenderAddr) == 0)
		return "Not approved.";
	int amount = Atoi(amountChar);
    if (amount <= 0)
    	return "TransferFrom amount cannot be less than or equal to 0.";
	int balance_from = Atoi(BalanceOf(fromAddr));
	if (balance_from < amount)
    	return "No sufficient balance.";
    char * allowedKey = concat(fromAddr, spenderAddr);
    int allowed = Atoi(ZPT_Storage_Get(allowedKey));
    if (allowed < amount)
    	return "No sufficient allowance.";
    
    balance_from -= amount;
    if (balance_from == 0) {
    	ZPT_Storage_Delete(fromAddr);
    	ZPT_Storage_Delete(allowedKey);
    }
    else
    	ZPT_Storage_Put(fromAddr,Itoa(balance_from));
    
    if (isStored(toAddr) == 0)
    	ZPT_Storage_Put(toAddr, amountChar);
	else {
		int balance_to = Atoi(BalanceOf(toAddr));
    	balance_to += amount;
    	ZPT_Storage_Put(toAddr,Itoa(balance_to));
	}
    
    allowed -= amount;
    if (allowed == 0)
    	ZPT_Storage_Delete(allowedKey);
    else
    	ZPT_Storage_Put(allowedKey, Itoa(allowed));
    return "TransferFrom success.";
}
```
#### approve
Allows `spenderAddr` to withdraw from `ownerAddr` account multiple times, up to the `allowedChar` amount. If this function is called again it overwrites the current allowance.
The function SHOULD `return` directly if:
- The `ownerAddr` is inconsistent with the caller address;
- The allowance is larger than the balance of the `ownerAddr`.
Approves 0 values are prohibited for the consideration of our chain utilization.
```c
char * approve(char * ownerAddr, char * spenderAddr, char * allowedChar){
	if (arrayLen(ZPT_Storage_Get("totalSupply")) == 0)
		return "Please init first.";
	if (ZPT_Runtime_CheckWitness(ownerAddr) == 0)
    	return "Inconsistent address.";
	if (isStored(ownerAddr) == 0)
		return "Owner address is not in our database.";
	int allowed = Atoi(allowedChar);
	if (allowed <= 0)
    	return "The allowance can not be less than or equal to 0.";
    int balance_owner = Atoi(BalanceOf(ownerAddr));
    if (balance_owner < allowed)
    	return "The allowance can not be larger than the balance of owner.";
    char * allowedKey = concat(ownerAddr, spenderAddr);
    ZPT_Storage_Put(allowedKey, Itoa(allowed));
    return "Approve success!";
}
```
#### allowance
Returns the amount which `spenderAddr` is still allowed to withdraw from `ownerAddr`.
```c
char * allowance(char * ownerAddr, char * spenderAddr){
	if (isStored(ownerAddr) == 0)
		return "Owner address is not in our database.";
	if (isApproved(ownerAddr, spenderAddr) == 0)
		return "Not approved yet.";
	char * allowedKey = concat(ownerAddr, spenderAddr);
    return ZPT_Storage_Get(allowedKey);
}
```
