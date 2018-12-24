# GCP-101 Smart Contract Protocol

Zeepin Token GCP101

## Directory
 - [Simple Summary](#Simple-Summary)
 - [Abstract](#Abstract)
 - [Specification](#Specification)
    * [Basic Interpretations](#Basic-Interpretations)
    * [Methods Analysis](#Methods-Analysis)
## Simple Summary
GCP-101(Galaxy Consensus Proposal - 101) is a standard interface based on GPC-10.
## Abstract
This standard adds authorities and several methods for a contract and allows the contract to be controlled and managed. They can be used to intialize the contract and realize the increase and decrease of the total token supply.
## Specification
### Basic Interpretations
You may need two kinds of special account address to control and manage the depolyed contract.
```c
char * ceoAddress = "/* address */"; //Highest authority
char * adminAddress = "/* address */"; //Administrator
```
ceoAddress intializes a contract, and it can invoke the pause and unPause method to control the contract. Some methods like init, transfer, transferFrom and approve in GPC-10 need to be modified to correspond to the new protocol.
### Methods Analysis
#### pause
Pause the contract when an unexpected situation occurs. It can only be invoked by `ceoAddress`. This method will stop the users to invoke the methods including `increaseTotal`, `decreaseTotal`, `transfer`, `transferFrom` and `approve`, but other methods can be invoked normally.
```c
char * pause(){
	if (Atoi(ZPT_Storage_Get("paused")) == 1)
		return "The contract has already been paused.";
	if (ZPT_Runtime_CheckWitness(ceoAddress) == 0){
		int paused = 1;
		ZPT_Storage_Put("paused", Itoa(paused));
		return "The cantract is paused successfully.";
	}
	return "You have no permission to pause the contract.";
}
```
#### unPause
Unpause the contract after handling the bad situation. It can only be invoked by `ceoAddress`. After unpausing, each method in contract can be invoked normally.
```c
char * unPause(){
	if (Atoi(ZPT_Storage_Get("paused")) == 0)
		return "No need to unpause because the contract is not under the pause situation.";
	if (ZPT_Runtime_CheckWitness(ceoAddress) == 0){
		int paused = 0;
		ZPT_Storage_Put("paused", Itoa(paused));
		return "The cantract is unpaused successfully.";
	}
	return "You have no permission to unpause the contract.";
}
```
#### init (modified)
Initialize the first deployed contract. It can only be invoked by `ceoAddress`. The initialization will determine the `totalSupply` of the token, and allot the same amount to the `adminAddress `. In addition, it will intialize `paused` to 0, which means the contract can be invoked normally.
Once a contract has been initialized, the `init` method can not be invoked again.
```c
char * init(char * totalSupply, char * adminBalance){
	if (arrayLen(ZPT_Storage_Get("totalSupply")) != 0)
		return "Already initialized.";
	if (ZPT_Runtime_CheckWitness(ceoAddress) == 0)
		return "You have no permission to init.";
	if (Atoi(totalSupply) <= 0)
		return "The totalsupply can not be less than or equal to 0.";
	if (Atoi(adminBalance) != Atoi(totalSupply))
		return "The balance alloted to admin must be equal to the totalSupply.";
	ZPT_Storage_Put("totalSupply", totalSupply);
	ZPT_Storage_Put(adminAddress, adminBalance);
	ZPT_Storage_Put("paused", "0");
    return "Init success!";
}
```
#### increaseTotal
Invoke this method to increase the total token supply. It can only be invoked by `ceoAddress` or `adminAddress`, and it can not be invoked when the contract is paused.
```c
char * increaseTotal(char * valueChar){
	if (Atoi(ZPT_Storage_Get("paused")) == 1) 
		return "The contract has been paused.";
	if ((ZPT_Runtime_CheckWitness(ceoAddress) == 0) && (ZPT_Runtime_CheckWitness(adminAddress) == 0))
		return "You have no permission to increase the totalSupply.";
	char * totalSupply = ZPT_Storage_Get("totalSupply");
    if (arrayLen(totalSupply) == 0)
    	return "Please init first.";
	int value = Atoi(valueChar);
	if (value <= 0)
		return "The value increased can not be less than or equal to 0.";
	int totalSupplyNew = Atoi(totalSupply) + value;
	ZPT_Storage_Put("totalSupply", Itoa(totalSupplyNew));
	return "Increase success.";
}
```
#### decreaseTotal
Invoke this method to decrease the total token supply. It can only be invoked by `ceoAddress` or `adminAddress`, and it can not be invoked when the contract is paused.
```c
char * decreaseTotal(char * valueChar){
	if (Atoi(ZPT_Storage_Get("paused")) == 1) 
		return "The contract has been paused.";
	if ((ZPT_Runtime_CheckWitness(ceoAddress) == 0) && (ZPT_Runtime_CheckWitness(adminAddress) == 0))
		return "You have no permission to decrease the totalSupply.";
	char * totalSupply = ZPT_Storage_Get("totalSupply");
    if (arrayLen(totalSupply) == 0)
    	return "Please init first.";
	int value = Atoi(valueChar);
	if (value <= 0)
		return "The value decreased can not be less than or equal to 0.";
	int totalSupplyNew = Atoi(totalSupply);
	if (totalSupplyNew <= value)
		return "The value decreased must be less than the current totalSupply.";
	totalSupplyNew -= value;
	ZPT_Storage_Put("totalSupply", Itoa(totalSupplyNew));
	return "Decrease success.";
}
```
#### transfer (modified)
It still includes the basic functionality of transfer method in GCP-10, but will be invalid when the contract is paused.
```c
char * transfer(char * fromAddr, char * toAddr, char * amountChar){
	if (Atoi(ZPT_Storage_Get("paused")) == 1) 
		return "The contract has been paused.";
		// The following is same as GCP-10.
```
#### transferFrom (modified)
It still includes the basic functionality of transferFrom method in GCP-10, but will be invalid when the contract is paused.
```c
char * transferFrom(char *fromAddr, char *spenderAddr, char *toAddr, char *amountChar){
	if (Atoi(ZPT_Storage_Get("paused")) == 1) 
		return "The contract has been paused.";
		// The following is same as GCP-10.
```
#### approve (modified)
It still includes the basic functionality of approve method in GCP-10, but will be invalid when the contract is paused.
```c
char * approve(char * ownerAddr, char * spenderAddr, char * allowedChar){
	if (Atoi(ZPT_Storage_Get("paused")) == 1) 
		return "The contract has been paused.";
		// The following is same as GCP-10.
```
