#include "Base64.h"

/*
标准的Base64并不适合直接放在URL里传输，因为URL编码器会把标准Base64中的“/”和“+”字符变为形如“%XX”的形式，
而这些“%”号在存入数据库时还需要再进行转换，因为ANSI SQL中已将“%”号用作通配符。
    为解决此问题，可采用一种用于URL的改进Base64编码，它在末尾填充'='号，并将标准Base64中的“+”和“/”分别改成了“-”和“_”，这样就免去了在URL编解码和数据库存储时所要作的转换，避免了编码信息长度在此过程中的增加，并统一了数据库、表单等处对象标识符的格式。
    另有一种用于正则表达式的改进Base64变种，它将“+”和“/”改成了“!”和“-”，因为“+”,“*”以及前面在IRCu中用到的“[”和“]”在正则表达式中都可能具有特殊含义。
    此外还有一些变种，它们将“+/”改为“_-”或“._”（用作编程语言中的标识符名称）或“.-”（用于XML中的Nmtoken）甚至“_:”（用于XML中的Name）。
    Base64要求把每三个8Bit的字节转换为四个6Bit的字节（3*8 = 4*6 = 24），然后把6Bit再添两位高位0，组成四个8Bit的字节，也就是说，转换后的字符串理论上将要比原来的长1/3。
    规则
    关于这个编码的规则：
    ①.把3个字节变成4个字节。
    ②每76个字符加一个换行符。
    ③.最后的结束符也要处理。
*/

//原实现参考
//https://blog.csdn.net/r5014/article/details/83345819
//根据查找表优化待续
//https://blog.csdn.net/flushhip/article/details/82498670
//使用Base64Decrypt需要用try-catch代码块包裹起来，因为对密文进行解码，密文可能不合法，
//这个时候Base64Decrypt类只能通过抛出std::invalid_argument异常来告诉用户。

//Web安全版Base64
//Web安全版Base64其实和标准版一样，只不过映射表中的+对应-, /对应_，所有代码只要根据这两个地方具体改动，
//就可以完成Web安全版的Base64，Web安全版Base64也叫SafeUrlBase64。

//refactor by lizulin 20191209

//基于查找表实现
static const unsigned char Base64EncodeMap_STD[64]=
{
    'A','B','C','D','E','F','G','H',
    'I','J','K','L','M','N','O','P',
    'Q','R','S','T','U','V','W','X',
    'Y','Z','a','b','c','d','e','f',
    'g','h','i','j','k','l','m','n',
    'o','p','q','r','s','t','u','v',
    'w','x','y','z','0','1','2','3',
    '4','5','6','7','8','9','+','/'
};

static const unsigned char Base64EncodeMap_URL[64]=
{
    'A','B','C','D','E','F','G','H',
    'I','J','K','L','M','N','O','P',
    'Q','R','S','T','U','V','W','X',
    'Y','Z','a','b','c','d','e','f',
    'g','h','i','j','k','l','m','n',
    'o','p','q','r','s','t','u','v',
    'w','x','y','z','0','1','2','3',
    '4','5','6','7','8','9','-','_'
};

static unsigned char Base64DecodeMap_STD[256]=
{
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0x3E,0xFF,0xFF,0xFF,0x3F,    //对应两处不为FF的对应 + /
    0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,
    0x3C,0x3D,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
    0xFF,0x00,0x01,0x02,0x03,0x04,0x05,0x06,
    0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,
    0x0F,0x10,0x11,0x12,0x13,0x14,0x15,0x16,
    0x17,0x18,0x19,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F,0x20,
    0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,
    0x29,0x2A,0x2B,0x2C,0x2D,0x2E,0x2F,0x30,
    0x31,0x32,0x33,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF
};
static unsigned char Base64DecodeMap_URL[256]=
{
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    //0xFF,0xFF,0xFF,0x3E,0xFF,0xFF,0xFF,0x3F,      //old
    0xFF,0xFF,0xFF,0xFF,0xFF,0x3E,0xFF,0xFF,        //0x3E 对应'-'
    0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,
    0x3C,0x3D,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
    0xFF,0x00,0x01,0x02,0x03,0x04,0x05,0x06,
    0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,
    0x0F,0x10,0x11,0x12,0x13,0x14,0x15,0x16,
    //0x17,0x18,0x19,0xFF,0xFF,0xFF,0xFF,0xFF,  //old
    0x17,0x18,0x19,0xFF,0xFF,0xFF,0xFF,0x3F,     //0X3F对应_’
    0xFF,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F,0x20,    
    0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,
    0x29,0x2A,0x2B,0x2C,0x2D,0x2E,0x2F,0x30,
    0x31,0x32,0x33,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF
};

