# GCP-101智能合约协议

GCP-101 : [English](/English/GCP101_EN.md) | [中文](/Chinese/GCP101_CN.md)

## 目录
 - [概述](#概述)
 - [摘要](#摘要)
 - [详述](#详述)
    * [基本说明](#基本说明)
    * [方法分析](#方法分析)

## 概述
GCP-101是基于GCP-10的标准接口协议。

## 摘要
该标准为智能合约添加了权限及新的方法，允许控制和管理智能合约。权限将被用于合约的初始化中，并且用来控制实现代币总发行量的增加和减少。此外，GPC-10协议中的部分方法（transfer，transferFrom和approve）将根据新的标准进行修改。

## 详述
### 基本说明
由两种账户类型控制和管理智能合约。
```c
char * ceoAddress = "/* address */"; //最高权限者
char * adminAddress = "/* address */"; //管理员
```
`ceoAddress`初始化一个智能合约，并且可以调用`pause`和`unPause`方法来控制合约。在初始化合约时，根据`totalSupply`的值向`adminAddress`账户分配等额的余额，此后再由`adminAddress`向普通用户转出。

### 方法分析
#### pause
当意外情况发生时暂停合约，仅能被`ceoAddress`调用。该方法将暂停用户调用`increaseTotal`, `decreaseTotal`, `transfer`, `transferFrom`, `approve`等方法，其他方法可以正常调用。
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
当处理完意外情况后，恢复被暂停的合约，同样仅能被`ceoAddress`调用。在恢复合约之后，所有方法均可正常调用。
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
仅能由`ceoAddress`初始化第一次被部署的合约，在初始化中将决定代币的总发行量，并分配给`adminAddress`相同的金额。初始化时将置`paused`值为0，代表着合约可被正常调用。
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
调用此方法来增加代币的总发行量。该方法仅能被`ceoAddress`或`adminAddress`调用，且在合约暂停期间不能调用。
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
调用此方法来减少代币的总发行量。该方法仅能被`ceoAddress`或`adminAddress`调用，且在合约暂停期间不能调用。
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
该方法依旧包含了GCP-10中`transfer`的基本功能，但在合约暂停期间失效。
```c
char * transfer(char * fromAddr, char * toAddr, char * amountChar){
	if (Atoi(ZPT_Storage_Get("paused")) == 1) 
		return "The contract has been paused.";
		// The following is same as GCP-10.
```
#### transferFrom (modified)
该方法依旧包含了GCP-10中`transferFrom`的基本功能，但在合约暂停期间失效。
```c
char * transferFrom(char *fromAddr, char *spenderAddr, char *toAddr, char *amountChar){
	if (Atoi(ZPT_Storage_Get("paused")) == 1) 
		return "The contract has been paused.";
		// The following is same as GCP-10.
```
#### approve (modified)
该方法依旧包含了GCP-10中`approve`的基本功能，但在合约暂停期间失效。
```c
char * approve(char * ownerAddr, char * spenderAddr, char * allowedChar){
	if (Atoi(ZPT_Storage_Get("paused")) == 1) 
		return "The contract has been paused.";
		// The following is same as GCP-10.
```
