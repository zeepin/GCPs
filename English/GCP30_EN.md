<h1 align="center">GCP-30 Smart Contract Protocol</h1>

GCP-30 : [English](/English/GCP30_EN.md) | [中文](/Chinese/GCP30_CN.md)

## Directory
   
* [Simple Summary](#Simple-Summary)
* [Abstract](#Abstract)
* [Motivation](#Motivation)
* [Specification](#Specification)
    * [Specific analysis](#Specific-analysis)
    * [Implementation interface](#Implementation-interface)
    * [Precautions](#Precautions)


### Simple Summary
GCP-30(Galaxy Consensus Proposal) is a standard interface for non-fungible tokens, also similar with ERC-721.

### Abstract
The following standard allows for the implementation of a standard API for NFTs within smart contracts. This standard provides basic functionality to track and transfer NFTs.

We considered use cases of NFTs being owned and transacted by individuals as well as consignment to third party brokers/wallets/auctioneers ("operators"). NFTs can represent ownership over digital or physical assets. We considered a diverse universe of assets, and we know you will dream up many more:

* Physical property — houses, unique artwork
* Virtual collectables — unique pictures of kittens, collectable cards
* "Negative value" assets — loans, burdens and other responsibilities

In general, all houses are distinct and no two kittens are alike. NFTs are distinguishable and you must track the ownership of each one separately.
### Motivation
A standard interface allows wallet/broker/auction applications to work with any NFT on Zeepin. We provide for simple GCP-30 smart contracts as well as contracts that track an arbitrarily large number of NFTs.

### Specification
**Every GCP-30 compliant contract must implement the `GCP-30` interfaces**，The following is the GCP-30 standard smart contract written in WA based on WASM. In this environment, the parameters are passed and the returned values are returned in the form of char, so we need to pay attention to the data type conversion when we write the contract.

Not much to say, let's start the implementation of GCP-30 directly. First, we need understand the main function interfaces that we need to use:
```c
char *OwnerOf(char *TokenID)
//Returns the address of the tokenId  holder

char *BalanceOf(char *address)
//Returns the number of NFTs held by address

void Transfer(char *from, char *to, char *TokenID)
//Change the control of the NFT, changed address from-->to

char *Approve(char *from, char *to, char *TokenID)
//Grant address to have control of tokenId, trigger the Approval event after the method succeeds

char *ApproverOf(char *TokenID)
//Returns authorized address of the tokenId

char *ApprovedFor(char *address, char *TokenID)
//Check if the address matches the authorized address of the tokenID

char *TransferFromOwner(char *owner, char *to, char *TokenID)
//The NFT owner directly transfers control of its tokenID to the authorized address

char *TransferFromApproval(char *from, char *to, char *approval, char *TokenID）
//The tokenID is transferred from the address (from ) to the address (to) by the token address of the tokenID

```

#### Specific analysis
You can edit directly online at https://wasdk.github.io/WasmFiddle and click Build at any time to check for any editing errors.

##### init
Initialize the NFTs token and set the start to 0. The sample code is as follows:
```c
#define true "1";
#define false "0";
int init_amount = 0;

char *Init()
{
    if (arrayLen(ZPT_Storage_Get("totalSupply")) != 0)    //First determine whether the initialization has been completed, or the token already exists, and return false if the total token amount is not empty.
    {
        return "init has~~~~~ finished!";
    }
    ZPT_Storage_Put("totalSupply", Itoa(init_amount));   //The total number of tokens is stored in the form of key--value, and init_amount is int type. When saving, it needs to be converted to char type by interface Itoa().
    return "init success!";
}
```

##### totalSupply
Can be used to query the total number of current NFTs tokens. If you use this command, the current total amount will be queried. The sample code is as follows:
```c
char *TotalSupply()
{
    if (arrayLen(ZPT_Storage_Get("totalSupply")) == 0)  //Determine if initialization already exists
    {
        return "you need init!";
    }
    return ZPT_Storage_Get("totalSupply");  //Returns the total number of values based on the key of "totalSupply"
}
```

##### create
Create a new NFT token, each tokenID corresponding to a unique address. The sample code is as follows:
```c
char *Create(char *TokenID, char *address){
 if (arrayLen(ZPT_Storage_Get("totalSupply")) == 0)   //Determine if there is initialization
    {
        return "You need `init!";
    }
    char *Result = ZPT_Storage_Get(TokenID);    //Get the address of the TokenID. If the address is not empty, it means that the NFT token already exists.
    if (arrayLen(Result) != 0)
        return "Your TokenID is existed";
    Transfer("", address, TokenID);             //Call the Transfer() function to save the new NFT token created. The first parameter here is null.
    int totalSupply = Atoi(IncreaseTotalSupply());  //Here to do an increase in the total amount of token totalSupply, call the IncreaseTotalSupply () function
    IncreaseIndex(Itoa(totalSupply), TokenID);   //Here, the IncreaseIndex() function is called, and a count list of TokenIDs is stored, and each new token serial number is recorded.
    return "create success";
}

```
* Then look at the internal implementation of the function **Transfer()**:

The content of the specific implementation is divided into two parts.

1. When the address `from` is empty, the deposit of the TokenID and the address `to` is performed, and the amount of the NFTs owned by the corresponding address `to` is added;

2. When the address `from` is not empty, a transfer relationship occurs. At the same time as the deposit of the TokenID and the address `to`, the address corresponding to the authorized newTokenID is deleted, and the amount of the NFTs owned by the address `from` is made Subtraction.

```c
void Transfer(char *from, char *to, char *TokenID)
{

    ZPT_Storage_Put(TokenID, to);                    //add storage TokenID and address `to`
    int amount = Atoi(ZPT_Storage_Get(to));          //Get the number of NFTs tokens owned by the address `to`
    if (amount == 0)
    {
        amount = 0;
    }
    amount = amount + 1;
    ZPT_Storage_Put(to, Itoa(amount));                //The result of adding the token amount to the address `to`
    if (from != "")                                   //if address is not null
    {
        int fromAmount = Atoi(ZPT_Storage_Get(from));      //Get the number of NFTs tokens owned by the address `from`
        fromAmount = fromAmount - 1;                  //make Subtraction
        if (fromAmount == 0)
        {
            ZPT_Storage_Delete(from);                 //If the number of NFTs tokens owned by the address `from` is 0, the value of the NFTs tokens owned before the address `from` is deleted.
        }
        else
            ZPT_Storage_Put(from, Itoa(fromAmount));  //The result of reducing the number of tokens deposited on the address `from`

        char *ap = "ap.";
        char *newTokenID = strconcat(ap, TokenID);
        ZPT_Storage_Delete(newTokenID);               //Delete the record in the authorization
    }
}
```
* Look at the internal implementation of the function **IncreaseTotalSupply()**:

Specifically, the total number of totalSupply is accumulated, and then returns totalSupply.

```c
char *IncreaseTotalSupply()
{
    int totalSupply = Atoi(ZPT_Storage_Get("totalSupply"));   //Retrieve the current value
    totalSupply = totalSupply + 1;                            //addition processing
    ZPT_Storage_Put("totalSupply", Itoa(totalSupply));        //Save the new value
    return Itoa(totalSupply);                                 //return the total number of strings
}
```
* Finally, look at the internal implementation of the function **IncreaseIndex()**:

Directly do a storage process, store each TokenID with a tag number.
```c
void IncreaseIndex(char *totalSupply, char *TokenID)
{
    ZPT_Storage_Put(totalSupply, TokenID);
}
```

##### ownerOf

Query the address that owns this TokenID.
```c
char *OwnerOf(char *TokenID)
{
    return ZPT_Storage_Get(TokenID);
}
```

##### owns

Determine if the address is consistent with the address that owns this TokenID.

Steps:

* Extract the address that owns this TokenID, ZPT_Storage_Get(TokenID);
* Then strcmp() compares the two addresses and returns the int type;
* Treat the int type as a char type.

```c
char *Owns(char *TokenID, char *address)
{
    return  Itoa(strcmp(ZPT_Storage_Get(TokenID), address));    
}
```

##### balanceOf

Function: Query the balance of NFTs tokens owned by a given address.

```c
char *BalanceOf(char *address)
{
    if (Atoi(ZPT_Storage_Get(address)) == 0)
        return Itoa(0);
    else
        return ZPT_Storage_Get(address);
}
```
##### approve

Function: Authorize the TokenID owned by the address from to the address to

Steps:

* The address to is not empty;
* Determine that the owner of this TokenID is consistent with the address from;
* Confirm that the address from has the rights of the party.

```c
char *Approve(char *from, char *to, char *TokenID)
{
    if (to == "")
        return false;
    if (Atoi(Owns(TokenID, from)) == 0)
        return false;
    if (ZPT_Runtime_CheckWitness(from))
        return false;
    ApproveInternal(to, TokenID);
    return true;
}
```
Then look at the function **ApproveInternal()** did:

Here, the TokenID is added with an ap. prefix and then stored with the address to.

```c
void ApproveInternal(char *to, char *TokenID)
{
    char *ap = "ap.";
    char *newTokenID = strconcat(ap, TokenID);
    ZPT_Storage_Put(newTokenID, to);
}
```

##### approverOf

Function: Query the authorized address of this TokenID.

Steps:

* Add TokenID prefix ap.;
* It is judged whether the TokenID after the prefix is added has the corresponding address, and if it is empty, the input is incorrect;
* Get the address

```c
char *ApproverOf(char *TokenID)
{
  char *ap = "ap.";
  char *newTokenID = concat(ap, TokenID);
  if (arrayLen(ZPT_Storage_Get(newTokenID)) == 0) {
    return false;
  }
  return ZPT_Storage_Get(newTokenID);
}
```

##### approvedFor

Function: Query whether the authorized address of this TokenID matches the given address.

Steps:

* Add TokenID prefix ap.;
* Extract the address that owns this newTokenID, ZPT_Storage_Get(newTokenID);
* Then strcmp() compares the two addresses and returns the int type;
* Treat the int type as a char type and return 0 or 1.

```c
char *ApprovedFor(char *address, char *TokenID)
{
    char *ap = "ap.";
    char *newTokenID = strconcat(ap, TokenID);
    return  Itoa(strcmp(ZPT_Storage_Get(newTokenID), address));
}
```

##### transferFromOwner

Function: The current TokenID owner calls the contract for the transfer of NFT ownership

Steps:

* The address to is not empty;
* Determine if the owner address of this TokenID matches the given address;
* Confirm that the address from has the authority of the party;
* Call the Transfer() function to transfer the ownership of the NFT;

```c
char *TransferFromOwner(char *owner, char *to, char *TokenID)
{
    if (to == "")
        return false;
    if (Atoi(Owns(TokenID, owner)) == 0)
        return false;
    if (ZPT_Runtime_CheckWitness(owner))
        return false;
    Transfer(owner, to, TokenID);
    return true;
}
```

##### transferFromOwner

Function: The current TokenID is authorized to call the contract to transfer the NFT ownership.

Steps:

* The address to is not empty;
* Call the ApprovedFor() function to confirm that the TokenID authorized address matches the given address;
* from must be the owner of the TokenID;
* Confirm that the authorized address has the rights of the parties;
* Call the Transfer() function to transfer the ownership of the NFT;

```c
char *TransferFromApproval(char *from, char *to, char *approval, char *TokenID)
{
    if (to == "")
        return false;
    if (Atoi(ApprovedFor(approval, TokenID)) == 0)
        return false;
    if (Atoi(Owns(TokenID, from)) == 0)
        return false;
    if (ZPT_Runtime_CheckWitness(approval))
        return false;
    Transfer(from, to, TokenID);
    return true;
}
```

##### getTokenIDList

Function: Query the TokenID name of all NFTs tokens owned by this address, and return the list.

```c
char *GetTokenIDList(char *address)
{
    int totalSupply = Atoi(ZPT_Storage_Get("totalSupply"));
    char *ForTrim = {"...."};
    char *Hash = {""};
    char *Result = {""};
    for (int i = 1; i <= totalSupply; i = i + 1)       //Traverse the serial numbers of all TokenIDs
    {
        Hash = ZPT_Storage_Get(Itoa(i));
        if (strcmp(ZPT_Storage_Get(Hash), address) == 1)         //Filter the TokenID owned by the current given address and splicing
        {
            Result = strconcat(Result, Hash);
            Result = strconcat(Result, ForTrim);
        }
    }
    return Result;
}
```

#### Implementation interface
After reading the above specific method implementation, finally look at the interface that calls the above method, the sample code is as follows:
```c
char *invoke(char *method, char *args)
{
    if (strcmp(method, "init") == 0)         //“init” is function name
    {
        char *value = Init();
        char *result = ZPT_JsonMashalResult(value,"string",1);    //Return value format setting, "string" is the returned format, 1 means successful execution
        ZPT_Runtime_Notify(result);          //notify event
        return result;
    }

    if (strcmp(method, "create") == 0)
    {

        struct Params                                                       //Use a structure to contain the parameters of the required input
        {
            char *TokenID;
            char *address;
        };
        struct Params *p = (struct Params *)malloc(sizeof(struct Params));   //Allocate space to the parameter structure
        ZPT_JsonUnmashalInput(p, sizeof(struct Params), args);               //Parsing the input   
        char *value = Create(p->TokenID, p->address);                        //Incoming parameters
        char * result = ZPT_JsonMashalResult(value,"string",1);
        ZPT_Runtime_Notify(result);
        return result;
    }

    if (strcmp(method, "totalSupply") == 0)
    {
        char *value = TotalSupply();
        char * result = ZPT_JsonMashalResult(value,"string",1);
        ZPT_Runtime_Notify(result);
        return result;
    }

    if (strcmp(method, "ownerOf") == 0)
    {

        struct Params
        {
            char *TokenID;
        };
        struct Params *p = (struct Params *)malloc(sizeof(struct Params));
        ZPT_JsonUnmashalInput(p, sizeof(struct Params), args);
        char *value = OwnerOf(p->TokenID);
        char * result = ZPT_JsonMashalResult(value,"string",1);
        ZPT_Runtime_Notify(result);
        return result;
    }

    if (strcmp(method, "owns") == 0)
    {

        struct Params
        {
            char *TokenID;
            char *address;
        };
        struct Params *p = (struct Params *)malloc(sizeof(struct Params));
        ZPT_JsonUnmashalInput(p, sizeof(struct Params), args);
        char *value = Owns(p->TokenID, p->address);
        char * result = ZPT_JsonMashalResult(value,"string",1);
        ZPT_Runtime_Notify(result);
        return result;
    }

    if (strcmp(method, "balanceOf") == 0)
    {

        struct Params
        {
            char *address;
        };
        struct Params *p = (struct Params *)malloc(sizeof(struct Params));
        ZPT_JsonUnmashalInput(p, sizeof(struct Params), args);
        char *value = BalanceOf(p->address);
        char * result = ZPT_JsonMashalResult(value,"string",1);
        ZPT_Runtime_Notify(result);
        return result;
    }

    if (strcmp(method, "approve") == 0)
    {

        struct Params
        {
            char *from;
            char *to;
            char *TokenID;
        };
        struct Params *p = (struct Params *)malloc(sizeof(struct Params));
        ZPT_JsonUnmashalInput(p, sizeof(struct Params), args);
        char *value = Approve(p->from, p->to, p->TokenID);
        char * result = ZPT_JsonMashalResult(value,"string",1);
        ZPT_Runtime_Notify(result);
        return result;
    }
    
    if (strcmp(method, "approverOf") == 0)
  {

    struct Params
    {
      char *TokenID;
    };
    struct Params *p = (struct Params *)malloc(sizeof(struct Params));
    ZPT_JsonUnmashalInput(p, sizeof(struct Params), args);
    char *value = ApproverOf(p->TokenID);
    char * result = ZPT_JsonMashalResult(value,"string",1);
    ZPT_Runtime_Notify(result);
    return result;
  }

  if (strcmp(method, "approvedFor") == 0)
  {

    struct Params
    {
      char *TokenID;
      char *address;
    };
    struct Params *p = (struct Params *)malloc(sizeof(struct Params));
    ZPT_JsonUnmashalInput(p, sizeof(struct Params), args);
    char *value = ApprovedFor(p->address, p->TokenID);
    char * result = ZPT_JsonMashalResult(value,"string",1);
    ZPT_Runtime_Notify(result);
    return result;
  }

    if (strcmp(method, "transferFromOwner") == 0)
    {

        struct Params
        {
            char *from;
            char *to;
            char *TokenID;
        };
        struct Params *p = (struct Params *)malloc(sizeof(struct Params));
        ZPT_JsonUnmashalInput(p, sizeof(struct Params), args);
        char *value = TransferFromOwner(p->from, p->to, p->TokenID);
        char * result = ZPT_JsonMashalResult(value,"string",1);
        ZPT_Runtime_Notify(result);
        return result;
    }

    if (strcmp(method, "transferFromApproval") == 0)
    {

        struct Params
        {
            char *from;
            char *to;
            char *approval;
            char *TokenID;
        };
        struct Params *p = (struct Params *)malloc(sizeof(struct Params));
        ZPT_JsonUnmashalInput(p, sizeof(struct Params), args);
        char *value = TransferFromApproval(p->from, p->to, p->approval, p->TokenID);
        char * result = ZPT_JsonMashalResult(value,"string",1);
        ZPT_Runtime_Notify(result);
        return result;
    }

    if (strcmp(method, "getTokenIDList") == 0)
    {

        struct Params
        {
            char *address;
        };
        struct Params *p = (struct Params *)malloc(sizeof(struct Params));
        ZPT_JsonUnmashalInput(p, sizeof(struct Params), args);
        char *value = GetHashList(p->address);
        char * result = ZPT_JsonMashalResult(value,"string",1);
        ZPT_Runtime_Notify(result);
        return result;
    }  
}
```
At this point, the analysis of the specific sample code is over. After all the writing is completed, you can click Build. After no error, you can click the Wasm on the page to download, and then you can use this file to deploy the smart contract.

**I believe that you have seen through the above GCP-30 example, basically have the excellent contract code written in the c language based on wasm!**

#### Precautions
The following are the anomalies that occur when an individual is writing.

* Because the C language is used for contract development, all incoming and outgoing calls are char types, so pay attention to the conversion of data types;
* The function written pays attention to its order. The C language calling function will only call the function that appeared before, so the called function should be placed above the calling function, otherwise it will report an error;

