<h1 align="center">GCP-30合约标准</h1>

GCP-30 : [English](/English/GCP30_EN.md) | [中文](/Chinese/GCP30_CN.md)


## 目录
   
* [摘要](#摘要)
* [概念](#概念)
* [目标](#目标)
* [规格](#规格)
    * [具体分析](#具体分析)
    * [实现接口](#实现接口)
    * [注意事项](#注意事项)


### 摘要
GCP-30是智品区块链用于非同质资产(Non-Fungible Token，以下简称NFT)的协议，对应以太坊的ERC721协议。


### 概念
以下标准允许在智能合约中实施NFT的标准API。该标准提供了跟踪和传输NFT的基本功能。

我们认为NFT的用例由个人拥有和交易以及托付给第三方经纪人/钱包/拍卖商（“运营商”）。NFT可以代表对数字或实物资产的所有权，例如：

* 物理财产 - 房屋，独特的艺术品
* 虚拟收藏品 - 小猫，可收集独特卡片的图片
* “负值”资产 - 贷款，负担和其他责任

一般来说，所有房屋都是独特的，没有两只小猫是相同的。NFT是可区分的，您必须分别跟踪每个NFT的所有权。

### 目标
标准协议允许钱包/第三方经纪人/拍卖商与WASM上的任何NFT一起使用，我们提供GCP-30智能合约模版以及跟踪任意大量NFT的合约。

### 规格
**每个符合GCP-30标准的合同都必须实施GCP-30接口**，以下是用C语言基于WASM编写的GCP-30标准智能合约，此环境中参数的传递以及返回的值都以字符的形式返回，所以我们在自己编写合约的时候需要注意数据类型的转换。

我们开始GCP-30智能合约模版的学习，首先先认识下所需要用到的主要函数接口：
```c
char *OwnerOf(char *TokenID)
//返回tokenId代币持有者的地址

char *BalanceOf(char *address)
//返回由address 持有的NFTs的数量

void Transfer(char *from, char *to, char *TokenID)
//变更NFT的控制权，从地址from-->to

char *Approve(char *from, char *to, char *TokenID)
//授予地址 to具有 tokenId的控制权，方法成功后需触发Approval 事件

char *ApproverOf(char *TokenID)
//返回 tokenId授权的地址

char *ApprovedFor(char *address, char *TokenID)
//查询地址是否是另一个地址名下的tokenID的授权地址

char *TransferFromOwner(char *owner, char *to, char *TokenID)
//由NFT拥有者直接将其tokenID的控制权转移给已被授权的地址

char *TransferFromApproval(char *from, char *to, char *approval, char *TokenID）
//由tokenID的授权者地址（approval）将tokenID从 地址from 转移给 地址to

```

#### 具体分析
可在https://wasdk.github.io/WasmFiddle 网页上直接在线编辑，随时点击Build来检查有没有存在语法编辑错误等。

##### init
初始化NFTs代币，起始设置为0。示例代码如下：
```c
#define true "1";
#define false "0";
int init_amount = 0;

char *Init()
{
    if (arrayLen(ZPT_Storage_Get("totalSupply")) != 0)    //先判断是否已经初始化完成，或是已存在代币了，如果代币总量不为空则返回false
    {
        return "init has~~~~~ finished!";
    }
    ZPT_Storage_Put("totalSupply", Itoa(init_amount));   //以key--value的形式存入代币总数量0，init_amount为int类型，存入时需用接口Itoa()转换成char类型
    return "init success!";
}
```

##### totalSupply
可以用来查询当前NFTs代币的总数量，当前使用这个命令的话则会查询到当前总量为0。示例代码如下：
```c
char *TotalSupply()
{
    if (arrayLen(ZPT_Storage_Get("totalSupply")) == 0)  //判断是否已经存在初始化
    {
        return "you need init!";
    }
    return ZPT_Storage_Get("totalSupply");  //根据“totalSupply”的键key，返回总数的值value
}
```

##### create
创建一个新的NFT代币，每个TokenID都对应唯一的地址。示例代码如下：
```c
char *Create(char *TokenID, char *address){
 if (arrayLen(ZPT_Storage_Get("totalSupply")) == 0)   //判断有无进行初始化
    {
        return "You need `init!";
    }
    char *Result = ZPT_Storage_Get(TokenID);    //获取TokenID的地址，如果地址不为空则表示已存在此NFT代币
    if (arrayLen(Result) != 0)
        return "Your TokenID is existed";
    Transfer("", address, TokenID);             //调用Transfer()函数来存入创建的新NFT代币，这边第一个参数为空值
    int totalSupply = Atoi(IncreaseTotalSupply());  //这边做一个代币总量totalSupply的增加，调用IncreaseTotalSupply()函数
    IncreaseIndex(Itoa(totalSupply), TokenID);   //这里调用IncreaseIndex()函数，存入一个TokenID的计数列表，记录下每一个新增的代币序号
    return "create success";
}

```
* 然后看下函数 **Transfer()** 的内部实现：

具体实现的内容分为两部分

1.当地址from为空的时候，则执行TokenID与地址to的存入，相应的地址to所拥有的NFTs代币量做加法；

2.当地址from不为空的时候，则发生了转让关系，TokenID与地址to的存入的同时，将授权过的newTokenID所对应的地址删除，并对地址from所拥有的NFTs代币量做减法。
```c
void Transfer(char *from, char *to, char *TokenID)
{

    ZPT_Storage_Put(TokenID, to);                    //存入TokenID与地址to
    int amount = Atoi(ZPT_Storage_Get(to));          //获取地址to所拥有的NFTs代币的数量
    if (amount == 0)
    {
        amount = 0;
    }
    amount = amount + 1;
    ZPT_Storage_Put(to, Itoa(amount));                //在地址to上存入代币数量增加后的结果
    if (from != "")                                   //地址不为空时
    {
        int fromAmount = Atoi(ZPT_Storage_Get(from));      //获取地址from所拥有的NFTs代币的数量
        fromAmount = fromAmount - 1;                  //做减法
        if (fromAmount == 0)
        {
            ZPT_Storage_Delete(from);                 //如果地址from所拥有的NFTs代币的数量为0，则删除地址from之前拥有的NFTs代币的数量值
        }
        else
            ZPT_Storage_Put(from, Itoa(fromAmount));  //在地址from上存入代币数量减少后的结果

        char *ap = "ap.";
        char *newTokenID = strconcat(ap, TokenID);
        ZPT_Storage_Delete(newTokenID);               //删除授权当中的记录
    }
}
```
* 再看下函数 **IncreaseTotalSupply()** 的内部实现：

具体实现了将totalSupply数量进行累加，然后返回totalSupply的字符串

```c
char *IncreaseTotalSupply()
{
    int totalSupply = Atoi(ZPT_Storage_Get("totalSupply"));   //取出当前值
    totalSupply = totalSupply + 1;                            //加法处理
    ZPT_Storage_Put("totalSupply", Itoa(totalSupply));        //存入新的值
    return Itoa(totalSupply);                                 //返回总量的字符串
}
```
* 最后看下函数 ***IncreaseIndex()*** 的内部实现：

直接做了个存储处理，将每个TokenID都标记序号存储
```c
void IncreaseIndex(char *totalSupply, char *TokenID)
{
    ZPT_Storage_Put(totalSupply, TokenID);
}
```

##### ownerOf

查询拥有此TokenID所有权的地址
```c
char *OwnerOf(char *TokenID)
{
    return ZPT_Storage_Get(TokenID);
}
```

##### owns

判断该地址是不是与拥有此TokenID所有权的地址一致

所做的步骤：

* 提取拥有此TokenID的地址，ZPT_Storage_Get(TokenID)；
* 然后strcmp（）进行两个地址的比较，返回int类型；
* 将int类型做个处理，转成char类型。

```c
char *Owns(char *TokenID, char *address)
{
    return  Itoa(strcmp(ZPT_Storage_Get(TokenID), address));    
}
```

##### balanceOf

功能：查询给定地址所拥有的NFTs代币的余额

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

功能：将地址from所拥有的TokenID授权给地址to

这里所做的处理：

* 地址to不为空；
* 判断此TokenID的拥有者与地址from配对一致；
* 确认地址from是否具有当事人的权限。

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
然后看下函数 **ApproveInternal()** 做了什么：

这边将TokenID增加个ap.前缀，拼接成一个newTokenID，然后再和地址to一起存储。

```c
void ApproveInternal(char *to, char *TokenID)
{
    char *ap = "ap.";
    char *newTokenID = strconcat(ap, TokenID);
    ZPT_Storage_Put(newTokenID, to);
}
```

##### approverOf

功能：查询此TokenID被授权的地址。

这里所做的处理：

* 将TokenID增加前缀ap.；
* 判断增加了前缀后的newTokenID是否有相对应的地址，如果为空，则输入错误；
* 获取地址

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

功能：查询此TokenID被授权的地址与给定的地址是否相匹配

这里所做的处理：

* 将TokenID增加前缀ap.；
* 提取拥有此newTokenID的地址，ZPT_Storage_Get(newTokenID)；
* 然后strcmp（）进行两个地址的比较，返回int类型；
* 将int类型做个处理，转成char类型，返回 0 或 1。

```c
char *ApprovedFor(char *address, char *TokenID)
{
    char *ap = "ap.";
    char *newTokenID = strconcat(ap, TokenID);
    return  Itoa(strcmp(ZPT_Storage_Get(newTokenID), address));
}
```

##### transferFromOwner

功能：当前TokenID的所有者调用合约进行NFT所有权的转让

这里所做的处理：

* 地址to不为空；
* 判断此TokenID的拥有者地址是否与给定的地址匹配；
* 确认地址from是否具有当事人的权限；
* 调用Transfer()函数进行NFT所有权的转让；

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

功能：当前TokenID被授权的地址调用合约进行NFT所有权的转让

这里所做的处理：

* 地址to不为空；
* 调用ApprovedFor()函数，来确认此TokenID被授权的地址与给定的地址是否相匹配；
* from必须是TokenID的所有者；
* 确认被授权的地址是否具有当事人的权限；
* 调用Transfer()函数进行NFT所有权的转让；

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

功能：查询此地址所拥有的所有NFTs代币的TokenID名，返回列表。

```c
char *GetTokenIDList(char *address)
{
    int totalSupply = Atoi(ZPT_Storage_Get("totalSupply"));
    char *ForTrim = {"...."};
    char *Hash = {""};
    char *Result = {""};
    for (int i = 1; i <= totalSupply; i = i + 1)       //将所有TokenID的序号做个循环
    {
        Hash = ZPT_Storage_Get(Itoa(i));
        if (strcmp(ZPT_Storage_Get(Hash), address) == 1)         //筛选当前给定地址所拥有的TokenID，进行拼接
        {
            Result = strconcat(Result, Hash);
            Result = strconcat(Result, ForTrim);
        }
    }
    return Result;
}
```

#### 实现接口
看完以上的具体方法实现，最后看下调用以上方法的接口，示例代码如下：
```c
char *invoke(char *method, char *args)
{
    if (strcmp(method, "init") == 0)         //“init”为方法名
    {
        char *value = Init();
        char *result = ZPT_JsonMashalResult(value,"string",1);    //返回值格式的设置，“string”为返回的格式，1代表成功执行
        ZPT_Runtime_Notify(result);          //触发事件
        return result;
    }

    if (strcmp(method, "create") == 0)
    {

        struct Params                                                       //用一个结构体来包含所需输入的参数
        {
            char *TokenID;
            char *address;
        };
        struct Params *p = (struct Params *)malloc(sizeof(struct Params));   //给参数结构体分配空间
        ZPT_JsonUnmashalInput(p, sizeof(struct Params), args);               //对输入进行解析   
        char *value = Create(p->TokenID, p->address);                        //传入参数
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
到此具体的示例代码分析结束了，当编写完成后就可以点击Build，如果没有错误后便可以点击页面上Wasm进行下载文件，之后便可以用此文件来部署该智能合约了。

**相信看通过以上GCP30的示例，你已基本具备了使用c语言基于wasm编写出出色的合约代码了的能力！**

#### 注意事项
以下是个人在编写时发生的异常情况：

* 因为使用C语言进行合约开发的时候，所有的传入传出都是char类型，所以要注意数据类型的转换；
* 编写的函数注意其先后顺序，C语言调用函数只会调用前面出现过的函数，所以被调用函数要放在调用函数的上方，否则会报错；