//兼容标准与url版本的解码映射表  “已经其他变种版本兼容待续 !”和“-”
static unsigned char Base64DecodeMap_Mix[256]=
{
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    //0xFF,0xFF,0xFF,0x3E,0xFF,0xFF,0xFF,0x3F,      //old
    0xFF,0xFF,0xFF,0x3E,0xFF,0x3E,0xFF,0x3F,        //0x3E 对应'-'
    0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,
    0x3C,0x3D,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
    0xFF,0x00,0x01,0x02,0x03,0x04,0x05,0x06,
    0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,
    0x0F,0x10,0x11,0x12,0x13,0x14,0x15,0x16,
    //0x17,0x18,0x19,0xFF,0xFF,0xFF,0xFF,0xFF,  //old
    0x17,0x18,0x19,0xFF,0xFF,0xFF,0xFF,0x3F,     //0X3F对应_’
    0xFF,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F,0x20,    
    0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,
    0x29,0x2A,0x2B,0x2C,0x2D,0x2E,0x2F,0x30,
    0x31,0x32,0x33,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF
};


void Base64Encrypt::initMember()
{
    Base64EncodeMap=(m_option&Encode_URL)?Base64EncodeMap_URL:Base64EncodeMap_STD;
    groupLen=0;
}

void Base64Encrypt::Update(const void *input, size_t length)
{
    //注意_buf.reserve(_buf.size() + (length - (LEN - _groupLength) + LEN - 1) / LEN * 4 + 1);这行代码，好好体会下。
    const unsigned int LEN = 3;
    Buf.reserve(Buf.size()+(length-(LEN-groupLen)+LEN-1)/LEN*4+1);
    const unsigned char *pBufIn = reinterpret_cast<const unsigned char *>(input);

    for (size_t k= 0;k<length;++k)
    {
        group[groupLen++]=*pBufIn++;
        if (groupLen == LEN)
        {
            Encode();
        }
    }
    bdoFinal=false;
}

void Base64Encrypt::Encode()
{
    unsigned char tCh;
    tCh = group[0]>>2;                                              //第一个字节取前6位并将其右移2位 
    Buf.push_back(Base64EncodeMap[tCh]);
    tCh = ((group[0]&0x03)<<4)|(group[1]>>4);  //第一个字节取后2位并左移4位 + 第二个字节取前4位并右移4位 
    Buf.push_back(Base64EncodeMap[tCh]);
    tCh = ((group[1]&0x0F)<< 2) | (group[2] >> 6);  //第二个字节取后4位并左移2位 + 第三个字节取前2位并右移6位 
    Buf.push_back(Base64EncodeMap[tCh]);
    tCh = group[2] & 0x3F;                                          //第三个字节取后六位并且无需移位 
    Buf.push_back(Base64EncodeMap[tCh]);               //从Base64编码表中查询split对应元素索引的Base64编码 

    //优化待续--凑齐4个再一起push
    //Buf.push_back(retChar,4);   //没有push N的实现，待续

     groupLen = 0;
}

