#ifndef	_QRCODE_H_
#define	_QRCODE_H_

/////////////////////////////////////////////////////////////////////////////
// QR Level
#define QR_LEVEL_L	0
#define QR_LEVEL_M	1
#define QR_LEVEL_Q	2
#define QR_LEVEL_H	3

// QR Mode
#define QR_MODE_NUMERAL		0
#define QR_MODE_ALPHABET	1
#define QR_MODE_8BIT		2
#define QR_MODE_KANJI		3

// QR Version
#define QR_VRESION_S	0 // 1 ~ 9
#define QR_VRESION_M	1 // 10 ~ 26
#define QR_VRESION_L	2 // 27 ~ 40

#define MAX_ALLCODEWORD	 3706 // MAX ALL CODE WORD
#define MAX_DATACODEWORD 2956 // MAX DATACODE WORD(Ver40-L)
#define MAX_CODEBLOCK	  153 // MAX CODE BLOCK
#define MAX_MODULESIZE	  177 // (Ver1~Ver40 21x21~177x177 stepSize=4)

#define QR_MARGIN	4

/////////////////////////////////////////////////////////////////////////////
typedef struct tagRS_BLOCKINFO
{
	int ncRSBlock;		// 
	int ncAllCodeWord;	// 
	int ncDataCodeWord;	// 
} RS_BLOCKINFO, *LPRS_BLOCKINFO;

/////////////////////////////////////////////////////////////////////////////
// QR
typedef struct tagQR_VERSIONINFO
{
	int nVersionNo;	   // VersionNo(1~40)
	int ncAllCodeWord; // 

	// CorrectLevel(0 = L, 1 = M, 2 = Q, 3 = H) 
	int ncDataCodeWord[4];	//

	int ncAlignPoint;	// 
	int nAlignPoint[6];	// 

	RS_BLOCKINFO RS_BlockInfo1[4]; //
	RS_BLOCKINFO RS_BlockInfo2[4]; //

} QR_VERSIONINFO, *LPQR_VERSIONINFO;

#define ABS(x)  ((x) > (0) ? (x) : (-x))
#define MIN(x,y)  ((x) > (y) ? (y) : (x))

int GetEncodeVersion(int nVersion, char* lpsSource, int ncLength);
char EncodeData(int nLevel, int nVersion, char bAutoExtent, int nMaskingNo, char* lpsSource, int ncSource);
char EncodeSourceData(char* lpsSource, int ncLength, int nVerGroup);
int GetBitLength(unsigned char nMode, int ncData, int nVerGroup);
int SetBitStream(int nIndex, unsigned short wData, int ncData);
char IsNumeralData(unsigned char c);
char IsAlphabetData(unsigned char c);
char IsKanjiData(unsigned char c1, unsigned char c2);
unsigned char AlphabetToBinaly(unsigned char c);
unsigned short KanjiToBinaly(unsigned short wc);
void GetRSCodeWord(unsigned char* lpbyRSWork, int ncDataCodeWord, int ncRSCodeWord);
void FormatModule(void);
void SetFunctionModule(void);
void SetFinderPattern(unsigned int x, unsigned int y);
void SetAlignmentPattern(unsigned int x, unsigned int y);
void SetVersionPattern(void);
void SetCodeWordPattern(void);
void SetMaskingPattern(unsigned int nPatternNo);
void SetFormatInfoPattern(int nPatternNo);
unsigned int CountPenalty(void);
void DrawQRCode(char* qrc);
#endif


