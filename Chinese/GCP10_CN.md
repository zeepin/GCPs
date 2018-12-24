# GCP-10智能合约协议

GCP-10 : [English](GCP10_EN.md) | [中文](GCP10_CN.md)

## 目录
 - [概述](#概述)
 - [摘要](#摘要)
 - [目标](#目标)
 - [详述](#详述)
    * [基本说明](#基本说明)
    * [方法分析](#方法分析)

## 概述
GCP-10是智品区块链运用于同质资产（Fungible Token）的协议，对应于以太坊的ERC20协议。

## 摘要
以下标准基于C语言编写，为部署于智品链上的智能合约提供了同质资产所需的标准API。这些标准包括了资产转让，以及授权第三方进行资产转让等基本功能。

## 目标
无论小型钱包或者大型交易平台，协议的标准接口允许智品链上的资产可被第三方应用多次使用。

## 详述
### 基本说明
与数据库操作有关的三个重要接口：
```c
void ZPT_Storage_Put(char * key,char * value); //向数据库添加一组键值对
char * ZPT_Storage_Get(char * key); //得到键所对应的值
void ZPT_Storage_Delete(char * key); //根据键从数据库里删除一组键值对
```
数据库只能存储`char`型数据，请不要修改上述输入参数的数据类型。另有两个接口“array to int”和“int to array”可以用来进行`char`和`int`间的数据转换。
```c
int Atoi(char * s); //array to int
char * Itoa(int a); //int to array
```
下面的两个函数分别用来验证一个账户是否存储于数据库中，以及一个账户是否授权另一账户。
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
### 方法分析
#### init
首次部署于链上的合约需先进行初始化，初始化将决定代币的总发行量。该方法仅能被调用一次，合约被初始化之后它将无法再调用。
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
返回代币的总发行量。如果合约尚未初始化，调用该方法时会提示调用者先进行初始化。
```c
char * totalSupply(){
	char * total_supply = ZPT_Storage_Get("totalSupply");
    if (arrayLen(total_supply) == 0)
    	return "Please init first.";
    return total_supply;
}
```
#### balanceOf
返回一个已存储于数据库里的账户的余额。
```c
char * balanceOf(char * address){
	char * balance = ZPT_Storage_Get(address);
	if (arrayLen(balance) == 0)
		return "Address is not in our database.";
	return balance;
}
```
#### transfer
从`fromAddr`向`toAddr`转账，转账金额为`amountChar`。这里的输入参数`amountChar`是`char`类型，在函数内部使用时会使用`Atoi`接口对它进行转`int`型的处理。

为了提高链的利用率，禁止转账金额为0的交易。

该函数在下述情况时应立即`return`：
- `fromAddr`与合约调用者地址不一致，可以使用接口`ZPT_Runtime_CheckWitness`来进行验证。
- `fromAddr`的账户余额不足以支付转账金额。
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
该方法允许授权第三方调用合约进行转账。
首先，`fromAddr`授权`spenderAddr`一定量的可花费金额，称之为授权金额，之后`spenderAddr`可调用合约代表`fromAddr`来花费这部分金额。

同样，禁止转账金额为0的交易。

该函数在下述情况时应立即`return`：
- `spenderAddr`与合约调用者地址不一致。
- `fromAddr`未授权`spenderAddr`。
- 授权金额不足以支付转账金额。
- `fromAddr`的余额不足以支付转账金额。
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
允许`spenderAddr`多次花费`ownerAddr`账户中的金额，最高额度至`amountChar`。每一次调用此方法都会覆盖当前的授权金额值。

同转账类似，禁止授权金额为0。

该函数在下述情况时应立即`return`：
- `ownerAddr`与合约调用者地址不一致。
- 授权金额超过了`ownerAddr`自身的余额。
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
返回`spenderAddr`仍旧可以从`ownerAddr`账户中提取的金额。
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
