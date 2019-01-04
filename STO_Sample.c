//system apis
void * calloc(int count,int length);
void * malloc(int size);
int arrayLen(void *a);
int memcpy(void * dest,void * src,int length);
int memset(void * dest,char c,int length);

//utility apis
int strcmp(char *a,char *b);
char * strconcat(char *a,char *b);
int Atoi(char * s);
long long Atoi64(char *s);
char * Itoa(int a);
char * I64toa(long long amount,int radix);
char * SHA1(char *s);
char * SHA256(char *s);

//parameter apis
int ZPT_ReadInt32Param(char *args);
long long ZPT_ReadInt64Param(char * args);
char * ZPT_ReadStringParam(char * args);
void ZPT_JsonUnmashalInput(void * addr,int size,char * arg);
char * ZPT_JsonMashalResult(void * val,char * types,int succeed);
char * ZPT_JsonMashalParams(void * s);
char * ZPT_RawMashalParams(void *s);
char * ZPT_GetCallerAddress();
char * ZPT_GetSelfAddress();
char * ZPT_CallContract(char * address,char * contractCode,char * method,char * args);
char * ZPT_MarshalNativeParams(void * s);
char * ZPT_MarshalNeoParams(void * s);

//Runtime apis
int ZPT_Runtime_CheckWitness(char * address);
void ZPT_Runtime_Notify(char * address);
int ZPT_Runtime_CheckSig(char * pubkey,char * data,char * sig);
int ZPT_Runtime_GetTime();
void ZPT_Runtime_Log(char * message);

//Attribute apis
int ZPT_Attribute_GetUsage(char * data);
char * ZPT_Attribute_GetData(char * data);

//Block apis
char * ZPT_Block_GetCurrentHeaderHash();
int ZPT_Block_GetCurrentHeaderHeight();
char * ZPT_Block_GetCurrentBlockHash();
int ZPT_Block_GetCurrentBlockHeight();
char * ZPT_Block_GetTransactionByHash(char * hash);
int * ZPT_Block_GetTransactionCountByBlkHash(char * hash);
int * ZPT_Block_GetTransactionCountByBlkHeight(int height);
char ** ZPT_Block_GetTransactionsByBlkHash(char * hash);
char ** ZPT_Block_GetTransactionsByBlkHeight(int height);


//Blockchain apis
int ZPT_BlockChain_GetHeight();
char * ZPT_BlockChain_GetHeaderByHeight(int height);
char * ZPT_BlockChain_GetHeaderByHash(char * hash);
char * ZPT_BlockChain_GetBlockByHeight(int height);
char * ZPT_BlockChain_GetBlockByHash(char * hash);
char * ZPT_BlockChain_GetContract(char * address);

//header apis
char * ZPT_Header_GetHash(char * data);
int ZPT_Header_GetVersion(char * data);
char * ZPT_Header_GetPrevHash(char * data);
char * ZPT_Header_GetMerkleRoot(char  * data);
int ZPT_Header_GetIndex(char * data);
int ZPT_Header_GetTimestamp(char * data);
long long ZPT_Header_GetConsensusData(char * data);
char * ZPT_Header_GetNextConsensus(char * data);

//storage apis
void ZPT_Storage_Put(char * key,char * value);
char * ZPT_Storage_Get(char * key);
void ZPT_Storage_Delete(char * key);

//transaction apis
char * ZPT_Transaction_GetHash(char * data);
int ZPT_Transaction_GetType(char * data);
char * ZPT_Transaction_GetAttributes(char * data);


//Find how many times a character occurs in an array
int count(char * a){
    int len = arrayLen(a);
    int cnt = 0;
    for(int i = 0; i < len; i++){
        if(a[i] == '{')
            cnt++;
    }
    cnt--;
    return cnt;
}

//Joint two arrays
char * concat(char * a, char * b){
    int lena = arrayLen(a);
    int lenb = arrayLen(b);
    char * res = (char *)malloc((lena + lenb)*sizeof(char));
    for (int i = 0 ;i < lena ;i++){
        res[i] = a[i];
    }
    for (int j = 0; j < lenb ;j++){
        res[lena + j] = b[j];
    }
    return res;
}