void Base64Encrypt::Final()
{
    unsigned char resChar;
    if (groupLen == 1)
    {
        group[1] = 0;
        resChar = group[0] >> 2;
        Buf.push_back(Base64EncodeMap[resChar]);
        resChar = ((group[0] & 0x03) << 4) | (group[1] >> 4);
        Buf.push_back(Base64EncodeMap[resChar]);
        if ((m_option&OmitTrailingEquals)!=0)   //add new
        {
            Buf.push_back('=');
            Buf.push_back('=');
        }
    }
    else if (groupLen == 2)
    {
        group[2] = 0;
        resChar = group[0] >> 2;
        Buf.push_back(Base64EncodeMap[resChar]);
        resChar = ((group[0] & 0x03) << 4) | (group[1] >> 4);
        Buf.push_back(Base64EncodeMap[resChar]);
        resChar = ((group[1] & 0x0F) << 2) | (group[2] >> 6);
        Buf.push_back(Base64EncodeMap[resChar]);

        if ((m_option&OmitTrailingEquals)!=0)   //add new
        {
             Buf.push_back('=');
        }
    }
    groupLen = 0;
    //若为自动换行版本,结束时候重新添加换行符,每76个字符加一个换行符。
    if (m_option&Encode_AutoWrap)
    {
            std::vector<unsigned char> newBuf;
            for (size_t n=0;n<Buf.size();n++)
            {
                newBuf.push_back(Buf[n]);
                if ((n%76)==75)
                {
#ifdef _WIN32
                     newBuf.push_back('\r');
                     newBuf.push_back('\n');
#else
                     newBuf.push_back('\n');
#endif
                }
            }
            Buf=newBuf;
    }
}


void Base64Decrypt::initMember()
{
    //Base64DecodeMap=(m_option&Encode_URL)?Base64DecodeMap_URL:Base64DecodeMap_STD;
    Base64DecodeMap=Base64DecodeMap_Mix;   //解码可以兼容各种版本,没有必要判断
    groupLen=0;
}

void Base64Decrypt::Update(const void *input, size_t length)
{
    const unsigned int LEN = 4;
    Buf.reserve(Buf.size()+(length+(LEN-groupLen)+LEN-1)/LEN*3+1);

    const unsigned char *pBufIn = reinterpret_cast<const unsigned char *>(input);
    for (size_t k = 0;k<length;++k)
    {
        unsigned char tmpChar=*pBufIn++;

        //忽略换行回车 add new
        if (tmpChar=='\r'||tmpChar=='\n')
        {
            continue;
        }

        if (Base64DecodeMap[tmpChar]==0xFF)     
        {
            throw std::invalid_argument("ciphertext is illegal");
        }

        group[groupLen++]=tmpChar;
        if (groupLen == LEN)
        {
            Decode();
        }
    }
}

void Base64Decrypt::Decode()
{
    unsigned char resChar[3];
    unsigned int top = 1;
    if (group[0]=='='|| group[1]=='=')    //一次合并判断，优化待续
    {
        throw std::invalid_argument("ciphertext is illegal");
    }

    resChar[0] = (Base64DecodeMap[group[0]] << 2) | (Base64DecodeMap[group[1]] >> 4);
    if (group[2]!='=')
    {
        resChar[1] = ((Base64DecodeMap[group[1]] & 0x0F) << 4) | (Base64DecodeMap[group[2]] >> 2);
        top = 2;
    }
    if (group[3]!='=')
    {
        resChar[2] = (Base64DecodeMap[group[2]] << 6) |Base64DecodeMap[group[3]];
        top = 3;
    }

    for (unsigned int i = 0; i < top; ++i)
    {
        Buf.push_back(resChar[i]);
    }

    groupLen = 0;
}


//
//Base64::Base64(void)
//{
//
//}
//
//Base64::~Base64(void)
//{
//
//}


const char alphabet_base64[] ="ABCDEFGH" "IJKLMNOP" "QRSTUVWX" "YZabcdef"
    "ghijklmn" "opqrstuv" "wxyz0123" "456789+/";

const char alphabet_base64url[] ="ABCDEFGH" "IJKLMNOP" "QRSTUVWX" "YZabcdef"
    "ghijklmn" "opqrstuv" "wxyz0123" "456789-_";



std::string Base64Encode(const char* buf,size_t len,int option)
{
    Base64Encrypt Encoder(option);
    Encoder.Update(buf,len);
    Encoder.Final();
    return Encoder.GetString();
}

bool Base64Decode(const std::string& sSrc,char* out,size_t& outLen)
{
    Base64Decrypt Decoder;
    Decoder.Update(sSrc.data(),sSrc.length());
    if (!Decoder.CheckFinal())
    {
        outLen=0;
        return false;
    }
    //copy result bytes to dst buffer
    const std::vector<unsigned char>& Res=Decoder.const_Data();
    for (size_t n=0;n<Res.size();n++)
    {
        out[n]=Res[n];
    }
    outLen=Res.size();
    return true;
}

std::string encode_base64(const std::string sourceData, int options /*= Base64Encoding*/)
{
    const char *const alphabet = (options&Encode_URL)? alphabet_base64url : alphabet_base64;
    const char padchar = '=';
    int padlen = 0;

    std::string tmp;
    tmp.resize((sourceData.size()+2)/3*4);

    int i = 0;
    char *out = &tmp[0];
    while (i < sourceData.size()) {
        // encode 3 bytes at a time
        int chunk = 0;
        chunk |= int(uchar(sourceData.data()[i++])) << 16;
        if (i == sourceData.size()) {
            padlen = 2;
        } else {
            chunk |= int(uchar(sourceData.data()[i++])) << 8;
            if (i == sourceData.size())
                padlen = 1;
            else
                chunk |= int(uchar(sourceData.data()[i++]));
        }

        int j = (chunk & 0x00fc0000) >> 18;
        int k = (chunk & 0x0003f000) >> 12;
        int l = (chunk & 0x00000fc0) >> 6;
        int m = (chunk & 0x0000003f);
        *out++ = alphabet[j];
        *out++ = alphabet[k];

        if (padlen > 1) {
            if ((options & OmitTrailingEquals) == 0)
                *out++ = padchar;
        } else {
            *out++ = alphabet[l];
        }
        if (padlen > 0) {
            if ((options & OmitTrailingEquals) == 0)
                *out++ = padchar;
        } else {
            *out++ = alphabet[m];
        }
    }
    //assert((options & OmitTrailingEquals) || (out == tmp.size() + tmp.data()));
    if (options&OmitTrailingEquals)
    {
        tmp.resize(out - tmp.data());
    }
    return tmp;
}

std::string decode_base64(const std::string sourceData, int options /*= Base64Encoding*/)
{
    unsigned int buf = 0;
    int nbits = 0;
    std::string tmp;
    tmp.resize((sourceData.size()*3)/4);

    int offset = 0;
    for (int i = 0; i < sourceData.size(); ++i)
    {
        int ch = sourceData.at(i);
        int d;

        if (ch >= 'A' && ch <= 'Z')
            d = ch - 'A';
        else if (ch >= 'a' && ch <= 'z')
            d = ch - 'a' + 26;
        else if (ch >= '0' && ch <= '9')
            d = ch - '0' + 52;
        else if (ch == '+' && (options & Encode_URL) == 0)
            d = 62;
        else if (ch == '-' && (options & Encode_URL) != 0)
            d = 62;
        else if (ch == '/' && (options & Encode_URL) == 0)
            d = 63;
        else if (ch == '_' && (options & Encode_URL) != 0)
            d = 63;
        else
            d = -1;

        if (d != -1)
        {
            buf = (buf << 6) | d;
            nbits += 6;
            if (nbits >= 8) {
                nbits -= 8;
                tmp[offset++] = buf >> nbits;
                buf &= (1 << nbits) - 1;
            }
        }
    }

    tmp.resize(offset);
    return tmp;
}


#ifdef UNIT_TEST

#include <iostream>
void Test_Base64()
{
    std::string srcData = "ABCDEFGHIGKLMN...+++---000123456789~!@#$%^&*()_+";
    std::string srcData_base64 = encode_base64(srcData);
    std::string dstData_base64 = decode_base64(srcData_base64);

    std::cout<< "src data:" << srcData << std::endl;
    std::cout<< "to base64:" << srcData_base64 << std::endl;
    std::cout <<"from base64:" << dstData_base64 << std::endl;

    //url

    srcData = "https://mp.csdn.net/postedit/83345819";
    srcData_base64 = encode_base64(srcData, Encode_URL & OmitTrailingEquals);
    dstData_base64 = decode_base64(srcData_base64, Encode_URL & OmitTrailingEquals);

    std::cout<< "src data:" << srcData << std::endl;
    std::cout<< "to base64:" << srcData_base64 << std::endl;
    std::cout <<"from base64:" << dstData_base64 << std::endl;
}
#endif