char * symbol = "MOX";

//contract administration
char * ceoAddress = "ZEuzshrCsE1cnvPuuRrDYgnVYNDtyt5d3X";
char * adminAddress = "ZNEo7CMRpQXGDgSwvhm2iDGPTXhVRJcMfc";

//Pause the contract to stop increaseTotal, decreaseTotal, transfer, transferFrom and approve methods
char * pause(){
    if (Atoi(ZPT_Storage_Get("paused")) == 1)
        return "The contract has already been paused.";
    if (ZPT_Runtime_CheckWitness(ceoAddress) == 0)
        return "You have no permission to pause the contract.";
    ZPT_Storage_Put("paused", "1");
    return "The cantract is paused successfully.";
}

//Unpause the contract which is paused.
char * unPause(){
    if (Atoi(ZPT_Storage_Get("paused")) == 0)
        return "No need to unpause because the contract is not under pause situation.";
    if (ZPT_Runtime_CheckWitness(ceoAddress) == 0)
        return "You have no permission to unpause the contract.";
    ZPT_Storage_Put("paused", "0");
    return "The cantract is unpaused successfully.";
}

//Check if an account is in the database
int isStored(char * key){
    if (arrayLen(ZPT_Storage_Get(key)) == 0)
        return 0;
    return 1;
}

//Check if an account is appreved by another one
int isApproved(char * owner, char * spender){
    char * allowedKey = concat(owner, spender);
    if (arrayLen(ZPT_Storage_Get(allowedKey)) == 0)
        return 0;
    return 1;
}

//Initialize the contract, includes the totalSupply value.
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

char * totalSupply(){
    char * total_supply = ZPT_Storage_Get("totalSupply");
    if (arrayLen(total_supply) == 0)
        return "Please init first.";
    return total_supply;
}

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

char * balanceOf(char * address){
    char * balance = ZPT_Storage_Get(address);
    if (arrayLen(balance) == 0)
        return "Address is not in our database.";
    return balance;
}

char * approve(char * ownerAddr, char * spenderAddr, char * allowedChar){
    if (Atoi(ZPT_Storage_Get("paused")) == 1)
        return "The contract has been paused.";
    if (arrayLen(ZPT_Storage_Get("totalSupply")) == 0)
        return "Please init first.";
    if (ZPT_Runtime_CheckWitness(ownerAddr) == 0)
        return "Inconsistent address.";
    if (isStored(ownerAddr) == 0)
        return "Owner address is not in our database.";
    int allowed = Atoi(allowedChar);
    if (allowed <= 0)
        return "The allowance can not be less than or equal to 0.";
    int balance_owner = Atoi(ZPT_Storage_Get(ownerAddr));
    if (balance_owner < allowed)
        return "The allowance can not be larger than the balance of owner.";
    char * allowedKey = concat(ownerAddr, spenderAddr);
    ZPT_Storage_Put(allowedKey, Itoa(allowed));
    return "Approve success!";
}

char * allowance(char * ownerAddr, char * spenderAddr){
    if (isStored(ownerAddr) == 0)
        return "Owner address is not in our database.";
    if (isApproved(ownerAddr, spenderAddr) == 0)
        return "Not approved yet.";
    char * allowedKey = concat(ownerAddr, spenderAddr);
    return ZPT_Storage_Get(allowedKey);
}


///////////////////// STO 新增功能 /////////////////////

//设置黑名单
char * setBlack(char * address){
    if ((ZPT_Runtime_CheckWitness(ceoAddress) == 0) && (ZPT_Runtime_CheckWitness(adminAddress) == 0))
        return "You have no permission to set account black.";
    char * addrStatus = concat(address, "status");
    ZPT_Storage_Put(addrStatus, "1");
    return "Set account black success.";
}

//冻结账户
char * setFrozen(char * address){
    if ((ZPT_Runtime_CheckWitness(ceoAddress) == 0) && (ZPT_Runtime_CheckWitness(adminAddress) == 0))
        return "You have no permission to freeze an account.";
    char * addrStatus = concat(address, "status");
    ZPT_Storage_Put(addrStatus, "2");
    return "Set account frozen success";
}

//设置账户为正常状态
char * setNormal(char * address){
    if ((ZPT_Runtime_CheckWitness(ceoAddress) == 0) && (ZPT_Runtime_CheckWitness(adminAddress) == 0))
        return "You have no permission to set account normal.";
    char * addrStatus = concat(address, "status");
    ZPT_Storage_Put(addrStatus, "0");
    return "Set account normal success";
}

//返回账户状态
char * status(char * address){
    char * addrStatus = concat(address, "status");
    if (Atoi(ZPT_Storage_Get(addrStatus)) == 0)
        return "normal";
    if (Atoi(ZPT_Storage_Get(addrStatus)) == 1)
        return "black";
    if (Atoi(ZPT_Storage_Get(addrStatus)) == 2)
        return "frozen";
    return "unchecked";
}

//转账限制的判断函数
int detectTransferRestriction(char * from, char * to){
    if (strcmp(status(from),"unchecked") ==0)
        return 1;
    if (strcmp(status(from),"black") ==0)
        return 2;
    if (strcmp(status(from),"frozen") ==0)
        return 3;
    if (strcmp(status(to),"unchecked") == 0)
        return 4;
    if (strcmp(status(to),"black") == 0)
        return 5;
    return 0;
}

//判断结果的解释函数
char * messageForTransferRestriction(int restrictionCode){
    if (restrictionCode == 1)
        return "Sender is an unchecked account.";
    if (restrictionCode == 2)
        return "Sender is in blacklist.";
    if (restrictionCode == 3)
        return "Sender account has been frozen.";
    if (restrictionCode == 4)
        return "Receiver is an unchecked account.";
    if (restrictionCode == 5)
        return "Receiver is in blacklist.";
}

//在transfer和transferFrom中添加合规检查
char * transfer(char * fromAddr, char * toAddr, char * amountChar){
    //合规检查
    int restrictionCode = detectTransferRestriction(fromAddr, toAddr);
    if (restrictionCode != 0)
        return messageForTransferRestriction(restrictionCode);
    //////////
    
    if (Atoi(ZPT_Storage_Get("paused")) == 1)
        return "The contract has been paused.";
    if (arrayLen(ZPT_Storage_Get("totalSupply")) == 0)
        return "Please init first.";
    if (ZPT_Runtime_CheckWitness(fromAddr) == 0)
        return "Inconsistent address.";
    if (isStored(fromAddr) == 0)
        return "Sender address is not in our database.";
    int amount = Atoi(amountChar);
    if (amount <= 0)
        return "Transfer amount cannot be less than or equal to 0.";
    int balance_from = Atoi(ZPT_Storage_Get(fromAddr));
    if (balance_from < amount)
        return "No sufficient balance.";
    
    balance_from -= amount;
    if (balance_from == 0)
        ZPT_Storage_Delete(fromAddr);
    else
        ZPT_Storage_Put(fromAddr,Itoa(balance_from));
    int balance_to = Atoi(ZPT_Storage_Get(toAddr));
    balance_to += amount;
    ZPT_Storage_Put(toAddr,Itoa(balance_to));
    return "Transfer success.";
}

char * transferFrom(char *fromAddr, char *spenderAddr, char *toAddr, char *amountChar){
    //合规检查
    int restrictionCode = detectTransferRestriction(fromAddr, toAddr);
    if (restrictionCode != 0)
        return messageForTransferRestriction(restrictionCode);
    //////////
    
    if (Atoi(ZPT_Storage_Get("paused")) == 1)
        return "The contract has been paused.";
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
    int balance_from = Atoi(ZPT_Storage_Get(fromAddr));
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
    int balance_to = Atoi(ZPT_Storage_Get(toAddr));
    balance_to += amount;
    ZPT_Storage_Put(toAddr,Itoa(balance_to));
    allowed -= amount;
    if (allowed == 0)
        ZPT_Storage_Delete(allowedKey);
    else
        ZPT_Storage_Put(allowedKey, Itoa(allowed));
    return "TransferFrom success.";
}



char * invoke(char * method,char * args){
    
    char * result;
    
    if (strcmp(method, "getSymbol")==0){
        ZPT_Runtime_Notify(symbol);
        return symbol;
    }
    
    if (strcmp(method, "getCeoAddress")==0){
        ZPT_Runtime_Notify(ceoAddress);
        return ceoAddress;
    }
    
    if (strcmp(method, "getAdminAddress")==0){
        ZPT_Runtime_Notify(adminAddress);
        return adminAddress;
    }
    
    if (strcmp(method ,"pause")==0){
        if(count(args) != 1){
            ZPT_Runtime_Notify("The number of params is incorrect. Please input one params.");
            return "The number of params is incorrect. Please input one params.";
        }
        struct Params{
            char * address;
        };
        struct Params *p = (struct Params *)malloc(sizeof(struct Params));
        ZPT_JsonUnmashalInput(p,sizeof(struct Params),args);
        result = pause(p->address);
        ZPT_Runtime_Notify(result);
        return result;
    }
    
    if (strcmp(method ,"unpause")==0){
        if(count(args) != 1){
            ZPT_Runtime_Notify("The number of params is incorrect. Please input one params.");
            return "The number of params is incorrect. Please input one params.";
        }
        struct Params{
            char * address;
        };
        struct Params *p = (struct Params *)malloc(sizeof(struct Params));
        ZPT_JsonUnmashalInput(p,sizeof(struct Params),args);
        result = unPause(p->address);
        ZPT_Runtime_Notify(result);
        return result;
    }
    
    if (strcmp(method ,"init")==0){
        if(count(args) != 2){
            ZPT_Runtime_Notify("The number of params is incorrect. Please input two params.");
            return "The number of params is incorrect. Please input two params.";
        }
        struct Params{
            char * totalSupply;
            char * adminAddress;
        };
        struct Params *p = (struct Params *)malloc(sizeof(struct Params));
        ZPT_JsonUnmashalInput(p,sizeof(struct Params),args);
        result = init(p->totalSupply, p->adminAddress);
        ZPT_Runtime_Notify(result);
        return result;
    }
    
    if (strcmp(method, "totalSupply")==0){
        result = totalSupply();
        ZPT_Runtime_Notify(result);
        return result;
    }
    
    if (strcmp(method, "increaseTotal")==0){
        if(count(args) != 1){
            ZPT_Runtime_Notify("The number of params is incorrect. Please input one param.");
            return "The number of params is incorrect. Please input one param.";
        }
        struct Params{
            char * value;
        };
        struct Params *p = (struct Params *)malloc(sizeof(struct Params));
        ZPT_JsonUnmashalInput(p,sizeof(struct Params),args);
        result = increaseTotal(p->value);
        ZPT_Runtime_Notify(result);
        return result;
    }
    
    if (strcmp(method, "decreaseTotal")==0){
        if(count(args) != 1){
            ZPT_Runtime_Notify("The number of params is incorrect. Please input one param.");
            return "The number of params is incorrect. Please input one param.";
        }
        struct Params{
            char * value;
        };
        struct Params *p = (struct Params *)malloc(sizeof(struct Params));
        ZPT_JsonUnmashalInput(p,sizeof(struct Params),args);
        result = decreaseTotal(p->value);
        ZPT_Runtime_Notify(result);
        return result;
    }
    
    if (strcmp(method, "balanceOf")==0){
        if(count(args) != 1){
            ZPT_Runtime_Notify("The number of params is incorrect. Please input one param.");
            return "The number of params is incorrect. Please input one param.";
        }
        struct Params{
            char * address;
        };
        struct Params *p = (struct Params *)malloc(sizeof(struct Params));
        ZPT_JsonUnmashalInput(p,sizeof(struct Params),args);
        result = balanceOf(p->address);
        ZPT_Runtime_Notify(result);
        return result;
    }
    
    if (strcmp(method, "setBlack")==0){
        if(count(args) != 1){
            ZPT_Runtime_Notify("The number of params is incorrect. Please input one param.");
            return "The number of params is incorrect. Please input one param.";
        }
        struct Params{
            char * address;
        };
        struct Params *p = (struct Params *)malloc(sizeof(struct Params));
        ZPT_JsonUnmashalInput(p,sizeof(struct Params),args);
        result = setBlack(p->address);
        ZPT_Runtime_Notify(result);
        return result;
    }
    
    if (strcmp(method, "setFrozen")==0){
        if(count(args) != 1){
            ZPT_Runtime_Notify("The number of params is incorrect. Please input one param.");
            return "The number of params is incorrect. Please input one param.";
        }
        struct Params{
            char * address;
        };
        struct Params *p = (struct Params *)malloc(sizeof(struct Params));
        ZPT_JsonUnmashalInput(p,sizeof(struct Params),args);
        result = setFrozen(p->address);
        ZPT_Runtime_Notify(result);
        return result;
    }
    
    if (strcmp(method, "setNormal")==0){
        if(count(args) != 1){
            ZPT_Runtime_Notify("The number of params is incorrect. Please input one param.");
            return "The number of params is incorrect. Please input one param.";
        }
        struct Params{
            char * address;
        };
        struct Params *p = (struct Params *)malloc(sizeof(struct Params));
        ZPT_JsonUnmashalInput(p,sizeof(struct Params),args);
        result = setNormal(p->address);
        ZPT_Runtime_Notify(result);
        return result;
    }
    
    if (strcmp(method, "status")==0){
        if(count(args) != 1){
            ZPT_Runtime_Notify("The number of params is incorrect. Please input one param.");
            return "The number of params is incorrect. Please input one param.";
        }
        struct Params{
            char * address;
        };
        struct Params *p = (struct Params *)malloc(sizeof(struct Params));
        ZPT_JsonUnmashalInput(p,sizeof(struct Params),args);
        result = status(p->address);
        ZPT_Runtime_Notify(result);
        return result;
    }
    
    
    if (strcmp(method, "transfer")==0){
        if(count(args) != 3){
            ZPT_Runtime_Notify("The number of params is incorrect. Please input three params.");
            return "The number of params is incorrect. Please input three params.";
        }
        struct Params{
            char * from;
            char * to;
            char * amount;
        };
        struct Params *p = (struct Params *)malloc(sizeof(struct Params));
        ZPT_JsonUnmashalInput(p,sizeof(struct Params),args);
        result = transfer(p->from, p->to, p->amount);
        ZPT_Runtime_Notify(result);
        return result;
    }
    
    if (strcmp(method, "transferFrom")==0){
        if(count(args) != 4){
            ZPT_Runtime_Notify("The number of params is incorrect. Please input four params.");
            return "The number of params is incorrect. Please input four params.";
        }
        struct Params{
            char * from;
            char * spender;
            char * to;
            char * amount;
        };
        struct Params *p = (struct Params *)malloc(sizeof(struct Params));
        ZPT_JsonUnmashalInput(p,sizeof(struct Params),args);
        result = transferFrom(p->from, p->spender, p->to, p->amount);
        ZPT_Runtime_Notify(result);
        return result;
    }
    
    if (strcmp(method, "approve")==0){
        if(count(args) != 3){
            ZPT_Runtime_Notify("The number of params is incorrect. Please input three params.");
            return "The number of params is incorrect. Please input three params.";
        }
        struct Params{
            char * owner;
            char * spender;
            char * allowed;
        };
        struct Params *p = (struct Params *)malloc(sizeof(struct Params));
        ZPT_JsonUnmashalInput(p,sizeof(struct Params),args);
        result = approve(p->owner, p->spender, p->allowed);
        ZPT_Runtime_Notify(result);
        return result;
    }
    
    if (strcmp(method, "allowance")==0){
        if(count(args) != 2){
            ZPT_Runtime_Notify("The number of params is incorrect. Please input two params.");
            return "The number of params is incorrect. Please input two params.";
        }
        struct Params{
            char * owner;
            char * spender;
        };
        struct Params *p = (struct Params *)malloc(sizeof(struct Params));
        ZPT_JsonUnmashalInput(p,sizeof(struct Params),args);
        result = allowance(p->owner, p->spender);
        ZPT_Runtime_Notify(result);
        return result;
    }
    
    result = "Invalid invoke method.";
    ZPT_Runtime_Notify(result);
    return result;
}
